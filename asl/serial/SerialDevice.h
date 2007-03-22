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
//    $RCSfile: SerialDevice.h,v $
//     $Author: ulrich $
//   $Revision: 1.12 $
//
//=============================================================================

#ifndef ASL_SERIAL_DEVICE_INCLUDED
#define ASL_SERIAL_DEVICE_INCLUDED

#include <asl/Enum.h>
#include <asl/Exception.h>
#include <string>

namespace asl {

    DEFINE_EXCEPTION(SerialPortException, asl::Exception);

    class SerialDevice {
        public:
            virtual ~SerialDevice();

            /// Parity.
            enum ParityModeEnum {
                NO_PARITY,
                EVEN_PARITY,
                ODD_PARITY,
                ParityModeEnum_MAX
            };

            DEFINE_ENUM( ParityMode, ParityModeEnum );

            /// Status line bits.
            enum StatusBits {
                CTS = 0x01,    ///< Clear To Send
                DSR = 0x02,    ///< Data Set Ready
                RI = 0x04,     ///< Ring Indicator
                CD = 0x08,     ///< Carrier Detect
                DTR = 0x10,    ///< Data Terminal Ready
                RTS = 0x20,    ///< Request To Send
            };

            /**
             * Open the serial device using the given I/O parameters.
             */
            virtual void open(unsigned int theBaudRate, unsigned int theDataBits = 8,
                              ParityMode theParityMode = NO_PARITY, unsigned int theStopBits = 1,
                              bool theHWHandShakeFlag = false) = 0;
            virtual void close() = 0;

            virtual bool read(char * theBuffer, size_t & theSize) = 0;
            virtual void write(const char * theBuffer, size_t theSize) = 0;
            virtual unsigned peek() = 0;

            virtual void setStatusLine(unsigned theStatusMask) = 0;
            virtual unsigned getStatusLine() = 0;

            bool isOpen() const;
            const std::string & getDeviceName() const;

            /// Enable/disable debug messages.
            void setNoisy(bool theNoisyFlag) {
                _myNoisyFlag = theNoisyFlag;
            }
            bool isNoisy() const {
                return _myNoisyFlag;
            }

        protected:
            SerialDevice(const std::string & theDeviceName);
            void isOpen(bool theFlag);

        private:
            SerialDevice();

            bool        _myNoisyFlag;
            bool        _isOpen;
            std::string _myDeviceName;
    };
}

#endif
