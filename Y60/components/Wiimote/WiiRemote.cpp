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
#include <asl/Assure.h>

#include <y60/GenericEvent.h>

using namespace std;
using namespace asl;

namespace y60 {

WiiRemote::WiiRemote(PosixThread::WorkFunc theThreadFunction, unsigned theId) : 
    PosixThread(theThreadFunction),
    _myLeftPoint( -1 ),
    _myLowPassedOrientation(0, 1, 0),
    _myOrientation( 0 ),
    _myControllerId( theId ),
    _myRumbleFlag( false ),
    _myInputListening( false ),
    _isConnected( false ),
    _myReportMode( BUTTON ),
    _myContinousReportFlag( false ),
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
            
    registerStandardTypes( * _myValueFactory );
    registerSomTypes( * _myValueFactory );

    startThread();
}

WiiRemote::~WiiRemote() {
}

void 
WiiRemote::dispatchInputReport(const unsigned char * theBuffer, int theOffset) {

    const unsigned char * myAddress = theBuffer + theOffset;
    if (INPUT_REPORT_BUTTONS == theBuffer[theOffset]) {
        _myLock.lock();

        handleButtonEvents( myAddress );

        _myLock.unlock();
    } else if (INPUT_REPORT_MOTION == theBuffer[theOffset]) {
        _myLock.lock();
        handleButtonEvents( myAddress );
        handleMotionEvents( myAddress );
        _myLock.unlock();
    } else if (0x21 == theBuffer[theOffset]) {
        // XXX TODO find out how to use it
        Vector3f zero_point(theBuffer[6] - 128.0f, theBuffer[7] - 128.0f, theBuffer[8] - 128.0f);
        _myZeroPoint = zero_point;
        Vector3f one_g_point(theBuffer[10] - 128.0f, theBuffer[11] - 128.0f, theBuffer[12] - 128.0f);
        _myOneGPoint = one_g_point;

    } else if (INPUT_REPORT_IR == theBuffer[theOffset]) {
        _myLock.lock();
        handleIREvents( myAddress );
        handleButtonEvents( myAddress );
        handleMotionEvents( myAddress );
        _myLock.unlock();
    } else if( INPUT_REPORT_STATUS == theBuffer[theOffset]) {
        _myLock.lock();
        _myEventQueue.push( WiiEvent(_myControllerId, myAddress[6], myAddress[3]));
        _myLock.unlock();
    } else {
        // TODO: handle more response types
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
WiiRemote::createEvent( int theID, const asl::Vector2i theIRData[4],
                        const asl::Vector2f & theNormalizedScreenCoordinates, const float & theAngle ) {
    _myEventQueue.push( WiiEvent( theID, theIRData, theNormalizedScreenCoordinates ) );
}

void
WiiRemote::createEvent( int theID, asl::Vector3f & theMotionData) {
    _myEventQueue.push( WiiEvent(theID, theMotionData) );
}

void
WiiRemote::createEvent( int theID, const std::string & theButtonName, bool thePressedState) {
    _myEventQueue.push( WiiEvent( theID, theButtonName, thePressedState ) );
}

Vector2i
parseIRData( const unsigned char * theBuffer, unsigned theOffset, int & theSizeHint) {
    Vector2i myIRPos(0,0);

    myIRPos[0] = theBuffer[theOffset];
    myIRPos[1] = theBuffer[theOffset + 1];
    myIRPos[1] |= ( (theBuffer[theOffset + 2] >> 6) << 8);
    myIRPos[0] |= ( ((theBuffer[theOffset + 2] & 0x30) >> 4) << 8);

    theSizeHint = theBuffer[theOffset+2] & 0x0F;

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
    }
    
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
WiiRemote::setRumble( bool theFlag) {
    _myRumbleFlag = theFlag;
    // There is no explicit output report to control the rumbler. Instead
    // bit zero of the first payload byte of every output report contains
    // the rumbler state. So we abuse the LED output report to set the 
    // rumble bit.
    setLEDState();
}

bool 
WiiRemote::isRumbling() const {
    return _myRumbleFlag;
}

void 
WiiRemote::setLED(int theIndex, bool theFlag) {
    _myLEDState[ theIndex ] = theFlag;
    setLEDState();
}

void 
WiiRemote::setLEDs(bool theLED0, bool theLED1, bool theLED2, bool theLED3 ) {
    _myLEDState[ 0 ] = theLED0;
    _myLEDState[ 1 ] = theLED1;
    _myLEDState[ 2 ] = theLED2;
    _myLEDState[ 3 ] = theLED3;
    setLEDState();
}

bool
WiiRemote::isLedOn(int i) const {
    return _myLEDState[i];
}

void
WiiRemote::setLEDState() {
    unsigned char myLedReport[2] = { OUT_SET_LEDS, 0x00 };

    myLedReport[1] |= _myLEDState[0] ? 0x10 : 0x00;
    myLedReport[1] |= _myLEDState[1] ? 0x20 : 0x00;
    myLedReport[1] |= _myLEDState[2] ? 0x40 : 0x00;
    myLedReport[1] |= _myLEDState[3] ? 0x80 : 0x00;

    sendOutputReport( myLedReport, 2 );
}

void 
WiiRemote::sendOutputReport(unsigned char theOutputReport[], unsigned theNumBytes) {
    // preserve current state ... add the rumble bit
    theOutputReport[1] |= _myRumbleFlag ? 0x01 : 0x00;

    if ( _isConnected ) {
        send( theOutputReport, theNumBytes );
    }
}

void 
WiiRemote::setContinousReportFlag( bool theFlag ) {
    _myContinousReportFlag = theFlag;
}

bool 
WiiRemote::getContinousReportFlag() const {
    return _myContinousReportFlag;
}

void
WiiRemote::addContinousReportBit( unsigned char * theOutputReport ) {
    ASSURE( theOutputReport[0] == OUT_DATA_REPORT_MODE );
    theOutputReport[1] |= _myContinousReportFlag ? 0x04 : 0x00;
}

void 
WiiRemote::writeMemoryOrRegister(Unsigned32 theAddress, unsigned char * theData,
                                 unsigned theNumBytes, bool theWriteRegisterFlag)
{
    unsigned char myOutputReport[22];
    memset( myOutputReport, 0, 22);

    myOutputReport[0] = OUT_WRITE_DATA;

    myOutputReport[1] |= theWriteRegisterFlag ? 0x04 : 0x00;

    myOutputReport[2] = (theAddress & 0x00ff0000) >> 16;
    myOutputReport[3] = (theAddress & 0x0000ff00) >> 8;
    myOutputReport[4] = (theAddress & 0x000000ff);

    myOutputReport[5] = theNumBytes;

    memcpy( & myOutputReport[6], theData, theNumBytes );

    sendOutputReport( myOutputReport, 16 + 6); // allways send 16 data bytes
}

bool 
WiiRemote::isConnected() const {
    return _isConnected;
}

void 
WiiRemote::disconnect() {
    _isConnected = false;
    _myLock.lock();
    _myEventQueue.push( WiiEvent( _myControllerId, WII_LOST_CONNECTION ));
    _myLock.unlock();
}

void 
WiiRemote::setReportMode( WiiReportMode theReportMode ) {
    switch (theReportMode) {
        case BUTTON:
            requestButtonData();
            break;
        case MOTION:
            requestMotionData();
            break;
        case INFRARED:
            requestInfraredData();
            break;
    }
    _myReportMode = theReportMode;
}

WiiReportMode 
WiiRemote::getReportMode() const {
    return _myReportMode;
}

void
WiiRemote::requestButtonData() {
    unsigned char myReportMode[3] = { OUT_DATA_REPORT_MODE , 0x00, INPUT_REPORT_BUTTONS };
    addContinousReportBit( myReportMode );
    sendOutputReport(myReportMode, 3);
}

void
WiiRemote::requestMotionData() {
    unsigned char myCalibrationReport[7] = { OUT_READ_DATA, 0x00, 0x00, 0x00, 0x16, 0x00, 0x0F };
    sendOutputReport(myCalibrationReport, 7);

    unsigned char myReportMode[3] = { OUT_DATA_REPORT_MODE, 0x00, INPUT_REPORT_MOTION };
    addContinousReportBit( myReportMode );
    sendOutputReport(myReportMode, 3);
}

void
WiiRemote::requestInfraredData() {
    unsigned char myIRCameraEnable[2] = {  OUT_IR_CMAERA_ENABLE, IR_CAMERA_ENABLE_BIT};
    sendOutputReport(myIRCameraEnable, 2);
    msleep(10);

    unsigned char myIRCameraEnable2[2] = { OUT_IR_CMAERA_ENABLE_2, IR_CAMERA_ENABLE_BIT};
    sendOutputReport(myIRCameraEnable2, 2);
    msleep(10);

    // sorry for the unimaginative variable name, but nobody seems to know
    // what this is for [DS]
    unsigned char myUnknownPurposeByte( 0x01 );
    writeMemoryOrRegister( 0xb00030, & myUnknownPurposeByte, 1, true);
    msleep(10);

    unsigned char mySensitivitySetting1[9] = { 0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xaa, 0x00, 0x64 };
    writeMemoryOrRegister( 0xb00000, mySensitivitySetting1, 9, true);
    msleep(10);

    unsigned char mySensitivitySetting2[2] = {0x63, 0x03};
    writeMemoryOrRegister( 0xb0001a, mySensitivitySetting2, 2, true);
    msleep(10);

    myUnknownPurposeByte = 0x08;
    writeMemoryOrRegister( 0xb00030, & myUnknownPurposeByte, 1, true);
    msleep(10);

    unsigned char myIRModeSetting = 0x03;
    writeMemoryOrRegister( 0xb00033, & myIRModeSetting, 1, true);
    msleep(10);

    unsigned char myIRDataFormat[3] = { OUT_DATA_REPORT_MODE, 0x00, INPUT_REPORT_IR};
    addContinousReportBit( myIRDataFormat );
    sendOutputReport(myIRDataFormat, 3);
    msleep(10);
}

void
WiiRemote::requestStatusReport() {
    unsigned char myStatusInformationRequest[2] = { OUT_STATUS_REQUEST, 0x00 };
    sendOutputReport(myStatusInformationRequest, 2);
}

void 
WiiRemote::pollEvents( y60::EventPtrList & theEventList, std::vector<unsigned> & theLostWiiIds ) {
    if ( _myLock.nonblock_lock() == 0) {

        while ( ! _myEventQueue.empty() ) {
            const WiiEvent & myWiiEvent = _myEventQueue.front();
            y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
                        _myValueFactory));
            dom::NodePtr myNode = myEvent->getNode();
            myNode->appendAttribute<int>("id", myWiiEvent.id);
            if (myWiiEvent.type == WII_BUTTON) {

                myNode->appendAttribute("type", "button");
                myNode->appendAttribute<string>("buttonname", myWiiEvent.buttonname);
                myNode->appendAttribute<bool>("pressed", myWiiEvent.pressed);

            } else if (myWiiEvent.type == WII_MOTION ) {
                myNode->appendAttribute<string>("type", string("motiondata"));
                myNode->appendAttribute<Vector3f>("motiondata", myWiiEvent.acceleration);
            } else if (myWiiEvent.type == WII_INFRARED ) {

                myNode->appendAttribute<string>("type", string("infrareddata"));

                for (unsigned i = 0; i < 4; ++i) {
                    if (myWiiEvent.irPositions[i][0] != 1023 && myWiiEvent.irPositions[i][1] != 1023) {
                        myNode->appendAttribute<Vector2i>(string("irposition") + as_string(i),
                                myWiiEvent.irPositions[i]);
                        //myNode->appendAttribute<float>(string("angle"), theAngle);
                    }
                }
                myNode->appendAttribute<Vector2f>(string("screenposition"),
                        myWiiEvent.screenPosition);
            } else if (myWiiEvent.type == WII_LOST_CONNECTION) {
                myNode->appendAttribute<string>("type", string("lost_connection"));
                theLostWiiIds.push_back( myWiiEvent.id );
            } else if (myWiiEvent.type == WII_STATUS) {
                myNode->appendAttribute<string>("type", string("status"));
                myNode->appendAttribute<float>("battery_level", myWiiEvent.batteryLevel);
                myNode->appendAttribute<bool>("extension", myWiiEvent.extensionConnected);
                myNode->appendAttribute<bool>("speaker_enabled", myWiiEvent.speakerEnabled);
                myNode->appendAttribute<bool>("continous_reports", myWiiEvent.continousReports);
                myNode->appendAttribute<bool>("led0", myWiiEvent.leds[0]);
                myNode->appendAttribute<bool>("led1", myWiiEvent.leds[1]);
                myNode->appendAttribute<bool>("led2", myWiiEvent.leds[2]);
                myNode->appendAttribute<bool>("led3", myWiiEvent.leds[3]);
            } else {
                throw WiiException("unhandled event type", PLUS_FILE_LINE);
            }
            theEventList.push_back( myEvent );
            _myEventQueue.pop();
        }

        _myLock.unlock();
    }
}

} // end of namespace 
