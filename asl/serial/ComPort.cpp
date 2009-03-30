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

#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>

using namespace std;

#define DB(x) // x

namespace asl {

    ComPort::ComPort(const std::string & theDeviceName) :
        SerialDevice(theDeviceName),
        _myBlockingFlag(false)
    {}

    ComPort::~ComPort() {
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

        _myPortHandle = CreateFile(getDeviceName().c_str(),
                                   GENERIC_READ | GENERIC_WRITE,
                                   0,
                                   NULL,
                                   OPEN_EXISTING,
                                   0,
                                   NULL);
        checkError(PLUS_FILE_LINE);

        // configure signalling mode
        DCB myDCB;
        if ( ! GetCommState(_myPortHandle, & myDCB)) {
            checkError(PLUS_FILE_LINE);
        }
        myDCB.BaudRate = convertBaudRate(theBaudRate);
        myDCB.ByteSize = static_cast<BYTE>(theDataBits);
        myDCB.Parity   = convertParity(theParityMode);
        myDCB.StopBits = convertStopBits(theStopBits);
        myDCB.fRtsControl = (theHWHandShakeFlag ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_DISABLE);
        myDCB.fDtrControl = (theHWHandShakeFlag ? DTR_CONTROL_HANDSHAKE : DTR_CONTROL_DISABLE);

        if ( ! SetCommState(_myPortHandle, & myDCB)) {
            checkError(PLUS_FILE_LINE);
        }

        // configure blocking timeouts
        //  see MSDN docs about COMMTIMEOUTS,
        //      Stevens - Advanced Programming in the Unix Environment (pp.352)
        //      and termios man page
        if ( theMinBytesPerRead == 0 && theTimeout == 0) {
            _myTimeouts.ReadIntervalTimeout = MAXDWORD;
            _myTimeouts.ReadTotalTimeoutMultiplier = 0;
            _myTimeouts.ReadTotalTimeoutConstant = 0;
            _myBlockingFlag = false;
        } else if (theMinBytesPerRead > 0 && theTimeout == 0) {
            _myTimeouts.ReadIntervalTimeout = 0;
            _myTimeouts.ReadTotalTimeoutMultiplier = 0;
            _myTimeouts.ReadTotalTimeoutConstant = 0;
            _myBlockingFlag = true;
        } else if (theMinBytesPerRead > 0 && theTimeout > 0) {
            _myTimeouts.ReadIntervalTimeout = theTimeout * 100;
            _myTimeouts.ReadTotalTimeoutMultiplier = 0;
            _myTimeouts.ReadTotalTimeoutConstant = 0;
            _myBlockingFlag = true;
        } else if (theMinBytesPerRead == 0 && theTimeout > 0) {
            _myTimeouts.ReadIntervalTimeout = MAXDWORD;
            _myTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
            _myTimeouts.ReadTotalTimeoutConstant = theTimeout * 100;
            _myBlockingFlag = true;
        }

        // TODO: Think about write timeouts
        _myTimeouts.WriteTotalTimeoutConstant = 0;
        _myTimeouts.WriteTotalTimeoutMultiplier = 0;

        _myMinReadBytes = theMinBytesPerRead;

        if ( ! SetCommTimeouts(_myPortHandle, & _myTimeouts)) {
            checkError(PLUS_FILE_LINE);
        }

        // increase buffer size to something reasonable
        SetupComm(_myPortHandle, 2048, 2048);

        isOpen(true);
    }

    void
    ComPort::close() {
        if (isOpen()) {
            CloseHandle(_myPortHandle);
            isOpen(false);
        } else {
            throw SerialPortException(string("Can not close device ") + getDeviceName() +
                                   ". Device is not open.", PLUS_FILE_LINE);
        }
    }

    bool
    ComPort::read(char * theBuffer, size_t & theSize) {
        if ( ! isOpen()) {
            throw SerialPortException(string("Can not read from device '") + getDeviceName() +
                                   "'. Device is not open.", PLUS_FILE_LINE);
        }
        size_t myReadBytes;
        ReadFile(_myPortHandle, theBuffer, DWORD(theSize), (DWORD *) & myReadBytes, NULL);

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
        WriteFile(_myPortHandle, theBuffer, theSize,  (DWORD *)& myWrittenBytes, NULL);
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
        if ( ! PurgeComm( _myPortHandle, PURGE_RXCLEAR | PURGE_TXCLEAR ) ) {
            checkError("ComPort::flush()");
        }
    }

    void
    ComPort::setStatusLine(unsigned theStatusMask) {
        if (!isOpen()) {
            throw SerialPortException(string("Unable to set status bits on device '") +
                getDeviceName() + "'. Device is not open.", PLUS_FILE_LINE);
        }

        if (!EscapeCommFunction(_myPortHandle, (theStatusMask & DTR ? SETDTR : CLRDTR))) {
            throw SerialPortException(string("Unable to modify DTR on device '") +
                getDeviceName() + "'.", PLUS_FILE_LINE);
        }
        if (!EscapeCommFunction(_myPortHandle, (theStatusMask & RTS ? SETRTS : CLRRTS))) {
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
        if (!GetCommModemStatus(_myPortHandle, &dwModemStatus)) {
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
