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
#include "JSSpinButton.h"

#include "JSSignalProxies.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::SpinButton, asl::Ptr<Gtk::SpinButton>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::SpinButton@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSSpinButton::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSSpinButton::Properties() {
    static JSPropertySpec myProperties[] = {
        {"digits", PROP_digits,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"increments", PROP_increments,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"range", PROP_range,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"value", PROP_value,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_value_changed", PROP_signal_value_changed, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSSpinButton::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSSpinButton::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSSpinButton::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_digits:
            *vp = as_jsval(cx, theNative.get_digits());
            return JS_TRUE;
        case PROP_increments:
            try {
                double myStep = 0;
                double myPage = 0;
                theNative.get_increments(myStep, myPage);
                Vector2f myIncrements = Vector2f(float(myStep), float(myPage));
                *vp = as_jsval(cx, myIncrements);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_range:
            try {
                double myMin = 0;
                double myMax = 0;
                theNative.get_range(myMin, myMax);
                Vector2f myRange = Vector2f(float(myMin), float(myMax));
                *vp = as_jsval(cx, myRange);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_value:
            *vp = as_jsval(cx, theNative.get_value());
            return JS_TRUE;
        case PROP_signal_value_changed:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_value_changed()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSSpinButton::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_digits:
            try {
                unsigned theDigits;
                convertFrom(cx, *vp, theDigits);
                theNative.set_digits(theDigits);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION
        case PROP_increments:
            try {
                asl::Vector2f myIncrements;
                convertFrom(cx, *vp, myIncrements);
                theNative.set_increments(myIncrements[0], myIncrements[1]);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION
        case PROP_range:
            try {
                asl::Vector2f myRange;
                convertFrom(cx, *vp, myRange);
                theNative.set_range(myRange[0], myRange[1]);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION
        case PROP_value:
            try {
                double theValue;
                convertFrom(cx, *vp, theValue);
                theNative.set_value(theValue);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSSpinButton::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSSpinButton * myNewObject = 0;
    double myClimbRate;
    unsigned int myDigits;

    switch (argc) {
        case 0:
            newNative = new Gtk::SpinButton();
            break;
        case 1:
            if (! convertFrom(cx, argv[0], myClimbRate)) {
                JS_ReportError(cx,"Constructor for %s: bad arguments 1: expected float",ClassName());
                return JS_FALSE;
            }
            newNative = new Gtk::SpinButton(myClimbRate);
            break;
        case 2:
            if (! convertFrom(cx, argv[0], myClimbRate)) {
                JS_ReportError(cx,"Constructor for %s: bad arguments 1: expected float",ClassName());
                return JS_FALSE;
            }
            if (! convertFrom(cx, argv[1], myDigits)) {
                JS_ReportError(cx,"Constructor for %s: bad arguments 2: expected unsigned int",ClassName());
                return JS_FALSE;
            }
            newNative = new Gtk::SpinButton(myClimbRate, myDigits);
            break;
        default:
            JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
            return JS_FALSE;
    }
    myNewObject = new JSSpinButton(OWNERPTR(newNative), newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSSpinButton::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSSpinButton::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
            "POST",              PROP_HTTP_POST,         Request::HTTP_POST,
            "GET",               PROP_HTTP_GET,          Request::HTTP_GET,
            "PUT",               PROP_HTTP_PUT,          Request::HTTP_PUT,
        {0}
    };
    return myProperties;
};
*/

void
JSSpinButton::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSBASE::ClassName());
}

JSObject *
JSSpinButton::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSSpinButton::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSSpinButton::OWNERPTR theOwner, JSSpinButton::NATIVE * theNative) {
    JSObject * myReturnObject = JSSpinButton::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}


