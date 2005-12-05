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
//    $RCSfile: SerialDeviceFactory.cpp,v $
//
//     $Author: david $
//
//   $Revision: 1.1 $
//
//=============================================================================

#include "SerialDeviceFactory.h"

#include <asl/string_functions.h>

#include <string>

#ifdef WIN32
#   include "ComPort.h"
#else
#   include "TTYPort.h"
#endif

namespace asl {

SerialDevice * 
getSerialDevice(unsigned int theIndex) {
#ifdef WIN32
    return new ComPort(std::string("COM") + as_string(theIndex + 1) + ":");
#else
    return new TTYPort(std::string("/dev/ttyS") + as_string(theIndex));
#endif
}

}
