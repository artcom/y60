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

#ifndef ASL_SERIAL_DEVICE_INCLUDED
#define ASL_SERIAL_DEVICE_INCLUDED

#include "asl_serial_settings.h"

#include <asl/base/Enum.h>
#include <asl/base/Exception.h>
#include <string>

namespace asl {
    /*! @addtogroup aslserial */
    /* @{ */

    DEFINE_EXCEPTION(SerialPortException, asl::Exception);

    /*! Platform independent, abstract baseclass for serial ports. Use
     * getSerialDevice() or getSerialDeviceByName() to obtain the right
     * object for the current platform.
     */
    class ASL_SERIAL_DECL SerialDevice {
        public:
            virtual ~SerialDevice();

            /// Parity.
            enum ParityModeEnum {
                NO_PARITY,
                EVEN_PARITY,
                ODD_PARITY,
                ParityModeEnum_MAX
            };

            DEFINE_ENUM( ParityMode, ParityModeEnum, ASL_SERIAL_DECL );

            /// Status line bits.
            enum StatusBits {
                CTS = 0x01,    //< Clear To Send
                DSR = 0x02,    //< Data Set Ready
                RI = 0x04,     //< Ring Indicator
                CD = 0x08,     //< Carrier Detect
                DTR = 0x10,    //< Data Terminal Ready
                RTS = 0x20,    //< Request To Send
            };

            /**
             * Open the serial device using the given parameters. The arguments
             * @p theBaudRate, @p theDataBits, @p theParityMode, @p theStopBits,
             * @p theHWHandShakeFlag configure the usual communication parameters.
             *
             * The default is to open the port in non-blocking mode. If the @p
             * theMinBytesPerRead and @p theTimeout arguments are non zero,
             * the port is opened in blocking mode.
             *
             * Four diffrent behaviours are supported:
             *    - @p theMinBytesPerRead = 0 @p theTimeout = 0
             *       - perform non-blocking IO
             *    - @p theMinBytesPerRead = 0 @p theTimeout > 0
             *       - the timer is started when read is called.
             *       - read() returns all currently available bytes, or the
             *         first byte received or nothing if the timer expires
             *         before a byte becomes available.
             *    - @p theMinBytesPerRead > 0 @p theTimeout > 0
             *       - @p theTimeout is used as an interbyte timer and is started after the
             *         first byte is received.
             *       - On Unix read() returns after at least @p theMinBytesPerRead bytes
             *         have been received.
             *       - On Win32 read() returns when the buffer given to read() is full.
             *       - To achieve the same behaviour on all platforms call read() with
             *         a buffer of @p theMinBytesPerRead bytes.
             *       - All platforms return if the interbyte timer expires between the
             *         arrival of any two bytes.
             *    - @p theMinBytesPerRead > 0 @p theTimeout = 0
             *       - block indefinitely until the read() is satisfied.
             *       - On Unix the read() is satisfied when at least @p theMinBytesPerRead
             *         bytes hav been received
             *       - On Win32 the read() is satisfied when the buffer given to read()
             *         is full.
             *       - To achieve the same behaviour on all platforms call read() with
             *         a buffer of @p theMinBytesPerRead bytes.
             *
             */
            virtual void open(unsigned int theBaudRate, unsigned int theDataBits = 8,
                              ParityMode theParityMode = NO_PARITY, unsigned int theStopBits = 1,
                              bool theHWHandShakeFlag = false,
                              int theMinBytesPerRead = 0, int theTimeout = 0) = 0;
            /**
             * Close the device.
             */
            virtual void close() = 0;

            /*
             * Read up to @p theSize bytes from the device and return them in
             * @p theBuffer. The actual number of bytes read is returned in
             * @p theSize.
             */
            virtual bool read(char * theBuffer, size_t & theSize) = 0;
            virtual void write(const char * theBuffer, size_t theSize) = 0;
            virtual void flush() = 0;

            virtual void setStatusLine(unsigned theStatusMask) = 0;
            virtual unsigned getStatusLine() = 0;

            bool isOpen() const;
            bool isBlocking() const;
            const std::string & getDeviceName() const;

        protected:
            SerialDevice(const std::string & theDeviceName);
            void isOpen(bool theFlag);

        private:
            SerialDevice();

            bool        _isOpen;
            std::string _myDeviceName;
    };

/* @} */

}


#endif
