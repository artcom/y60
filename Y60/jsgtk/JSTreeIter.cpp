//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSTreeIter.cpp,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2004/11/26 13:01:25 $
//
//
//=============================================================================

#include "JSTreeIter.h"
#include "jsgtk.h"
#include <y60/JScppUtils.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = std::string("GtkTreeIter@") + as_string(obj);
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
set_value(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        // native method call
        Ptr<Gtk::TreeIter> myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        int myColNum = 0;
        convertFrom(cx, argv[0], myColNum);
        // TODO: replace hard-coded column types with something more dynamic
        if (myColNum == 0) {
            unsigned myColValue;
            convertFrom(cx, argv[1], myColValue);
            (*(*myNative)).set_value(myColNum, myColValue);
        } else {
            Glib::ustring myColValue;
            convertFrom(cx, argv[1], myColValue);
            (*(*myNative)).set_value(myColNum, myColValue);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
get_value(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc,1);
        // native method call
        Ptr<Gtk::TreeIter> myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        int myColNum = 0;
        convertFrom(cx, argv[0], myColNum);
        // TODO: replace hard-coded column types with something more dynamic
        if (myColNum == 0) {
            unsigned myColValue;
            (*(*myNative)).get_value(myColNum, myColValue);
            *rval = as_jsval(cx, myColValue);
        } else {
            Glib::ustring myColValue;
            (*(*myNative)).get_value(myColNum, myColValue);
            *rval = as_jsval(cx, myColValue);
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSTreeIter::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"set_value",            set_value,               2},
        {"get_value",            get_value,               1},
        {0}
    };
    return myFunctions;
}


JSPropertySpec *
JSTreeIter::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}
// getproperty handling
JSBool
JSTreeIter::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSTreeIter::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSTreeIter::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

// setproperty handling
JSBool
JSTreeIter::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSTreeIter::setProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSTreeIter::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSTreeIter::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSTreeIter * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
        myNewObject = new JSTreeIter(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSTreeIter::Constructor: bad parameters");
    return JS_FALSE;
    JS_ReportError(cx,"%s is abstract and can not be instatiated", ClassName() );
    return JS_FALSE;
}

void
JSTreeIter::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, 0);
}

JSObject *
JSTreeIter::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, 0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
    } else {
        cerr << "JSTreeIter::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSTreeIter::OWNERPTR theOwner, JSTreeIter::NATIVE * theNative) {
    JSObject * myReturnObject = JSTreeIter::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, Ptr<Gtk::TreeIter> & theNativePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<Gtk::TreeIter>::Class()) {
                theNativePtr = JSClassTraits<Gtk::TreeIter>::getNativeOwner(cx, myArgument);
                return true;
            }
        }
    }
    return false;
}

}
