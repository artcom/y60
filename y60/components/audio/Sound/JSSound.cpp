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

#include "JSSound.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>

using namespace std;
using namespace asl;

namespace jslib {

    template class JSWrapper<y60::Sound, y60::SoundPtr, jslib::StaticAccessProtocol>;

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
    setVolumes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Set volumes for each channel."); DOC_END;
        return Method<JSSound::NATIVE>::call(&JSSound::NATIVE::setVolumes,cx,obj,argc,argv,rval);
    }
    static JSBool
    fadeToVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Fading to a specified volume over a given duration.");
        DOC_PARAM("theVolume", "", DOC_TYPE_FLOAT);
        DOC_PARAM("theDuration", "", DOC_TYPE_FLOAT);
        DOC_END;
        return Method<JSSound::NATIVE>::call(&JSSound::NATIVE::fadeToVolume,cx,obj,argc,argv,rval);
    }
    static JSBool
    seek(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Seeking to a given position.");
        DOC_PARAM("theTime", "seek time", DOC_TYPE_FLOAT);
        DOC_END;
        return Method<JSSound::NATIVE>::call(&JSSound::NATIVE::seek,cx,obj,argc,argv,rval);
    }
    static JSBool
    seekRelative(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Seeking a timespan ahead of the current position.");
        DOC_PARAM("theTime", "seek time", DOC_TYPE_FLOAT);
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
            {"setVolumes",           setVolumes,        2},            
            {"fadeToVolume",         fadeToVolume,      2},
            {"seek",                 seek,              1},
            {"seekRelative",         seekRelative,      1},
            {0}
        };
        return myFunctions;
    }

    JSConstIntPropertySpec *
    JSSound::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSPropertySpec *
    JSSound::Properties() {
        static JSPropertySpec myProperties[] = {
            {"volume",    PROP_volume,   JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"looping",   PROP_looping,  JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"playing",   PROP_playing,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"src",       PROP_src,      JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"duration",  PROP_duration, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"time",      PROP_time,     JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {0}
        };
        return myProperties;
    }

    JSPropertySpec *
    JSSound::StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSFunctionSpec *
    JSSound::StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {{0}};
        return myFunctions;
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
                *vp = as_jsval(cx, JSClassTraits<NATIVE>::ScopedNativeRef(cx, obj).getNative().getCurrentTime());
                /**vp = as_jsval(cx, openNative().getCurrentTime());
                closeNative();*/
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
            case PROP_looping:
                return Method<NATIVE>::call(&NATIVE::setLooping, cx, obj, 1, vp, &dummy);
            default:
                JS_ReportError(cx,"JSSound::setPropertySwitch: index %d out of range", theID);
        }
        return JS_FALSE;
    }

    JSBool
    JSSound::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Creates a Sound from URI. Does not start playback.");
        DOC_PARAM("theURI", "", DOC_TYPE_STRING);
        DOC_PARAM_OPT("theLoopflag", "", DOC_TYPE_BOOLEAN, false);
        DOC_PARAM_OPT("theUseCacheFlag", "", DOC_TYPE_BOOLEAN, true);
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

            bool myLoopFlag = false;
            if (argc > 1) {
                if (!convertFrom(cx, argv[1], myLoopFlag)) {
                    JS_ReportError(cx, "JSSound::Constructor(): argument #2 must be a boolean (Loop Flag)");
                    return JS_FALSE;
                }
            }

            bool myCacheFlag = false;
            if (argc > 2) {
                if (!convertFrom(cx, argv[2], myCacheFlag)) {
                    JS_ReportError(cx, "JSSound::Constructor(): argument #3 must be a boolean (Cache Flag)");
                    return JS_FALSE;
                }
            }

            OWNERPTR myNewNative;
            if (argc == 0) {
                // Construct empty Sound that will be filled by copy Construct()
//                AC_PRINT << "JSSound::Constructor: empty";
                myNewNative = OWNERPTR();
            } else {
//                AC_PRINT << "JSSound::Constructor: " << myURI;
                myNewNative = y60::SoundManager::get().createSound(myURI, myLoopFlag, myCacheFlag);
            }

            JSSound * myNewObject = new JSSound(myNewNative, myNewNative.get());
            if (myNewObject) {
                JS_SetPrivate(cx, obj, myNewObject);
                return JS_TRUE;
            } else  {
                JS_ReportError(cx,"JSSound::Constructor: bad parameters");
                return JS_FALSE;
            }
        } HANDLE_CPP_EXCEPTION;
    }

    JSObject *
    JSSound::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
        DOC_MODULE_CREATE("Sound", JSSound);
        return myClass;
    }

    jsval as_jsval(JSContext *cx, JSSound::OWNERPTR theOwner) {
        //AC_PRINT << "JSSound::as_jsval";
        JSObject * myReturnObject = JSSound::Construct(cx, theOwner, theOwner.get());
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSSound::OWNERPTR theOwner, JSSound::NATIVE * theNative) {
        //AC_PRINT << "JSSound::as_jsval with Native";
        JSObject * myObject = JSSound::Construct(cx, theOwner, theNative);
        return OBJECT_TO_JSVAL(myObject);
    }
}

extern "C"
EXPORT asl::PlugInBase * Sound_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::JSSoundPlugIn(myDLHandle);
}
