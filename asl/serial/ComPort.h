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
//    $RCSfile: ComPort.h,v $
//     $Author: ulrich $
//   $Revision: 1.9 $
//
//=============================================================================

#ifndef ASL_COM_PORT_INCLUDED
#define ASL_COM_PORT_INCLUDED

#include "SerialDevice.h"
#include <asl/Block.h>
#include <string>
#include <windows.h>

namespace asl {

    /*! @addtogroup aslserial */
    /* @{ */

    /*! Win32 implementation of a serial port.
     * @see SerialDevice
     */
    class ComPort : public SerialDevice {
        public:
            ComPort(const std::string & theDeviceName);
            virtual ~ComPort();

            void open(unsigned int theBaudRate, unsigned int theDataBits,
                      ParityMode theParityMode, unsigned int theStopBits,
                      bool theHWHandShakeFlag,
                      int theMinBytesPerRead, int theTimeout );
            void close();

            bool read(char * theBuffer, size_t & theSize);
            void write(const char * theBuffer, size_t theSize);
            unsigned peek();
            void flush();

            void setStatusLine(unsigned theStatusMask);
            unsigned getStatusLine();

        private:
            ComPort();
            bool readFromDevice(unsigned char * theBuffer, size_t & theSize);
            bool readBlocking(unsigned char * theBuffer, size_t & theSize);
            bool readNonBlocking(unsigned char * theBuffer, size_t & theSize);
            void checkError(const std::string & theLocationString);
            DWORD convertBaudRate(unsigned int theBaudRate);
            BYTE convertParity(ParityMode theParity);
            BYTE convertStopBits(unsigned int  theStopBits);

            HANDLE       _myPortHandle;
            COMMTIMEOUTS _myTimeouts;
            Block        _myBuffer;
            bool         _myBlockingFlag;
            unsigned     _myMinReadBytes;
    };

    /* @} */
}

#endif
