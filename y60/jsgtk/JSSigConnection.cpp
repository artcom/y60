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
#include "JSSigConnection.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<sigc::connection, asl::Ptr<sigc::connection>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("sigc::connection@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
disconnect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    sigc::connection * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
    myNative->disconnect();
    return JS_TRUE;
}


JSBool
JSSigConnection::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSSigConnection * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
        myNewObject = new JSSigConnection(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSSigConnection::Constructor: bad parameters");
    return JS_FALSE;
}
JSPropertySpec *
JSSigConnection::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSSigConnection::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"disconnect",           disconnect,              0},
        {0}
    };
    return myFunctions;
}

#define DEFINE_GDK_PROP( NAME ) { #NAME, PROP_ ## NAME, NAME}

#if 0
JSConstIntPropertySpec *
JSSigConnection::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};
#endif


// getproperty handling
JSBool
JSSigConnection::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    // common properties:
    switch (theID) {
        case 0:
        default:
            break;
    }
    JS_ReportError(cx,"JSSigConnection::getProperty: index %d out of range", theID);
    return JS_FALSE;
}

JSObject *
JSSigConnection::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        // /*JSObject * myConstructorFuncObj =*/ JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        //JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSSigConnection::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, 0);
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSSigConnection::OWNERPTR theOwner, JSSigConnection::NATIVE * theNative) {
    JSObject * myReturnObject = JSSigConnection::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, JSSigConnection::NATIVE * & theConnection) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSSigConnection::NATIVE >::Class()) {
                typedef JSClassTraits<JSSigConnection::NATIVE>::ScopedNativeRef NativeRef;
                NativeRef myObj(cx, myArgument);
                theConnection = & myObj.getNative();
                return true;
            }
        }
    }
    return false;
}


}
