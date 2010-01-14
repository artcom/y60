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


#ifndef ASL_SERIAL_DEVICE_FACTORY_INCLUDED
#define ASL_SERIAL_DEVICE_FACTORY_INCLUDED

#include "asl_serial_settings.h"

#include <string>
#include <vector>

namespace asl {

    class SerialDevice;

    /*! @addtogroup aslserial */
    /* @{ */

    /*! This function creates a new serial device. The returned object is
     * either of type TTYPort (*nix like systems) or ComPort (Win32),
     * depending on the platform. The application is responsible to delete
     * the returned object.
     * @param theIndex Zero based index of serial port (0 gives COM1/ttyS0)
     */
    ASL_SERIAL_DECL SerialDevice * getSerialDevice(unsigned int theIndex);
    /*! This function creates a new serial device. The returned object is
     * either of type TTYPort (*nix like systems) or ComPort (Win32),
     * depending on the platform. The application is responsible to delete
     * the returned object.
     * @param theDevice The name of the device to use. Usefull for stuff like
     *                  /dev/ttyUSB0
     */
    ASL_SERIAL_DECL SerialDevice * getSerialDeviceByName(const std::string & theDevice);
    ASL_SERIAL_DECL bool getSerialDeviceNames(std::vector<std::string> & theSerialDevicesNames);

    /* @} */
}

#endif // ASL_SERIAL_DEVICE_FACTORY_INCLUDED
