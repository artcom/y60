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


#include "stdafx.h"
#include "Utils.h"
#include "HIDDevice.h"

#include <asl/Vector234.h>
#include <asl/Ptr.h>
#include <dom/Nodes.h>
#include <y60/GenericEvent.h>
#include <y60/Scene.h>

#include <iostream>
#include <vector>
#include <math.h>

#pragma once

const static char INPUT_REPORT_BUTTONS		= 0x30;
const static char OUTPUT_REPORT_SET_MOTION	= 0x12;
const static char OUTPUT_WRITE_DATA			= 0x16;
const static char OUTPUT_READ_DATA			= 0x17;
const static char INPUT_REPORT_MOTION		= 0x31;

const static char INPUT_READ_DATA			= 0x21;
const static char INPUT_WRITE_DATA			= 0x22;



namespace y60 {
            
    class Win32mote :
        public HIDDevice
    { 
    public:
        Win32mote();
        ~Win32mote() {}

        static std::vector<Win32mote> discover();
        void close();

        y60::GenericEventPtr createEvent( int theID, const std::string & theType, std::string & theButtonName, bool thePressedState);
        y60::GenericEventPtr createEvent( int theID, const std::string & theType, asl::Vector3i & theMotionData);
        asl::Vector3i getLastMotionData() const { return _myLastMotion; }

        Button getButton(std::string label);
        std::vector<Button> getButtons() const { return _myButtons; }
        
    protected:
        
    
        bool startListening();
        bool stopListening();
        bool isListening() const { return _myListenerThread != NULL; }
        
        static unsigned CALLBACK Win32mote::InputReportListener(void * param);

        std::vector<Button> setButtons(int code);
        std::vector<Button>	_myButtons;

        asl::Vector3i		_myLastMotion;
        asl::Vector3i		_myZeroPoint;
        asl::Vector3i		_myOneGPoint;
        
        dom::NodePtr                 _myEventSchema;
        asl::Ptr<dom::ValueFactory>  _myValueFactory;
        
        bool			_myInputListening;
        unsigned int _myListenerThreadId;
        HANDLE _myListenerThread;
        
    };
    typedef asl::Ptr<Win32mote, dom::ThreadingModel> Win32motePtr;

    
}



#endif // _Y60_WIN32MOTE_DRIVER_INCLUDED_

