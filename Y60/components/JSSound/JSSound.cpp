//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSSound.h"
#include <y60/JScppUtils.h>

using namespace std;
using namespace asl;

namespace jslib {

    static JSBool
    play(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Start playing the sound."); DOC_END;
        return Method<JSSound::NATIVE>::call(&JSSound::NATIVE::play,cx,obj,argc,argv,rval);
    }
    static JSBool
    stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Stop playing the sound."); DOC_END;
        return Method<JSSound::NATIVE>::call(&JSSound::NATIVE::stop,cx,obj,argc,argv,rval);
    }
    static JSBool
    pause(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Pause playing the sound."); DOC_END;
        return Method<JSSound::NATIVE>::call(&JSSound::NATIVE::pause,cx,obj,argc,argv,rval);
    }
    static JSBool
    fadeToVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Fading to a specified volume over a given duration.");
        DOC_PARAM("volume", DOC_TYPE_FLOAT);
        DOC_PARAM("duration", DOC_TYPE_FLOAT);
        DOC_END;
        return Method<JSSound::NATIVE>::call(&JSSound::NATIVE::fadeToVolume,cx,obj,argc,argv,rval);
    }
    static JSBool
    seek(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Seeking to a given position.");
        DOC_PARAM("seek time", DOC_TYPE_FLOAT);
        DOC_END;
        return Method<JSSound::NATIVE>::call(&JSSound::NATIVE::seek,cx,obj,argc,argv,rval);
    }
    static JSBool
    seekRelative(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Seeking a timespan ahead of the current position.");
        DOC_PARAM("seek time", DOC_TYPE_FLOAT);
        DOC_END;
        return Method<JSSound::NATIVE>::call(&JSSound::NATIVE::seekRelative,cx,obj,argc,argv,rval);
    }

    JSFunctionSpec *
    JSSound::Functions() {
        static JSFunctionSpec myFunctions[] = {
            // name                  native            nargs
            {"play",                 play,              0},
            {"stop",                 stop,              0},
            {"pause",                pause,             0},
            {"fadeToVolume",         fadeToVolume,      2},
            {"seek",                 seek,              1},
            {"seekRelative",         seekRelative,      1},
            {0}
        };
        return myFunctions;
    }

    JSPropertySpec *
    JSSound::Properties() {
        static JSPropertySpec myProperties[] = {
            {"volume",    PROP_volume,   JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"looping",   PROP_looping,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"playing",   PROP_playing,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"src",       PROP_src,      JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"duration",  PROP_duration, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"time",      PROP_time,     JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {0}
        };
        return myProperties;
    }

    // getproperty handling
    JSBool
    JSSound::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        switch (theID) {
            case PROP_volume:
                *vp = as_jsval(cx, getNative().getVolume());
                return JS_TRUE;
            case PROP_looping: 
                *vp = as_jsval(cx, getNative().isLooping());
                return JS_TRUE;
            case PROP_playing:
                *vp = as_jsval(cx, getNative().isPlaying());
                return JS_TRUE;
            case PROP_src: 
                *vp = as_jsval(cx, getNative().getName());
                return JS_TRUE;
            case PROP_duration: 
                *vp = as_jsval(cx, getNative().getDuration());
                return JS_TRUE;
            case PROP_time: 
                *vp = as_jsval(cx, getNative().getCurrentTime());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSSound::getProperty: index %d out of range", theID);
                return JS_FALSE;
        }
    }

    // setproperty handling
    JSBool
    JSSound::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        jsval dummy;
        switch (theID) {
            case PROP_volume:                
                return Method<NATIVE>::call(&NATIVE::setVolume, cx, obj, 1, vp, &dummy);
            default:
                JS_ReportError(cx,"JSSound::setPropertySwitch: index %d out of range", theID);
        }
        return JS_FALSE;
    }

    JSBool
    JSSound::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Create a Sound from URI.");
        DOC_PARAM("URI", DOC_TYPE_STRING);
//        DOC_PARAM_OPT("Volume", DOC_TYPE_FLOAT, 1.0);
//        DOC_PARAM_OPT("Seektime", DOC_TYPE_FLOAT, 0.0);
        DOC_PARAM_OPT("Loopflag", DOC_TYPE_BOOLEAN, false);
        DOC_END;
        try {
            if (JSA_GetClass(cx,obj) != Class()) {
                JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
                return JS_FALSE;
            }
        
            if (argc > 4) {
                throw BadArgumentException(string("JSSound::Constructor(): Wrong number of arguments. Got ") +
                    as_string(argc) + ", expected between one and four.", PLUS_FILE_LINE);
            }

            string myURI = "";
            if (argc > 0) {
                if (!convertFrom(cx, argv[0], myURI)) {
                    JS_ReportError(cx, "JSSound::Constructor(): argument #1 must be a string (URI)");
                    return JS_FALSE;
                }
            }

            float myVolume = 1.0;
            if (argc > 1) {
                if (!convertFrom(cx, argv[1], myVolume)) {
                    JS_ReportError(cx, "JSSound::Constructor(): argument #2 must be a double (Volume)");
                    return JS_FALSE;
                }
            }

            double mySeekTime = 0.0;
            if (argc > 2) {
                if (!convertFrom(cx, argv[2], mySeekTime)) {
                    JS_ReportError(cx, "JSSound::Constructor(): argument #3 must be a double (Seek Time)");
                    return JS_FALSE;
                }
            }

            bool myLoopFlag = false;
            if (argc > 3) {
                if (!convertFrom(cx, argv[3], myLoopFlag)) {
                    JS_ReportError(cx, "JSSound::Constructor(): argument #4 must be a boolean (Loop Flag)");
                    return JS_FALSE;
                }
            }

            OWNERPTR myNewNative;
            if (argc == 0) {
                // Construct empty Sound that will be filled by copy Construct()
                myNewNative = OWNERPTR(0);
            } else {
//                HWSampleSinkPtr mySampleSink = Pump::get().createSampleSink(myURI);
                cerr << "Huh?" << endl;
                myNewNative = OWNERPTR(new y60::Sound(myURI, 0, myLoopFlag));
                myNewNative->setSelf(myNewNative);
            }

            JSSound * myNewObject = new JSSound(myNewNative, &(*myNewNative));
            if (myNewObject) {
                JS_SetPrivate(cx, obj, myNewObject);
                return JS_TRUE;
            } else  {
                JS_ReportError(cx,"JSSound::Constructor: bad parameters");
                return JS_FALSE;
            }
        } HANDLE_CPP_EXCEPTION;
    }

    JSConstIntPropertySpec *
    JSSound::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSObject *
    JSSound::initClass(JSContext *cx, JSObject *theGlobalObject) {
        return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    }

    jsval as_jsval(JSContext *cx, JSSound::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSSound::Construct(cx, theOwner, &(*theOwner));
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSSound::OWNERPTR theOwner, JSSound::NATIVE * theNative) {
        JSObject * myObject = JSSound::Construct(cx, theOwner, theNative);
        return OBJECT_TO_JSVAL(myObject);
    }
}

extern "C"
EXPORT asl::PlugInBase * y60JSSound_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::JSSoundPlugIn(myDLHandle);
}
