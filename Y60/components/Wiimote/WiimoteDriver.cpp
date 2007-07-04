//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "WiimoteDriver.h"

#include <y60/JSScriptablePlugin.h>
#include <y60/Documentation.h>

using namespace asl;
using namespace jslib;

namespace y60 {

WiimoteDriver::WiimoteDriver(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    y60::IEventSource(),
    _myEventSchema( new dom::Document( oureventxsd ) ),
    _myValueFactory( new dom::ValueFactory()),
    _myEventQueue( new std::queue<WiiEvent>() ),
    _myLock( new ThreadLock()),
    _myRequestMode ( INFRARED )
{
#ifdef WIN32
    _myWiimotes = Win32mote::discover();
#elif defined( LINUX )
    _myWiimotes = Liimote::discover();
#elif defined( OSX )
    // TODO
#endif
    AC_PRINT << "Found " << _myWiimotes.size() << " Wii controller" << (_myWiimotes.size() != 1 ? "s" : "" );
    for (unsigned i = 0; i < _myWiimotes.size(); ++i) {
        //_myBluetoothMap.insert(std::make_pair(_myWiimotes[i]->getControllerID(),
        //            _myWiimotes[i]->getDeviceName()));
        _myWiimotes[i]->setEventQueue( _myEventQueue, _myLock );
        _myWiimotes[i]->startThread();

        // XXX set LEDs to controller id (binary) ;-)
        setLEDs(i, i+1 & (1<<0) ,i+1 & (1<<1),i+1 & (1<<2),i+1 & (1<<3));
    }

    registerStandardTypes( * _myValueFactory );
    registerSomTypes( * _myValueFactory );

    //requestMotionData();
    //requestButtonData();
    requestInfraredData();
}  

WiimoteDriver::~WiimoteDriver() {
    for (int i = 0; i < _myWiimotes.size(); ++i) {
        setLEDs(i, 0,0,0,0);
        setRumble(i, false);
    }
}

y60::EventPtrList 
WiimoteDriver::poll() {
    y60::EventPtrList myEvents;

    _myLock->lock();

    while ( ! _myEventQueue->empty() ) {
        const WiiEvent & myWiiEvent = _myEventQueue->front();
        y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
                    _myValueFactory));
        dom::NodePtr myNode = myEvent->getNode();
        myNode->appendAttribute<int>("id", myWiiEvent.id);
        if (myWiiEvent.type == WII_BUTTON) {

            myNode->appendAttribute("type", "button");
            myNode->appendAttribute<std::string>("buttonname", myWiiEvent.buttonname);
            myNode->appendAttribute<bool>("pressed", myWiiEvent.pressed);

        } else if (myWiiEvent.type == WII_MOTION ) {
            myNode->appendAttribute<std::string>("type", std::string("motiondata"));
            myNode->appendAttribute<Vector3f>("motiondata", myWiiEvent.acceleration);
        } else if (myWiiEvent.type == WII_INFRARED ) {

            myNode->appendAttribute<std::string>("type", std::string("infrareddata"));
            
            for (unsigned i = 0; i < 4; ++i) {
                if (myWiiEvent.irPositions[i][0] != 1023 && myWiiEvent.irPositions[i][1] != 1023) {
                    myNode->appendAttribute<Vector2i>(std::string("irposition") + as_string(i),
                            myWiiEvent.irPositions[i]);
                    //myNode->appendAttribute<float>(string("angle"), theAngle);
                }
            }
            myNode->appendAttribute<Vector2f>(std::string("screenposition"),
                    myWiiEvent.screenPosition);
        } else {
            throw WiiException("unhandled event type", PLUS_FILE_LINE);
        }
        myEvents.push_back( myEvent );
        _myEventQueue->pop();
    }

    _myLock->unlock();
    return myEvents;
}

void
WiimoteDriver::setLEDs(int theIndex, bool led1, bool led2, bool led3, bool led4) {
    _myWiimotes[ theIndex ]->setLEDs( led1, led2, led3, led4);
}

void
WiimoteDriver::setLED(int theWiimoteIndex, int theLEDIndex, bool theState) {
    _myWiimotes[ theWiimoteIndex ]->setLED( theLEDIndex, theState);
}


void
WiimoteDriver::setRumble(int theIndex, bool theFlag) {
    _myWiimotes[theIndex]->setRumble( theFlag );
}

void
WiimoteDriver::requestStatusReport(int theIndex) {
    unsigned char status_report[2] = { 0x15, 0x00 };
    _myWiimotes[theIndex]->sendOutputReport(status_report, 2);
}

void
WiimoteDriver::requestButtonData() {
    for (int i = 0; i < _myWiimotes.size(); ++i) {
        unsigned char set_motion_report[3] = { OUT_DATA_REPORT_MODE , 0x00, INPUT_REPORT_BUTTONS };
        _myWiimotes[i]->sendOutputReport(set_motion_report, 3);
    }
    _myRequestMode = BUTTON;
}

void
WiimoteDriver::requestMotionData() {
    for (int i = 0; i < _myWiimotes.size(); ++i) {
        unsigned char calibration_report[7] = { OUT_READ_DATA, 0x00, 0x00, 0x00, 0x16, 0x00, 0x0F };
        _myWiimotes[i]->sendOutputReport(calibration_report, 7);

        unsigned char set_motion_report[3] = { OUT_DATA_REPORT_MODE, 0x00, INPUT_REPORT_MOTION };
        _myWiimotes[i]->sendOutputReport(set_motion_report, 3);
    }
    _myRequestMode = MOTION;
}

void
WiimoteDriver::requestInfraredData() {
    for (int i = 0; i < _myWiimotes.size(); ++i) {
        unsigned char myIRCameraEnable[2] = {  OUT_IR_CMAERA_ENABLE, IR_CAMERA_ENABLE_BIT};
        _myWiimotes[i]->sendOutputReport(myIRCameraEnable, 2);

        unsigned char myIRCameraEnable2[2] = { OUT_IR_CMAERA_ENABLE_2, IR_CAMERA_ENABLE_BIT};
        _myWiimotes[i]->sendOutputReport(myIRCameraEnable2, 2);

        // sorry for the unimaginative variable name, but nobody seems to know
        // what this is for [DS]
        unsigned char myUnknownPurposeByte( 0x01 );
        _myWiimotes[i]->writeMemoryOrRegister( 0xb00030, & myUnknownPurposeByte, 1, true);

        unsigned char mySensitivitySetting1[9] = { 0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xaa, 0x00, 0x64 };
        _myWiimotes[i]->writeMemoryOrRegister(0xb00000, mySensitivitySetting1, 9, true);

        unsigned char mySensitivitySetting2[2] = {0x63, 0x03};
        _myWiimotes[i]->writeMemoryOrRegister(0xb0001a, mySensitivitySetting2, 2, true);

        unsigned char myUnknownPurposeByte = 0x08;
        _myWiimotes[i]->writeMemoryOrRegister( 0xb00030, & myUnknownPurposeByte, 1, true);

        unsigned char myIRModeSetting = 0x03;
        _myWiimotes[i]->writeMemoryOrRegister( 0xb00033, & myIRModeSetting, 1, true);
        
        unsigned char myIRDataFormat[3] = { OUT_DATA_REPORT_MODE, 0x00, INPUT_REPORT_IR};
        _myWiimotes[i]->sendOutputReport(myIRDataFormat, 3);
    }
    _myRequestMode = INFRARED;

}

unsigned
WiimoteDriver::getNumWiimotes() const {
    return _myWiimotes.size(); 
}

void 
WiimoteDriver::onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue)
{
}
void
WiimoteDriver::onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const
{
}
void
WiimoteDriver::onUpdateSettings(dom::NodePtr theSettings) {
}

JSFunctionSpec * 
WiimoteDriver::Functions() {
    static JSFunctionSpec myFunctions[] = {
        {"setRumble", SetRumble, 2},
        {"setLEDs", SetLEDs, 5},
        {"setLED", SetLED, 3},
        {"requestStatusReport", RequestStatusReport, 1},
        {0}
    };
    return myFunctions;
}

const char * 
WiimoteDriver::ClassName() {
    static const char * myClassName = "Wiimote";
    return myClassName;
}

JSBool
WiimoteDriver::SetRumble(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
    DOC_BEGIN("");
    DOC_END;
   
    Ptr<WiimoteDriver> myNative = getNativeAs<WiimoteDriver>(cx, obj);

    int myId;
    convertFrom(cx, argv[0], myId );

    bool myFlag;
    convertFrom(cx, argv[1], myFlag );

    if (myNative) {
        myNative->setRumble(myId, myFlag);
    } else {
        assert(myNative);
    }
    return JS_TRUE;
}


JSBool
WiimoteDriver::RequestStatusReport(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
    DOC_BEGIN("");
    DOC_END;
   
    Ptr<WiimoteDriver> myNative = getNativeAs<WiimoteDriver>(cx, obj);

    int myId;
    convertFrom(cx, argv[0], myId );

    if (myNative) {
        myNative->requestStatusReport(myId);
    } else {
        assert(myNative);
    }
    return JS_TRUE;
}

    
JSBool
WiimoteDriver::SetLEDs(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
    DOC_BEGIN("");
    DOC_END;
   
    Ptr<WiimoteDriver> myNative = getNativeAs<WiimoteDriver>(cx, obj);

    int myId;
    convertFrom(cx, argv[0], myId );

    int myLED0;
    convertFrom(cx, argv[1], myLED0 );
    int myLED1;
    convertFrom(cx, argv[2], myLED1 );
    int myLED2;
    convertFrom(cx, argv[3], myLED2 );
    int myLED3;
    convertFrom(cx, argv[4], myLED3 );

    if (myNative) {
        myNative->setLEDs(myId, myLED0, myLED1, myLED2, myLED3);
    } else {
        assert(myNative);
    }
    return JS_TRUE;
}

JSBool
WiimoteDriver::SetLED(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
    DOC_BEGIN("");
    DOC_END;
   
    Ptr<WiimoteDriver> myNative = getNativeAs<WiimoteDriver>(cx, obj);

    int myId;
    convertFrom(cx, argv[0], myId );

    int myLEDIndex;
    convertFrom(cx, argv[1], myLEDIndex );

    bool myState;
    convertFrom(cx, argv[2], myState );

    if (myNative) {
        myNative->setLED(myId, myLEDIndex, myState);
    } else {
        assert(myNative);
    }
    return JS_TRUE;
}

} // end of namespace y60

extern "C"
EXPORT PlugInBase* Wiimote_instantiatePlugIn(DLHandle myDLHandle) {
    return new y60::WiimoteDriver(myDLHandle);
}
