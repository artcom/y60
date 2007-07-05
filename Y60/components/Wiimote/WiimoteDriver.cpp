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

using namespace std;
using namespace asl;
using namespace jslib;

namespace y60 {

WiimoteDriver::WiimoteDriver(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    y60::IEventSource(),
    _myDefaultReportMode ( INFRARED )
{
#ifdef WIN32
    _myWiimotes = Win32mote::discover();
#elif defined( LINUX )
    _myWiimotes = Liimote::discover();
#elif defined( OSX )
    // TODO
#endif
    AC_PRINT << "Found " << _myWiimotes.size() << " Wii controller"
             << (_myWiimotes.size() != 1 ? "s." : "." );
    for (unsigned i = 0; i < _myWiimotes.size(); ++i) {

        // XXX set LEDs to controller id (binary) ;-)
        _myWiimotes[i]->setLEDs(i+1 & (1<<0) ,i+1 & (1<<1),i+1 & (1<<2),i+1 & (1<<3));
        _myWiimotes[i]->setReportMode( _myDefaultReportMode ); // XXX
    }

}  

WiimoteDriver::~WiimoteDriver() {
    for (int i = 0; i < _myWiimotes.size(); ++i) {
        _myWiimotes[i]->setLEDs(0,0,0,0);
        _myWiimotes[i]->setRumble(false);
    }
}

y60::EventPtrList 
WiimoteDriver::poll() {
    y60::EventPtrList myEvents;

    std::vector<unsigned> myLostWiiIds;
    for (unsigned i = 0; i < _myWiimotes.size(); ++i) {
        _myWiimotes[i]->pollEvents( myEvents, myLostWiiIds );
    }
    for (unsigned i = 0; i < myLostWiiIds.size(); ++i) {
        AC_PRINT << "Lost connection: " << _myWiimotes[ myLostWiiIds[i] ]->getDeviceName(); 
        _myWiimotes.erase( _myWiimotes.begin() + myLostWiiIds[i] );
    }
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

unsigned
WiimoteDriver::getNumWiimotes() const {
    return _myWiimotes.size(); 
}

void
WiimoteDriver::requestStatusReport(unsigned theId) {
    _myWiimotes[theId]->requestStatusReport();
}

void
WiimoteDriver::requestStatusReport() {
    for (unsigned i = 0; i < _myWiimotes.size(); ++i) {
        _myWiimotes[i]->requestStatusReport();
    }
}

void 
WiimoteDriver::onSetProperty(const string & thePropertyName,
        const PropertyValue & thePropertyValue)
{
}
void
WiimoteDriver::onGetProperty(const string & thePropertyName,
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
    if (argc == 1) {
        convertFrom(cx, argv[0], myId );
    }

    if (myNative) {
        if (argc == 1) {
            myNative->requestStatusReport(myId);
        } else {
            myNative->requestStatusReport();
        }
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
