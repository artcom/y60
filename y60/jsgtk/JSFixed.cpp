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
#include "JSFixed.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Fixed, asl::Ptr<Gtk::Fixed>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Fixed@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
Move(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 3);
        Gtk::Widget * myWidget(0);
        if ( ! convertFrom( cx, argv[0], myWidget)) {
            JS_ReportError(cx, "JSFixed::move(): argument #1 must be a widget");
            return JS_FALSE;
        }

        unsigned myX;
        if ( ! convertFrom( cx, argv[1], myX)) {
            JS_ReportError(cx, "JSFixed::move(): argument #2 must be a unsigned int.");
            return JS_FALSE;
        }
        unsigned myY;
        if ( ! convertFrom( cx, argv[2], myY)) {
            JS_ReportError(cx, "JSFixed::move(): argument #3 must be a unsigned int.");
            return JS_FALSE;
        }

        Gtk::Fixed * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
        mySelf->move( * myWidget, myX, myY );

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
Put(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 3);
        Gtk::Widget * myWidget(0);
        if ( ! convertFrom( cx, argv[0], myWidget)) {
            JS_ReportError(cx, "JSFixed::move(): argument #1 must be a widget");
            return JS_FALSE;
        }

        unsigned myX;
        if ( ! convertFrom( cx, argv[1], myX)) {
            JS_ReportError(cx, "JSFixed::move(): argument #2 must be a unsigned int.");
            return JS_FALSE;
        }
        unsigned myY;
        if ( ! convertFrom( cx, argv[2], myY)) {
            JS_ReportError(cx, "JSFixed::move(): argument #3 must be a unsigned int.");
            return JS_FALSE;
        }

        Gtk::Fixed * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
        mySelf->put( * myWidget, myX, myY );

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


JSFunctionSpec *
JSFixed::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"move",                 Move,                    3},
        {"put",                  Put,                     3},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSFixed::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSFixed::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSFixed::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSFixed::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSFixed::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSFixed::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSFixed * myNewObject = 0;

    if (argc == 0) {
        newNative = new Gtk::Fixed();
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none or three () %d",ClassName(), argc);
        return JS_FALSE;
    }
    myNewObject = new JSFixed(OWNERPTR(newNative), newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSFixed::Constructor: bad parameters");
    return JS_FALSE;
}

void
JSFixed::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSBASE::ClassName());
}

JSObject *
JSFixed::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSFixed::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSFixed::OWNERPTR theOwner, JSFixed::NATIVE * theNative) {
    JSObject * myReturnObject = JSFixed::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

