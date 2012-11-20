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
#include "JSButton.h"

#include "JSSignalProxies.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Button, asl::Ptr<Gtk::Button>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Button@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
Pressed(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Gtk::Button * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
    myNative->pressed();
    return JS_TRUE;
}

static JSBool
Released(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Gtk::Button * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
    myNative->released();
    return JS_TRUE;
}

static JSBool
Clicked(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Gtk::Button * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
    myNative->clicked();
    return JS_TRUE;
}

static JSBool
Enter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Gtk::Button * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
    myNative->enter();
    return JS_TRUE;
}

static JSBool
Leave(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Gtk::Button * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
    myNative->leave();
    return JS_TRUE;
}

JSFunctionSpec *
JSButton::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"pressed",              Pressed,                 0},
        {"released",             Released,                0},
        {"clicked",              Clicked,                 0},
        {"enter",                Enter,                   0},
        {"leave",                Leave,                   0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSButton::Properties() {
    static JSPropertySpec myProperties[] = {
        {"label",           PROP_label,           JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"use_underline",   PROP_use_underline,   JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"use_stock",       PROP_use_stock,       JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"focus_on_click",  PROP_focus_on_click,  JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"alignment",       PROP_alignment,       JSPROP_ENUMERATE|JSPROP_PERMANENT},
        // use add instead
        //{"image",           PROP_image,           JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_pressed",  PROP_signal_pressed,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_released", PROP_signal_released, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_clicked",  PROP_signal_clicked,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_enter",    PROP_signal_enter,    JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_leave",    PROP_signal_leave,    JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_activate", PROP_signal_activate, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSButton::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSButton::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSButton::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_relief:
            *vp = as_jsval(cx, static_cast<int>(theNative.get_relief()));
            return JS_TRUE;
        case PROP_label:
            *vp = as_jsval(cx, theNative.get_label());
            return JS_TRUE;
        case PROP_use_underline:
            *vp = as_jsval(cx, theNative.get_use_underline());
            return JS_TRUE;
        case PROP_use_stock:
            *vp = as_jsval(cx, theNative.get_use_stock());
            return JS_TRUE;
        case PROP_focus_on_click:
            *vp = as_jsval(cx, theNative.get_focus_on_click());
            return JS_TRUE;
        case PROP_alignment:
            {
                float myXAlign, myYAlign;
                theNative.get_alignment(myXAlign, myYAlign);
                asl::Vector2f myAlignment(myXAlign, myYAlign);
                *vp = as_jsval(cx, myAlignment);
                return JS_TRUE;
            }
        /*
        case PROP_image:
            *vp = as_jsval(cx, theNative.get_image());
            return JS_TRUE;
        */
        case PROP_signal_pressed:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_pressed()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_released:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_released()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_clicked:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_clicked()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_enter:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_enter()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_leave:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_leave()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_activate:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_activate()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSButton::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_relief:
            try {
                int myReliefStyle; // Gtk::ReliefStyle
                convertFrom(cx, *vp, myReliefStyle);
                theNative.set_relief(static_cast<Gtk::ReliefStyle>(myReliefStyle));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_label:
            try {
                Glib::ustring myLabel;
                convertFrom(cx, *vp, myLabel);
                theNative.set_label(myLabel);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_use_underline:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.set_use_underline(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_use_stock:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.set_use_stock(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_focus_on_click:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.set_focus_on_click(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_alignment:
            try {
                asl::Vector2f myAlignment;
                convertFrom(cx, *vp, myAlignment);
                theNative.set_alignment(myAlignment[0], myAlignment[1]);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        /*
        case PROP_image:
            try {
                Gtk::Widget * myImageWidget;
                convertFrom(cx, *vp, myImageWidget);
                theNative.set_image( * myImageWidget);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        */
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSButton::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSButton * myNewObject = 0;

    if (argc == 0) {
        newNative = new Gtk::Button();
    } else if (argc == 1) {
        Glib::ustring myLable;
        convertFrom(cx, argv[0], myLable);

        newNative = new Gtk::Button(myLable);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }
    myNewObject = new JSButton(OWNERPTR(newNative), newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSButton::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSButton::ConstIntProperties() {

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
JSButton::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
                JSBASE::ClassName());
}

JSObject *
JSButton::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSButton::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSButton::OWNERPTR theOwner, JSButton::NATIVE * theNative) {
    JSObject * myReturnObject = JSButton::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}


