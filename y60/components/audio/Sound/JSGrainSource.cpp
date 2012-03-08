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

#include "JSGrainSource.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/sound/FFMpegAudioDecoder.h>
#include <y60/jsbase/JSWrapper.impl>

#include <asl/audio/Pump.h>

using namespace asl;
using namespace y60;

namespace jslib {

    template class JSWrapper<y60::GrainSource, y60::GrainSourcePtr, jslib::StaticAccessProtocol>;

    static JSBool
    loadSoundFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Load a new soundfile (everything which can be decoded by ffmpeg) into the grainsource.");
        DOC_PARAM("path", "The path to the soundfile", DOC_TYPE_STRING);
        DOC_END;
        ensureParamCount(argc, 1);
        std::string mySoundFile;
        if (!convertFrom(cx, argv[0], mySoundFile)) {
            JS_ReportError(cx, "loadSoundFile: argument 1 is not a string");
            return JS_FALSE;
        }
        IAudioDecoder* myDecoder = new FFMpegAudioDecoder(mySoundFile);
        JSGrainSource::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        myDecoder->setSampleSink(myNative.get());
        myNative->clearAudioData();
        myDecoder->decodeEverything();
        delete myDecoder;
        JSGrainSource::getJSWrapper(cx, obj).closeNative();
        return JS_TRUE;
    }

    static JSBool
    enable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Start granular synthesis.");
        DOC_END;
        JSGrainSource::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        myNative->enable();
        return JS_TRUE;
    }

    static JSBool
    disable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Stop granular synthesis.");
        DOC_END;
        JSGrainSource::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        myNative->disable();
        return JS_TRUE;
    }

    static JSBool
    fadeToVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Fading to a specified volume over a given duration.");
        DOC_PARAM("theVolume", "", DOC_TYPE_FLOAT);
        DOC_PARAM("theDuration", "", DOC_TYPE_FLOAT);
        DOC_END;
        return Method<JSGrainSource::NATIVE>::call(&JSGrainSource::NATIVE::fadeToVolume,cx,obj,argc,argv,rval);
    }

    JSFunctionSpec *
    JSGrainSource::Functions() {
        static JSFunctionSpec myFunctions[] = {
            // name                  native            nargs
            {"enable",               enable,           0},
            {"disable",              disable,          0},
            {"fadeToVolume",         fadeToVolume,     2},
            {"loadSoundFile",        loadSoundFile,    1},
            {0}
        };
        return myFunctions;
    }

    JSConstIntPropertySpec *
    JSGrainSource::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSPropertySpec *
    JSGrainSource::Properties() {
        static JSPropertySpec myProperties[] = {
            {"volume", PROP_volume, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"size", PROP_size, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"sizejitter", PROP_sizejitter, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"rate", PROP_rate, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"ratejitter", PROP_ratejitter, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"position", PROP_position, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"positionjitter", PROP_positionjitter, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"ratio", PROP_ratio, JSPROP_ENUMERATE | JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"ratiojitter", PROP_ratiojitter, JSPROP_ENUMERATE | JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"transposition", PROP_transposition, JSPROP_ENUMERATE | JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {0}
        };
        return myProperties;
    }

    JSPropertySpec *
    JSGrainSource::StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSFunctionSpec *
    JSGrainSource::StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {{0}};
        return myFunctions;
    }

    JSBool
    JSGrainSource::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        switch (theID) {
        case PROP_volume:
            *vp = as_jsval(cx, getNative().getVolume());
            return JS_TRUE;
        case PROP_size:
            *vp = as_jsval(cx, getNative().getGrainSize());
            return JS_TRUE;
        case PROP_sizejitter:
            *vp = as_jsval(cx, getNative().getGrainSizeJitter());
            return JS_TRUE;
        case PROP_rate:
            *vp = as_jsval(cx, getNative().getGrainRate());
            return JS_TRUE;
        case PROP_ratejitter:
            *vp = as_jsval(cx, getNative().getGrainRateJitter());
            return JS_TRUE;
        case PROP_position:
            *vp = as_jsval(cx, getNative().getGrainPosition());
            return JS_TRUE;
        case PROP_positionjitter:
            *vp = as_jsval(cx, getNative().getGrainPositionJitter());
            return JS_TRUE;
        case PROP_ratio:
            *vp = as_jsval(cx, getNative().getRatio());
            return JS_TRUE;
        case PROP_ratiojitter:
            *vp = as_jsval(cx, getNative().getRatioJitter());
            return JS_TRUE;
        case PROP_transposition:
            *vp = as_jsval(cx, getNative().getTransposition());
            return JS_TRUE;

        default:
            JS_ReportError(cx, "JSGrainSource::getProperty: index %d out of range", theID);
            return JS_FALSE;
        }
    }

    JSBool
    JSGrainSource::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        jsval dummy;
        switch (theID) {
        case PROP_volume:
            return Method<NATIVE>::call(&NATIVE::setVolume, cx, obj, 1, vp, &dummy);
        case PROP_size:
            return Method<NATIVE>::call(&NATIVE::setGrainSize, cx, obj, 1, vp, &dummy);
        case PROP_sizejitter:
            return Method<NATIVE>::call(&NATIVE::setGrainSizeJitter, cx, obj, 1, vp, &dummy);
        case PROP_rate:
            return Method<NATIVE>::call(&NATIVE::setGrainRate, cx, obj, 1, vp, &dummy);
        case PROP_ratejitter:
            return Method<NATIVE>::call(&NATIVE::setGrainRateJitter, cx, obj, 1, vp, &dummy);
        case PROP_position:
            return Method<NATIVE>::call(&NATIVE::setGrainPosition, cx, obj, 1, vp, &dummy);
        case PROP_positionjitter:
            return Method<NATIVE>::call(&NATIVE::setGrainPositionJitter, cx, obj, 1, vp, &dummy);
        case PROP_ratio:
            return Method<NATIVE>::call(&NATIVE::setRatio, cx, obj, 1, vp, &dummy);
        case PROP_ratiojitter:
            return Method<NATIVE>::call(&NATIVE::setRatioJitter, cx, obj, 1, vp, &dummy);
        case PROP_transposition:
            return Method<NATIVE>::call(&NATIVE::setTransposition, cx, obj, 1, vp, &dummy);
        default:
            JS_ReportError(cx,"JSGrainSource::setPropertySwitch: index %d out of range", theID);
        }
        return JS_FALSE;
    }

    JSBool
    JSGrainSource::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Create a new grain source.");
        DOC_PARAM_OPT("grain size", "The length of a grain in ms", DOC_TYPE_INTEGER, 50);
        DOC_PARAM_OPT("grain rate", "The time, when the next grain starts in ms", DOC_TYPE_INTEGER, 25);
        DOC_END;
        try {
            if (JSA_GetClass(cx,obj) != Class()) {
                JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
                return JS_FALSE;
            }

            OWNERPTR myNewNative;

            if (argc == 1) {
                unsigned myGrainSize = 50;
                convertFrom(cx, argv[0], myGrainSize);
                myNewNative = GrainSourcePtr(new GrainSource("aGrainSource",
                                                             Pump::get().getNativeSampleFormat(),
                                                             Pump::get().getNativeSampleRate(),
                                                             2,myGrainSize));
            } else if (argc == 2) {
                unsigned myGrainSize = 50;
                unsigned myGrainRate = 25;
                convertFrom(cx, argv[0], myGrainSize);
                convertFrom(cx, argv[1], myGrainRate);
                myNewNative = GrainSourcePtr(new GrainSource("aGrainSource",
                                                             Pump::get().getNativeSampleFormat(),
                                                             Pump::get().getNativeSampleRate(),
                                                             2,myGrainSize, myGrainRate));
            } else {
                myNewNative = GrainSourcePtr(new GrainSource("aGrainSource",
                                                             Pump::get().getNativeSampleFormat(),
                                                             Pump::get().getNativeSampleRate(),
                                                             2));
            }

            Pump::get().addSampleSource(myNewNative);

            JSGrainSource * myNewObject = new JSGrainSource(myNewNative, myNewNative.get());
            if (myNewObject) {
                JS_SetPrivate(cx, obj, myNewObject);
                return JS_TRUE;
            } else  {
                JS_ReportError(cx,"JSGrainSource::Constructor: bad parameters");
                return JS_FALSE;
            }
        } HANDLE_CPP_EXCEPTION;
    }

    bool convertFrom(JSContext *cx, jsval theValue, GrainSourcePtr & theGrainSource) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<y60::GrainSource>::Class()) {
                    theGrainSource = JSClassTraits<y60::GrainSource>::getNativeOwner(cx,myArgument);
                    return true;
                }
            }
        }
        return false;
    }

    JSObject *
    JSGrainSource::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
        DOC_MODULE_CREATE("Sound", JSGrainSource);
        return myClass;
    }

    jsval as_jsval(JSContext *cx, JSGrainSource::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSGrainSource::Construct(cx, theOwner, theOwner.get());
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSGrainSource::OWNERPTR theOwner, JSGrainSource::NATIVE * theNative) {
        JSObject * myObject = JSGrainSource::Construct(cx, theOwner, theNative);
        return OBJECT_TO_JSVAL(myObject);
    }
}
