/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//    $RCSfile: TTYPort.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.9 $
//
//=============================================================================

#include "TTYPort.h"

#include <asl/base/string_functions.h>
#include <asl/base/Block.h>
#include <asl/base/Logger.h>

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <cstring>

using namespace std;
using namespace asl;

namespace asl {

// XXX Q: What in eris' name is this? -IA (8. Nov 2007)
//     A: IIRC this is hack by Christian Hardenberg to work around a missing define
//        in his hopelessly outdated kernel headers. I think it should be safe to 
//        remove it.
//        IMHO it would be better to use TIOCINQ anyway, because we are dealing with 
//        a terminal here. [DS]
#ifndef FIONREAD
#define FIONREAD       0x541B
#endif

TTYPort::TTYPort(const std::string & theDeviceName) :
    SerialDevice(theDeviceName)
{
}

TTYPort::~TTYPort() {
    if (isOpen()) {
        close();
    }
}

void
TTYPort::open(unsigned int theBaudRate, unsigned int theDataBits,
              ParityMode theParityMode, unsigned int theStopBits,
              bool theHWHandShakeFlag,
              int theMinBytesPerRead, int theTimeout)
{
    if (isOpen()) {
        throw SerialPortException(string("Can not open device '") + getDeviceName() +
                "'. Device allready open.", PLUS_FILE_LINE);
    }

    if (isNoisy()) {
        cerr << "Opening " << getDeviceName() << " with " << endl;
        cerr << "    baudrate:     " << theBaudRate << endl;
        cerr << "    databits:     " << theDataBits << endl;
        cerr << "    partity mode: " << theParityMode << endl;
        cerr << "    stopbits:     " << theStopBits << endl;
        cerr << "    hw handshake: " << theHWHandShakeFlag << endl;
    }

    mode_t myOpenMode = O_RDWR | O_NOCTTY;

    if (theMinBytesPerRead == 0 && theTimeout == 0) {
        myOpenMode |= O_NONBLOCK;
    } else {
        if (theMinBytesPerRead > 255) {
            AC_WARNING << "TTYPort::open(): theMinBytesPerRead to large. Clamping to 255.";
            theMinBytesPerRead = 255;
        }
        if (theTimeout > 255) {
            AC_WARNING << "TTYPort::open(): theTimeout to large. Clamping to 255.";
            theTimeout = 255;
        }
    }

    _myPortHandle = ::open(getDeviceName().c_str(), myOpenMode);
    if (_myPortHandle < 0) {
        throw SerialPortException(string("Can not open device '") + getDeviceName() + "': " +
                                strerror(errno), PLUS_FILE_LINE);
    }

    int myBaudRate    = convertBaudRate(theBaudRate);
    int myDataBits    = convertDataBits(theDataBits);
    int myParityMode  = convertParityMode(theParityMode);
    int myStopBits    = convertStopBits(theStopBits);
    int myFlowControl = convertHandShaking(theHWHandShakeFlag);

    struct termios myTermIO;
    tcgetattr(_myPortHandle, & myTermIO);

    myTermIO.c_cflag = myBaudRate | myDataBits | myParityMode |
                       myStopBits | myFlowControl | CLOCAL | CREAD;
    myTermIO.c_iflag = IGNPAR;
    myTermIO.c_oflag = 0;
    myTermIO.c_lflag = 0;

    myTermIO.c_cc[VMIN] = cc_t(theMinBytesPerRead);
    myTermIO.c_cc[VTIME]= cc_t(theTimeout);

    tcflush(_myPortHandle, TCIOFLUSH);

    isOpen(true);

    if (tcsetattr(_myPortHandle, TCSANOW, & myTermIO) != 0) {
        close();
        throw SerialPortException(string("Failed to set requested device settings on ") +
                               getDeviceName() + ".",
                               PLUS_FILE_LINE);
    }
}

void
TTYPort::close() {
    if (isOpen()) {
        ::close(_myPortHandle);
        isOpen(false);
    } else {
        throw SerialPortException(string("Can not close device ") + getDeviceName() +
                               ". Device is not open.", PLUS_FILE_LINE);
    }

}

unsigned
TTYPort::peek() {
    if (!isOpen()) {
        throw SerialPortException(string("Can not peek from device ") + getDeviceName() +
                               ". Device is not open.", PLUS_FILE_LINE);
    }

    int myResult;
    int myErrorCode = ioctl(_myPortHandle, FIONREAD, &myResult);    
    if (myErrorCode != 0) {
        throw SerialPortException(string("Can not peek from device ") + getDeviceName() +
                               ": " + strerror(errno), PLUS_FILE_LINE);
    }
    
    return myResult;
}

void 
TTYPort::flush() {
    if (!isOpen()) {
        throw SerialPortException(string("Can not flush device ") + getDeviceName() +
                               ". Device is not open.", PLUS_FILE_LINE);
    }
    if ( tcflush( _myPortHandle, TCIOFLUSH ) ) {
        throw SerialPortException(string("Failed to flush device ") + getDeviceName() +
                               ": " + strerror(errno), PLUS_FILE_LINE);
    }
}

void
TTYPort::setStatusLine(unsigned theStatusMask) {
    if (!isOpen()) {
        throw SerialPortException(string("Unable to set status bits on device '") +
            getDeviceName() + "'. Device is not open.", PLUS_FILE_LINE);
    }

    int myStatus = 0;
    if (theStatusMask & DTR) {
        myStatus |= TIOCM_DTR;
    }
    if (theStatusMask & RTS) {
        myStatus |= TIOCM_RTS;
    }
    if (ioctl(_myPortHandle, TIOCMSET, & myStatus) < 0) {
        throw SerialPortException(string("Unable to set status bits on device '") +
            getDeviceName() + "': " + strerror(errno), PLUS_FILE_LINE);
    }
}

unsigned
TTYPort::getStatusLine() {
    if (!isOpen()) {
        throw SerialPortException(string("Unable to get status bits on device '") +
            getDeviceName() + "'. Device is not open.", PLUS_FILE_LINE);
    }

    int myStatus = 0;
    if (ioctl(_myPortHandle, TIOCMGET, & myStatus) < 0) {
        throw SerialPortException(string("Unable to get status bits on device '") +
            getDeviceName() + "': " + strerror(errno), PLUS_FILE_LINE);
    }

    unsigned myStatusMask = 0;
    if (myStatus & TIOCM_CTS) {
        myStatusMask |= CTS;
    }
    if (myStatus & TIOCM_DSR) {
        myStatusMask |= DSR;
    }
    if (myStatus & TIOCM_RNG) {
        myStatusMask |= RI;
    }
    if (myStatus & TIOCM_CD) {
        myStatusMask |= CD;
    }
    if (myStatus & TIOCM_RTS) {
        myStatusMask |= RTS;
    }
    if (myStatus & TIOCM_DTR) {
        myStatusMask |= DTR;
    }
    if (isNoisy()) {
        cerr << "TTYPort::getStatusLine(): status mask=0x" << hex << myStatusMask << dec << endl;
    }

    return myStatusMask;
}

bool
TTYPort::read(char * theBuffer, size_t & theSize) {
    if ( ! isOpen()) {
        throw SerialPortException(string("Can not read from device ") + getDeviceName() +
                               ". Device is not open.", PLUS_FILE_LINE);
    }
    int myReadBytes = ::read(_myPortHandle, theBuffer, theSize);
    if (myReadBytes == -1) {
        if (errno == EAGAIN) {
            theSize = 0;
            return false;
        } else {
            throw SerialPortException(string("Failed to read from device ") + getDeviceName()
                    + ": " + strerror(errno), PLUS_FILE_LINE);
        }
    }

    if (isNoisy() && myReadBytes > 0) {
        cerr << "TTYPort::read(): '" << Block((unsigned char *)theBuffer,
            (unsigned char *)(theBuffer + myReadBytes)) << "'" << endl;
    }

    if (myReadBytes == theSize) {
        return true;
    } else {
        theSize = myReadBytes;
        return false;
    }
}

void
TTYPort::write(const char * theBuffer, size_t theSize) {
    if (!isOpen()) {
        throw SerialPortException(string("Can not write to device ") + getDeviceName() +
                               ". Device is not open.", PLUS_FILE_LINE);
    }
    if (isNoisy()) {
        cerr << "TTYPort::write(): '" << Block((unsigned char *)theBuffer,
            (unsigned char *)(theBuffer + theSize)) << "'" << endl;
    }
    ssize_t myWrittenBytes = ::write(_myPortHandle, theBuffer, theSize);
    if (/* ! blocking && */ myWrittenBytes == -1 && errno == EAGAIN) {
        AC_WARNING << "EAGAIN: write() would block.";
        return;
    }

    if (theSize != myWrittenBytes) {
        throw SerialPortException(string("Can not write ") + as_string(theSize) +
                               " bytes to " + getDeviceName() + ". Only " +
                               as_string(myWrittenBytes) + " bytes got written.",
                               PLUS_FILE_LINE);
    }
}

int
TTYPort::convertBaudRate(unsigned int theBaudRate) {
#ifdef LINUX
    switch (theBaudRate) {
        case 50:
            return B50;
        case 75:
            return B75;
        case 110:
            return B110;
        case 134:
            return B134;
        case 150:
            return B150;
        case 200:
            return B200;
        case 300:
            return B300;
        case 600:
            return B600;
        case 1200:
            return B1200;
        case 1800:
            return B1800;
        case 2400:
            return B2400;
        case 4800:
            return B4800;
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
#ifdef B57600
        case 57600:
            return B57600;
#endif
#ifdef B115200
        case 115200:
            return B115200;
#endif
#ifdef B230400
        case 230400:
            return B230400;
#endif
#ifdef B460800
        case 460800:
            return B460800;
#endif
#ifdef B500000
        case 500000:
            return B500000;
#endif
#ifdef B576000
        case 576000:
            return B576000;
#endif
#ifdef B921600
        case 921600:
            return B921600;
#endif
#ifdef B1000000
        case 1000000:
            return B1000000;
#endif
#ifdef B1152000
        case 1152000:
            return B1152000;
#endif
#ifdef B1500000
        case 1500000:
            return B1500000;
#endif
#ifdef B2000000
        case 2000000:
            return B2000000;
#endif
#ifdef B2500000
        case 2500000:
            return B2500000;
#endif
#ifdef B3000000
        case 3000000:
            return B3000000;
#endif
#ifdef B3500000
        case 3500000:
            return B3500000;
#endif
#ifdef B4000000
        case 4000000:
            return B4000000;
#endif
        default:
            throw SerialPortException(string("Can not set device to ") +
                    asl::as_string(theBaudRate) + " bps",
                    PLUS_FILE_LINE);
    }
#else // not Linux, Mac OS X that is
    // On Mac OS X you can just pass the actual baudrate [DS]
    return theBaudRate;
#endif
}

int
TTYPort::convertDataBits(unsigned int theDataBits) {
    switch (theDataBits) {
        case 5:
            return CS5;
        case 6:
            return CS6;
        case 7:
            return CS7;
        case 8:
            return CS8;
        default:
            throw SerialPortException(string("Can not handle ") + as_string(theDataBits) +
                                   " data bits.", PLUS_FILE_LINE);
    }
}

int
TTYPort::convertParityMode(ParityMode theParity) {
    switch (theParity) {
        case NO_PARITY:
            return 0;
        case EVEN_PARITY:
            return PARENB;
        case ODD_PARITY:
            return PARENB | PARODD;
        default:
            throw SerialPortException("Unknown parity mode.", PLUS_FILE_LINE);
    }
}

int
TTYPort::convertStopBits(unsigned int theStopBits) {
    switch (theStopBits) {
        case 1:
            return 0;
        case 2:
            return CSTOPB;
        default:
            throw SerialPortException(string("Can not handle ") + as_string(theStopBits)
                                   + " stop bits.", PLUS_FILE_LINE);
    }
}

int
TTYPort::convertHandShaking(bool theFlag) {
    if (theFlag) {
        return CRTSCTS;
    }
    return 0;
}

#if defined( LINUX ) && (  \
! defined( B57600 ) || \
! defined( B115200 ) || \
! defined( B230400 ) || \
! defined( B460800 ) || \
! defined( B500000 ) || \
! defined( B576000 ) || \
! defined( B921600 ) || \
! defined( B1000000 ) || \
! defined( B1152000 ) || \
! defined( B1500000 ) || \
! defined( B2000000 ) || \
! defined( B2500000 ) || \
! defined( B3000000 ) || \
! defined( B3500000 ) || \
! defined( B4000000 ))
#   warning "High baudrates are not defined. If on Linux update your libc."
#endif

} // end of namespace asl
