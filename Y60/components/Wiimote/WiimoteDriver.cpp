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

using namespace jslib;

namespace y60 {






WiimoteDriver::WiimoteDriver(asl::DLHandle theDLHandle) :
    asl::PlugInBase(theDLHandle),
    y60::IEventSource(),
    _myEventSchema( new dom::Document( oureventxsd ) ),
    _myValueFactory( new dom::ValueFactory()),
    _myEventQueue( new std::queue<WiiEvent>() ),
    _myLock( new asl::ThreadLock()),
    _myRequestMode ( INFRARED )
{
#ifdef WIN32
    _myIOHandles = Win32mote::discover();
#elif defined( LINUX )
    _myIOHandles = Liimote::discover();
#elif defined( OSX )
    // TODO
#endif
    AC_PRINT << "discover" << _myIOHandles.size();
    for (unsigned i = 0; i < _myIOHandles.size(); ++i) {
        //_myBluetoothMap.insert(std::make_pair(_myIOHandles[i]->getControllerID(),
        //            _myIOHandles[i]->getDeviceName()));
        _myIOHandles[i]->setEventQueue( _myEventQueue, _myLock );
        _myIOHandles[i]->startThread();

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
    for (int i = 0; i < _myIOHandles.size(); ++i) {
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
            myNode->appendAttribute<asl::Vector3f>("motiondata", myWiiEvent.acceleration);
        } else if (myWiiEvent.type == WII_INFRARED ) {

            myNode->appendAttribute<std::string>("type", std::string("infrareddata"));

            for (unsigned i = 0; i < 4; ++i) {
                if (myWiiEvent.irPositions[i][0] != 1023 && myWiiEvent.irPositions[i][1] != 1023) {
                    myNode->appendAttribute<asl::Vector2i>(std::string("irposition") + asl::as_string(i),
                            myWiiEvent.irPositions[i]);
                    //myNode->appendAttribute<float>(string("angle"), theAngle);
                }
            }
            myNode->appendAttribute<asl::Vector2f>(std::string("screenposition"),
                    myWiiEvent.screenPosition);
        } else {
            throw asl::Exception("unhandled event type", PLUS_FILE_LINE);
        }
        myEvents.push_back( myEvent );
        _myEventQueue->pop();
    }

    _myLock->unlock();
    return myEvents;
}

void
WiimoteDriver::requestStatusReport(int theIndex) {
    unsigned char status_report[2] = { 0x15, 0x00 };
    //if( _myRequestMode == INFRARED ) {
    //    rpt[1] |= 0x04;
    //}
    _myIOHandles[theIndex]->sendOutputReport(status_report, 2);
}

void
WiimoteDriver::setLEDs(int theIndex, bool led1, bool led2, bool led3, bool led4) {
    char out = 0;

    if (led1) out |= 0x10;
    if (led2) out |= 0x20;
    if (led3) out |= 0x40;
    if (led4) out |= 0x80;

    unsigned char rpt[2] = { 0x11, out };
    _myIOHandles[theIndex]->sendOutputReport(rpt, 2);
}


void
WiimoteDriver::setRumble(int theIndex, bool on) {
    unsigned char rpt[2] = { 0x13, on ? 0x01 : 0x00 };
    if( _myRequestMode == INFRARED ) {
        rpt[1] |= 0x04;
    }
    _myIOHandles[theIndex]->sendOutputReport(rpt, 2);
}

void
WiimoteDriver::requestButtonData() {
    for (int i = 0; i < _myIOHandles.size(); ++i) {
        unsigned char set_motion_report[3] = { OUTPUT_REPORT_SET , 0x00, INPUT_REPORT_BUTTONS };
        _myIOHandles[i]->sendOutputReport(set_motion_report, 3);
    }
    _myRequestMode = BUTTON;
}

void
WiimoteDriver::requestInfraredData() {
    for (int i = 0; i < _myIOHandles.size(); ++i) {
        unsigned char set_ir_report[2] = {  0x13, 0x04 };
        unsigned char set_ir_report2[2] = { 0x1a, 0x04 };
        set_ir_report[1] |= 0x01;
        _myIOHandles[i]->sendOutputReport(set_ir_report, 2);
        asl::msleep(10);
        _myIOHandles[i]->sendOutputReport(set_ir_report2, 2);
        asl::msleep(10);

        // Write 0x08 to register 0xb00030
        //16 MM FF FF FF SS DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD
        unsigned char write_register[7] = {0x16, 0x04,
            0xb0, 0x00, 0x30, // address
            0x01,             // size
            0x01 };           // data
        _myIOHandles[i]->sendOutputReport( write_register, 7 );
        asl::msleep(10);
        //Write Sensitivity Block 1 to registers at 0xb00000
        //Write Sensitivity Block 2 to registers at 0xb0001a 
        unsigned char sensivity_setting_block1[15] = {0x16, 0x04,
            0xb0, 0x00, 0x00, // address
            0x09,             // size
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0xC0 }; // data
        _myIOHandles[i]->sendOutputReport(sensivity_setting_block1, 15);
        asl::msleep(10);

        unsigned char sensivity_setting_block2[8] = {0x16, 0x04,
            0xb0, 0x00, 0x1a,  // address
            0x02,               // size
            0x40, 0x00 };      // data
        _myIOHandles[i]->sendOutputReport(sensivity_setting_block2, 8);
        asl::msleep(10);

        unsigned char write_register2[7] = {0x16, 0x04,
            0xb0, 0x00, 0x30, // address
            0x01,             // size
            0x08 };           // data
        _myIOHandles[i]->sendOutputReport( write_register2, 7 );
        asl::msleep(10);
        unsigned char ir_mode_setting[7] = { 0x16, 0x04,
            0xb0, 0x00, 0x33, // adress
            0x01,             // size
            0x03};            // data (extended mode ... for now)
        _myIOHandles[i]->sendOutputReport(ir_mode_setting, 7);



        unsigned char set_motion_report[3] = { OUTPUT_REPORT_SET, 0x00, 0x33 };
        _myIOHandles[i]->sendOutputReport(set_motion_report, 3);
    }
    _myRequestMode = INFRARED;

}

void
WiimoteDriver::requestMotionData() {
    for (int i = 0; i < _myIOHandles.size(); ++i) {
        unsigned char calibration_report[7] = { OUTPUT_READ_DATA, 0x00, 0x00, 0x00, 0x16, 0x00, 0x0F };
        _myIOHandles[i]->sendOutputReport(calibration_report, 7);

        unsigned char set_motion_report[3] = { OUTPUT_REPORT_SET, 0x00, INPUT_REPORT_MOTION };
        _myIOHandles[i]->sendOutputReport(set_motion_report, 3);
    }
    _myRequestMode = MOTION;
}

unsigned
WiimoteDriver::getNumWiimotes() const {
    return _myIOHandles.size(); 
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
   
    asl::Ptr<WiimoteDriver> myNative = getNativeAs<WiimoteDriver>(cx, obj);

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
   
    asl::Ptr<WiimoteDriver> myNative = getNativeAs<WiimoteDriver>(cx, obj);

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
   
    asl::Ptr<WiimoteDriver> myNative = getNativeAs<WiimoteDriver>(cx, obj);

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

} // end of namespace y60

extern "C"
EXPORT asl::PlugInBase* Wiimote_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::WiimoteDriver(myDLHandle);
}
