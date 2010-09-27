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

    static JSBool
    getVolumes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Returns an array of volumes, one element per channel.");
        DOC_END;
        try {
            ensureParamCount(argc, 0, 0, PLUS_FILE_LINE);

            JSSoundManager::OWNERPTR myNative;
            convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

            std::vector<float> myVolumes;
            
            myNative->getVolumes(myVolumes);
            *rval = as_jsval(cx, myVolumes);
            
            return JS_TRUE;
        } HANDLE_CPP_EXCEPTION;
    }
    
    static JSBool
    setVolumes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Set volumes of all channels");
        DOC_PARAM("theVolume", "one float for each channel", DOC_TYPE_VECTOROFFLOAT);
        DOC_END;
        return Method<JSSoundManager::NATIVE>::call(&JSSoundManager::NATIVE::setVolumes,
                cx,obj,argc,argv,rval);
    }

    JSFunctionSpec *
    JSSoundManager::Functions() {
        static JSFunctionSpec myFunctions[] = {
            // name                  native            nargs
            {"stopAll",              stopAll,           0},
            {"fadeToVolume",         fadeToVolume,      2},
            {"preloadSound",         preloadSound,      1},
            {"getVolumes",           getVolumes,        0},
            {"setVolumes",           setVolumes,        1},
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
            {"channelcount", PROP_channelcount, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
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
            case PROP_channelcount:
                *vp = as_jsval(cx, getNative().getChannelCount());
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

            JSSoundManager * myNewObject = new JSSoundManager(myNewNative, myNewNative);
            if (myNewObject) {
                JS_SetPrivate(cx, obj, myNewObject);
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
        DOC_MODULE_CREATE("Sound", JSSoundManager);
        return myClass;
    }

    jsval as_jsval(JSContext *cx, JSSoundManager::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSSoundManager::Construct(cx, theOwner, theOwner);
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSSoundManager::OWNERPTR theOwner, JSSoundManager::NATIVE * theNative) {
        JSObject * myObject = JSSoundManager::Construct(cx, theOwner, theNative);
        return OBJECT_TO_JSVAL(myObject);
    }
    bool convertFrom(JSContext *cx, jsval theValue, JSSoundManager::OWNERPTR & theSoundManager) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<y60::SoundManager>::Class()) {
                    theSoundManager = JSClassTraits<y60::SoundManager>::getNativeOwner(cx,myArgument);
                    return true;
                }
            }
        }
        return false;
    }
}

