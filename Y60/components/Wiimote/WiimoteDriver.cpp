/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "WiimoteDriver.h"

#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/jsbase/Documentation.h>
#include <y60/input/GenericEvent.h>

using namespace std;
using namespace asl;
using namespace jslib;

extern std::string oureventxsd;

namespace y60 {

WiimoteDriver::WiimoteDriver(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    y60::IEventSource(),
    _myDefaultReportMode ( INFRARED ),
    _myEventSchema( new dom::Document( oureventxsd ) ),
    _myValueFactory( new dom::ValueFactory())
{
    registerStandardTypes( * _myValueFactory );
    registerSomTypes( * _myValueFactory );
}  

WiimoteDriver::~WiimoteDriver() {
    DeviceMap::iterator myIt = _myWiimotes.begin();
    for (; myIt != _myWiimotes.end(); ++myIt) {
        myIt->second->setLEDs(0,0,0,0);
        myIt->second->setRumble(false);
    }
}

y60::EventPtrList 
WiimoteDriver::poll() {
    y60::EventPtrList myEvents;

    std::vector<string> myLostWiiIds;
    DeviceMap::iterator myIt = _myWiimotes.begin();
    for (; myIt != _myWiimotes.end(); ++myIt) {
        myIt->second->pollEvents( myEvents, myLostWiiIds );
    }
    for (unsigned i = 0; i < myLostWiiIds.size(); ++i) {
        //AC_PRINT << "Lost Wii Remote " << myLostWiiIds[i]; 
        _myWiimotes.erase( myLostWiiIds[i] );
    }

    std::vector<WiiRemotePtr> myNewWiis;
    _myScanner.poll( myNewWiis, myLostWiiIds );

    for (unsigned i = 0; i < myNewWiis.size(); ++i) {
        int myWiiNum = _myWiimotes.size() + 1;
        myNewWiis[i]->setLEDs(myWiiNum & (1<<0) , myWiiNum & (1<<1),
                              myWiiNum & (1<<2), myWiiNum & (1<<3));
        myNewWiis[i]->setReportMode( _myDefaultReportMode ); // XXX

        _myWiimotes.insert( std::make_pair( myNewWiis[i]->getControllerID(), myNewWiis[i] ) );
        //AC_PRINT << "Found Wii Remote " << myNewWiis[i]->getControllerID(); 

        y60::GenericEventPtr myEvent( new GenericEvent("onWiiEvent", _myEventSchema,
                    _myValueFactory));
        dom::NodePtr myNode = myEvent->getNode();
        myNode->appendAttribute<string>("id", myNewWiis[i]->getControllerID());
        myNode->appendAttribute<string>("type", std::string("found_wii"));
        myEvents.push_back( myEvent );
    }
    
    return myEvents;
}


WiiRemotePtr
WiimoteDriver::getWiiById( const std::string & theId ) {
    DeviceMap::iterator myIt = _myWiimotes.find( theId );
    if (myIt == _myWiimotes.end()) {
        throw WiiException(string("Unknown id '") + theId + "'", PLUS_FILE_LINE );
    }
    return myIt->second;
}

void
WiimoteDriver::setLEDs(const string &theId, bool led1, bool led2, bool led3, bool led4) {
    getWiiById(theId)->setLEDs( led1, led2, led3, led4);
}

void
WiimoteDriver::setLED(const string & theId, int theLEDIndex, bool theState) {
    getWiiById(theId)->setLED( theLEDIndex, theState);
}


void
WiimoteDriver::setRumble(const string & theId, bool theFlag) {
    getWiiById( theId )->setRumble( theFlag );
}

unsigned
WiimoteDriver::getNumWiimotes() const {
    return _myWiimotes.size(); 
}

void
WiimoteDriver::requestStatusReport(const string & theId) {
    getWiiById( theId )->requestStatusReport();
}

void
WiimoteDriver::requestStatusReport() {
    DeviceMap::iterator myIt( _myWiimotes.begin() );
    for (; myIt != _myWiimotes.end(); ++myIt) {
        myIt->second->requestStatusReport();
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

    string myId;
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

    string myId;
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

    string myId;
    convertFrom(cx, argv[0], myId );

    bool myLED0;
    convertFrom(cx, argv[1], myLED0 );
    bool myLED1;
    convertFrom(cx, argv[2], myLED1 );
    bool myLED2;
    convertFrom(cx, argv[3], myLED2 );
    bool myLED3;
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

    string myId;
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
