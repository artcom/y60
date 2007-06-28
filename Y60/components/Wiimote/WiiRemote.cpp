//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "WiiRemote.h"

#include <asl/Logger.h>

using namespace std;
using namespace asl;

namespace y60 {

WiiRemote::WiiRemote(PosixThread::WorkFunc theThreadFunction) : //HIDDevice(),
                         PosixThread(theThreadFunction),
//                         _myEventObject(NULL),
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
}

WiiRemote::~WiiRemote() {
}

Button
WiiRemote::getButton(std::string label) {
    for (int i = 0; i < _myButtons.size(); i++) {
        if (_myButtons[i].getName().compare(label) == 0)
            return _myButtons[i];
    }
    throw asl::Exception(string("Could not find button with label '") + label + "'",
    										 PLUS_FILE_LINE);
}
    
    
std::vector<Button>
WiiRemote::setButtons(int code) {
    std::vector<Button> changed_state;
    for (int i = 0; i < _myButtons.size(); i++) {
        if (_myButtons[i].setCode(code))
            changed_state.push_back(_myButtons[i]);
    }
    
    return changed_state;
}

    
void
WiiRemote::createEvent( int theID, const asl::Vector2i theIRData[4],
                        const asl::Vector2f & theNormalizedScreenCoordinates, const float & theAngle ) {
    /*
    y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    
    myNode->appendAttribute<int>("id", theID);
    myNode->appendAttribute<std::string>("type", string("infrareddata"));
    
    bool myGotDataFlag = false;
    for (unsigned i = 0; i < 4; ++i) {
        if (theIRData[i][0] != 1023 && theIRData[i][1] != 1023) {
            myNode->appendAttribute<Vector2i>(string("irposition") + asl::as_string(i), theIRData[i]);
            myNode->appendAttribute<Vector2f>(string("screenposition"), theNormalizedScreenCoordinates);
            myNode->appendAttribute<float>(string("angle"), theAngle);
            myGotDataFlag = true;
            break;

        }
    }
    */


    _myEventQueue->push( WiiEvent( theID, theIRData, theNormalizedScreenCoordinates ) );
}

void
WiiRemote::createEvent( int theID, asl::Vector3f & theMotionData) {
    /*
    y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    
    myNode->appendAttribute<int>("id", theID);
    myNode->appendAttribute<std::string>("type", string("motiondata"));
    myNode->appendAttribute<Vector3f>("motiondata", theMotionData);
    */
    
    _myEventQueue->push( WiiEvent(theID, theMotionData) );
}

void
WiiRemote::createEvent( int theID, std::string & theButtonName, bool thePressedState)
{
    
    /*
    y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
                                                   _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    
    
    myNode->appendAttribute<int>("id", theID);
    myNode->appendAttribute("type", "button");
    myNode->appendAttribute<std::string>("buttonname", theButtonName);
    myNode->appendAttribute<bool>("pressed", thePressedState);
    */
       
    _myEventQueue->push( WiiEvent( theID, theButtonName, thePressedState ) );
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
WiiRemote::handleButtonEvents( const unsigned char * theInputReport ) {
    int key_state = Util::GetInt2(theInputReport, 1);
    vector<Button> myChangedButtons = setButtons(key_state);
    for( unsigned i=0; i < myChangedButtons.size(); ++i) {
        Button myButton = myChangedButtons[i];
        createEvent( _myControllerId, myButton.getName(), myButton.pressed());
    }
}

void
WiiRemote::handleMotionEvents( const unsigned char * theInputReport ) {
    // swizzle vector from xzy-order to y60-order (xyz)
    Vector3f m(theInputReport[3] - 128.0f, theInputReport[5] - 128.0f,  - (theInputReport[4] - 128.0f) );
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

    createEvent( _myControllerId, m);
}

void
WiiRemote::handleIREvents( const unsigned char * theInputReport ) {
    Vector2i myIRPositions[4];
    myIRPositions[0] = parseIRData( theInputReport, 6);
    myIRPositions[1] = parseIRData( theInputReport, 9);
    myIRPositions[2] = parseIRData( theInputReport, 12);
    myIRPositions[3] = parseIRData( theInputReport, 15);

    float ox(0);
    float oy(0);
    Vector2f myNormalizedScreenCoordinates(0.0f, 0.0f);
    float angle = 0.0;
    
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
        
		float dx = float( myIRPositions[ myRightIndex ][0] - myIRPositions[ myLeftIndex ][0]);
		float dy = float( myIRPositions[ myRightIndex ][1] - myIRPositions[ myLeftIndex ][1]);
		
		float d = sqrt( dx * dx + dy * dy);
		
		
		// normalized distance between bright spots
		dx /= d;
		dy /= d;
		
		angle = atan2(dy, dx);

    //AC_PRINT << angle-3.14159;
    
		float cx = float( myIRPositions[ myLeftIndex ][0] + myIRPositions[ myRightIndex ][0]) / 1024.0f - 1.0f;
		float cy = float( myIRPositions[ myLeftIndex ][1] + myIRPositions[ myRightIndex ][1]) / 1024.0f - .75f;
		
		ox = -dy * cy - dx * cx;
		oy = -dx * cy + dy * cx;

    myNormalizedScreenCoordinates[0] = ox;
    myNormalizedScreenCoordinates[1] = oy;
    
    //AC_PRINT << "x: " << ox << " y: " << oy;

    } else {
        // not tracking anything
        ox = oy = -100;
        if ( _myLeftPoint != -1) {
            _myLeftPoint = -1;
        }
        //myNormalizedScreenCoordinates[0] = ox;
        //myNormalizedScreenCoordinates[1] = oy;
    }
    
    //cout << "pos " << myIRPositions[0] << endl;
    createEvent( _myControllerId, myIRPositions, myNormalizedScreenCoordinates, angle );
}


void 
WiiRemote::startThread() {
    AC_PRINT << "Starting thread: " << _myControllerId;
    _myInputListening = true;
    fork();
}

    
void
WiiRemote::stopThread() {
    if (isActive()) {
        AC_PRINT << "Stopped thread: " << _myControllerId;
        _myInputListening = false;
        join();
    }

    closeDevice();
}

bool 
WiiRemote::getListeningFlag() const {
    return _myInputListening;
}

void
WiiRemote::setEventQueue( asl::Ptr<std::queue<WiiEvent> > theQueue,
               asl::Ptr<asl::ThreadLock> theLock)
{
    _myLock = theLock;
    _myEventQueue = theQueue;
}


} // end of namespace 
