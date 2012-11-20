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
#include "JSProgressBar.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::ProgressBar, asl::Ptr<Gtk::ProgressBar>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::ProgressBar@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
pulse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Gtk::ProgressBar * myProgressBar=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myProgressBar);
        myProgressBar->pulse();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSProgressBar::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"pulse",                pulse,                   0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSProgressBar::Properties() {
    static JSPropertySpec myProperties[] = {
        {"text",        PROP_text,          JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"fraction",    PROP_fraction,      JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"pulse_step",  PROP_pulse_step,    JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"orientation", PROP_orientation,   JSPROP_ENUMERATE|JSPROP_PERMANENT},
        //{"ellipsize",   PROP_ellipsize,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSProgressBar::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSProgressBar::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSProgressBar::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_text:
            *vp = as_jsval(cx, theNative.get_text());
            return JS_TRUE;
        case PROP_fraction:
            *vp = as_jsval(cx, theNative.get_fraction());
            return JS_TRUE;
        case PROP_pulse_step:
            *vp = as_jsval(cx, theNative.get_pulse_step());
            return JS_TRUE;
        case PROP_orientation:
            *vp = as_jsval(cx, static_cast<int>(theNative.get_orientation()));
            return JS_TRUE;
        /*
        case PROP_ellipsize:
            *vp = as_jsval(cx, static_cast<int>(theNative.get_ellipsize()));
            return JS_TRUE;
        */
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSProgressBar::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_text:
            try {
                Glib::ustring myText;
                convertFrom(cx, *vp, myText);
                theNative.set_text(myText);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_fraction:
            try {
                double myFraction;
                convertFrom(cx, *vp, myFraction);
                theNative.set_fraction(myFraction);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_pulse_step:
            try {
                double myPulseStep;
                convertFrom(cx, *vp, myPulseStep);
                theNative.set_pulse_step(myPulseStep);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_orientation:
            try {
                int myOrientation;
                convertFrom(cx, *vp, myOrientation);
                theNative.set_orientation(static_cast<Gtk::ProgressBarOrientation>(myOrientation));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        /*
        case PROP_ellipsize:
            try {
                int myEllipsizeMode;
                convertFrom(cx, *vp, myEllipsizeMode);
                theNative.set_ellipsize(static_cast<Gtk::EllipsizeMode>(myEllipsizeMode));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
            */
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSProgressBar::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSProgressBar * myNewObject = 0;

    if (argc == 0) {
        newNative = new Gtk::ProgressBar();
        myNewObject = new JSProgressBar(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSProgressBar::Constructor: bad parameters");
    return JS_FALSE;
}

#define DEFINE_PROGRESS_PROP( NAME ) { "PROGRESS_" #NAME, PROP_PROGRESS_ ## NAME, Gtk::PROGRESS_ ## NAME}

JSConstIntPropertySpec *
JSProgressBar::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_PROGRESS_PROP(LEFT_TO_RIGHT),
        DEFINE_PROGRESS_PROP(RIGHT_TO_LEFT),
        DEFINE_PROGRESS_PROP(BOTTOM_TO_TOP),
        DEFINE_PROGRESS_PROP(TOP_TO_BOTTOM),
       {0}
    };
    return myProperties;
};

void
JSProgressBar::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            ConstIntProperties(), 0, 0, JSBASE::ClassName());
}

JSObject *
JSProgressBar::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSProgressBar::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSProgressBar::OWNERPTR theOwner, JSProgressBar::NATIVE * theNative) {
    JSObject * myReturnObject = JSProgressBar::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}


