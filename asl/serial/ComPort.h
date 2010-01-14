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
//    $RCSfile: ComPort.h,v $
//     $Author: ulrich $
//   $Revision: 1.9 $
//
//=============================================================================

#ifndef ASL_COM_PORT_INCLUDED
#define ASL_COM_PORT_INCLUDED

#include "asl_serial_settings.h"

#include "SerialDevice.h"
#include <asl/base/Block.h>
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

            // opening and closing
            void open(unsigned int theBaudRate, unsigned int theDataBits,
                      ParityMode theParityMode, unsigned int theStopBits,
                      bool theHWHandShakeFlag,
                      int theMinBytesPerRead, int theTimeout );
            void close();

            // input and output
            bool read(char * theBuffer, size_t & theSize);
            void write(const char * theBuffer, size_t theSize);
            void flush();

            // status line access
            void setStatusLine(unsigned theStatusMask);
            unsigned getStatusLine();

        private:
            ComPort();

            // error handling
            void ensureDeviceOpen(const std::string & theMethodCall, const std::string & theLocationString);
            void handleSystemError(const std::string & theSystemCall, const std::string & theLocation, DWORD theError = GetLastError());
            void checkCommError(const std::string & theLocation);
            void commErrorsToString(DWORD theErrors, std::string & theString);

            // value conversion routines
            BYTE  convertParity  (ParityMode theParity);
            BYTE  convertStopBits(unsigned int  theStopBits);

            // members
            HANDLE       _myHandle;
    };

    /* @} */
}

#endif
