/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
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

#ifndef _Y60_WIN32MOTE_DRIVER_INCLUDED_
#define _Y60_WIN32MOTE_DRIVER_INCLUDED_


#include "WiiRemote.h"

#include "stdafx.h"
#include "Utils.h"
//#include "HIDDevice.h"

#include <asl/math/Vector234.h>
#include <asl/base/Ptr.h>
#include <asl/dom/Nodes.h>
#include <y60/scene/Scene.h>
#include <asl/base/PosixThread.h>
#include <asl/base/ThreadLock.h>

#include <iostream>
#include <vector>
#include <queue>
#include <math.h>

#pragma once

#ifdef WIN32
extern "C" {
#   include "hidsdi.h"
#   include "setupapi.h"
}
#endif

namespace y60 {
            
    class Win32mote :
        public WiiRemote
    { 
    public:
        Win32mote(PSP_DEVICE_INTERFACE_DETAIL_DATA & detailData, HANDLE & theDevice);
        virtual ~Win32mote();
            
        //static std::vector<WiiRemotePtr> discover();

    protected:
        void send(unsigned char theOutputReport[], unsigned theNumBytes);
        static void Win32mote::inputReportListener(PosixThread & theThread); 
        void closeDevice();

        HIDP_CAPS       _myCapabilities;
        HANDLE          _myDeviceHandle;
        HANDLE          _myReadHandle;
        HANDLE          _myWriteHandle;
        HANDLE          _myEventObject;
        OVERLAPPED      _myHIDOverlap;

        LPCTSTR _myDevicePath;
        
    private:
        void PrepareForOverlappedTransfer();
        void GetDeviceCapabilities();

        Win32mote();
    };
    typedef asl::Ptr<Win32mote> Win32motePtr;
    
}

#endif // _Y60_WIN32MOTE_DRIVER_INCLUDED_

