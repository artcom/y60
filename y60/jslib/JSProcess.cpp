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
#include "JSProcess.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<asl::Process, asl::Ptr<asl::Process>, StaticAccessProtocol>;

static JSBool
Launch(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Launches the process.");
    DOC_END;

    JSProcess::getJSWrapper(cx,obj).openNative().launch();

    *rval = JSVAL_VOID;

    return JS_TRUE;
}

static JSBool
Kill(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Kills the process.");
    DOC_END;

    JSProcess::getJSWrapper(cx,obj).openNative().kill();

    *rval = JSVAL_VOID;

    return JS_TRUE;
}

static JSBool
WaitForTermination(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Waits until the process terminates.");
    DOC_END;

    JSProcess::getJSWrapper(cx,obj).openNative().waitForTermination();

    *rval = JSVAL_VOID;

    return JS_TRUE;
}

static JSBool
PollForTermination(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Poll for process termination.");
    DOC_END;

    bool myResult = JSProcess::getJSWrapper(cx,obj).openNative().pollForTermination();

    *rval = as_jsval(cx, myResult);

    return JS_TRUE;
}

static JSBool
SetWorkingDirectory(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set working directory for this process");
    DOC_END;

    if (argc != 1) {
        JS_ReportError(cx, "JSProcess::setWorkingDirectory(): Wrong number of arguments, "
                           "expected one (the working directory), got %d.", argc);
        return JS_FALSE;
    }

    std::string myWorkingDirectory;
    if(!convertFrom(cx, argv[0], myWorkingDirectory)) {
        JS_ReportError(cx, "JSProcess::setWorkingDirectory(): Argument 0 must be a string ", argc);
        return JS_FALSE;
    }

    JSProcess::getJSWrapper(cx,obj).openNative().setWorkingDirectory(myWorkingDirectory);

    *rval = JSVAL_VOID;

    return JS_TRUE;
}

JSFunctionSpec *
JSProcess::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"launch",               Launch,                    0},
        {"kill",                 Kill,                      0},
        {"waitForTermination",   WaitForTermination,        0},
        {"pollForTermination",   PollForTermination,        0},
        {"setWorkingDirectory",  SetWorkingDirectory,       1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSProcess::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSProcess::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JS_ReportError(cx,"JSProcess::getProperty: index %d out of range", theID);
    return JS_FALSE;
}

// setproperty handling
JSBool
JSProcess::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JS_ReportError(cx,"JSProcess::setPropertySwitch: index %d out of range", theID);
    return JS_FALSE;
}

JSBool
JSProcess::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new process");
    DOC_PARAM("theCommand", "Command to execute", DOC_TYPE_STRING);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSProcess * myNewObject = 0;

    if (argc == 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"JSProcess::Constructor: bad argument #1 (undefined)");
            return JS_FALSE;
        }

        string myCommand;
        if (convertFrom(cx, argv[0], myCommand)) {
            OWNERPTR myNewProcess = OWNERPTR(new asl::Process(myCommand));
            myNewObject = new JSProcess(myNewProcess, myNewProcess.get());
        } else {
            JS_ReportError(cx, "JSProcess::Constructor: argument #1 must be a string.");
            return JS_FALSE;
        }

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1 (PortNumber) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSProcess::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSProcess::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSProcess::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSProcess::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        // name                    native               nargs
        {0}};
    return myFunctions;
}

JSObject *
JSProcess::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties(),0,StaticFunctions());
    DOC_CREATE(JSProcess);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSProcess::NATIVE & theProcess) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSProcess::NATIVE >::Class()) {
                theProcess = JSClassTraits<JSProcess::NATIVE>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSProcess::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSProcess::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSProcess::OWNERPTR theOwner, JSProcess::NATIVE * theProcess) {
    JSObject * myObject = JSProcess::Construct(cx, theOwner, theProcess);
    return OBJECT_TO_JSVAL(myObject);
}

}
