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
//    $RCSfile: DebugPort.h,v $
//     $Author: ulrich $
//   $Revision: 1.5 $
//
//=============================================================================

#ifndef ASL_DEBUG_PORT_INCLUDED
#define ASL_DEBUG_PORT_INCLUDED

#include "asl_serial_settings.h"

#include "SerialDevice.h"
#include <asl/math/numeric_functions.h>
#include <asl/base/Logger.h>
#include <string>

namespace asl {

    class DebugPort : public SerialDevice {
        public:
            DebugPort(const std::string & theDeviceName)
                : SerialDevice(theDeviceName), _myStatusMask(CTS | DSR | RI | CD) {
            }
            virtual ~DebugPort() {}

            void open(unsigned int theBaudRate, unsigned int theDataBits,
                      ParityMode theParityMode, unsigned int theStopBits,
                      bool theHWHandShakeFlag,
                      int theMinReadSize, int theInterCharTimeout) 
            {
                std::cerr << "DebugPort::open() - Opening " << getDeviceName() << " with " << std::endl;
                std::cerr << "    baudrate:     " << theBaudRate << std::endl;
                std::cerr << "    databits:     " << theDataBits << std::endl;
                std::cerr << "    partity mode: " << theParityMode << std::endl;
                std::cerr << "    stopbits:     " << theStopBits << std::endl;
                std::cerr << "    hw handshake: " << theHWHandShakeFlag << std::endl;
                if ( theMinReadSize != -1 || theInterCharTimeout != -1) {
                    AC_WARNING << "Blocking mode not implemented for DebugPort.";
                }
                isOpen(true);
            }

            void close() {
                std::cerr << "DebugPort::close() - Closing " << getDeviceName() << std::endl;
                isOpen(false);
            }

            bool read(char * theBuffer, size_t & theSize) {
                theSize = asl::minimum(theSize, _myData.size());
                std::copy (_myData.begin(), _myData.begin() + theSize, theBuffer);
                std::cerr << "DebugPort::read() - '" << std::string(_myData.begin(), _myData.begin() + theSize) << "'" << std::endl;
                _myData.erase(_myData.begin(), _myData.begin() + theSize); 
                return true;
            }
            
            void write(const char * theBuffer, size_t theSize) {
                _myData.append(theBuffer, theSize);
                std::cerr << "DebugPort::write() - '" << std::string(_myData.end() - theSize, _myData.end()) << "'" << std::endl;
            }

            unsigned peek() {
                return _myData.size();
            }

            void flush() {
                _myData.clear();
            }

            /// Sets the internal status lines.
            void setStatusLine(unsigned theStatusMask) {
                _myStatusMask = theStatusMask;
            }

            /// Returns the internal status lines.
            unsigned getStatusLine() {
                return _myStatusMask;
            }

        private:
            DebugPort();
            std::string _myData;
            unsigned _myStatusMask;
    };

}

#endif
