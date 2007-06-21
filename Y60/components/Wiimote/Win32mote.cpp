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
                         _myEventSchema( new dom::Document( oureventxsd ) ),
                         _myValueFactory( new dom::ValueFactory() )

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
    
    _myListenerThreadId = -1;
    _myListenerThread = NULL;
    
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

    
GenericEventPtr
Win32mote::createEvent( int theID, const std::string & theType, asl::Vector3i & theMotionData)
{
    y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    
    myNode->appendAttribute<int>("id", theID);
    myNode->appendAttribute<std::string>("type", theType);
    myNode->appendAttribute<Vector3i>("motiondata", theMotionData);
    return myEvent;
    
}

GenericEventPtr
Win32mote::createEvent( int theID, const std::string & theType, std::string & theButtonName, bool thePressedState)
{
    
    y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
                                                   _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    
    
    myNode->appendAttribute<int>("id", theID);
    myNode->appendAttribute<std::string>("type", theType);
    myNode->appendAttribute<std::string>("buttonname", theButtonName);
    myNode->appendAttribute<bool>("pressed", thePressedState);
       
    return myEvent;
    
}

    

unsigned CALLBACK Win32mote::InputReportListener(void * param)
{
	Win32mote * myDevice = reinterpret_cast<Win32mote*>(param);

	while (myDevice->_myInputListening)
	{
		unsigned char myInputReport[256];
		ZeroMemory(myInputReport, 256);
		
		DWORD result, bytes_read;
		
		// Issue a read request
		if (myDevice->m_read_handle != INVALID_HANDLE_VALUE)
		{
			result = ReadFile(myDevice->m_read_handle, 
				myInputReport, 
				myDevice->m_capabilities.InputReportByteLength, 
				&bytes_read, 
				(LPOVERLAPPED)&myDevice->m_hid_overlapped);																						  
		}
	 
		// Wait for read to finish
		result = WaitForSingleObject(myDevice->m_event_object, 300);

		ResetEvent(myDevice->m_event_object);

		// If the wait didn't result in a sucessful read, try again
		if (result != WAIT_OBJECT_0)
			continue;

		if (INPUT_REPORT_BUTTONS == myInputReport[0])
		{
        int key_state = Util::GetInt2(myInputReport, 1);
        
        vector<Button> changed = myDevice->setButtons(key_state);
        cout << "buttons changes " << changed.size();
    }
		else if (INPUT_REPORT_MOTION == myInputReport[0])
		{
			Vector3i m(myInputReport[3] - 128, myInputReport[4] - 128, myInputReport[5] - 128);
      
			int key_state = Util::GetInt2(myInputReport, 1);
			vector<Button> changed = myDevice->setButtons(key_state);
      
			myDevice->_myLastMotion = m;
    }
		else if (0x21 == myInputReport[0]) // reading calibration data
		{
			 Vector3i zero_point(myInputReport[6] - 128, myInputReport[7] - 128, myInputReport[8] - 128);
			 myDevice->_myZeroPoint = zero_point;
			 Vector3i one_g_point(myInputReport[10] - 128, myInputReport[11] - 128, myInputReport[12] - 128);
			 myDevice->_myOneGPoint = one_g_point;
		}
	}

	return 0;
}


bool Win32mote::startListening()
{
	printf("Started listening: %i", m_controller_id);

  
	_myInputListening = true;
	_myListenerThread = (HANDLE)_beginthreadex(
		0, 
		0, 
		InputReportListener, 
		this, 
		0, 
		&_myListenerThreadId); 

	printf("Created thread: %i", _myListenerThreadId);

	return false;
}

    
bool Win32mote::stopListening()
{
	printf("Stopped listening: %i", m_controller_id);

	_myInputListening = false;

	WaitForSingleObject(m_event_object, 300);

	// Needs to clean up thread -- Heap corruption exception?
	//CloseHandle(_myListenerThread);
	_myListenerThread = NULL;

	return true;
}


std::vector<Win32mote>
Win32mote::discover() {
    vector<Win32mote> wiimotes = HIDDevice::ConnectToHIDDevices<Win32mote>();
    if( wiimotes.size() > 0 ) {
        for(unsigned i=0; i<wiimotes.size(); ++i){
            wiimotes[i].startListening();
        }
        cout << "wiimotes found " << wiimotes.size() << endl;
    } else {
        cout << "no wiimotes found " << endl;
    }
    return wiimotes;
}

void Win32mote::close()
{
	if (isListening())
		stopListening();

	CloseHandle(_myListenerThread);
	CloseHandle(m_device_handle);
	CloseHandle(m_read_handle);
	CloseHandle(m_write_handle);
}

}
