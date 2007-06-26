//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


#include "stdafx.h"
#include "assert.h"
#include "Win32mote.h"
#include "Utils.h"

#include <iostream>

extern std::string oureventxsd;

using namespace asl;
using namespace std;

namespace y60 {

Win32mote::Win32mote() : HIDDevice(),
                         PosixThread(InputReportListener),
                         _myEventSchema( new dom::Document( oureventxsd ) ),
                         _myValueFactory( new dom::ValueFactory())
{
	 // Add all Wii buttons to the our internal list
    _myButtons.push_back(Button("1",		0x0002));
    _myButtons.push_back(Button("2",		0x0001));
    _myButtons.push_back(Button("A",		0x0008));
    _myButtons.push_back(Button("B",		0x0004));
    _myButtons.push_back(Button("+",		0x1000));
    _myButtons.push_back(Button("-",		0x0010));
    _myButtons.push_back(Button("Home",	0x0080));
    _myButtons.push_back(Button("Up",	0x0800));
    _myButtons.push_back(Button("Down",	0x0400));
    _myButtons.push_back(Button("Right", 0x0200));
    _myButtons.push_back(Button("Left",	0x0100));
            
    _myInputListening = false;
    
    //_myListenerThreadId = -1;
    //_myListenerThread = NULL;
    
    registerStandardTypes( * _myValueFactory );
    registerSomTypes( * _myValueFactory );

    
}

Button
Win32mote::getButton(std::string label) {
    for (int i = 0; i < _myButtons.size(); i++) {
        if (_myButtons[i].getName().compare(label) == 0)
            return _myButtons[i];
    }
    throw exception();
}
    
    
std::vector<Button>
Win32mote::setButtons(int code) {
    std::vector<Button> changed_state;
    for (int i = 0; i < _myButtons.size(); i++) {
        if (_myButtons[i].setCode(code))
            changed_state.push_back(_myButtons[i]);
    }
    
    return changed_state;
}

    
void
Win32mote::createEvent( int theID, const asl::Vector2i theIRData[4] ) {
    y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    
    myNode->appendAttribute<int>("id", theID);
    myNode->appendAttribute<std::string>("type", string("infrareddata"));
    
    bool myGotDataFlag = false;
    for (unsigned i = 0; i < 4; ++i) {
        if (theIRData[i][0] != 1023 && theIRData[i][1] != 1023) {
            myNode->appendAttribute<Vector2i>(string("irposition") + asl::as_string(i), theIRData[i]);
            myGotDataFlag = true;
            break;

        }
    }

    if (myGotDataFlag) {
        _myEventVector.push_back( myEvent );
    }
}

void
Win32mote::createEvent( int theID, asl::Vector3i & theMotionData) {
    y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    
    myNode->appendAttribute<int>("id", theID);
    myNode->appendAttribute<std::string>("type", string("motiondata"));
    myNode->appendAttribute<Vector3i>("motiondata", theMotionData);
    
    _myEventVector.push_back( myEvent );
}

void
Win32mote::createEvent( int theID, std::string & theButtonName, bool thePressedState)
{
    
    y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
                                                   _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    
    
    myNode->appendAttribute<int>("id", theID);
    myNode->appendAttribute("type", "button");
    myNode->appendAttribute<std::string>("buttonname", theButtonName);
    myNode->appendAttribute<bool>("pressed", thePressedState);
       
    _myEventVector.push_back( myEvent );
}

Vector2i
parseIRData( const unsigned char * theBuffer, unsigned theOffset) {
    Vector2i myIRPos(0,0);
    myIRPos[0] = theBuffer[theOffset];
    myIRPos[1] = theBuffer[theOffset + 1];
    myIRPos[1] |= ( (theBuffer[theOffset + 2] >> 6) << 8);
    myIRPos[0] |= ( ((theBuffer[theOffset + 2] & 0x30) >> 4) << 8);
    
    int mySizeHint( theBuffer[ theOffset + 2] & 0x0f );

    return myIRPos;
}


void
Win32mote::handleButtonEvents( const unsigned char * theInputReport ) {
    int key_state = Util::GetInt2(theInputReport, 1);
    vector<Button> myChangedButtons = setButtons(key_state);
    for( unsigned i=0; i < myChangedButtons.size(); ++i) {
        Button myButton = myChangedButtons[i];
        createEvent( m_controller_id, myButton.getName(), myButton.pressed());
    }
}

void
Win32mote::handleMotionEvents( const unsigned char * theInputReport ) {
    // swizzle vector from xzy-order to y60-order (xyz)
    Vector3i m(theInputReport[3] - 128, theInputReport[5] - 128,  - (theInputReport[4] - 128) );
    _myLastMotion = m;
    createEvent( m_controller_id, m);
}

void
Win32mote::handleIREvents( const unsigned char * theInputReport ) {
    Vector2i myIRPositions[4];
    myIRPositions[0] = parseIRData( theInputReport, 6);
    myIRPositions[1] = parseIRData( theInputReport, 9);
    myIRPositions[2] = parseIRData( theInputReport, 12);
    myIRPositions[3] = parseIRData( theInputReport, 15);

    //cout << "pos " << myIRPositions[0] << endl;
    createEvent( m_controller_id, myIRPositions );
}

void
Win32mote::InputReportListener(PosixThread & theThread)
{
    Win32mote & myDevice = dynamic_cast<Win32mote&>(theThread);

    while (myDevice._myInputListening) {
        unsigned char myInputReport[256];
        ZeroMemory(myInputReport, 256);

        DWORD result, bytes_read;

        // Issue a read request
        if (myDevice.m_read_handle != INVALID_HANDLE_VALUE) {
            result = ReadFile(myDevice.m_read_handle, 
                    myInputReport, 
                    myDevice.m_capabilities.InputReportByteLength, 
                    &bytes_read, 
                    (LPOVERLAPPED)&myDevice.m_hid_overlapped);																						  
        }

        // Wait for read to finish
        result = WaitForSingleObject(myDevice.m_event_object, 300);

        ResetEvent(myDevice.m_event_object);

        // If the wait didn't result in a sucessful read, try again
        if (result != WAIT_OBJECT_0) {
            continue;
        }

        if (INPUT_REPORT_BUTTONS == myInputReport[0]) {

            myDevice._myLock->lock();

            myDevice.handleButtonEvents( myInputReport );

            myDevice._myLock->unlock();
        } else if (INPUT_REPORT_MOTION == myInputReport[0]) {

            
            myDevice._myLock->lock();


            myDevice.handleButtonEvents( myInputReport );
            myDevice.handleMotionEvents( myInputReport );

            myDevice._myLock->unlock();
            

        } else if (0x21 == myInputReport[0]) {
            Vector3i zero_point(myInputReport[6] - 128, myInputReport[7] - 128, myInputReport[8] - 128);
            myDevice._myZeroPoint = zero_point;
            Vector3i one_g_point(myInputReport[10] - 128, myInputReport[11] - 128, myInputReport[12] - 128);
            myDevice._myOneGPoint = one_g_point;
            
        } else if (INPUT_REPORT_IR == myInputReport[0]) {
            //int myX1 = myInputReport
            //cout << myInputReport << endl;

            
            myDevice.handleIREvents( myInputReport );
            myDevice.handleButtonEvents( myInputReport );
            myDevice.handleMotionEvents( myInputReport );

            myDevice._myLock->lock();
            for(unsigned i=0; i<myDevice._myEventVector.size(); ++i) {
                myDevice._myEventQueue->push(myDevice._myEventVector[i]);
            }
            myDevice._myEventVector.clear();
            myDevice._myLock->unlock();

        }
    }

}


void Win32mote::startListening() {
    AC_PRINT << "Started listening: " << m_controller_id;

    _myInputListening = true;
    fork();
}

    
void Win32mote::stopListening()
{
    AC_PRINT << "Stopped listening: " << m_controller_id;

    _myInputListening = false;

    join();

}


std::vector<Win32motePtr>
Win32mote::discover() {
    vector<Win32motePtr> wiimotes = HIDDevice::ConnectToHIDDevices<Win32mote>();
    /*
    if( wiimotes.size() > 0 ) {
        for(unsigned i=0; i<wiimotes.size(); ++i){
            wiimotes[i]->startListening();
        }
        cout << "wiimotes found " << wiimotes.size() << endl;
    } else {
        cout << "no wiimotes found " << endl;
    }
    */
    return wiimotes;
}

void Win32mote::close()
{
	if (isActive()) {
		stopListening();
    }

	//CloseHandle(_myListenerThread);
	CloseHandle(m_device_handle);
	CloseHandle(m_read_handle);
	CloseHandle(m_write_handle);
}

void
Win32mote::setEventQueue( asl::Ptr<std::queue<y60::GenericEventPtr> > theQueue,
               asl::Ptr<asl::ThreadLock> theLock)
{
    _myLock = theLock;
    _myEventQueue = theQueue;
}
}
