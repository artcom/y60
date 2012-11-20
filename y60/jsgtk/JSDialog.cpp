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
#include "JSDialog.h"

#include "JSWindow.h"
#include "JSStockID.h"
#include "JSSignalProxies.h"
#include "JSButton.h"
#include "jsgtk.h"
#include "JSVBox.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Dialog, asl::Ptr<Gtk::Dialog>, StaticAccessProtocol>;

typedef Gtk::Dialog NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = JSDialog::getJSWrapper(cx,obj).getNative().get_title();
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
Run(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        // native method call
        Gtk::Dialog * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        int myRetVal = myNative->run();
        *rval = as_jsval(cx, myRetVal);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
Set_response_sensitive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        // native method call
        Gtk::Dialog * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        int myResponseId;
        convertFrom(cx, argv[0], myResponseId);
        bool isSensitive;
        convertFrom(cx, argv[1], isSensitive);

        myNative->set_response_sensitive(myResponseId, isSensitive);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
Set_default_response(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        // native method call
        Gtk::Dialog * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        int myResponseId;
        convertFrom(cx, argv[0], myResponseId);

        myNative->set_default_response(myResponseId);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
Add_button(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        // native method call
        Gtk::Dialog * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        JSObject * myArgument;
        if (JS_ValueToObject(cx, argv[0], &myArgument)) {

            int myResponseId;
            convertFrom(cx, argv[1], myResponseId);

            Gtk::Button * myButton = 0;
            if (JSA_GetClass(cx,myArgument) == JSStockID::Class()) {
                Gtk::StockID  * myButtonStockId(0);
                convertFrom(cx, argv[0], myButtonStockId);
                myButton = myNative->add_button( * myButtonStockId, myResponseId);
            } else {
                string myButtonLabel;
                convertFrom(cx, argv[0], myButtonLabel);
                myButton = myNative->add_button(myButtonLabel, myResponseId);
            }

            *rval = as_jsval(cx, myButton);
            return JS_TRUE;
        }

    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

JSFunctionSpec *
JSDialog::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"run",                  Run,                     0},
        {"add_button",           Add_button,              2},
        {"set_response_sensitive",Set_response_sensitive, 2},
        {"set_default_response",  Set_default_response,   1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSDialog::Properties() {
    static JSPropertySpec myProperties[] = {
        {"signal_response", PROP_signal_response, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"vbox", PROP_vbox, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"has_separator", PROP_has_separator, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSDialog::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSDialog::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSDialog::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_vbox:
            {
                * vp = as_jsval(cx, asl::Ptr<Gtk::VBox>(), theNative.get_vbox());
                return JS_TRUE;
            }
        case PROP_has_separator:
            {
                * vp = as_jsval(cx, theNative.get_has_separator());
                return JS_TRUE;
            }
        case PROP_signal_response:
            {
                JSSignalProxy1<void, int>::OWNERPTR mySignal( new
                        JSSignalProxy1<void, int>::NATIVE(theNative.signal_response()));
                * vp = as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSDialog::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_has_separator:
            try {
                bool myHasSeparatorFlag;
                convertFrom(cx, *vp, myHasSeparatorFlag);
                theNative.set_has_separator(myHasSeparatorFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}


JSBool
JSDialog::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSDialog * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
        // newNative->set_events(Gdk::ALL_EVENTS_MASK);

        myNewObject = new JSDialog(OWNERPTR(newNative), newNative);
    } else if (argc >= 2 ) {
        Glib::ustring myTitle;
        if ( ! convertFrom(cx, argv[0], myTitle)) {
            JS_ReportError(cx,"Constructor for %s: Argument 1 must be a string.",ClassName());
            return JS_FALSE;
        }

        Gtk::Window * myParent(0);
        if ( ! convertFrom(cx, argv[1], myParent)) {
            JS_ReportError(cx,"Constructor for %s: Argument 2 must be Gtk::Window.",ClassName());
            return JS_FALSE;
        }
        if ( ! myParent ) {
            JS_ReportError(cx,"Constructor for %s: Failed to get parent window.",ClassName());
            return JS_FALSE;
        }
        bool myModal = false;
        if (argc >= 3) {
            if ( ! convertFrom(cx, argv[2], myModal)) {
                JS_ReportError(cx,"Constructor for %s: Argument 3 must be bool",ClassName());
                return JS_FALSE;
            }
        }
        newNative = new NATIVE(myTitle, *myParent, myModal);
        myNewObject = new JSDialog(OWNERPTR(newNative), newNative);

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSDialog::Constructor: bad parameters");
    return JS_FALSE;
}

#define DEFINE_RESPONSE_PROP( NAME ) { "RESPONSE_" #NAME, PROP_RESPONSE_ ## NAME, Gtk::RESPONSE_ ## NAME}

JSConstIntPropertySpec *
JSDialog::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_RESPONSE_PROP(NONE),
        DEFINE_RESPONSE_PROP(ACCEPT),
        DEFINE_RESPONSE_PROP(DELETE_EVENT),
        DEFINE_RESPONSE_PROP(OK),
        DEFINE_RESPONSE_PROP(CANCEL),
        DEFINE_RESPONSE_PROP(CLOSE),
        DEFINE_RESPONSE_PROP(YES),
        DEFINE_RESPONSE_PROP(NO),
        DEFINE_RESPONSE_PROP(APPLY),
        DEFINE_RESPONSE_PROP(HELP),
       {0}
    };
    return myProperties;
};


void
JSDialog::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), ConstIntProperties(), 0, 0, JSBASE::ClassName());
}

JSObject *
JSDialog::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSDialog::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSDialog::OWNERPTR theOwner, JSDialog::NATIVE * theNative) {
    JSObject * myReturnObject = JSDialog::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

