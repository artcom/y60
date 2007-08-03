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
//    $RCSfile: ComPort.cpp,v $
//     $Author: thomas $
//   $Revision: 1.18 $
//
//=============================================================================

#include "ComPort.h"
#include <asl/Block.h>
#include <asl/Logger.h>

#include <asl/string_functions.h>
#include <asl/numeric_functions.h>

using namespace std;

#define DB(x) // x

namespace asl {

    const unsigned READ_BUFFER_SIZE = 1024;

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

        DCB myDCB;
        if ( ! GetCommState(_myPortHandle, & myDCB)) {
            checkError(PLUS_FILE_LINE);
        }
        myDCB.BaudRate = convertBaudRate(theBaudRate);
        myDCB.ByteSize = theDataBits;
        myDCB.Parity   = convertParity(theParityMode);
        myDCB.StopBits = convertStopBits(theStopBits);
        myDCB.fRtsControl = (theHWHandShakeFlag ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_DISABLE);
        myDCB.fDtrControl = (theHWHandShakeFlag ? DTR_CONTROL_HANDSHAKE : DTR_CONTROL_DISABLE);

        if ( ! SetCommState(_myPortHandle, & myDCB)) {
            checkError(PLUS_FILE_LINE);
        }

        // see MSDN docs about COMMTIMEOUTS,
        //     Stevens - Advanced Programming in the Unix Environment (pp.352)
        //     and termios man page
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
    ComPort::readFromDevice(unsigned char * theBuffer, size_t & theSize) {
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

    bool
    ComPort::readBlocking(unsigned char * theBuffer, size_t & theSize) {
        size_t myMinReadBytes = _myMinReadBytes == 0 ? theSize : _myMinReadBytes;
        if (_myBuffer.size() < myMinReadBytes) {
            size_t myBytesToRead = myMinReadBytes - _myBuffer;
            asl::Block myTmpBuffer(myBytesToRead);
            readFromDevice(myTmpBuffer.begin(), myBytesToRead);
            myTmpBuffer.resize( myBytesToRead );

            _myBuffer.append( myTmpBuffer );
        }
        size_t myBytesToReturn = asl::minimum(theSize, _myBuffer.size() );
        _myBuffer.readBytes(theBuffer, myBytesToReturn, 0);
        size_t myRestSize = _myBuffer.size() - myBytesToReturn;
        _myBuffer.readBytes(_myBuffer.begin(), myRestSize, myBytesToReturn);
        _myBuffer.resize( myRestSize );

        if (myBytesToReturn == theSize) {
            return true;
        } else {
            theSize = myBytesToReturn;
            return false;
        }
    }

    bool
    ComPort::readNonBlocking(unsigned char * theBuffer, size_t & theSize) {
        size_t mySize;
        mySize = asl::maximum(theSize, READ_BUFFER_SIZE);
        asl::Block myTmpBuffer(mySize);
        readFromDevice(myTmpBuffer.begin(), mySize);
        myTmpBuffer.resize(mySize);

        _myBuffer.append(myTmpBuffer);
        DB(cerr << "_myBuffer post readDev:" << _myBuffer << endl);
        size_t myReadBytes = 0;
        if (_myBuffer.size() > theSize) {
            // copy data from internal buffer to outside buffer
            _myBuffer.readBytes(theBuffer, theSize, 0);
            // copy rest of buffer to begin
            size_t myRestSize = _myBuffer.size() - theSize;
            _myBuffer.readBytes(_myBuffer.begin(), myRestSize, theSize);
            DB(cerr << "myRestSize = " << myRestSize << endl);
            _myBuffer.resize(myRestSize);
            myReadBytes = theSize;
        } else {
            // hand out all we have
            myReadBytes = _myBuffer.size();
            _myBuffer.readBytes(theBuffer, myReadBytes, 0);
            _myBuffer.resize(0);
        }
 
        DB(cerr << "_myBuffer at end      :" << _myBuffer << endl);


        if (isNoisy() && myReadBytes > 0) {
            cerr << "ComPort::read(): '" << Block((unsigned char *)theBuffer,
                (unsigned char *)(theBuffer + myReadBytes)) << "'" << endl;
        }

        if (theSize == myReadBytes) {
            return true;
        } else {
            theSize = myReadBytes;
            return false;
        }
    }

    bool
    ComPort::read(char * theBuffer, size_t & theSize) {
        DB(cerr << "_myBuffer at start    :" << _myBuffer << endl);
        if (_myBlockingFlag) {
            return readBlocking( (unsigned char*) theBuffer, theSize );
        } else {
            return readNonBlocking( (unsigned char*) theBuffer, theSize );
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
#ifdef WINDOWS_WOULD_BE_CONSISTENT_WE_COULD_USE
        DWORD myAvailableBytes = 0;
        if (!PeekNamedPipe(_myPortHandle, NULL, 0, 0, &myAvailableBytes, NULL)) {
            checkError("ComPort::peek()");
        }
        return myAvailableBytes;
#else // but we need:
        size_t mySize = READ_BUFFER_SIZE;
        asl::Block myTmpBuffer(mySize);
        if ( _myBlockingFlag) {
            COMMTIMEOUTS myNonBlockingTimeouts;
            myNonBlockingTimeouts.ReadIntervalTimeout = MAXDWORD;
            myNonBlockingTimeouts.ReadTotalTimeoutMultiplier = 0;
            myNonBlockingTimeouts.ReadTotalTimeoutConstant = 0;
            myNonBlockingTimeouts.WriteTotalTimeoutConstant = 0;
            myNonBlockingTimeouts.WriteTotalTimeoutMultiplier = 0;
            if ( ! SetCommTimeouts(_myPortHandle, & myNonBlockingTimeouts)) {
                checkError(PLUS_FILE_LINE);
            }

        }
        readFromDevice(myTmpBuffer.begin(), mySize);
        if ( _myBlockingFlag ) {
            if ( ! SetCommTimeouts(_myPortHandle, & _myTimeouts)) {
                checkError(PLUS_FILE_LINE);
            }
        }
        myTmpBuffer.resize(mySize);
        _myBuffer.append(myTmpBuffer);
        return _myBuffer.size();
#endif
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
