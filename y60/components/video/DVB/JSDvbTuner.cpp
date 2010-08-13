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

#include "JSDvbTuner.h"

#include <asl/base/os_functions.h>
#include <y60/jsbase/JSBlock.h>
#include <asl/dom/Nodes.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSWrapper.impl>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;

#define DB(x) // x

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns information about the DvbTuner."); DOC_END;
    const DvbTuner & myDvbTuner = JSDvbTuner::getJSWrapper(cx,obj).getNative();

    std::string myStatusString = "";
    myStatusString = std::string("\nStatus: is open [videoPID: ") + asl::as_string(myDvbTuner.getVideoPID()) +
        ", audioPID: " + asl::as_string(myDvbTuner.getAudioPID()) +
        ", videoTextPID:" + asl::as_string(myDvbTuner.getVideoTextPID()) +
        "]";

    std::string myStringRep = string("DVB Device: [deviceName: " +
                                     myDvbTuner.getDeviceName() + "]" +
                                     myStatusString );

    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
tuneChannel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set active Channel for the DvbDevice and try to lock onto it.");
    DOC_PARAM("theChannelName", "channel to switch to", DOC_TYPE_STRING);
    DOC_RVAL("True if succesful", DOC_TYPE_BOOLEAN);
    DOC_END;

    return Method<JSDvbTuner::NATIVE>::call(&JSDvbTuner::NATIVE::tuneChannel, cx, obj, argc, argv, rval);
}

static JSBool
startTeleTextDecoder(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
    DOC_BEGIN("Start Teletext decoder.");
    DOC_END;

    return Method<JSDvbTuner::NATIVE>::call(&JSDvbTuner::NATIVE::startTeleTextDecoder, cx, obj, argc, argv, rval);
}

static JSBool
stopTeleTextDecoder(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
    DOC_BEGIN("Stop Teletext decoder.");
    DOC_END;

    return Method<JSDvbTuner::NATIVE>::call(&JSDvbTuner::NATIVE::stopTeleTextDecoder, cx, obj, argc, argv, rval);
}

static JSBool
getPage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
    DOC_BEGIN("Get Teletext page from decoder.");
    DOC_PARAM("thePageNumber", "teletext page number", DOC_TYPE_INTEGER);
    DOC_RVAL("Page content as string", DOC_TYPE_STRING);
    DOC_END;

    return Method<JSDvbTuner::NATIVE>::call(&JSDvbTuner::NATIVE::getPage, cx, obj, argc, argv, rval);
}

static JSBool
deviceAvailable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Check if a certain dvbdevice is currently available.");
    DOC_PARAM("theDeviceName", "the device to check for. example: [/dev/dvb/adapter0]", DOC_TYPE_INTEGER);
    DOC_RVAL("true if device can be used, false otherwise.", DOC_TYPE_STRING);
    DOC_END;

    try {
        string myDeviceString;
        if (argc == 1) {
            if (!convertFrom(cx, argv[0], myDeviceString)) {
                JS_ReportError(cx, "JSDvbTuner::deviceAvailable: argument #1 must be a string");
                return JS_FALSE;
            }
        } else {
            JS_ReportError(cx, "JSDvbTuner::deviceAvailable: need one argument");
            return JS_FALSE;
        }
        *rval = as_jsval(cx, DvbTuner::deviceAvailable(myDeviceString));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSDvbTuner::~JSDvbTuner() {
}

JSFunctionSpec *
JSDvbTuner::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name          native         nargs
        {"toString",             toString,              0},
        {"tuneChannel",          tuneChannel,           1},
        {"startTeleTextDecoder", startTeleTextDecoder,  0},
        {"stopTeleTextDecoder",  stopTeleTextDecoder,   0},
        {"getPage",              getPage,               1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSDvbTuner::Properties() {
    static JSPropertySpec myProperties[] = {
        {"carrierFrequency", PROP_carrierFrequency,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"videoPID",         PROP_videoPID,          JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"audioPID",         PROP_audioPID,          JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"videoTextPID",     PROP_videoTextPID,      JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"deviceName",       PROP_deviceName,        JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"channel",          PROP_channel,           JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSDvbTuner::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSDvbTuner::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        // name             native           nargs
        {"deviceAvailable", deviceAvailable, 1},
        {0}
    };
    return myFunctions;
}

// getproperty handling
JSBool
JSDvbTuner::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
    case PROP_videoPID:
        *vp = as_jsval(cx, getNative().getVideoPID());
        return JS_TRUE;
    case PROP_audioPID:
        *vp = as_jsval(cx, getNative().getAudioPID());
        return JS_TRUE;
    case PROP_videoTextPID:
        *vp = as_jsval(cx, getNative().getVideoTextPID());
        return JS_TRUE;
    case PROP_deviceName:
        *vp = as_jsval(cx, getNative().getDeviceName());
        return JS_TRUE;
    case PROP_channel:
        *vp = as_jsval(cx, getNative().getChannelName());
        return JS_TRUE;
    default:
        JS_ReportError(cx,"JSDvbTuner::getProperty: index %d out of range", theID);
        return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSDvbTuner::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
    default:
        JS_ReportError(cx,"JSDvbTuner::setPropertySwitch: index %d out of range", theID);
        return JS_FALSE;
    }
}

JSBool
JSDvbTuner::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a DvbTuner Device. Call tuneChannel to tune for a certain channel");
    DOC_PARAM_OPT("theDvbDevice", "The dvb device of desire.", DOC_TYPE_STRING, "/dev/dvb/adapter0");
    DOC_END;

    try{
        ensureParamCount(argc, 1, 2);

        if (JSA_GetClass(cx,obj) != Class()) {
            JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
            return JS_FALSE;
        }

        JSDvbTuner * myNewObject = 0;
        string myDeviceName = "/dev/dvb/adapter0";

        dom::NodePtr myConfigurationNode;
        if (!convertFrom(cx, argv[0], myConfigurationNode)) {
            JS_ReportError(cx, "JSDvbTuner::Constructor: argument #1 must be a node (channel configuration node)");
            return JS_FALSE;
        }

        if (argc == 2 ) {
            if (!convertFrom(cx, argv[1], myDeviceName)) {
                JS_ReportError(cx, "JSDvbTuner::Constructor: argument #2 must be a string (dvb device name [/dev/dvb/adapter0])");
                return JS_FALSE;
            }
        }

        OWNERPTR myDvbTuner = OWNERPTR(new DvbTuner(myConfigurationNode, myDeviceName));
        myNewObject = new JSDvbTuner(myDvbTuner, myDvbTuner.get());

        if (myNewObject) {
            JS_SetPrivate(cx, obj, myNewObject);
            return JS_TRUE;
        }

        JS_ReportError(cx,"JSDvbTuner::Constructor: bad parameters");

        return JS_FALSE;
    } HANDLE_CPP_EXCEPTION;
}

JSConstIntPropertySpec *
JSDvbTuner::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

JSObject *
JSDvbTuner::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties(), 0, StaticFunctions());
    DOC_MODULE_CREATE("DVB", JSDvbTuner);
    return myClass;
}

jsval as_jsval(JSContext *cx, JSDvbTuner::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSDvbTuner::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval(JSContext *cx, JSDvbTuner::OWNERPTR theOwner, JSDvbTuner::NATIVE * theSerial) {
    JSObject * myObject = JSDvbTuner::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}
