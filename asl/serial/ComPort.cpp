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
//    $RCSfile: ComPort.cpp,v $
//     $Author: thomas $
//   $Revision: 1.18 $
//
//=============================================================================

//own header
#include "ComPort.h"

#include <asl/base/Block.h>
#include <asl/base/Logger.h>

#include <asl/base/error_functions.h>
#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>

using namespace std;

#define DB(x) // x

namespace asl {

    ComPort::ComPort(const std::string & theDeviceName) :
        SerialDevice(theDeviceName),
        _myHandle(INVALID_HANDLE_VALUE)
    {
        AC_DEBUG << "ComPort::ComPort()";
    }

    ComPort::~ComPort() {
        AC_DEBUG << "ComPort::~ComPort()";

        if (isOpen()) {
            close();
        }
    }

    void
    ComPort::open(unsigned int theBaudRate, unsigned int theDataBits,
                  ParityMode theParityMode, unsigned int theStopBits,
                  bool theHWHandShakeFlag,
                  int theMinBytesPerRead, int theTimeout)
    {
        AC_DEBUG << "ComPort::open()";

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

        // open our handle
        AC_TRACE << "Opening device";
        HANDLE myHandle = CreateFile(getDeviceName().c_str(),
                                     GENERIC_READ | GENERIC_WRITE,
                                     0,
                                     NULL,
                                     OPEN_EXISTING,
                                     0,
                                     NULL);
        if(myHandle == INVALID_HANDLE_VALUE) {
            checkError(PLUS_FILE_LINE);
        }

        // get the comm properties for use during initialization
        AC_TRACE << "Getting comm properties";
        COMMPROP myProperties;
        if(!GetCommProperties(myHandle, &myProperties)) {
            checkError(PLUS_FILE_LINE);
        }

        // configure device control parameters
        //   XXX: verify if we set enough fields.
        //        there might be environmental state leaks in this.
        DCB myControlBlock;

        AC_TRACE << "Getting comm state";
        if (!GetCommState(myHandle, &myControlBlock)) {
            checkError(PLUS_FILE_LINE);
        }

        // normal COM port parameters
        myControlBlock.BaudRate = convertBaudRate(theBaudRate);
        myControlBlock.ByteSize = static_cast<BYTE>(theDataBits);
        myControlBlock.Parity   = convertParity(theParityMode);
        myControlBlock.StopBits = convertStopBits(theStopBits);
        myControlBlock.fRtsControl = (theHWHandShakeFlag ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_DISABLE);
        myControlBlock.fDtrControl = (theHWHandShakeFlag ? DTR_CONTROL_HANDSHAKE : DTR_CONTROL_DISABLE);

        myControlBlock.fAbortOnError = false;

        AC_TRACE << "Setting comm state";
        if (!SetCommState(myHandle, &myControlBlock)) {
            checkError(PLUS_FILE_LINE);
        }

        // configure device timing parameters

        COMMTIMEOUTS myTimeouts;
        if (theMinBytesPerRead == 0 && theTimeout == 0) {
            // nonblocking
            myTimeouts.ReadIntervalTimeout = MAXDWORD;
            myTimeouts.ReadTotalTimeoutMultiplier = 0;
            myTimeouts.ReadTotalTimeoutConstant = 0;
        } else if (theMinBytesPerRead > 0 && theTimeout == 0) {
            // fully blocking
            myTimeouts.ReadIntervalTimeout = 0;
            myTimeouts.ReadTotalTimeoutMultiplier = 0;
            myTimeouts.ReadTotalTimeoutConstant = 0;
        } else if (theMinBytesPerRead > 0 && theTimeout > 0) {
            // blocking with time limit starting at first received
            myTimeouts.ReadIntervalTimeout = theTimeout * 100;
            myTimeouts.ReadTotalTimeoutMultiplier = 0;
            myTimeouts.ReadTotalTimeoutConstant = 0;
        } else if (theMinBytesPerRead == 0 && theTimeout > 0) {
            // blocking with time limit starting at call
            myTimeouts.ReadIntervalTimeout = MAXDWORD;
            myTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
            myTimeouts.ReadTotalTimeoutConstant = theTimeout * 100;
        }

        // TODO: Think about write timeouts
        myTimeouts.WriteTotalTimeoutConstant = 0;
        myTimeouts.WriteTotalTimeoutMultiplier = 0;

        AC_TRACE << "Setting comm timeouts";
        if (!SetCommTimeouts(myHandle, &myTimeouts)) {
            checkError(PLUS_FILE_LINE);
        }

        // set buffer size to maximum
        AC_TRACE << "Setting comm buffer sizes";
        if(!SetupComm(myHandle, 8192, 8192)) {
            checkError(PLUS_FILE_LINE);
        }

        // finally, set our handle
        _myHandle = myHandle;

        isOpen(true);
    }

    void
    ComPort::close() {
        AC_DEBUG << "ComPort::close()";
        if (isOpen()) {
            CloseHandle(_myHandle);
            _myHandle = INVALID_HANDLE_VALUE;
            isOpen(false);
        } else {
            throw SerialPortException(string("Can not close device ") + getDeviceName() +
                                   ". Device is not open.", PLUS_FILE_LINE);
        }
    }

    bool
    ComPort::read(char * theBuffer, size_t & theSize) {
        if (!isOpen()) {
            throw SerialPortException(string("Can not read from device '") + getDeviceName() +
                                             "'. Device is not open.", PLUS_FILE_LINE);
        }

        size_t myReadBytes;
        if(!ReadFile(_myHandle, theBuffer, DWORD(theSize), (DWORD *) & myReadBytes, NULL)) {
            DWORD myError = GetLastError();
            if(myError != ERROR_OPERATION_ABORTED) {
                checkError(PLUS_FILE_LINE);
            } else {
                // we accept and ignore OPERATION_ABORTED, which is the win32 equivalent of EAGAIN
                // XXX: we might want to re-read, but handling that might be horror
                theSize = 0;
                return false;
            }
        }

        if (theSize == myReadBytes) {
            return true;
        } else {
            theSize = myReadBytes;
            return false;
        }
    }

    void
    ComPort::write(const char * theBuffer, size_t theSize) {
        if (!isOpen()) {
            throw SerialPortException(string("Can not write to device '") + getDeviceName() +
                                      "'. Device is not open.", PLUS_FILE_LINE);
        }

        if (isNoisy()) {
            cerr << "ComPort::write(): '" << Block((unsigned char *)theBuffer,
                (unsigned char *)(theBuffer + theSize)) << "'" << endl;
        }

        size_t myWrittenBytes;
        if(!WriteFile(_myHandle, theBuffer, theSize,  (DWORD *)& myWrittenBytes, NULL)) {
            DWORD myError = GetLastError();
            if(myError != ERROR_OPERATION_ABORTED) {
                checkError(PLUS_FILE_LINE);
            } else {
                // we accept and ignore OPERATION_ABORTED, which is the win32 equivalent of EAGAIN.
                // however, since we do not offer an incremental writing interface, we are bound to fail.
                myWrittenBytes = 0;
            }
        }

        // XXX: the following "error handling" is total utter bullshit and should be replaced with
        //      either queueing or incremental write support (returning a written-byte count).
        if (theSize != myWrittenBytes) {
            throw SerialPortException(string("Can not write ") + asl::as_string(theSize) +
                                   " bytes to device '" + getDeviceName() +
                                   "'. Only " + asl::as_string(myWrittenBytes) + " bytes got written.",
                                   PLUS_FILE_LINE);
        }
    }

    unsigned
    ComPort::peek() {
         if (!isOpen()) {
            throw SerialPortException(string("Can not peek device '") + getDeviceName() +
                                   "'. Device is not open.", PLUS_FILE_LINE);
        }
        // XXX: never guarantee anything.
        //      this is correct but possibly confusing.
        return 0;
    }

    void 
    ComPort::flush() {
        if (!isOpen()) {
            throw SerialPortException(string("Can not flush device '") + getDeviceName() +
                    "'. Device is not open.", PLUS_FILE_LINE);
        }
        if ( ! PurgeComm( _myHandle, PURGE_RXCLEAR | PURGE_TXCLEAR ) ) {
            checkError("ComPort::flush()");
        }
    }

    void
    ComPort::setStatusLine(unsigned theStatusMask) {
        if (!isOpen()) {
            throw SerialPortException(string("Unable to set status bits on device '") +
                getDeviceName() + "'. Device is not open.", PLUS_FILE_LINE);
        }

        if (!EscapeCommFunction(_myHandle, (theStatusMask & DTR ? SETDTR : CLRDTR))) {
            throw SerialPortException(string("Unable to modify DTR on device '") +
                getDeviceName() + "'.", PLUS_FILE_LINE);
        }
        if (!EscapeCommFunction(_myHandle, (theStatusMask & RTS ? SETRTS : CLRRTS))) {
            throw SerialPortException(string("Unable to modify RTS on device '") +
                getDeviceName() + "'.", PLUS_FILE_LINE);
        }
    }

    unsigned
    ComPort::getStatusLine() {
        if (!isOpen()) {
            throw SerialPortException(string("Unable to get status bits on device '") +
                getDeviceName() + "'. Device is not open.", PLUS_FILE_LINE);
        }

        DWORD dwModemStatus = 0;
        if (!GetCommModemStatus(_myHandle, &dwModemStatus)) {
            throw SerialPortException(string("Unable to get status bits on device '") +
                getDeviceName() + "'. Error getting status.", PLUS_FILE_LINE);
        }

        unsigned myStatusMask = 0;
        if (dwModemStatus & MS_CTS_ON) {
            myStatusMask |= CTS;
        }
        if (dwModemStatus & MS_DSR_ON) {
            myStatusMask |= DSR;
        }
        if (dwModemStatus & MS_RING_ON) {
            myStatusMask |= RI;
        }
        if (dwModemStatus & MS_RLSD_ON) {
            myStatusMask |= CD;
        }
        if (isNoisy()) {
            cerr << "ComPort::peek(): status mask=0x" << hex << myStatusMask << dec << endl;
        }

        return myStatusMask;
    }

    void
    ComPort::checkError(const std::string & theLocationString) {
        DWORD myRetVal = GetLastError();
        if (myRetVal) {
            LPVOID myMessageBuffer;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS, NULL, myRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPTSTR) & myMessageBuffer, 0, NULL);
            string myErrorString = string("'")+getDeviceName() + "': ";
            myErrorString.append((LPTSTR) myMessageBuffer);
            LocalFree(myMessageBuffer);
            SetLastError(0);
            throw SerialPortException(myErrorString, PLUS_FILE_LINE);
        }
    }

    DWORD
    ComPort::convertBaudRate(unsigned int theBaudRate) {
        // On Win32 you can just pass the actual baudrate [DS]
        return theBaudRate;
    }

    BYTE
    ComPort::convertParity(ParityMode theParity) {
        switch (theParity) {
            case NO_PARITY:
                return NOPARITY;
            case EVEN_PARITY:
                return EVENPARITY;
            case ODD_PARITY:
                return ODDPARITY;
            default:
                throw SerialPortException("Unknown parity mode.", PLUS_FILE_LINE);
        }
    }

    BYTE
    ComPort::convertStopBits(unsigned int theStopBits) {
        switch (theStopBits) {
            case 1:
                return ONESTOPBIT;
            case 2:
                return TWOSTOPBITS;
            default:
                throw SerialPortException(string("Can not handle ") + asl::as_string(theStopBits) +
                                        " stop bits.", PLUS_FILE_LINE);
        }
    }
}
