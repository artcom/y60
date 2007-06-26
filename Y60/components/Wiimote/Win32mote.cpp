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
                         _myValueFactory( new dom::ValueFactory()),
                         _myLeftPoint( -1 ),
                         _myLowPassedOrientation(0, 1, 0),
                         _myOrientation( 0 )
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
    throw asl::Exception(string("Could not find button with label '") + label + "'",
    										 PLUS_FILE_LINE);
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
        _myEventQueue->push( myEvent );
    }
}

void
Win32mote::createEvent( int theID, asl::Vector3f & theMotionData) {
    y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    
    myNode->appendAttribute<int>("id", theID);
    myNode->appendAttribute<std::string>("type", string("motiondata"));
    myNode->appendAttribute<Vector3f>("motiondata", theMotionData);
    
    _myEventQueue->push( myEvent );
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
       
    _myEventQueue->push( myEvent );
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
    Vector3f m(theInputReport[3] - 128, theInputReport[5] - 128,  - (theInputReport[4] - 128) );
    _myLastMotion = m;

    _myLowPassedOrientation = 0.9 * _myLowPassedOrientation + 0.1 * _myLastMotion;

    // XXX maybe swizzled!!!
    float myAbsX = abs( _myLowPassedOrientation[0] );
    float myAbsZ = abs( _myLowPassedOrientation[2] );

    if (_myOrientation == 0 || _myOrientation == 2) {
        myAbsX -= 5;
    }

    if (_myOrientation == 1 || _myOrientation == 3) {
        myAbsZ -= 5;
    }

    if (myAbsZ >= myAbsX) {
        if (myAbsZ > 5) {
            // XXX see above
            _myOrientation = (_myLowPassedOrientation[2] > 128)?0:2;
        }
    } else {
        if (myAbsX > 5) {
            _myOrientation = (_myLowPassedOrientation[0] > 128)?3:1;
        }
    }

    createEvent( m_controller_id, m);
}

void
Win32mote::handleIREvents( const unsigned char * theInputReport ) {
    Vector2i myIRPositions[4];
    myIRPositions[0] = parseIRData( theInputReport, 6);
    myIRPositions[1] = parseIRData( theInputReport, 9);
    myIRPositions[2] = parseIRData( theInputReport, 12);
    myIRPositions[3] = parseIRData( theInputReport, 15);

    float ox(0);
    float oy(0);

    if (myIRPositions[0][0] < 1023 && myIRPositions[1][0] < 1023) {
        int myLeftIndex = _myLeftPoint;
        int myRightIndex;
        if (_myLeftPoint == -1) {
			switch (_myOrientation) {
				case 0:
                    myLeftIndex = (myIRPositions[0][0] < myIRPositions[1][0]) ? 0 : 1;
                    break;
				case 1:
                    myLeftIndex = (myIRPositions[0][1] > myIRPositions[1][1]) ? 0 : 1;
                    break;
				case 2:
                    myLeftIndex = (myIRPositions[0][0] > myIRPositions[1][0]) ? 0 : 1;
                    break;
				case 3:
                    myLeftIndex = (myIRPositions[0][1] < myIRPositions[1][1]) ? 0 : 1;
                    break;
			}
			_myLeftPoint = myLeftIndex;
		}
		myRightIndex = 1 - myLeftIndex;
        
		float dx = myIRPositions[ myRightIndex ][0] - myIRPositions[ myLeftIndex ][0];
		float dy = myIRPositions[ myRightIndex ][1] - myIRPositions[ myLeftIndex ][1];
		
		float d = sqrt( dx * dx + dy * dy);
		
		
		// normalized distance between bright spots
		dx /= d;
		dy /= d;
		
		float angle = atan2(dy, dx);
		
		float cx = (myIRPositions[ myLeftIndex ][0] + myIRPositions[ myRightIndex ][0]) / 1024.0 - 1;
		float cy = (myIRPositions[ myLeftIndex ][1] + myIRPositions[ myRightIndex ][1]) / 1024.0 - .75;
		
		ox = -dy * cy - dx * cx;
		oy = -dx * cy + dy * cx;

        AC_PRINT << "x: " << ox << " y: " << oy;

    } else {
        // not tracking anything
		ox = oy = -100;
		if ( _myLeftPoint != -1) {
			_myLeftPoint = -1;
		}
    }

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
        		//AC_PRINT << "unsucessful read";
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
            Vector3f zero_point(myInputReport[6] - 128, myInputReport[7] - 128, myInputReport[8] - 128);
            myDevice._myZeroPoint = zero_point;
            Vector3f one_g_point(myInputReport[10] - 128, myInputReport[11] - 128, myInputReport[12] - 128);
            myDevice._myOneGPoint = one_g_point;
            
        } else if (INPUT_REPORT_IR == myInputReport[0]) {
            //int myX1 = myInputReport
            //cout << myInputReport << endl;
            
            myDevice._myLock->lock();
            myDevice.handleIREvents( myInputReport );
            myDevice.handleButtonEvents( myInputReport );
            myDevice.handleMotionEvents( myInputReport );

           
            // for(unsigned i=0; i<myDevice._myEventVector.size(); ++i) {
//                 myDevice._myEventQueue->push(myDevice._myEventVector[i]);
//             }
//             myDevice._myEventVector.clear();
            myDevice._myLock->unlock();

        } else {
        	AC_PRINT << "unknown report" << ios::hex << myInputReport[0] << 	ios::dec;
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
