//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSDvbTuner.h"

#include <asl/os_functions.h>
#include <y60/JSBlock.h>
#include <dom/Nodes.h>
#include <y60/JSNode.h>

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
    jsval dummy;
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
        myNewObject = new JSDvbTuner(myDvbTuner, &(*myDvbTuner));
    
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
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_MODULE_CREATE("Components", JSDvbTuner);
    return myClass;
}

jsval as_jsval(JSContext *cx, JSDvbTuner::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSDvbTuner::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval(JSContext *cx, JSDvbTuner::OWNERPTR theOwner, JSDvbTuner::NATIVE * theSerial) {
    JSObject * myObject = JSDvbTuner::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}
