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
#include <asl/PosixThread.h>
#include <asl/ThreadLock.h>


#include <iostream>
#include <vector>
#include <queue>
#include <math.h>

#pragma once

const static char INPUT_REPORT_BUTTONS		= 0x30;
const static char INPUT_REPORT_IR         = 0x33;
const static char OUTPUT_REPORT_SET       	= 0x12;
const static char OUTPUT_WRITE_DATA			= 0x16;
const static char OUTPUT_READ_DATA			= 0x17;
const static char INPUT_REPORT_MOTION		= 0x31;
const static char IR_ENABLE             = 0x13;
const static char OUTPUT_REPORT_IR      = 0x04;

const static char INPUT_READ_DATA			= 0x21;
const static char INPUT_WRITE_DATA			= 0x22;



namespace y60 {
            
    class Win32mote :
        public HIDDevice,
        public asl::PosixThread
    { 
    public:
        Win32mote();
        ~Win32mote() {
            close();
        }

        void setEventQueue( asl::Ptr<std::queue<y60::GenericEventPtr> > theQueue,
                            asl::Ptr<asl::ThreadLock> theLock);

        static std::vector<asl::Ptr< Win32mote, dom::ThreadingModel> > discover();

        asl::Vector3i getLastMotionData() const { return _myLastMotion; }

        Button getButton(std::string label);
        std::vector<Button> getButtons() const { return _myButtons; }
        
        void startListening();
        void stopListening();

        //void handleButtonEvents( const unsigned char * theInputReport );
        //void handleMotionEvents( const unsigned char * theInputReport );
        //void handleIREvents( const unsigned char * theInputReport );

    protected:
        
        void createEvent( int theID, std::string & theButtonName, bool thePressedState);
        void createEvent( int theID, asl::Vector3i & theMotionData);
        void createEvent( int theID, const asl::Vector2i theIRData[4] );
    
        //asl::Vector2i parseIRData(const unsigned char * theBuffer, unsigned theOffset);
        void close();
                
        static void Win32mote::InputReportListener(PosixThread & theThread); 

        std::vector<Button> setButtons(int code);
        std::vector<Button>	_myButtons;

        asl::Vector3i		_myLastMotion;
        asl::Vector3i		_myZeroPoint;
        asl::Vector3i		_myOneGPoint;
        
        dom::NodePtr                 _myEventSchema;
        asl::Ptr<dom::ValueFactory>  _myValueFactory;
        
        bool			_myInputListening;
        //unsigned int _myListenerThreadId;
        //HANDLE _myListenerThread;
        //PosixThread _myThread;

        asl::Ptr< std::queue<y60::GenericEventPtr> > _myEventQueue;
        asl::Ptr< asl::ThreadLock > _myLock;
        
    };
    typedef asl::Ptr<Win32mote, dom::ThreadingModel> Win32motePtr;

    
}



#endif // _Y60_WIN32MOTE_DRIVER_INCLUDED_

