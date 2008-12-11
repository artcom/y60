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
//    $RCSfile: SerialDevice.cpp,v $
//
//     $Author: ulrich $
//
//   $Revision: 1.5 $
//
//=============================================================================

#include "SerialDevice.h"

namespace asl {

    static const char * ParityModeStrings[] = {
        "no_parity",
        "even_parity",
        "odd_parity",
        ""
    };

    IMPLEMENT_ENUM( SerialDevice::ParityMode, ParityModeStrings );
    

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
