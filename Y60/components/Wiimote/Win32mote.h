//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_WIN32MOTE_DRIVER_INCLUDED_
#define _Y60_WIN32MOTE_DRIVER_INCLUDED_


#include "WiiRemote.h"

#include "stdafx.h"
#include "Utils.h"
//#include "HIDDevice.h"

#include <asl/Vector234.h>
#include <asl/Ptr.h>
#include <dom/Nodes.h>
#include <y60/Scene.h>
#include <asl/PosixThread.h>
#include <asl/ThreadLock.h>

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

