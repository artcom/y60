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
#include "JSHistogram.h"

#include "JSSignal1.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;
using namespace acgtk;

namespace jslib {

template class JSWrapper<acgtk::Histogram, asl::Ptr<acgtk::Histogram>,
        StaticAccessProtocol>;

typedef acgtk::Histogram NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = "Histogram";
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
SetWindowCenter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::setWindowCenter,cx,obj,argc,argv,rval);

}

static JSBool
SetWindowWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::setWindowWidth,cx,obj,argc,argv,rval);

}

static JSBool
SetLower(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::setLower,cx,obj,argc,argv,rval);

}

static JSBool
SetUpper(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::setUpper,cx,obj,argc,argv,rval);

}

static JSBool
SetHistogram(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::setHistogram,cx,obj,argc,argv,rval);

}

JSFunctionSpec *
JSHistogram::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"setWindowCenter",      SetWindowCenter,         1},
        {"setWindowWidth",       SetWindowWidth,          1},
        {"setLower",             SetLower,                1},
        {"setUpper",             SetUpper,                1},
        {"setHistogram",         SetHistogram,            1},
        {0}
    };
    return myFunctions;
}

#define DEFINE_PROPERTY( NAME ) \
    { #NAME, PROP_ ## NAME,  JSPROP_ENUMERATE|JSPROP_PERMANENT}

#define DEFINE_RO_PROPERTY( NAME ) \
    { #NAME, PROP_ ## NAME,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT}

JSPropertySpec *
JSHistogram::Properties() {
    static JSPropertySpec myProperties[] = {
        DEFINE_PROPERTY(show_window),
        DEFINE_PROPERTY(show_window_center),
        DEFINE_PROPERTY(value_range),
        DEFINE_PROPERTY(logarithmic_scale),
        DEFINE_PROPERTY(mode),
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSHistogram::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSHistogram::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSHistogram::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_show_window:
            *vp = as_jsval(cx, theNative.getShowWindow());
            return JS_TRUE;
        case PROP_show_window_center:
            *vp = as_jsval(cx, theNative.getShowWindowCenter());
            return JS_TRUE;
        case PROP_value_range:
            *vp = as_jsval(cx, theNative.getValueRange());
            return JS_TRUE;
        case PROP_logarithmic_scale:
            *vp = as_jsval(cx, theNative.getLogarithmicScale());
            return JS_TRUE;
        case PROP_mode:
            *vp = as_jsval(cx, theNative.getMode());
            return JS_TRUE;
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSHistogram::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_show_window:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.setShowWindow(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_show_window_center:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.setShowWindowCenter(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_value_range:
            try {
                Vector2f myValueRange;
                convertFrom(cx, *vp, myValueRange);
                theNative.setValueRange(myValueRange);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_logarithmic_scale:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.setLogarithmicScale(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_mode:
            try {
                int myMode;
                convertFrom(cx, *vp, myMode);
                theNative.setMode(static_cast<Histogram::Mode>(myMode));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSHistogram::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;
    JSHistogram * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
    } else {
        JS_ReportError(cx,"Constructor for %s takes one or zero params, got %d.",ClassName(), argc);
        return JS_FALSE;
    }
    OWNERPTR newOwner(newNative);
    myNewObject = new JSHistogram(newOwner, newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSHistogram::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSHistogram::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {"MODE_CENTER_WIDTH",   PROP_MODE_CENTER_WIDTH, acgtk::Histogram::MODE_CENTER_WIDTH},
        {"MODE_LOWER_UPPER",    PROP_MODE_LOWER_UPPER, acgtk::Histogram::MODE_LOWER_UPPER},
        {0}
    };
    return myProperties;
};

void
JSHistogram::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);

    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    //TODO - multiple inheritance
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSBASE::ClassName());
}

JSObject *
JSHistogram::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSHistogram::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSHistogram::NATIVE * theNative) {
    JSObject * myReturnObject = JSHistogram::Construct(cx, JSHistogram::OWNERPTR(), theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<Histogram> & theHistogram) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSHistogram::NATIVE >::Class()) {
                theHistogram = JSClassTraits<JSHistogram::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

}

