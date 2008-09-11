//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

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

