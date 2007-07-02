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

WiiRemote::WiiRemote(PosixThread::WorkFunc theThreadFunction, unsigned theId) : 
                         PosixThread(theThreadFunction),
                         _myLeftPoint( -1 ),
                         _myLowPassedOrientation(0, 1, 0),
                         _myOrientation( 0 ),
                         _myControllerId( theId )
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

void 
WiiRemote::dispatchInputReport(const unsigned char * theBuffer, int theOffset) {

    const unsigned char * myAddress = theBuffer + theOffset;
    if (INPUT_REPORT_BUTTONS == theBuffer[theOffset]) {
        _myLock->lock();

        handleButtonEvents( myAddress );

        _myLock->unlock();
    } else if (INPUT_REPORT_MOTION == theBuffer[theOffset]) {
        _myLock->lock();
        handleButtonEvents( myAddress );
        handleMotionEvents( myAddress );
        _myLock->unlock();
    } else if (0x21 == theBuffer[theOffset]) {
        // XXX TODO find out how to use it
        Vector3f zero_point(theBuffer[6] - 128.0f, theBuffer[7] - 128.0f, theBuffer[8] - 128.0f);
        _myZeroPoint = zero_point;
        Vector3f one_g_point(theBuffer[10] - 128.0f, theBuffer[11] - 128.0f, theBuffer[12] - 128.0f);
        _myOneGPoint = one_g_point;

    } else if (INPUT_REPORT_IR == theBuffer[theOffset]) {
        //AC_PRINT << "irdatareport";
        
        _myLock->lock();
        handleIREvents( myAddress );
        handleButtonEvents( myAddress );
        handleMotionEvents( myAddress );
        _myLock->unlock();
    } else if( INPUT_REPORT_STATUS == theBuffer[theOffset]) {
        printStatus(myAddress);
    } else {
        //throw WiiException(string("Unknown report") + as_string( int( theBuffer[theOffset])), PLUS_FILE_LINE);
    }

}

Button
WiiRemote::getButton(std::string label) {
    for (int i = 0; i < _myButtons.size(); i++) {
        if (_myButtons[i].getName().compare(label) == 0)
            return _myButtons[i];
    }
    throw WiiException(string("Could not find button with label '") + label + "'",
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
WiiRemote::printStatus(const unsigned char * theBuffer) {
    unsigned batteryByte = 6;
    unsigned infoByte    = 3;
    AC_PRINT << "\n============= status report =============";
    double batteryLevel = (double)theBuffer[batteryByte];
    batteryLevel /= (double)0xC0;
    AC_PRINT << " Battery Level              : " << batteryLevel;
    std::string extension = (theBuffer[infoByte] & 0x02) ? "yes" : "no";
    AC_PRINT << " Extension Controller       : " << extension;
    std::string speaker = (theBuffer[infoByte] & 0x04) ? "yes" : "no";
    AC_PRINT << " Speaker enabled            : " << speaker;
    std::string reporting = (theBuffer[infoByte] & 0x08) ? "yes" : "no";
    AC_PRINT << " Continuous reports enabled : " << reporting;
    std::string led0 = (theBuffer[infoByte] & 0x10) ? "yes" : "no";
    AC_PRINT << " LED 1 enabled              : " << led0;
    std::string led1 = (theBuffer[infoByte] & 0x20) ? "yes" : "no";
    AC_PRINT << " LED 2 enabled              : " << led1;
    std::string led2 = (theBuffer[infoByte] & 0x40) ? "yes" : "no";
    AC_PRINT << " LED 3 enabled              : " << led2;
    std::string led3 = (theBuffer[infoByte] & 0x80) ? "yes" : "no";
    AC_PRINT << " LED 4 enabled              : " << led3;
    AC_PRINT << "=========================================\n";
           
}
    
void
WiiRemote::createEvent( int theID, const asl::Vector2i theIRData[4],
                        const asl::Vector2f & theNormalizedScreenCoordinates, const float & theAngle ) {
    _myEventQueue->push( WiiEvent( theID, theIRData, theNormalizedScreenCoordinates ) );
}

void
WiiRemote::createEvent( int theID, asl::Vector3f & theMotionData) {
    _myEventQueue->push( WiiEvent(theID, theMotionData) );
}

void
WiiRemote::createEvent( int theID, const std::string & theButtonName, bool thePressedState) {
    _myEventQueue->push( WiiEvent( theID, theButtonName, thePressedState ) );
}

Vector2i
parseIRData( const unsigned char * theBuffer, unsigned theOffset, int & theSizeHint) {
    Vector2i myIRPos(0,0);

    myIRPos[0] = ((uint16_t)theBuffer[theOffset+2] & 0x30)<<4 | (uint16_t)theBuffer[theOffset];
    myIRPos[1] = ((uint16_t)theBuffer[theOffset+2] & 0xC0)<<2 |
				                  (uint16_t)theBuffer[theOffset+1];
    theSizeHint = theBuffer[theOffset+2] & 0x0F;

    
    // myIRPos[0] = theBuffer[theOffset];
//     myIRPos[1] = theBuffer[theOffset + 1];
//     myIRPos[1] |= ( (theBuffer[theOffset + 2] >> 6) << 8);
//     myIRPos[0] |= ( ((theBuffer[theOffset + 2] & 0x30) >> 4) << 8);
    
//     int mySizeHint( theBuffer[ theOffset + 2] & 0x0f );

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
    int mySizeHint[4];
    myIRPositions[0] = parseIRData( theInputReport, 6, mySizeHint[0]);
    myIRPositions[1] = parseIRData( theInputReport, 9, mySizeHint[1]);
    myIRPositions[2] = parseIRData( theInputReport, 12, mySizeHint[2]);
    myIRPositions[3] = parseIRData( theInputReport, 15, mySizeHint[3]);

    //AC_PRINT << myIRPositions[0] << "  " << myIRPositions[1] << "  " << myIRPositions[2] << "  " << myIRPositions[3];
    //AC_PRINT << mySizeHint[0] << "  " << mySizeHint[1] << "  " << mySizeHint[2] << "  " << mySizeHint[3];
    float ox(0);
    float oy(0);
    Vector2f myNormalizedScreenCoordinates(0.0f, 0.0f);
    float angle = 0.0;

    
    
    if (mySizeHint[0] < 15 && mySizeHint[1] < 15) {
        int myLeftIndex = _myLeftPoint;
        int myRightIndex;
        if (_myLeftPoint == -1) {
            // found IR cursor
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
        //AC_PRINT << myLeftIndex;
        
        float dx = float( myIRPositions[ myRightIndex ][0] - myIRPositions[ myLeftIndex ][0]);
        float dy = float( myIRPositions[ myRightIndex ][1] - myIRPositions[ myLeftIndex ][1]);
		
        float d = sqrt( dx * dx + dy * dy);
		
		
        // normalized distance between bright spots
        dx /= d;
        dy /= d;
		
        angle = atan2(dy, dx);

        //AC_PRINT << angle;
    
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
        // lost IR cursor
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
    _myInputListening = true;
    fork();
}

    
void
WiiRemote::stopThread() {
    if (isActive()) {
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
