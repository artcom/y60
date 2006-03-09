//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSPerlinNoise.cpp,v $
//   $Author: christian $
//   $Revision: 1.6 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSPerlinNoise.h"
#include "JScppUtils.h"
#include "JSVector.h"

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

static JSBool
noise2D(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Calculates an noise value for given 2d position");
    DOC_PARAM("thePosition", "The position to calculate the noise for", DOC_TYPE_VECTOR2F);
    DOC_RVAL("Return the noise value", DOC_TYPE_FLOAT);
    DOC_END;
    try {
        if (argc != 1) {
            JS_ReportError(cx, "JSPerlinNoise::noise2D(): Wrong number of arguments, expected one, got %d.", argc);
            return JS_FALSE;
        }


        Vector2f thePosition;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], thePosition)) {
            JS_ReportError(cx, "JSPerlinNoise::noise2D(): Argument #1 must be Vector2f (position)");
            return JS_FALSE;
        }

        float myNoise = JSPerlinNoise::getJSWrapper(cx,obj).openNative()(thePosition[0], thePosition[1]);
        JSPerlinNoise::getJSWrapper(cx,obj).closeNative();
        *rval = as_jsval(cx, myNoise);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSPerlinNoise::Functions() {
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"noise2D",              noise2D,                 1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSPerlinNoise::Properties() {
    static JSPropertySpec myProperties[] = {
        {"persistence", PROP_persistance, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"octavecount", PROP_octavecount, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSPerlinNoise::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_persistance:
            *vp = as_jsval(cx, getNative().getPersistence());
            return JS_TRUE;
        case PROP_octavecount:
            *vp = as_jsval(cx, getNative().getOctaveCount());
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSPerlinNoise::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSPerlinNoise::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_persistance:
            {
                jsval dummy;
                return Method<NATIVE>::call(&NATIVE::setPersistence, cx, obj, 1, vp, &dummy);
            }
        default:
            JS_ReportError(cx,"JSPerlinNoise::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSPerlinNoise::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new perlin noise object");
    DOC_PARAM("thePersistence", "A parameter that controls frequency and amplitude of the noise function. Frequency = 2^i. Amplitude = Persistence^i. i is the ith noise function to be added.", DOC_TYPE_FLOAT);
    DOC_PARAM("theAmplitudes", "The number of amplitudes of the noise function.", DOC_TYPE_INTEGER);
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSPerlinNoise * myNewObject = 0;

    if (argc != 2) {
        JS_ReportError(cx, "JSPerlinNoise::Constructor(): Wrong number of arguments, expected two, got %d.", argc);
        return JS_FALSE;
    }

    float thePersistence;
    if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], thePersistence)) {
        JS_ReportError(cx, "JSPerlinNoise::Constructor(): Argument #1 must be float (thePersistence)");
        return JS_FALSE;
    }

    unsigned theAmplitudes;
    if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], theAmplitudes)) {
        JS_ReportError(cx, "JSPerlinNoise::Constructor(): Argument #1 must be unsigend (theAmplitudes)");
        return JS_FALSE;
    }

    NoiseSeed mySeed;
    PrimePool::createNoiseSeed(theAmplitudes, mySeed);
    OWNERPTR myNewPerlinNoise = OWNERPTR(new PerlinNoise2D(mySeed, thePersistence));
    myNewObject = new JSPerlinNoise(myNewPerlinNoise, &(*myNewPerlinNoise));

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSPerlinNoise::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSPerlinNoise::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSPerlinNoise::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSPerlinNoise::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

JSObject *
JSPerlinNoise::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_CREATE(JSPerlinNoise);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSPerlinNoise::NATIVE & thePerlinNoise) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSPerlinNoise::NATIVE >::Class()) {
                thePerlinNoise = JSClassTraits<JSPerlinNoise::NATIVE>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSPerlinNoise::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSPerlinNoise::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSPerlinNoise::OWNERPTR theOwner, JSPerlinNoise::NATIVE * thePerlinNoise) {
    JSObject * myObject = JSPerlinNoise::Construct(cx, theOwner, thePerlinNoise);
    return OBJECT_TO_JSVAL(myObject);
}

}
