//============================================================================
//
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: DebugPort.h,v $
//     $Author: ulrich $
//   $Revision: 1.5 $
//
//=============================================================================

#ifndef ASL_DEBUG_PORT_INCLUDED
#define ASL_DEBUG_PORT_INCLUDED

#include "SerialDevice.h"
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
                      bool theHWHandShakeFlag = false) 
            {
                std::cerr << "DebugPort::open() - Opening " << getDeviceName() << " with " << std::endl;
                std::cerr << "    baudrate:     " << theBaudRate << std::endl;
                std::cerr << "    databits:     " << theDataBits << std::endl;
                std::cerr << "    partity mode: " << theParityMode << std::endl;
                std::cerr << "    stopbits:     " << theStopBits << std::endl;
                std::cerr << "    hw handshake: " << theHWHandShakeFlag << std::endl;
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
