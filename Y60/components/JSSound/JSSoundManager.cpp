//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSSoundManager.h"
#include "JSSound.h"
#include <y60/JScppUtils.h>

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

    static JSBool
    stopAll(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Stop playing the sound."); DOC_END;
        return Method<JSSoundManager::NATIVE>::call(&JSSoundManager::NATIVE::stopAll,
                cx,obj,argc,argv,rval);
    }

    static JSBool
    fadeToVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Fading to a specified volume over a given duration.");
        DOC_PARAM("theVolume", "", DOC_TYPE_FLOAT);
        DOC_PARAM("theDuration", "", DOC_TYPE_FLOAT);
        DOC_END;
        return Method<JSSoundManager::NATIVE>::call(&JSSoundManager::NATIVE::fadeToVolume,
                cx,obj,argc,argv,rval);
    }

    static JSBool
    preloadSound(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Loads and decodes the sound given and stores it in the cache. Invalid for streams.");
        DOC_PARAM("theURI", "", DOC_TYPE_STRING);
        DOC_END;
        return Method<JSSoundManager::NATIVE>::call(&JSSoundManager::NATIVE::preloadSound,
                cx,obj,argc,argv,rval);
    }

    JSFunctionSpec *
    JSSoundManager::Functions() {
        static JSFunctionSpec myFunctions[] = {
            // name                  native            nargs
            {"stopAll",              stopAll,           0},
            {"fadeToVolume",         fadeToVolume,      2},
            {"preloadSound",         preloadSound,      1},
            {0}
        };
        return myFunctions;
    }

    JSPropertySpec *
    JSSoundManager::Properties() {
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
    JSSoundManager::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
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
                JS_ReportError(cx,"JSSoundManager::getProperty: index %d out of range", theID);
                return JS_FALSE;
        }
    }

    // setproperty handling
    JSBool
    JSSoundManager::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        jsval dummy;
        switch (theID) {
            case PROP_volume:
                return Method<NATIVE>::call(&NATIVE::setVolume, cx, obj, 1, vp, &dummy);
            default:
                JS_ReportError(cx,"JSSoundManager::setPropertySwitch: index %d out of range", theID);
        }
        return JS_FALSE;
    }

    JSBool
    JSSoundManager::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Create a SoundManager.");
        DOC_PARAM_OPT("theSamplerate", "", DOC_TYPE_INTEGER, 44100);
        DOC_PARAM_OPT("theOutputChannels", "", DOC_TYPE_INTEGER, 2);
        DOC_PARAM_OPT("theLatency", "", DOC_TYPE_FLOAT, 0.2);
        DOC_END;
        try {
            checkForUndefinedArguments("JSSoundManager::Constructor()", argc, argv);

            if (argc > 3) {
                throw BadArgumentException(string("JSSoundManager::Constructor(): Wrong number of arguments. Got ") +
                    as_string(argc) + ", expected between none and three.", PLUS_FILE_LINE);
            }

            OWNERPTR myNewNative = &(Singleton<SoundManager>::get());
            int mySampleRate = 0;
            if (argc > 0) {
                if (!convertFrom(cx, argv[0], mySampleRate)) {
                    JS_ReportError(cx, "JSSoundManager::Constructor(): argument #1 must be an integer (Samplerate)");
                    return JS_FALSE;
                }

                if (argc > 1) {
                    int myNumOutputChannels = 0;
                    if (!convertFrom(cx, argv[1], myNumOutputChannels)) {
                        JS_ReportError(cx, "JSSoundManager::Constructor(): argument #1 must be an integer (NumOutputChannels)");
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
                    JS_ReportError(cx, "JSSoundManager::Constructor(): argument #2 must be a double (Latency)");
                    return JS_FALSE;
                }
                myNewNative->setSysConfig(myLatency);
            }


            JSSoundManager * myNewObject = new JSSoundManager(myNewNative, &*myNewNative);
            if (myNewObject) {
                JS_SetPrivate(cx, obj, myNewObject);
                return JS_TRUE;
            } else  {
                JS_ReportError(cx,"JSSoundManager::Constructor: bad parameters");
                return JS_FALSE;
            }
            return JS_TRUE;

        } HANDLE_CPP_EXCEPTION;

    }

    JSConstIntPropertySpec *
    JSSoundManager::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSObject *
    JSSoundManager::initClass(JSContext *cx, JSObject *theGlobalObject) {
        return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    }

    jsval as_jsval(JSContext *cx, JSSoundManager::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSSoundManager::Construct(cx, theOwner, &(*theOwner));
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSSoundManager::OWNERPTR theOwner, JSSoundManager::NATIVE * theNative) {
        JSObject * myObject = JSSoundManager::Construct(cx, theOwner, theNative);
        return OBJECT_TO_JSVAL(myObject);
    }
}

