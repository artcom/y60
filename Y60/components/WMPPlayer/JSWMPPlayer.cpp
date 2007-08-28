//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSWMPPlayer.cpp,v $
//   $Author: christian $
//   $Revision: 1.12 $
//   $Date: 2005/04/28 17:12:56 $
//
//
//=============================================================================

#include <y60/JSVector.h>
#include <asl/Logger.h>
#include "JSWMPPlayer.h"

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) // x

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    const WMPPlayer & myNative = JSWMPPlayer::getJSWrapper(cx,obj).getNative();
    *rval = as_jsval(cx, "WMPPlayer");
    return JS_TRUE;
}

static JSBool
load(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Load a media file.");
    DOC_PARAM("theURL", "", DOC_TYPE_STRING);
    DOC_END;
    return Method<JSWMPPlayer::NATIVE>::call(&JSWMPPlayer::NATIVE::load,cx,obj,argc,argv,rval);
}

static JSBool
setup(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Setup the player given a parent window. Call this before any usage");
    DOC_PARAM("theParentWindow", "", DOC_TYPE_STRING);
    DOC_END;
    return Method<JSWMPPlayer::NATIVE>::call(&JSWMPPlayer::NATIVE::setup,cx,obj,argc,argv,rval);
}

static JSBool
play(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Starts playback of the movie at a given position" \
              "If no position is given, it starts at the current position");
    DOC_PARAM("theStartPosition", "Start position in Seconds", DOC_TYPE_FLOAT);
    DOC_END;
    try {
        ensureParamCount(argc, 0, 1);

        JSWMPPlayer::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        if (argc == 0) {
            myNative->play();
        } else {
            float myStartPosition;
            convertFrom(cx, argv[0], myStartPosition);
            myNative->play(myStartPosition);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Stops playback of the movie. Current movie position is lost.");
    DOC_END;
    return Method<JSWMPPlayer::NATIVE>::call(&JSWMPPlayer::NATIVE::stop,cx,obj,argc,argv,rval);
}

static JSBool
pause(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Pauses playback of the movie. Current movie position is remembered.");
    DOC_END;
    return Method<JSWMPPlayer::NATIVE>::call(&JSWMPPlayer::NATIVE::pause,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSWMPPlayer::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"load",                 load,                    1},
        {"setup",                setup,                   1},
        {"play",                 play,                    0},
        {"stop",                 stop,                    0},
        {"pause",                pause,                   0},
        {0}
    };
    return myFunctions;
}

JSFunctionSpec *
JSWMPPlayer::StaticFunctions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {0}
    };
    return myFunctions;
}


JSPropertySpec *
JSWMPPlayer::Properties() {
    static JSPropertySpec myProperties[] = {
        {"width",          PROP_width, JSPROP_READONLY | JSPROP_PERMANENT},
        {"height",         PROP_height, JSPROP_READONLY | JSPROP_PERMANENT},
        {"canvasposition", PROP_canvasposition, JSPROP_PERMANENT},
        {"canvassize",     PROP_canvassize, JSPROP_PERMANENT},
        {"state",          PROP_playstate, JSPROP_READONLY | JSPROP_PERMANENT},
        {"volume",         PROP_volume, JSPROP_PERMANENT},
        {"duration",       PROP_duration, JSPROP_PERMANENT},
        {"currentposition",PROP_currentposition, JSPROP_PERMANENT},
        {"visible",        PROP_visible, JSPROP_PERMANENT},
        {"loopcount",      PROP_loopcount, JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSWMPPlayer::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

// getproperty handling
JSBool
JSWMPPlayer::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_width:
            *vp = as_jsval(cx, getNative().getSize()[0]);
            return JS_TRUE;
        case PROP_height:
            *vp = as_jsval(cx, getNative().getSize()[1]);
            return JS_TRUE;
        case PROP_canvasposition:
            *vp = as_jsval(cx, getNative().getCanvasPosition());
            return JS_TRUE;
        case PROP_currentposition:
            *vp = as_jsval(cx, getNative().getCurrentPosition());
            return JS_TRUE;    
        case PROP_canvassize:
            *vp = as_jsval(cx, getNative().getCanvasSize());
            return JS_TRUE;
        case PROP_playstate:
        {
            string myState = getNative().getPlayState();
            *vp = as_jsval(cx, myState);
            return JS_TRUE;
        }
        case PROP_volume:
            *vp = as_jsval(cx, getNative().getVolume());
            return JS_TRUE;
        case PROP_duration:
            *vp = as_jsval(cx, getNative().getDuration());
            return JS_TRUE;
        case PROP_visible:
            *vp = as_jsval(cx, getNative().isVisible());
            return JS_TRUE;
        case PROP_loopcount:
            *vp = as_jsval(cx, getNative().getLoopCount());
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSWMPPlayer::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSWMPPlayer::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_canvasposition:
        {
            Vector2i myPosition;
            convertFrom(cx, *vp, myPosition);
            openNative().setCanvasPosition(myPosition);
            closeNative();
            return JS_TRUE;
        }
        case PROP_canvassize:
        {
            Vector2i mySize;
            convertFrom(cx, *vp, mySize);
            openNative().setCanvasSize(mySize);
            closeNative();
            return JS_TRUE;
        }
        case PROP_volume:
            double myVolume;
            convertFrom(cx, *vp, myVolume);
            openNative().setVolume(myVolume);
            closeNative();
            return JS_TRUE;
        case PROP_currentposition:
            double myCurrentPosition;
            convertFrom(cx, *vp, myCurrentPosition);
            openNative().setCurrentPosition(myCurrentPosition);
            closeNative();
            return JS_TRUE;    
        case PROP_visible:
            bool myVisibilityFlag;
            convertFrom(cx, *vp, myVisibilityFlag);
            openNative().setVisible(myVisibilityFlag);
            closeNative();
            return JS_TRUE;
        case PROP_loopcount:
            unsigned myLoopCount;
            convertFrom(cx, *vp, myLoopCount);
            openNative().setLoopCount(myLoopCount);
            closeNative();
            return JS_TRUE;
        case 0:
        default:
            JS_ReportError(cx,"JSWMPPlayer::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSPropertySpec *
JSWMPPlayer::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSBool
JSWMPPlayer::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("constructs a WMPPlayer instance, nothing more.");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    OWNERPTR myNewNative;
    switch (argc) {
        case 0:
            myNewNative = OWNERPTR(new WMPPlayer());
            break;
        default:
            JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected none or one "
                    "(filepath), got %d", ClassName(), argc);
            return JS_FALSE;
    }

    JSWMPPlayer * myNewObject = new JSWMPPlayer(myNewNative, &(*myNewNative));
    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}

JSObject *
JSWMPPlayer::initClass(JSContext *cx, JSObject *theGlobalObject) {
	AC_DEBUG << "JSWMPPlayer::initClass()";
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
	AC_DEBUG << "Base::initClass done.";
	DOC_MODULE_CREATE("Components", JSWMPPlayer);
	AC_DEBUG << "JSWMPPlayer::initClass done.";
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSWMPPlayer::OWNERPTR & theWMPPlayer) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSWMPPlayer::NATIVE >::Class()) {
                theWMPPlayer = JSClassTraits<JSWMPPlayer::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSWMPPlayer::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSWMPPlayer::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSWMPPlayer::OWNERPTR theOwner, JSWMPPlayer::NATIVE * theWMPPlayer) {
    JSObject * myObject = JSWMPPlayer::Construct(cx, theOwner, theWMPPlayer);
    return OBJECT_TO_JSVAL(myObject);
}

}
