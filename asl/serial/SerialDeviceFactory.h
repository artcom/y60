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


#ifndef ASL_SERIAL_DEVICE_FACTORY_INCLUDED
#define ASL_SERIAL_DEVICE_FACTORY_INCLUDED

#include <string>

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
    SerialDevice * getSerialDevice(unsigned int theIndex);
    /*! This function creates a new serial device. The returned object is 
     * either of type TTYPort (*nix like systems) or ComPort (Win32),
     * depending on the platform. The application is responsible to delete
     * the returned object.
     * @param theDevice The name of the device to use. Usefull for stuff like
     *                  /dev/ttyUSB0
     */
    SerialDevice * getSerialDeviceByName(const std::string & theDevice);

    /* @} */
}

#endif // ASL_SERIAL_DEVICE_FACTORY_INCLUDED
