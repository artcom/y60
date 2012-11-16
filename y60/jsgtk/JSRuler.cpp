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
#include "JSRuler.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Ruler, asl::Ptr<Gtk::Ruler>, StaticAccessProtocol>;

static JSBool
Set_Range(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 4);
        // native method call
        Gtk::Ruler * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        float theLower;
        float theUpper;
        float thePosition;
        float theMaxSize;

        convertFrom(cx, argv[0], theLower);
        convertFrom(cx, argv[1], theUpper);
        convertFrom(cx, argv[2], thePosition);
        convertFrom(cx, argv[3], theMaxSize);
        myNative->set_range(theLower, theUpper, thePosition, theMaxSize);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Ruler@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSRuler::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"set_range",            Set_Range,               4},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSRuler::Properties() {
    static JSPropertySpec myProperties[] = {
        {"metric", PROP_metric, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"upper",  PROP_upper, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"lower",  PROP_lower, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"position", PROP_position, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"max_size", PROP_max_size, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSRuler::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSRuler::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSRuler::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    double myUpper, myLower, myPosition, myMaxSize;
    theNative.get_range(myLower, myUpper, myPosition, myMaxSize);
    switch (theID) {
        case PROP_metric:
            *vp = as_jsval(cx, static_cast<int>(theNative.get_metric()));
            return JS_TRUE;
        case PROP_upper:
            *vp = as_jsval(cx, myUpper);
            return JS_TRUE;
        case PROP_lower:
            *vp = as_jsval(cx, myLower);
            return JS_TRUE;
        case PROP_position:
            *vp = as_jsval(cx, myPosition);
            return JS_TRUE;
        case PROP_max_size:
            *vp = as_jsval(cx, myMaxSize);
            return JS_TRUE;
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSRuler::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    double myUpper, myLower, myPosition, myMaxSize;
    theNative.get_range(myLower, myUpper, myPosition, myMaxSize);
    switch (theID) {
        case PROP_metric:
            try {
                int theMetric;
                convertFrom(cx, *vp, theMetric);
                theNative.set_metric(static_cast<Gtk::MetricType>(theMetric));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_upper:
            try {
                convertFrom(cx, *vp, myUpper);
                theNative.set_range(myLower, myUpper, myPosition, myMaxSize);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_lower:
            try {
                convertFrom(cx, *vp, myLower);
                theNative.set_range(myLower, myUpper, myPosition, myMaxSize);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_position:
            try {
                convertFrom(cx, *vp, myPosition);
                theNative.set_range(myLower, myUpper, myPosition, myMaxSize);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_max_size:
            try {
                convertFrom(cx, *vp, myMaxSize);
                theNative.set_range(myLower, myUpper, myPosition, myMaxSize);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSRuler::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSRuler * myNewObject = 0;

    if (argc == 0) {
        newNative = 0; // Abstract
        myNewObject = new JSRuler(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSRuler::Constructor: bad parameters");
    return JS_FALSE;
}

#define DEFINE_PROP( NAME ) { #NAME, PROP_ ## NAME, Gtk::NAME}

JSConstIntPropertySpec *
JSRuler::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_PROP(PIXELS),
        DEFINE_PROP(INCHES),
        DEFINE_PROP(CENTIMETERS),
       {0}
    };
    return myProperties;
};

void
JSRuler::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddProperties(cx, theClassProto, Properties());
    JSA_AddFunctions(cx, theClassProto, Functions());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            ConstIntProperties(), 0, 0, JSBASE::ClassName());
}

JSObject *
JSRuler::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, 0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSRuler::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSRuler::OWNERPTR theOwner, JSRuler::NATIVE * theNative) {
    JSObject * myReturnObject = JSRuler::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

