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
#include "JSEntry.h"

#include "JSSignalProxies.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Entry, asl::Ptr<Gtk::Entry>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Entry@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}


// from Gtk::Editable
static JSBool
select_region(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Gtk::Entry * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

    int myStartPos;
    convertFrom(cx, argv[0], myStartPos);

    int myEndPos;
    convertFrom(cx, argv[1], myEndPos);

    myNative->select_region(myStartPos, myEndPos);
    return JS_TRUE;
}

JSFunctionSpec *
JSEntry::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"select_region",        select_region,           2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSEntry::Properties() {
    static JSPropertySpec myProperties[] = {
        {"visibility", PROP_visibility,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"has_frame", PROP_has_frame,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"max_length", PROP_max_length,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"activates_default", PROP_activates_default,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"width_chars", PROP_width_chars,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"text", PROP_text,     JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_changed",  PROP_signal_changed,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_activate", PROP_signal_activate,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_editing_done",  PROP_signal_editing_done,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSEntry::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSEntry::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSEntry::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_visibility:
            *vp = as_jsval(cx, theNative.get_visibility());
            return JS_TRUE;
        case PROP_has_frame:
            *vp = as_jsval(cx, theNative.get_has_frame());
            return JS_TRUE;
        case PROP_max_length:
            *vp = as_jsval(cx, theNative.get_max_length());
            return JS_TRUE;
        case PROP_activates_default:
            *vp = as_jsval(cx, theNative.get_activates_default());
            return JS_TRUE;
        case PROP_width_chars:
            *vp = as_jsval(cx, theNative.get_width_chars());
            return JS_TRUE;
        case PROP_text:
            *vp = as_jsval(cx, theNative.get_text());
            return JS_TRUE;
        case PROP_signal_changed:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_changed()));
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
        case PROP_signal_editing_done:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_editing_done()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSEntry::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_visibility:
            try {
                bool theFlag;
                convertFrom(cx, *vp, theFlag);
                theNative.set_visibility(theFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_has_frame:
            try {
                bool theFlag;
                convertFrom(cx, *vp, theFlag);
                theNative.set_has_frame(theFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_max_length:
            try {
                int theMaxLength;
                convertFrom(cx, *vp, theMaxLength);
                theNative.set_max_length(theMaxLength);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_activates_default:
            try {
                bool theFlag;
                convertFrom(cx, *vp, theFlag);
                theNative.set_activates_default(theFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_width_chars:
            try {
                int theWidthChars;
                convertFrom(cx, *vp, theWidthChars);
                theNative.set_width_chars(theWidthChars);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_text:
            try {
                Glib::ustring theText;
                convertFrom(cx, *vp, theText);
                theNative.set_text(theText);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSEntry::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSEntry * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE; // Abstract
        myNewObject = new JSEntry(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSEntry::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSEntry::ConstIntProperties() {

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
JSEntry::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
            JSBASE::ClassName());
}

JSObject *
JSEntry::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSEntry::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSEntry::OWNERPTR theOwner, JSEntry::NATIVE * theNative) {
    JSObject * myReturnObject = JSEntry::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

