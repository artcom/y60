//============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: TTYPort.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.9 $
//
//=============================================================================

#include "TTYPort.h"

#include <asl/string_functions.h>
#include <asl/Block.h>

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

using namespace std;
using namespace asl;

namespace asl {

#define FIONREAD       0x541B

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
              bool theHWHandShakeFlag)
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

    // TODO add blocking flag
    myOpenMode |= O_NONBLOCK;

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
    myTermIO.c_cc[VMIN] = 0;

    /* TODO
    if (_isBlocking) {
        newtio.c_cc[VTIME]=10;  // expect at least one char every second
    }
    */

    tcflush(_myPortHandle, TCIOFLUSH);
    if (tcsetattr(_myPortHandle, TCSANOW, & myTermIO) != 0) {
        throw SerialPortException(string("Failed to set requested device settings on ") +
                               getDeviceName() + ".",
                               PLUS_FILE_LINE);
    }

    isOpen(true);
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
    if (ioctl(_myPortHandle, TIOCMSET, myStatus) < 0) {
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
    if (ioctl(_myPortHandle, TIOCMGET, &myStatus) < 0) {
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
    if (isNoisy()) {
        cerr << "TTYPort::peek(): status mask=0x" << hex << myStatusMask << dec << endl;
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
    size_t myWrittenBytes = ::write(_myPortHandle, theBuffer, theSize);

    if (theSize != myWrittenBytes) {
        throw SerialPortException(string("Can not write ") + as_string(theSize) +
                               " bytes to " + getDeviceName() + ". Only " +
                               as_string(myWrittenBytes) + " bytes got written.",
                               PLUS_FILE_LINE);
    }
}

int
TTYPort::convertBaudRate(unsigned int theBaudRate) {
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
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        default:
            throw SerialPortException(string("Can not set device to ") +
                    asl::as_string(theBaudRate) + " bps",
                    PLUS_FILE_LINE);
    }
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


} // end of namespace asl
