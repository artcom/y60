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
//   $RCSfile: JSGtkMain.cpp,v $
//   $Author: christian $
//   $Revision: 1.8 $
//   $Date: 2005/04/28 17:29:13 $
//
//
//=============================================================================

#include "JSGtkMain.h"
#include "JSWindow.h"
#include "jsgtk.h"

#include <y60/JScppUtils.h>
#include <iostream>
#include <gtkmm/main.h>

using namespace std;
using namespace asl;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = std::string("Gtk::Main");
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
run(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        Gtk::Window * myMainWindow = 0;

        for (unsigned i = 0; i < argc; ++i) {
            if (JSVAL_IS_VOID(argv[i])) {
                JS_ReportError(cx, "Gtk::Main::run(): Argument #%d is undefined", i + 1);
                return JS_FALSE;
            }
        }

        if (argc == 1) {
            if (!convertFrom(cx, argv[0], myMainWindow)) {
                JS_ReportError(cx, "Gtk::Main::run(): argument #1 must be a Gtk::Window");
                return JS_FALSE;
            }
        } else if (argc > 1) {
            JS_ReportError(cx, "Gtk::Main::run(): Wrong number of arguments, 0 or 1 expected.");
            return JS_FALSE;
        }
        if (myMainWindow) {
            Gtk::Main::run(*myMainWindow);
        } else {
            Gtk::Main::run();
        }

        *rval = as_jsval(cx, 0);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
iteration(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        bool myBlockingFlag = true;

        for (unsigned i = 0; i < argc; ++i) {
            if (JSVAL_IS_VOID(argv[i])) {
                JS_ReportError(cx, "Gtk::Main::iteration(): Argument #%d is undefined", i + 1);
                return JS_FALSE;
            }
        }

        if (argc == 1) {
            if (!convertFrom(cx, argv[0], myBlockingFlag)) {
                JS_ReportError(cx, "Gtk::Main::iteration(): argument #1 must be a bool");
                return JS_FALSE;
            }
        } else if (argc > 1) {
            JS_ReportError(cx, "JSWindow::add(): Wrong number of arguments, 1 expected.");
            return JS_FALSE;
        }
        Gtk::Main::iteration(myBlockingFlag);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
events_pending(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);

        bool myPendingFlag = Gtk::Main::events_pending();
        *rval = as_jsval(cx, myPendingFlag);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
quit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0, 0);

        Gtk::Main::quit();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSGtkMain::StaticFunctions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"run",                  run,                     1},
        {"iteration",            iteration,               1},
        {"events_pending",       events_pending,          0},
        {"quit",                 quit,                    0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSGtkMain::StaticProperties() {
    static JSPropertySpec myProperties[] = {
//        {"responseCode", PROP_responseCode, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
//        {"responseString", PROP_responseString, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
//        {"errorString", PROP_errorString, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
//        {"URL", PROP_URL, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

JSBool
JSGtkMain::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    JSGtkMain::OWNERPTR myNewObject;

    std::string myGladeFilename;
    std::string myWidgetID;
    std::string myDomain;

    if (argc == 0) {
        myNewObject = OWNERPTR(new NATIVE);
    } else {
            JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none %d",ClassName(), argc);
            return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,new JSGtkMain(myNewObject, &(*myNewObject)));
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSGtkMain::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSGtkMain::initClass(JSContext *cx, JSObject *theGlobalObject) {
    return Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
            0, 0, 0, StaticProperties(), StaticFunctions());
}

bool convertFrom(JSContext *cx, jsval theValue, JSGtkMain::OWNERPTR & theMainInstance) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSGtkMain::NATIVE >::Class()) {
                theMainInstance = JSClassTraits<JSGtkMain::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSGtkMain::OWNERPTR theOwner, JSGtkMain::NATIVE * theMain) {
    JSObject * myReturnObject = JSGtkMain::Construct(cx, theOwner, theMain);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}


