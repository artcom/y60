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

// own header
#include "JSBox.h"

#include "JSVector.h"
#include "JSPlane.h"
#include "JSWrapper.impl"
#include "JScppUtils.impl"
#include <asl/math/Box.h>

#include <iostream>

using namespace std;

namespace jslib {

template class JSWrapper<asl::Box3<BoxNumber> >;

typedef BoxNumber Number;
typedef asl::Box3<Number> NATIVE;

static JSBool
makeEmpty(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Makes this Box contain nothing.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeEmpty,cx,obj,argc,argv,rval);
}
static JSBool
makeFull(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Makes this Box contain anything.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeFull,cx,obj,argc,argv,rval);
}
static JSBool
makeCorrect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Flips Coordinates so that the Box has a proper upper near left and lower far right corner.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeCorrect,cx,obj,argc,argv,rval);
}
static JSBool
extendBy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Extend the Box to minimally include a given point/box.");
    DOC_PARAM("theExtensionPoint", "a point defining the resulting extension", DOC_TYPE_POINT3F);
    DOC_RESET;
    DOC_PARAM("theExtensionBox", "a box defining the resulting extension", DOC_TYPE_BOX3F);
    DOC_END;
    if (argc == 1) {
        if (JSBox3f::matchesClassOf(cx, argv[0])) {
            typedef void (NATIVE::*MyMethod)(const asl::Box3<Number> &);
            return Method<NATIVE>::call((MyMethod)&NATIVE::extendBy,cx,obj,argc,argv,rval);
        } else {
            typedef void (NATIVE::*MyMethod)(const asl::Point3<Number> &);
            return Method<NATIVE>::call((MyMethod)&NATIVE::extendBy,cx,obj,argc,argv,rval);
        }
    }
    JS_ReportError(cx,"JSBox3f::extendBy: bad number of arguments, 1 expected");
    return JS_FALSE;
}

static JSBool
intersects(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("check if the Box intersects with another one");
    DOC_PARAM("theIntersectingBox", "", DOC_TYPE_BOX3F);
    DOC_RVAL("Returns TRUE if intersection of given boxes is a non-empty box.", DOC_TYPE_BOOLEAN);
    DOC_END;
    if (argc == 1) {
        if (JSBox3f::matchesClassOf(cx, argv[0])) {
            typedef bool (NATIVE::*MyMethod)(const asl::Box3<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::intersects,cx,obj,argc,argv,rval);
        } else {
            JS_ReportError(cx,"JSBox3f::intersects: bad arguments, Box3f expected");
            return JS_FALSE;
        }
    }
    JS_ReportError(cx,"JSBox3f::intersects: bad number of arguments, 1 expected");
    return JS_FALSE;
}

//static JSBool
//intersect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
//    DOC_BEGIN("Intersect the Box with another one.");
//    DOC_PARAM("theIntersectingBox", "", DOC_TYPE_BOX3F);
//    DOC_END;
//    if (argc == 1) {
//        if (JSBox3f::matchesClassOf(cx, argv[0])) {
//            typedef void (NATIVE::*MyMethod)(const asl::Box3<Number> &);
//            return Method<NATIVE>::call((MyMethod)&NATIVE::intersects,cx,obj,argc,argv,rval);
//        } else {
//            JS_ReportError(cx,"JSBox3f::intersect: bad arguments, Box3f expected");
//            return JS_FALSE;
//        }
//    }
//    JS_ReportError(cx,"JSBox3f::intersect: bad number of arguments, 1 expected");
//    return JS_FALSE;
//}
static JSBool
contains(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Check if the Box contains a point/box.");
    DOC_PARAM("theContainedPoint", "", DOC_TYPE_POINT3F);
    DOC_RESET;
    DOC_PARAM("theContainedBox", "", DOC_TYPE_BOX3F);
    DOC_RVAL("true/false", DOC_TYPE_BOOLEAN);
    DOC_END;
    if (argc == 1) {
        if (JSBox3f::matchesClassOf(cx, argv[0])) {
            typedef bool (NATIVE::*MyMethod)(const asl::Box3<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::contains,cx,obj,argc,argv,rval);
        } else {
            typedef bool (NATIVE::*MyMethod)(const asl::Point3<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::contains,cx,obj,argc,argv,rval);
        }
    }
    JS_ReportError(cx,"JSBox3f::contains: bad number of arguments, 1 expected");
    return JS_FALSE;
}

static JSBool
envelopes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Check if the Box contains and doesn't touch a point/box.");
    DOC_PARAM("theEnvelopedPoint", "", DOC_TYPE_POINT3F);
    DOC_RESET;
    DOC_PARAM("theEnvelopedBox", "", DOC_TYPE_BOX3F);
    DOC_RVAL("true/false", DOC_TYPE_BOOLEAN);
    DOC_END;
    if (argc == 1) {
        if (JSBox3f::matchesClassOf(cx, argv[0])) {
            typedef bool (NATIVE::*MyMethod)(const asl::Box3<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::envelopes,cx,obj,argc,argv,rval);
        } else {
            typedef bool (NATIVE::*MyMethod)(const asl::Point3<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::envelopes,cx,obj,argc,argv,rval);
        }
    }
    JS_ReportError(cx,"JSBox3f::envelopes: bad number of arguments, 1 expected");
    return JS_FALSE;
}
static JSBool
touches(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Check if the Box touches a point/box.");
    DOC_PARAM("theTouchingPoint", "", DOC_TYPE_POINT3F);
    DOC_RESET;
    DOC_PARAM("theTouchingBox", "", DOC_TYPE_BOX3F);
    DOC_RVAL("true/false", DOC_TYPE_BOOLEAN);
    DOC_END;
    if (argc == 1) {
        if (JSBox3f::matchesClassOf(cx, argv[0])) {
            typedef bool (NATIVE::*MyMethod)(const asl::Box3<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::touches,cx,obj,argc,argv,rval);
        } else {
            typedef bool (NATIVE::*MyMethod)(const asl::Point3<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::touches,cx,obj,argc,argv,rval);
        }
    }
    JS_ReportError(cx,"JSBox3f::touches: bad number of arguments, 1 expected");
    return JS_FALSE;
}
static JSBool
setBounds(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("set the bounds of the Box.");
    DOC_PARAM("theUpperNearLeftCorner", "", DOC_TYPE_POINT3F);
    DOC_PARAM("theLowerFarRightCorner", "", DOC_TYPE_POINT3F);
    DOC_END;
    typedef void (NATIVE::*MyMethod)(const asl::Point3<Number> &, const asl::Point3<Number> &);
    return Method<NATIVE>::call((MyMethod)&NATIVE::setBounds,cx,obj,argc,argv,rval);
}
static JSBool
add(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Translate the Box adding an offset to its corners.");
    DOC_PARAM("theTranslationVector", "", DOC_TYPE_VECTOR3F);
    DOC_END;
    typedef void (NATIVE::*MyMethod)(const asl::Vector3<Number> &);
    return Method<NATIVE>::call((MyMethod)&NATIVE::add,cx,obj,argc,argv,rval);
}
static JSBool
sub(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Translate the Box subtracting an offset to its corners.");
    DOC_PARAM("theTranslationVector", "", DOC_TYPE_VECTOR3F);
    DOC_END;
    typedef void (NATIVE::*MyMethod)(const asl::Vector3<Number> &);
    return Method<NATIVE>::call((MyMethod)&NATIVE::sub,cx,obj,argc,argv,rval);
}
static JSBool
mult(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Scale the Box multiplying each coordinate by a factor.");
    DOC_PARAM("theFactors", "", DOC_TYPE_VECTOR3F);
    DOC_END;
    typedef void (NATIVE::*MyMethod)(const asl::Vector3<Number> &);
    return Method<NATIVE>::call((MyMethod)&NATIVE::mult,cx,obj,argc,argv,rval);
}
static JSBool
div(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Scale the Box dividing each coordinate by a factor.");
    DOC_PARAM("theFactors", "", DOC_TYPE_VECTOR3F);
    DOC_END;
    typedef void (NATIVE::*MyMethod)(const asl::Vector3<Number> &);
    return Method<NATIVE>::call((MyMethod)&NATIVE::div,cx,obj,argc,argv,rval);
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("returns string representation of the Box.");
    DOC_RVAL("", DOC_TYPE_STRING);
    DOC_END;
    std::string myStringRep = asl::as_string(JSBox3f::getJSWrapper(cx,obj).getNative());
*rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSBox3f::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {"makeEmpty",          makeEmpty,      0         },
        {"makeFull",           makeFull,       0         },
        {"makeCorrect",        makeCorrect,    0         },
        {"extendBy",           extendBy,       1         }, // point, box
        {"contains",           contains,       1         }, // point, box
        {"intersects",         intersects,     1         }, // box
        {"intersect",          intersects,     1         }, // box
        {"envelopes",          envelopes,      1         },
        {"touches",            touches,        1         },
        {"setBounds",          setBounds,      2         },
        {"add",                add,            1         },
        {"sub",                sub,            1         },
        {"mult",               mult,           1         },
        {"div",                div,            1         },
        {"toString",           toString,       0         },
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSBox3f::Properties() {
    static JSPropertySpec myProperties[] = {
        {"length",      PROP_length,      JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {"hasPosition", PROP_hasPosition, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute boolean
        {"hasSize",     PROP_hasSize,     JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute boolean
        {"hasArea",     PROP_hasArea,     JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute boolean
        {"hasVolume",   PROP_hasVolume,   JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute boolean
        {"isEmpty",     PROP_isEmpty,     JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute boolean
        {"min",         PROP_min,         JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // attribute point
        {"max",         PROP_max,         JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // attribute point
        {"center",      PROP_center,      JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // attribute point
        {"size",        PROP_size,        JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute vector
        {"volume",      PROP_volume,      JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute number
        {"area",        PROP_area,        JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute boolean
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSBox3f::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSBox3f::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSBox3f::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}


// getproperty handling
JSBool
JSBox3f::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_length:
                *vp = as_jsval(cx, 2);
                return JS_TRUE;
            case PROP_hasPosition:
                *vp = as_jsval(cx, getNative().hasPosition());
                return JS_TRUE;
            case PROP_hasSize:
                *vp = as_jsval(cx, getNative().hasSize());
                return JS_TRUE;
            case PROP_hasArea:
                *vp = as_jsval(cx, getNative().hasArea());
                return JS_TRUE;
            case PROP_hasVolume:
                *vp = as_jsval(cx, getNative().hasVolume());
                return JS_TRUE;
            case PROP_isEmpty:
                *vp = as_jsval(cx, getNative().isEmpty());
                return JS_TRUE;
            case PROP_min:
                *vp = as_jsval(cx, getNative().getMin());
                return JS_TRUE;
            case PROP_max:
                *vp = as_jsval(cx, getNative().getMax());
                return JS_TRUE;
            case PROP_center:
                *vp = as_jsval(cx, getNative().getCenter());
                return JS_TRUE;
            case PROP_size:
                *vp = as_jsval(cx, getNative().getSize());
                return JS_TRUE;
            case PROP_volume:
                *vp = as_jsval(cx, getNative().getVolume());
                return JS_TRUE;
            case PROP_area:
                *vp = as_jsval(cx, getNative().getArea());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSBox3f::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
};
JSBool JSBox3f::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    *vp = as_jsval(cx, getNative()[theIndex]);
    return JS_TRUE;
};

// setproperty handling
JSBool
JSBox3f::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    jsval dummy;
    switch (theID) {
        case PROP_min:
            return Method<NATIVE>::call(&NATIVE::setMin, cx, obj, 1, vp, &dummy);
        case PROP_max:
            return Method<NATIVE>::call(&NATIVE::setMax, cx, obj, 1, vp, &dummy);
        case PROP_center:
            return Method<NATIVE>::call(&NATIVE::setCenter, cx, obj, 1, vp, &dummy);
        default:
            JS_ReportError(cx,"JSBox3f::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
};

JSBool
JSBox3f::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    if (JSVAL_IS_OBJECT(*vp)) {
        JSObject * myValObj = JSVAL_TO_OBJECT(*vp);
        if (JSVector<asl::Vector3<Number> >::Class() != JSA_GetClass(cx,myValObj)) {
            myValObj = JSVector<asl::Point3<Number> >::Construct(cx, *vp);
        }
        if (myValObj) {
            const asl::Point3<Number> & myNativeArg = JSClassTraits<asl::Point3<Number> >::getNativeRef(cx, JSVAL_TO_OBJECT(*vp));
            openNative()[theIndex] = myNativeArg;
            closeNative();
            return JS_TRUE;
        } else {
            JS_ReportError(cx,"JSBox3f::setPropertyIndex: bad argument type, Point3 expected");
            return JS_FALSE;
        }
    }
    return JS_TRUE;
};

JSBool
JSBox3f::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs a new box object");
    DOC_RESET;
    DOC_PARAM("theCorner", "One corner of the box", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theCorner", "Another corner of the box", DOC_TYPE_VECTOR3F);
    DOC_RESET;
    DOC_PARAM("theOtherBox", "Construct from another box (copy constructor)", DOC_TYPE_BOX3F);
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSBox3f * myNewObject = 0;
    JSBox3f::NativeValuePtr myNewValue = JSBox3f::NativeValuePtr(new dom::SimpleValue<asl::Box3<Number> >(0));
    asl::Box3<Number> & myNewBox3f = myNewValue->openWriteableValue(); // we close it only on success, otherwise we trash it anyway
    if (argc == 0) {
        // construct empty
        myNewObject=new JSBox3f(myNewValue);
    } else {
        if (argc == 2) {
            // construct from two points
            for (int i = 0; i < 2 ;++i) {
                JSObject * myObject = JSVector<asl::Vector3<Number> >::Construct(cx, argv[i]);
                if (!myObject) {
                    JS_ReportError(cx,"JSBox3f::Constructor: argument must be 2 vectors or arrays of size 3",ClassName());
                    return JS_FALSE;
                }
                (myNewBox3f)[i] = JSClassTraits<asl::Vector3<Number> >::getNativeRef(cx,myObject);
            }
            myNewBox3f.makeCorrect();
            myNewObject=new JSBox3f(myNewValue);
        } else if (argc == 1) {
            // construct from one Box3f
            JSObject * myArgument;
            if (!JS_ValueToObject(cx, argv[0], &myArgument)) {
                JS_ReportError(cx,"JSBox3f::Constructor: bad argument type, Box3f expected");
                return JS_FALSE;
            }
            if (JSA_GetClass(cx,myArgument) == Class()) {
                myNewBox3f = getJSWrapper(cx, myArgument).getNative();
                myNewObject=new JSBox3f(myNewValue);
            }
        } else {
            JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 0,1 or 3, got %d",ClassName(), argc);
            return JS_FALSE;
        }
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        myNewValue->closeWriteableValue();
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSBox3f::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSBox3f::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
    DOC_MODULE_CREATE("Math", JSBox3f);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Box3<Number>  & theBox3f) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Box3<Number> >::Class()) {
                theBox3f = JSClassTraits<asl::Box3<Number> >::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, const asl::Box3<Number>  & theValue) {
    JSObject * myReturnObject = JSBox3f::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSBox3f::NativeValuePtr theValue) {
    JSObject * myObject = JSBox3f::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

template jsval as_jsval(JSContext *cx, const std::vector<asl::Box3<float> > & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<asl::Box3<float> > & theVector);

}
