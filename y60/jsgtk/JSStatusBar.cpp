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
#include "JSStatusBar.h"

#include "JSBox.h"
#include "JSSignalProxies.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Statusbar, asl::Ptr<Gtk::Statusbar>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::StatusBar@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
Push(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1, 2);
        // native method call
        Gtk::Statusbar * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        Glib::ustring theMessage;
        convertFrom(cx, argv[0], theMessage);
        int theContext(0);
        if (argc > 1) {
            convertFrom(cx, argv[1], theContext);
        }
        guint myRetVal = myNative->push(theMessage, theContext);
        *rval = as_jsval(cx, myRetVal);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
Pop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0, 1);
        // native method call
        Gtk::Statusbar * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        int theContext(0);
        if (argc > 0) {
            convertFrom(cx, argv[0], theContext);
        }
        myNative->pop(theContext);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
GetContextId(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        // native method call
        Gtk::Statusbar * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        Glib::ustring theContextId;
        convertFrom(cx, argv[0], theContextId);
        int myRetVal = myNative->get_context_id(theContextId);
        *rval = as_jsval(cx, myRetVal);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
RemoveMessage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1, 2);
        // native method call
        Gtk::Statusbar * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        guint theMessageId;
        convertFrom(cx, argv[0], theMessageId);

        guint theContextId = 0;
        if (argc > 1) {
            convertFrom(cx, argv[1], theContextId);
        }

        switch (argc) {
            case 1:
                myNative->remove_message(theMessageId);
                return JS_TRUE;
            case 2:
                myNative->remove_message(theMessageId, theContextId);
                return JS_TRUE;
        }
        return JS_FALSE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSStatusBar::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"push",                 Push,                    2},
        {"pop",                  Pop,                     1},
        {"remove_message",       RemoveMessage,           2},
        {"get_context_id",       GetContextId,            1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSStatusBar::Properties() {
    static JSPropertySpec myProperties[] = {
        {"has_resize_grip", PROP_has_resize_grip, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_text_pushed", PROP_signal_text_pushed, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_text_popped", PROP_signal_text_popped, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSStatusBar::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSStatusBar::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSStatusBar::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_has_resize_grip:
            *vp = as_jsval(cx, theNative.get_has_resize_grip());
            return JS_TRUE;
        case PROP_signal_text_pushed:
            {
                JSSignalProxy2<void,guint,const Glib::ustring &>::OWNERPTR mySignal( new
                        JSSignalProxy2<void,guint,const Glib::ustring &>::NATIVE(theNative.signal_text_pushed()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_text_popped:
            {
                JSSignalProxy2<void,guint,const Glib::ustring &>::OWNERPTR mySignal( new
                        JSSignalProxy2<void,guint,const Glib::ustring &>::NATIVE(theNative.signal_text_popped()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSStatusBar::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_has_resize_grip:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.set_has_resize_grip(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSStatusBar::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSStatusBar * myNewObject = 0;

    if (argc == 0) {
        newNative = new Gtk::Statusbar();
        myNewObject = new JSStatusBar(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSStatusBar::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSStatusBar::ConstIntProperties() {

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
JSStatusBar::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSBASE::ClassName());
}

JSObject *
JSStatusBar::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSStatusBar::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSStatusBar::OWNERPTR theOwner, JSStatusBar::NATIVE * theNative) {
    JSObject * myReturnObject = JSStatusBar::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}
