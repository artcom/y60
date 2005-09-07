//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSMedia.h"
#include "JSSound.h"
#include <y60/JScppUtils.h>

using namespace std;
using namespace asl;

namespace jslib {

    static JSBool
    stopAll(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Stop playing the sound."); DOC_END;
        return Method<JSMedia::NATIVE>::call(&JSMedia::NATIVE::stopAll,cx,obj,argc,argv,rval);
    }
    static JSBool
    fadeToVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Fading to a specified volume over a given duration.");
        DOC_PARAM("volume", DOC_TYPE_FLOAT);
        DOC_PARAM("duration", DOC_TYPE_FLOAT);
        DOC_END;
        return Method<JSMedia::NATIVE>::call(&JSMedia::NATIVE::fadeToVolume,cx,obj,argc,argv,rval);
    }
    static JSBool
    createSound(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Creates a Sound from URI. Does not start playback.");
        DOC_PARAM("URI", DOC_TYPE_STRING);
//        DOC_PARAM_OPT("Volume", DOC_TYPE_FLOAT, 1.0);
//        DOC_PARAM_OPT("Seektime", DOC_TYPE_FLOAT, 0.0);
        DOC_PARAM_OPT("Loopflag", DOC_TYPE_BOOLEAN, false);
        DOC_END;
        switch(argc) {
            case 1: {
                    typedef y60::SoundPtr (y60::Media::*MyMethod)(const std::string & theURI);
                    return Method<JSMedia::NATIVE>::call
                            ((MyMethod)&JSMedia::NATIVE::createSound,cx,obj,argc,argv,rval);
                }
            case 2: {
                    typedef y60::SoundPtr (y60::Media::*MyMethod)(const std::string & theURI, bool theLoop);
                    return Method<JSMedia::NATIVE>::call
                            ((MyMethod)&JSMedia::NATIVE::createSound,cx,obj,argc,argv,rval);
                }
            default: {
                    typedef y60::SoundPtr (y60::Media::*MyMethod)(const std::string & theURI, bool theLoop, 
                            const std::string & theName);
                    return Method<JSMedia::NATIVE>::call
                            ((MyMethod)&JSMedia::NATIVE::createSound,cx,obj,argc,argv,rval);
                }
        }
    }

    JSFunctionSpec *
    JSMedia::Functions() {
        static JSFunctionSpec myFunctions[] = {
            // name                  native            nargs
            {"stopAll",              stopAll,           0},
            {"fadeToVolume",         fadeToVolume,      2},
            {"createSound" ,         createSound,       4},
            {0}
        };
        return myFunctions;
    }

    JSPropertySpec *
    JSMedia::Properties() {
        static JSPropertySpec myProperties[] = {
            {"volume",       PROP_volume, JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"running",      PROP_running,      JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"soundcount",   PROP_soundcount,   JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {0}
        };
        return myProperties;
    }

    // getproperty handling
    JSBool
    JSMedia::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        switch (theID) {
            case PROP_volume:
                *vp = as_jsval(cx, getNative().getVolume());
                return JS_TRUE;
            case PROP_running:
                *vp = as_jsval(cx, getNative().isRunning());
                return JS_TRUE;
            case PROP_soundcount: 
                *vp = as_jsval(cx, getNative().getNumSounds());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSMedia::getProperty: index %d out of range", theID);
                return JS_FALSE;
        }
    }

    // setproperty handling
    JSBool
    JSMedia::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        jsval dummy;
        switch (theID) {
            case PROP_volume:                
                return Method<NATIVE>::call(&NATIVE::setVolume, cx, obj, 1, vp, &dummy);
            default:
                JS_ReportError(cx,"JSMedia::setPropertySwitch: index %d out of range", theID);
        }
        return JS_FALSE;
    }

    JSBool
    JSMedia::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Create a Sound from URI.");
        DOC_PARAM_OPT("Samplerate", DOC_TYPE_INTEGER, 44100);
        DOC_PARAM_OPT("OutputChannels", DOC_TYPE_INTEGER, 2);    
        DOC_PARAM_OPT("Latency", DOC_TYPE_FLOAT, 0.2);    
        DOC_END;
        try {
            checkForUndefinedArguments("JSAudio2::init()", argc, argv);

            if (argc > 3) {
                throw BadArgumentException(string("JSAudio2::init(): Wrong number of arguments. Got ") +
                    as_string(argc) + ", expected between none and two.", PLUS_FILE_LINE);
            }

            OWNERPTR myNewNative = OWNERPTR(new y60::Media());
            int mySampleRate = 0;
            if (argc > 0) {
                if (!convertFrom(cx, argv[0], mySampleRate)) {
                    JS_ReportError(cx, "JSMedia::Constructor(): argument #1 must be an integer (Samplerate)");
                    return JS_FALSE;
                }

                if (argc > 1) {
                    int myNumOutputChannels = 0;
                    if (!convertFrom(cx, argv[1], myNumOutputChannels)) {
                        JS_ReportError(cx, "JSMedia::Constructor(): argument #1 must be an integer (NumOutputChannels)");
                        return JS_FALSE;
                    }
                    myNewNative->setAppConfig(mySampleRate, myNumOutputChannels);        
                } else {
                    myNewNative->setAppConfig(mySampleRate);
                }
            }

            double myLatency = -1.0;
            if (argc == 3) {
                if (!convertFrom(cx, argv[2], myLatency)) {
                    JS_ReportError(cx, "JSMedia::Constructor(): argument #2 must be a double (Latency)");
                    return JS_FALSE;
                }
                myNewNative->setSysConfig(myLatency);
            }


            JSMedia * myNewObject = new JSMedia(myNewNative, &*myNewNative);
            if (myNewObject) {
                JS_SetPrivate(cx, obj, myNewObject);
                return JS_TRUE;
            } else  {
                JS_ReportError(cx,"JSMedia::Constructor: bad parameters");
                return JS_FALSE;
            }
            return JS_TRUE;

        } HANDLE_CPP_EXCEPTION;
    
    }

    JSConstIntPropertySpec *
    JSMedia::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSObject *
    JSMedia::initClass(JSContext *cx, JSObject *theGlobalObject) {
        return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    }

    jsval as_jsval(JSContext *cx, JSMedia::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSMedia::Construct(cx, theOwner, &(*theOwner));
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSMedia::OWNERPTR theOwner, JSMedia::NATIVE * theNative) {
        JSObject * myObject = JSMedia::Construct(cx, theOwner, theNative);
        return OBJECT_TO_JSVAL(myObject);
    }
}

