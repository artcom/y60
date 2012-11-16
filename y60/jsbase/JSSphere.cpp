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
#include "JSSphere.h"

#include "JSVector.h"
#include "JSWrapper.impl"
#include "JScppUtils.impl"
#include <iostream>

using namespace std;

namespace jslib {

template class JSWrapper<asl::Sphere<SphereNumber> >;

typedef SphereNumber Number;
typedef asl::Sphere<Number> NATIVE;

static JSBool
makeEmpty(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Create empty/invalid Sphere (radius=-1)");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeEmpty,cx,obj,argc,argv,rval);
}
static JSBool
extendBy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Extend Sphere by given object");
    DOC_PARAM("theSphere", "", DOC_TYPE_SPHERE);
    DOC_RESET;
    DOC_PARAM("thePoint", "", DOC_TYPE_POINT3F);
    DOC_END;
    if (argc == 1) {
        if (JSSphere::matchesClassOf(cx, argv[0])) {
            typedef void (NATIVE::*MyMethod)(const asl::Sphere<Number> &);
            return Method<NATIVE>::call((MyMethod)&NATIVE::extendBy,cx,obj,argc,argv,rval);
        } else {
            typedef void (NATIVE::*MyMethod)(const asl::Point3<Number> &);
            return Method<NATIVE>::call((MyMethod)&NATIVE::extendBy,cx,obj,argc,argv,rval);
        }
    }
    JS_ReportError(cx,"JSSphere::extendBy: bad number of arguments, 1 expected");
    return JS_FALSE;
}
static JSBool
contains(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Test if object is contained in Sphere (may touch boundary)");
    DOC_PARAM("theSphere", "", DOC_TYPE_SPHERE);
    DOC_RESET;
    DOC_PARAM("thePoint", "", DOC_TYPE_POINT3F);
    DOC_RVAL("true if object is contained in Sphere", DOC_TYPE_BOOLEAN);
    DOC_END;
    if (argc == 1) {
        if (JSSphere::matchesClassOf(cx, argv[0])) {
            typedef bool (NATIVE::*MyMethod)(const asl::Sphere<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::contains,cx,obj,argc,argv,rval);
        } else {
            typedef bool (NATIVE::*MyMethod)(const asl::Point3<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::contains,cx,obj,argc,argv,rval);
        }
    }
    JS_ReportError(cx,"JSSphere::contains: bad number of arguments, 1 expected");
    return JS_FALSE;
}

static JSBool
envelopes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Test if object is enveloped by Sphere (may not touch boundary)");
    DOC_PARAM("theSphere", "", DOC_TYPE_SPHERE);
    DOC_RESET;
    DOC_PARAM("thePoint", "", DOC_TYPE_POINT3F);
    DOC_RVAL("true if object is enveloped by Sphere", DOC_TYPE_BOOLEAN);
    DOC_END;
    if (argc == 1) {
        if (JSSphere::matchesClassOf(cx, argv[0])) {
            typedef bool (NATIVE::*MyMethod)(const asl::Sphere<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::envelopes,cx,obj,argc,argv,rval);
        } else {
            typedef bool (NATIVE::*MyMethod)(const asl::Point3<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::envelopes,cx,obj,argc,argv,rval);
        }
    }
    JS_ReportError(cx,"JSSphere::envelopes: bad number of arguments, 1 expected");
    return JS_FALSE;
}
static JSBool
touches(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Test if object touches Sphere boundary");
    DOC_PARAM("theSphere", "", DOC_TYPE_SPHERE);
    DOC_RESET;
    DOC_PARAM("thePoint", "", DOC_TYPE_POINT3F);
    DOC_RVAL("true if object is enveloped by Sphere", DOC_TYPE_BOOLEAN);
    DOC_END;
    if (argc == 1) {
        if (JSSphere::matchesClassOf(cx, argv[0])) {
            typedef bool (NATIVE::*MyMethod)(const asl::Sphere<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::touches,cx,obj,argc,argv,rval);
        } else {
            typedef bool (NATIVE::*MyMethod)(const asl::Point3<Number> &) const;
            return Method<NATIVE>::call((MyMethod)&NATIVE::touches,cx,obj,argc,argv,rval);
        }
    }
    JS_ReportError(cx,"JSSphere::touches: bad number of arguments, 1 expected");
    return JS_FALSE;
}
static JSBool
intersects(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Test if object intersects Sphere");
    DOC_PARAM("theSphere", "", DOC_TYPE_SPHERE);
    DOC_RVAL("true if object intersects Sphere", DOC_TYPE_BOOLEAN);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::intersects,cx,obj,argc,argv,rval);
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints a string representation of a sphere.");
    DOC_END;
    std::string myStringRep = asl::as_string(JSSphere::getJSWrapper(cx,obj).getNative());
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSSphere::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {"makeEmpty",          makeEmpty,               0},
        {"extendBy",           extendBy,                1},
        {"contains",           contains,                1},
        {"envelopes",          envelopes,               1},
        {"touches",            touches,                 1},
        {"intersects",         intersects,              1},
        {"toString",           toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSSphere::Properties() {
    static JSPropertySpec myProperties[] = {
        {"center", PROP_center, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // readwrite attribute Vector3f
        {"radius", PROP_radius, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // readwrite attribute float
        {"hasPosition", PROP_hasPosition, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute boolean
        {"hasSize",     PROP_hasSize,     JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute boolean
        {"hasArea",     PROP_hasArea,     JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute boolean
        {"hasVolume",   PROP_hasVolume,   JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute boolean
        {"isEmpty",     PROP_isEmpty,     JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute boolean
        {"size",        PROP_size,        JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute vector
        {"volume",      PROP_volume,      JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute number
        {"area",        PROP_area,        JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute boolean
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSSphere::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSSphere::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSSphere::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}
// getproperty handling
JSBool
JSSphere::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_center:
                *vp = as_jsval(cx, getNative().center);
                return JS_TRUE;
            case PROP_radius:
                *vp = as_jsval(cx, getNative().radius);
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
                JS_ReportError(cx,"JSSphere::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
};

// setproperty handling
JSBool
JSSphere::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    jsval dummy;
    switch (theID) {
        case PROP_center:
            return Method<NATIVE>::call(&NATIVE::setCenter, cx, obj, 1, vp, &dummy);
        case PROP_radius:
            return Method<NATIVE>::call(&NATIVE::setRadius, cx, obj, 1, vp, &dummy);
        default:
            JS_ReportError(cx,"JSSphere::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
};


JSBool
JSSphere::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs a sphere from center/radius, center/point on surface, from another sphere or an empty one.");
    DOC_PARAM("a sphere", "", DOC_TYPE_SPHERE);
    DOC_RESET;
    DOC_PARAM("center", "", DOC_TYPE_POINT3F);
    DOC_PARAM("radius", "", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("center", "", DOC_TYPE_POINT3F);
    DOC_PARAM("point on surface", "", DOC_TYPE_POINT3F);
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSSphere * myNewObject = 0;
    JSSphere::NativeValuePtr myNewValue = JSSphere::NativeValuePtr(new dom::SimpleValue<asl::Sphere<Number> >(0));
    asl::Sphere<Number> & myNewSphere = myNewValue->openWriteableValue(); // we close it only on success, otherwise we trash it anyway
    if (argc == 0) {
        // construct empty
        myNewObject=new JSSphere(myNewValue);
    } else {
        if (argc == 2) {
            JSObject * myObject = JSVector<asl::Point3<Number> >::Construct(cx, argv[0]);
            if (!myObject) {
                JS_ReportError(cx,"JSSphere::Constructor: first argument must be a normal vector of size 3",ClassName());
                return JS_FALSE;
            }
            jsdouble myNumber;
            if (JS_ValueToNumber(cx,argv[1],&myNumber) && !JSDOUBLE_IS_NaN(myNumber)) {
                // construct from center/radius
                myNewSphere = asl::Sphere<Number>(JSVector<asl::Point3<Number> >::getNativeRef(cx,myObject), Number(myNumber));
            } else {
                // construct from center/point on surface
                JSObject * myObject2 = JSVector<asl::Point3<Number> >::Construct(cx, argv[1]);
                if (!myObject2) {
                    JS_ReportError(cx,"JSSphere::Constructor: second argument must be a distance number or point on the plane of size 3",ClassName());
                    return JS_FALSE;
                }
                myNewSphere = asl::Sphere<Number>(JSVector<asl::Point3<Number> >::getNativeRef(cx,myObject),
                                                  JSVector<asl::Point3<Number> >::getNativeRef(cx,myObject2));
            }
            myNewObject=new JSSphere(myNewValue);
        } else if (argc == 1) {
            // construct from one Sphere
            if (JSVAL_IS_VOID(argv[0])) {
                JS_ReportError(cx,"JSSphere::Constructor: bad argument #1 (undefined)");
                return JS_FALSE;
            }
            JSObject * myArgument;
            if (!JS_ValueToObject(cx, argv[0], &myArgument)) {
                JS_ReportError(cx,"JSSphere::Constructor: bad argument type, Sphere expected");
                return JS_FALSE;
            }
            if (JSA_GetClass(cx,myArgument) == Class()) {
                myNewSphere = getJSWrapper(cx, myArgument).getNative();
                myNewObject=new JSSphere(myNewValue);
            }
        } else {
            JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 0,1 or 2, got %d",ClassName(), argc);
            return JS_FALSE;
        }
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        myNewValue->closeWriteableValue();
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSSphere::Constructor: bad parameters");
    return JS_FALSE;
}


JSObject *
JSSphere::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
    DOC_MODULE_CREATE("Math",JSSphere);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Sphere<Number>  & theSphere) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Sphere<Number> >::Class()) {
                theSphere = JSClassTraits<asl::Sphere<Number> >::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}
/*
jsval as_jsval(JSContext *cx, const asl::Sphere<Number>  & theValue) {
    JSObject * myReturnObject = JSSphere::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}
*/
jsval as_jsval(JSContext *cx, asl::Sphere<SphereNumber> theValue) {
    JSObject * myReturnObject = JSSphere::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSSphere::NativeValuePtr theValue) {
    JSObject * myObject = JSSphere::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}


template jsval as_jsval(JSContext *cx, const std::vector<asl::Sphere<float> > & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<asl::Sphere<float> > & theVector);

}
