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
//    $RCSfile: SerialDevice.cpp,v $
//
//     $Author: ulrich $
//
//   $Revision: 1.5 $
//
//=============================================================================

#include "SerialDevice.h"

namespace asl {

    SerialDevice::SerialDevice(const std::string & theDeviceName) :
        _isOpen(false),
        _myNoisyFlag(false),
        _myDeviceName(theDeviceName)
    {}

    SerialDevice::~SerialDevice() {
    }

    bool
    SerialDevice::isOpen() const {
        return _isOpen;
    }

    void
    SerialDevice::isOpen(bool theFlag) {
        _isOpen = theFlag;
    }

    const std::string &
    SerialDevice::getDeviceName() const {
        return _myDeviceName;
    }
}
