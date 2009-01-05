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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "JSLogger.h"
#include <y60/jsbase/Documentation.h>
#include <y60/jsbase/JSWrapper.impl>
#include <asl/base/LogMessageSinks.h>
#include <iostream>

using namespace asl;
using namespace std;

namespace jslib {

template class JSWrapper<asl::Logger, asl::Ptr<asl::Logger>,
         StaticAccessProtocol>;

static JSBool log(asl::Severity theSeverity, JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        const char * myFilename;
        int myLineNo;
		if (! getFileLine(cx, obj, argc, argv, myFilename, myLineNo)) {
			myLineNo = 0;
			myFilename = "unknown";
		}
		std::string myMessage;
		ensureParamCount(argc, 1);
		convertFrom(cx, argv[0], myMessage);
		AC_LOG_CHECK(theSeverity, myFilename, myLineNo) << myMessage;
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool fatal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Log at fatal level");
    DOC_PARAM("theMessage", "", DOC_TYPE_STRING);
    DOC_END;
    return log(asl::SEV_FATAL, cx, obj, argc, argv, rval);
}
static JSBool error(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Log at error level");
    DOC_PARAM("theMessage", "", DOC_TYPE_STRING);
    DOC_END;
    return log(asl::SEV_ERROR, cx, obj, argc, argv, rval);
}
static JSBool warning(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Log at warning level");
    DOC_PARAM("theMessage", "", DOC_TYPE_STRING);
    DOC_END;
    return log(asl::SEV_WARNING, cx, obj, argc, argv, rval);
}
static JSBool trace(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Log at info level");
    DOC_PARAM("theMessage", "", DOC_TYPE_STRING);
    DOC_END;
    return log(asl::SEV_TRACE, cx, obj, argc, argv, rval);
}
static JSBool info(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Log at info level");
    DOC_PARAM("theMessage", "", DOC_TYPE_STRING);
    DOC_END;
    return log(asl::SEV_INFO, cx, obj, argc, argv, rval);
}
static JSBool debug(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Log at debug level");
    DOC_PARAM("theMessage", "", DOC_TYPE_STRING);
    DOC_END;
    return log(asl::SEV_DEBUG, cx, obj, argc, argv, rval);
}

static JSBool addFileSink(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Add a log file to the logger");
    DOC_PARAM("theLogFilename", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myLogFilename;
        convertFrom(cx, argv[0], myLogFilename);

        Logger::get().addMessageSink(asl::Ptr<MessageSink>(new asl::FilePrinter(myLogFilename)));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;

}
static JSBool addFarewellSink(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Add an abort-on-fatal errors sink to the logger");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Logger::get().addMessageSink(asl::Ptr<MessageSink>(new asl::FarewellMessageSink()));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;

}


static JSBool
setVerbosity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set global Verbosity of Logger.");
    DOC_PARAM("theVerbosity", "", DOC_TYPE_ENUMERATION);
    DOC_PARAM_OPT("theFilename", "", DOC_TYPE_STRING, "");
    DOC_PARAM_OPT("theMinId", "", DOC_TYPE_INTEGER, 0);
    DOC_PARAM_OPT("theMaxId", "", DOC_TYPE_INTEGER, 99999999);
    DOC_END;
    try {
        int myVerbosity;
        //ensureParamCount(argc, 2);
        convertFrom(cx, argv[0], myVerbosity);
        if (argc == 1) {
            Logger::get().setVerbosity(asl::Severity(myVerbosity));
            return JS_TRUE;
        }

        int myMinId, myMaxId;
        myMinId = 0;
        myMaxId = std::numeric_limits<int>::max();
        std::string myModule;
        convertFrom(cx, argv[1], myModule);
        if (argc > 2) {
            convertFrom(cx, argv[2], myMinId);
        }
        if (argc > 3) {
            convertFrom(cx, argv[3], myMaxId);
        }
        Logger::get().setModuleVerbosity(asl::Severity(myVerbosity), myModule, myMinId, myMaxId);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSLogger::StaticFunctions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                    native          nargs
        {"fatal",     fatal,     1},
        {"error",     error,     1},
        {"warning",     warning,     1},
        {"info",     info,     1},
        {"debug",     debug,     1},
        {"trace",     trace,     1},
        {"setVerbosity", setVerbosity, 4},
        {"addFileSink", addFileSink, 1},
        {"addFarewellSink", addFarewellSink, 0},
        {0}
    };
    return myFunctions;
}
JSFunctionSpec *
JSLogger::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSLogger::Properties() {
    static JSPropertySpec myProperties[] = {
       {0}
    };
    return myProperties;
}

enum PropertyNumbers {PROP_verbosity};

JSConstIntPropertySpec *
JSLogger::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {"FATAL", asl::SEV_FATAL, asl::SEV_FATAL},
        {"ERROR", asl::SEV_ERROR, asl::SEV_ERROR},
        {"WARNING", asl::SEV_WARNING, asl::SEV_WARNING},
        {"INFO", asl::SEV_INFO, asl::SEV_INFO},
        {"DEBUG", asl::SEV_DEBUG, asl::SEV_DEBUG},
        {"TRACE", asl::SEV_TRACE, asl::SEV_TRACE},
        {0}
    };
    return myProperties;
};


static JSBool
getStaticProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    int myID = JSVAL_TO_INT(id);
    switch (myID) {
    case PROP_verbosity:
        *vp = as_jsval(cx, int(Logger::get().getVerbosity()));
        break;
    default:
        JS_ReportError(cx,"JSLogger::getStaticProperty: index %d out of range", myID);
        return JS_FALSE;
    }
    return JS_TRUE;
}

static JSBool
setStaticProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    int myID = JSVAL_TO_INT(id);
    switch (myID) {
    case PROP_verbosity:
        int myVerbosity;
        convertFrom(cx, *vp, myVerbosity);
        Logger::get().setVerbosity(asl::Severity(myVerbosity));
        break;
    default:
        JS_ReportError(cx,"JSLogger::getStaticProperty: index %d out of range", myID);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSPropertySpec *
JSLogger::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {"verbosity", PROP_verbosity, JSPROP_ENUMERATE|JSPROP_PERMANENT, getStaticProperty, setStaticProperty},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSLogger::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            ;
    }
    JS_ReportError(cx,"JSLogger::getProperty: index %d out of range", theID);
    return JS_FALSE;
};

// setproperty handling
JSBool
JSLogger::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            ;
    }
    JS_ReportError(cx,"JSLogger::setPropertySwitch: index %d out of range", theID);
    return JS_FALSE;
};

JSBool
JSLogger::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("JSLogger cannot be constructed.");
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    /*
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    OWNERPTR myNewPtr = OWNERPTR(0);
    JSLogger * myNewObject=new JSLogger(myNewPtr, 0);
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    */
    JS_ReportError(cx,"JSLogger::Constructor: cannot be constructed.");
    return JS_FALSE;
}


JSObject *
JSLogger::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
        Properties(), Functions(), ConstIntProperties(), StaticProperties(), StaticFunctions());
    DOC_CREATE(JSLogger);
    return myClass;
}
} // namespace


