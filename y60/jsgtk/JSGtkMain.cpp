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
#include "JSGtkMain.h"

#include "JSWindow.h"
#include "JSSigConnection.h"
#include "jsgtk.h"
#include "JSSignal0.h"
#include "JSSignalProxy0.h"
#include "JSSignal1.h"

#include <y60/acgtk/GCObserver.h>
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

#if defined(_MSC_VER)
    // supress warnings caused by gtk in vc++
    #pragma warning(push,1)
    //#pragma warning(disable:4413 4244 4512 4250)
#endif //defined(_MSC_VER)
#include <gtkmm/main.h>
#include <sigc++/slot.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<int, asl::Ptr<int>, StaticAccessProtocol>;

asl::Ptr<MessageAcceptor<LocalPolicy> > JSGtkMain::ourAppAcceptor;
sigc::connection JSGtkMain::ourAcceptorTimeout;
JSGtkMain::OtherInstanceSignal JSGtkMain::ourOtherInstanceSignal;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = std::string("Gtk::Main");
    *rval = as_jsval(cx, myStringRep);
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

static JSBool
connect_timeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Install Gtk timeout callbacks");
    DOC_END;
    try {
        if (argc != 3) {
            JS_ReportError(cx, "GtkMain.connect_timeout(obj, func, msecs) needs three arguments.");
            return JS_FALSE;
        }
        JSObject * myTarget;
        if ( ! convertFrom(cx, argv[0], myTarget)) {
            JS_ReportError(cx, "GtkMain.connect_timeout() first argument is not an object.");
            return JS_FALSE;
        }

        Glib::ustring myMethodName;
        if ( ! convertFrom(cx, argv[1], myMethodName)) {
            JS_ReportError(cx, "GtkMain.connect_timeout() second argument is not a string.");
            return JS_FALSE;
        }

        unsigned myInterval;
        if ( ! convertFrom(cx, argv[2], myInterval)) {
            JS_ReportError(cx, "GtkMain.connect_timeout() third argument is not a number.");
            return JS_FALSE;
        }

        JSSigConnection::OWNERPTR myConnection = JSSigConnection::OWNERPTR(new sigc::connection);
        *myConnection = Glib::signal_timeout().connect( sigc::bind<JSContext*, JSObject*, std::string>(
                sigc::ptr_fun( & JSGtkMain::on_timeout ), cx, myTarget, myMethodName), myInterval);

        // register our target object with the GCObserver
        GCObserver::FinalizeSignal myFinalizer = GCObserver::get().watchObject(myTarget);
        // now add our cleanup code to the finalize signal,
        // binding the connection as an extra argument
        myFinalizer.connect(sigc::bind<sigc::connection>(
                    sigc::ptr_fun( & JSSignalAdapter0<bool>::on_target_finalized),
                    *myConnection));

        *rval = as_jsval(cx, myConnection, & ( * myConnection));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;

}

bool
JSGtkMain::on_timeout( JSContext * cx, JSObject * theJSObject, std::string theMethodName) {
    jsval myVal;
    /*JSBool bOK =*/ JS_GetProperty(cx, theJSObject, theMethodName.c_str(), &myVal);
    if (myVal == JSVAL_VOID) {
        AC_WARNING << "no JS event handler for event '" << theMethodName << "'";
    }
    // call the function
    jsval rval;
    AC_TRACE << "GtkMain::on_timeout calling JS event handler '" << theMethodName << "'";
    /*JSBool ok =*/ jslib::JSA_CallFunctionName(cx, theJSObject, theMethodName.c_str(), 0, 0, &rval);

    bool myResult = false;
    convertFrom(cx, rval, myResult);
    return myResult;
}

static JSBool
connect_idle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Install Gtk idle callbacks");
    DOC_END;
    try {
        if (argc != 2) {
            JS_ReportError(cx, "GtkMain.connect_idle(obj, func) needs two arguments.");
            return JS_FALSE;
        }

        JSObject * myTarget;
        if ( ! convertFrom(cx, argv[0], myTarget)) {
            JS_ReportError(cx, "GtkMain.connect_idle() first argument is not an object.");
            return JS_FALSE;
        }

        Glib::ustring myMethodName;
        if ( ! convertFrom(cx, argv[1], myMethodName)) {
            JS_ReportError(cx, "GtkMain.connect_idle() second argument is not a string.");
            return JS_FALSE;
        }

        /* TODO: wrap glibmm priority enum
        unsigned myInterval;
        if ( ! convertFrom(cx, argv[2], myInterval)) {
            JS_ReportError(cx, "GtkMain.connect_idle() third argument is not a number.");
            return JS_FALSE;
        }*/

        JSSigConnection::OWNERPTR myConnection = JSSigConnection::OWNERPTR(new sigc::connection);
        *myConnection = Glib::signal_idle().connect( sigc::bind<JSContext*, JSObject*, std::string>(
                sigc::ptr_fun( & JSGtkMain::on_idle ), cx, myTarget, myMethodName), Glib::PRIORITY_DEFAULT_IDLE);

        // register our target object with the GCObserver
        GCObserver::FinalizeSignal myFinalizer = GCObserver::get().watchObject(myTarget);
        // now add our cleanup code to the finalize signal,
        // binding the connection as an extra argument
        myFinalizer.connect(sigc::bind<sigc::connection>(
                    sigc::ptr_fun( & JSSignalAdapter0<int>::on_target_finalized),
                    *myConnection));

        *rval = as_jsval(cx, myConnection, & ( * myConnection));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;

}

int
JSGtkMain::on_idle( JSContext * cx, JSObject * theJSObject, std::string theMethodName) {
    jsval myVal;
    /*JSBool bOK =*/ JS_GetProperty(cx, theJSObject, theMethodName.c_str(), &myVal);
    if (myVal == JSVAL_VOID) {
        AC_WARNING << "no JS event handler for event '" << theMethodName << "'";
    }
    // call the function
    jsval rval;
    AC_TRACE << "GtkMain::on_idle calling JS event handler '" << theMethodName << "'";
    /*JSBool ok =*/ jslib::JSA_CallFunctionName(cx, theJSObject, theMethodName.c_str(), 0, 0, &rval);

    int myResult = 0;
    convertFrom(cx, rval, myResult);
    return myResult;
}


bool
JSGtkMain::onAcceptorTimeout() {
    asl::Ptr<MessageAcceptor<LocalPolicy>::Message> myMessage;
    while ( (myMessage = JSGtkMain::ourAppAcceptor->popIncomingMessage()) ) {
        AC_DEBUG << "received '" << myMessage->as_string() << "'";
        JSGtkMain::ourAppAcceptor->pushOutgoingMessage(myMessage->server, "ACK");
        AC_DEBUG << "sent ACK ";
        ourOtherInstanceSignal.emit(myMessage->as_string());
    }
    return true;
}

bool
JSGtkMain::sendToPrevInstance(const std::string & theSessionPipeId, const std::string & theInitialProjectfilename) {
    try {
        AC_DEBUG << "opening conduit to already running instance";
        Conduit<LocalPolicy> myAppConduit(theSessionPipeId);
        myAppConduit.send(theInitialProjectfilename);
        AC_DEBUG << "sending " << theInitialProjectfilename;
        std::string myResponse;
        myAppConduit.receive(myResponse, 1000);
        AC_DEBUG << "received " << myResponse;
    } catch (const ConduitRefusedException &) {
        return false;
    }
    return true;
}

static JSBool
GetSignalOtherInstance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSSignal1<void, const string &>::OWNERPTR mySignal( new
                JSSignal1<void, const string &>::NATIVE(JSGtkMain::ourOtherInstanceSignal));
        *rval = jslib::as_jsval(cx, mySignal);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
SetSingleInstance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Ensures only one instance of the app runs.");
    DOC_PARAM("theApplicationName", "the id of the application", DOC_TYPE_STRING);
    DOC_PARAM("theProjectFilename", "if another instance is detected, this string it sent to it.", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        string theApplicationName;
        string theProjectFilename;
        convertFrom(cx, argv[0], theApplicationName);
        if (JSVAL_IS_STRING(argv[1])) {
            convertFrom(cx, argv[1], theProjectFilename);
        }
        // generate a pipe name from the session id to ensure the app is
        // run only once per login session.
        try {
            JSGtkMain::ourAppAcceptor = asl::Ptr<MessageAcceptor<LocalPolicy> >(
                    new MessageAcceptor<LocalPolicy>(theApplicationName));
        } catch (const ConduitInUseException &) {
            if (!JSGtkMain::sendToPrevInstance(theApplicationName, theProjectFilename)) {
#ifdef LINUX
                // delete dead socket and try again
                deleteFile(UnixAddress::PIPE_PREFIX + theApplicationName);
                JSGtkMain::ourAppAcceptor = asl::Ptr<MessageAcceptor<LocalPolicy> >(
                        new MessageAcceptor<LocalPolicy>(theApplicationName));
#endif
            }
        }
        if (JSGtkMain::ourAppAcceptor) {
            JSGtkMain::ourAppAcceptor->start();
            JSGtkMain::ourAcceptorTimeout = Glib::signal_timeout().connect(sigc::ptr_fun(JSGtkMain::onAcceptorTimeout), 100);
            *rval = as_jsval(cx, true);
        } else {
            *rval = as_jsval(cx, false);
        }
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
        {"connect_timeout",      connect_timeout,         3},
        {"connect_idle",         connect_idle,            3},
        {"quit",                 quit,                    0},
        {"setSingleInstance",    SetSingleInstance, 2},
        {"get_signal_other_instance", GetSignalOtherInstance, 0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSGtkMain::StaticProperties() {
    static JSPropertySpec myProperties[] = {
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
        JS_SetPrivate(cx,obj,new JSGtkMain(myNewObject, myNewObject.get()));
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


