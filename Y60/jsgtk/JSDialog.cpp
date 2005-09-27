//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSDialog.cpp,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/04/21 16:25:02 $
//
//
//=============================================================================

#include "JSWindow.h"
#include "JSDialog.h"
#include "JSStockID.h"
#include "JSSignalProxies.h"
#include "jsgtk.h"
#include "JSVBox.h"
#include <y60/JScppUtils.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

typedef Gtk::Dialog NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = JSDialog::getJSWrapper(cx,obj).getNative().get_title();
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
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
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSDialog::Properties() {
    static JSPropertySpec myProperties[] = {
        {"signal_response", PROP_signal_response, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"vbox", PROP_vbox, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
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
                * vp = as_jsval(cx, Ptr<Gtk::VBox>(0), theNative.get_vbox());
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
        case 0:
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
    } else if (argc == 2 ) {
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
        AC_WARNING << "Creating dialog with parent.";

        newNative = new NATIVE(myTitle, *myParent);
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

