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
#include "JSCWRuler.h"

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

template class JSWrapper<acgtk::CWRuler, asl::Ptr<acgtk::CWRuler>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = "CWRuler";
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSCWRuler::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

#define DEFINE_PROPERTY( NAME ) \
    { #NAME, PROP_ ## NAME,  JSPROP_ENUMERATE|JSPROP_PERMANENT}

#define DEFINE_RO_PROPERTY( NAME ) \
    { #NAME, PROP_ ## NAME,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT}

JSPropertySpec *
JSCWRuler::Properties() {
    static JSPropertySpec myProperties[] = {
        DEFINE_PROPERTY(value_range),
        DEFINE_PROPERTY(window_center),
        DEFINE_PROPERTY(window_width),
        DEFINE_PROPERTY(lower),
        DEFINE_PROPERTY(upper),
        DEFINE_PROPERTY(mode),
        DEFINE_RO_PROPERTY(signal_center_changed),
        DEFINE_RO_PROPERTY(signal_width_changed),
        DEFINE_RO_PROPERTY(signal_lower_changed),
        DEFINE_RO_PROPERTY(signal_upper_changed),
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSCWRuler::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCWRuler::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCWRuler::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_value_range:
            *vp = jslib::as_jsval(cx, theNative.getValueRange());
            return JS_TRUE;
        case PROP_window_center:
            *vp = jslib::as_jsval(cx, theNative.getWindowCenter());
            return JS_TRUE;
        case PROP_window_width:
            *vp = jslib::as_jsval(cx, theNative.getWindowWidth());
            return JS_TRUE;
        case PROP_lower:
            *vp = jslib::as_jsval(cx, theNative.getLower());
            return JS_TRUE;
        case PROP_upper:
            *vp = jslib::as_jsval(cx, theNative.getUpper());
            return JS_TRUE;
        case PROP_mode:
            *vp = jslib::as_jsval(cx, static_cast<int>(theNative.getMode()));
            return JS_TRUE;
        case PROP_signal_center_changed:
            {
                JSSignal1<void,float>::OWNERPTR mySignal( new
                        JSSignal1<void,float>::NATIVE(theNative.signal_center_changed()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_width_changed:
            {
                JSSignal1<void,float>::OWNERPTR mySignal( new
                        JSSignal1<void,float>::NATIVE(theNative.signal_width_changed()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_lower_changed:
            {
                JSSignal1<void,float>::OWNERPTR mySignal( new
                        JSSignal1<void,float>::NATIVE(theNative.signal_lower_changed()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_upper_changed:
            {
                JSSignal1<void,float>::OWNERPTR mySignal( new
                        JSSignal1<void,float>::NATIVE(theNative.signal_upper_changed()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSCWRuler::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_value_range:
            try {
                asl::Vector2f myValueRange;
                convertFrom(cx, *vp, myValueRange);
                theNative.setValueRange(myValueRange);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_window_center:
            try {
                float myWindowCenter;
                convertFrom(cx, *vp, myWindowCenter);
                theNative.setWindowCenter(myWindowCenter);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_window_width:
            try {
                float myWindowWidth;
                convertFrom(cx, *vp, myWindowWidth);
                theNative.setWindowWidth(myWindowWidth);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_lower:
            try {
                float myLower;
                convertFrom(cx, *vp, myLower);
                theNative.setLower(myLower);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_upper:
            try {
                float myUpper;
                convertFrom(cx, *vp, myUpper);
                theNative.setUpper(myUpper);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_mode:
            try {
                int myMode;
                convertFrom(cx, *vp, myMode);
                theNative.setMode(static_cast<CWRuler::Mode>(myMode));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSCWRuler::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;
    JSCWRuler * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
    } else if (argc == 1) {
        int myMode;
        convertFrom(cx, argv[0], myMode);
        newNative = new NATIVE(static_cast<CWRuler::Mode>(myMode));
    } else {
        JS_ReportError(cx,"Constructor for %s takes one or zero params, got %d.",ClassName(), argc);
        return JS_FALSE;
    }
    OWNERPTR newOwner(newNative);
    myNewObject = new JSCWRuler(newOwner, newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSCWRuler::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSCWRuler::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {"MODE_THRESHOLD",    PROP_MODE_THRESHOLD, acgtk::CWRuler::MODE_THRESHOLD},
        {"MODE_CENTER_WIDTH", PROP_MODE_CENTER_WIDTH, acgtk::CWRuler::MODE_CENTER_WIDTH},
        {"MODE_LOWER_UPPER",  PROP_MODE_LOWER_UPPER, acgtk::CWRuler::MODE_LOWER_UPPER},
        {0}
    };
    return myProperties;
};

void
JSCWRuler::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);

    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    //TODO - multiple inheritance
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSBASE::ClassName());
}

JSObject *
JSCWRuler::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSCWRuler::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSCWRuler::NATIVE * theNative) {
    JSObject * myReturnObject = JSCWRuler::Construct(cx, JSCWRuler::OWNERPTR(), theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<CWRuler> & theCWRuler) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSCWRuler::NATIVE >::Class()) {
                theCWRuler = JSClassTraits<JSCWRuler::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

}

