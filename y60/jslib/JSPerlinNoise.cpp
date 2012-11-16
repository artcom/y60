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
*/

// own header
#include "JSPerlinNoise.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSWrapper.impl>

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<asl::PerlinNoise3D, asl::Ptr<asl::PerlinNoise3D>, StaticAccessProtocol>;

static JSBool
noise(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Calculates an noise value for given position");
    DOC_PARAM("thePosition", "The 1D-position to calculate the noise for", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("thePosition", "The 2D-position to calculate the noise for", DOC_TYPE_VECTOR2F);
    DOC_RESET;
    DOC_PARAM("thePosition", "The 3D-position to calculate the noise for", DOC_TYPE_VECTOR3F);
    DOC_RVAL("Return the noise value", DOC_TYPE_FLOAT);
    DOC_END;

    try {
        if (argc != 1) {
            JS_ReportError(cx, "JSPerlinNoise::noise(): Wrong number of arguments, expected one, got %d.", argc);
            return JS_FALSE;
        }

        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "JSPerlinNoise::noise2D(): Argument #1 must is null (position)");
            return JS_FALSE;
        }

        float myNoise = 0;
        float thePosition1D;
        Vector2f thePosition2D;
        Vector3f thePosition3D;
        if (convertFrom(cx, argv[0], thePosition1D)) {
            myNoise = JSPerlinNoise::getJSWrapper(cx,obj).openNative().noise(thePosition1D);
        } else if (convertFrom(cx, argv[0], thePosition2D)) {
            myNoise = JSPerlinNoise::getJSWrapper(cx,obj).openNative().noise(thePosition2D[0], thePosition2D[1]);
        } else if (convertFrom(cx, argv[0], thePosition3D)) {
            myNoise = JSPerlinNoise::getJSWrapper(cx,obj).openNative().noise(thePosition3D[0], thePosition3D[1],
                    thePosition3D[2]);
        } else {
            JS_ReportError(cx, "JSPerlinNoise::noise2D(): Argument #1 must be Vector2f (position)");
            return JS_FALSE;
        }

        JSPerlinNoise::getJSWrapper(cx,obj).closeNative();
        *rval = as_jsval(cx, myNoise);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSPerlinNoise::Functions() {
    static JSFunctionSpec myFunctions[] = {
        // name                native                   nargs
        {"noise",              noise,                 1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSPerlinNoise::Properties() {
    static JSPropertySpec myProperties[] = {
        {"octavecount", PROP_octavecount, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"amplitudefalloff", PROP_amplitudefalloff, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSPerlinNoise::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_amplitudefalloff:
            *vp = as_jsval(cx, getNative().getAmplitudeFalloff());
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
        case PROP_amplitudefalloff:
            {
                jsval dummy;
                return Method<NATIVE>::call(&NATIVE::setAmplitudeFalloff, cx, obj, 1, vp, &dummy);
            }
        case PROP_octavecount:
            {
                jsval dummy;
                return Method<NATIVE>::call(&NATIVE::setOctaveCount, cx, obj, 1, vp, &dummy);
            }
        default:
            JS_ReportError(cx,"JSPerlinNoise::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSPerlinNoise::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new perlin noise object");
    DOC_PARAM("theOctaveCount", "The number of amplitudes of the noise function.", DOC_TYPE_INTEGER);
    DOC_PARAM_OPT("theAmplitudeFalloff", "An parameter that controls the shape of the noise function", DOC_TYPE_FLOAT, 0.5);
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSPerlinNoise * myNewObject = 0;

    if (argc < 1 || argc > 2) {
        JS_ReportError(cx, "JSPerlinNoise::Constructor(): Wrong number of arguments, expected  one or two, got %d.", argc);
        return JS_FALSE;
    }

    unsigned theOctaveCount;
    if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], theOctaveCount)) {
        JS_ReportError(cx, "JSPerlinNoise::Constructor(): Argument #1 must be unsigned (theOctaveCount)");
        return JS_FALSE;
    }

    float theAmplitudeFalloff = 0.5;
    if (argc > 1) {
        if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], theAmplitudeFalloff)) {
            JS_ReportError(cx, "JSPerlinNoise::Constructor(): Argument #1 must be float (theAmplitudeFalloff)");
            return JS_FALSE;
        }
    }

    OWNERPTR myNewPerlinNoise = OWNERPTR(new PerlinNoise3D(theOctaveCount, theAmplitudeFalloff));
    myNewObject = new JSPerlinNoise(myNewPerlinNoise, myNewPerlinNoise.get());

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
    DOC_MODULE_CREATE("Math", JSPerlinNoise);
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
    JSObject * myReturnObject = JSPerlinNoise::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSPerlinNoise::OWNERPTR theOwner, JSPerlinNoise::NATIVE * thePerlinNoise) {
    JSObject * myObject = JSPerlinNoise::Construct(cx, theOwner, thePerlinNoise);
    return OBJECT_TO_JSVAL(myObject);
}

}
