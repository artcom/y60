//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSGrainSource.h"
#include <y60/JScppUtils.h>
#include <y60/FFMpegAudioDecoder.h>
#include <y60/JSWrapper.impl>

#include <asl/Pump.h>

using namespace asl;
using namespace y60;

namespace jslib {

    template class JSWrapper<y60::GrainSource, y60::GrainSourcePtr, jslib::StaticAccessProtocol>;


    static JSBool
    loadSoundFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;
        ensureParamCount(argc, 1);
        std::string mySoundFile;
        if (!convertFrom(cx, argv[0], mySoundFile)) {
            JS_ReportError(cx, "loadSoundFile: argument 1 is not a string");
            return JS_FALSE;
        }
        av_register_all();
        IAudioDecoder* myDecoder = new FFMpegAudioDecoder(mySoundFile);
        JSGrainSource::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        myDecoder->setSampleSink(&(*myNative));
        myDecoder->decodeEverything();
        delete myDecoder;
        JSGrainSource::getJSWrapper(cx, obj).closeNative();
        return JS_TRUE;
    }

    static JSBool
    enable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Start playing the sound.");
        DOC_END;
        //        return Method<JSGrainSource::NATIVE>::call(&JSGrainSource::NATIVE::enable,cx,obj,argc,argv,rval);
        JSGrainSource::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        myNative->enable();
        return JS_TRUE;
    }

    static JSBool
    disable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Stop playing the sound."); 
        DOC_END;
        //        return Method<JSGrainSource::NATIVE>::call(&JSGrainSource::NATIVE::disable,cx,obj,argc,argv,rval);
        JSGrainSource::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        myNative->disable();
        return JS_TRUE;
    }

    JSFunctionSpec * 
    JSGrainSource::Functions() {
        static JSFunctionSpec myFunctions[] = {
            // name                  native            nargs
            {"enable",               enable,           0},
            {"disable",              disable,          0},
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
            {"size", PROP_size, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"rate", PROP_rate, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"ratejitter", PROP_ratejitter, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"position", PROP_position, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
            {"positionjitter", PROP_positionjitter, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED},
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
        case PROP_size:
            *vp = as_jsval(cx, getNative().getGrainSize());
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
        default:
            JS_ReportError(cx, "JSGrainSource::getProperty: index %d out of range", theID);
            return JS_FALSE;
        }
    }

    JSBool
    JSGrainSource::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        jsval dummy;
        switch (theID) {
        case PROP_size:
            return Method<NATIVE>::call(&NATIVE::setGrainSize, cx, obj, 1, vp, &dummy);
        case PROP_rate:
            return Method<NATIVE>::call(&NATIVE::setGrainRate, cx, obj, 1, vp, &dummy);
        case PROP_ratejitter:
            return Method<NATIVE>::call(&NATIVE::setGrainRateJitter, cx, obj, 1, vp, &dummy);
        case PROP_position:
            return Method<NATIVE>::call(&NATIVE::setGrainPosition, cx, obj, 1, vp, &dummy);
        case PROP_positionjitter:
            return Method<NATIVE>::call(&NATIVE::setGrainPositionJitter, cx, obj, 1, vp, &dummy);
        default:
            JS_ReportError(cx,"JSGrainSource::setPropertySwitch: index %d out of range", theID);
        }
        return JS_FALSE;
    }

    JSBool
    JSGrainSource::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;
        try {
            if (JSA_GetClass(cx,obj) != Class()) {
                JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
                return JS_FALSE;
            }

            OWNERPTR myNewNative;
            myNewNative = GrainSourcePtr(new GrainSource("aGrainSource",
                                                         Pump::get().getNativeSampleFormat(),
                                                         Pump::get().getNativeSampleRate(),
                                                         2));
            Pump::get().addSampleSource(myNewNative);

            JSGrainSource * myNewObject = new JSGrainSource(myNewNative, &(*myNewNative));
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
        DOC_MODULE_CREATE("Components", JSGrainSource);
        return myClass;
    }

    jsval as_jsval(JSContext *cx, JSGrainSource::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSGrainSource::Construct(cx, theOwner, &(*theOwner));
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSGrainSource::OWNERPTR theOwner, JSGrainSource::NATIVE * theNative) {
        JSObject * myObject = JSGrainSource::Construct(cx, theOwner, theNative);
        return OBJECT_TO_JSVAL(myObject);
    }
}
