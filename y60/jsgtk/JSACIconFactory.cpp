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
#include "JSACIconFactory.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<acgtk::ACIconFactory, asl::Ptr<acgtk::ACIconFactory>, StaticAccessProtocol>;

typedef JSACIconFactory::NATIVE NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("ACIconFactory@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
add(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    try {
        JSACIconFactory::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        Glib::ustring myStockId;
        if ( ! convertFrom(cx, argv[0], myStockId)) {
            JS_ReportError(cx, "ACIconFactory::add() argument zero must be a string.");
            return JS_FALSE;
        }

        Glib::ustring myIconPath;
        if ( ! convertFrom(cx, argv[1], myIconPath)) {
            JS_ReportError(cx, "ACIconFactory::add() argument one must be a string.");
            return JS_FALSE;
        }

        Glib::ustring myLabel;
        if ( ! convertFrom(cx, argv[2], myLabel)) {
            JS_ReportError(cx, "ACIconFactory::add() argument two must be a string.");
            return JS_FALSE;
        }

        std::string myIconWithPath = searchFileRelativeToJSInclude(cx, obj, argc, argv, myIconPath);
        myNative->add(myStockId, myIconWithPath, myLabel);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}


JSFunctionSpec *
JSACIconFactory::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"add",                  add,                     2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSACIconFactory::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSACIconFactory::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSACIconFactory::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSACIconFactory::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSACIconFactory::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case 0:
            default:
                JS_ReportError(cx,"JSACIconFactory::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSACIconFactory::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSACIconFactory::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSACIconFactory::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSACIconFactory * myNewObject = 0;

    OWNERPTR myNewACIconFactory = OWNERPTR(new NATIVE());

    myNewObject = new JSACIconFactory(myNewACIconFactory, myNewACIconFactory.get());
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSACIconFactory::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSACIconFactory::initClass(JSContext *cx, JSObject *theGlobalObject) {
    DOC_CREATE(JSACIconFactory);
    return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
}

bool convertFrom(JSContext *cx, jsval theValue, JSACIconFactory::OWNERPTR & theACIconFactory) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSACIconFactory::NATIVE >::Class()) {
                theACIconFactory = JSClassTraits<JSACIconFactory::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSACIconFactory::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSACIconFactory::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

/*
jsval as_jsval(JSContext *cx, JSACIconFactory::OWNERPTR theOwner, JSACIconFactory::NATIVE * theSerial) {
    JSObject * myObject = JSACIconFactory::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}
*/

}

