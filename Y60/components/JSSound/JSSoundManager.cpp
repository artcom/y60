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
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

    template class JSWrapper<y60::SoundManager, 
            y60::SoundManager *, jslib::StaticAccessProtocol>;

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
        DOC_END;
        try {
            checkForUndefinedArguments("JSSoundManager::Constructor()", argc, argv);

            if (argc > 0) {
                throw BadArgumentException(string("JSSoundManager::Constructor(): Wrong number of arguments. Got ") +
                    as_string(argc) + ", expected between none.", PLUS_FILE_LINE);
            }

            OWNERPTR myNewNative = &(Singleton<SoundManager>::get());

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
    
    JSPropertySpec *
    JSSoundManager::StaticProperties() {
            static JSPropertySpec myProperties[] = {{0}};
            return myProperties;
        }

    JSFunctionSpec *
    JSSoundManager::StaticFunctions() {
            static JSFunctionSpec myFunctions[] = {{0}};
            return myFunctions;
        }


    JSObject *
    JSSoundManager::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
        DOC_MODULE_CREATE("Components", JSSoundManager);
        return myClass;
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

