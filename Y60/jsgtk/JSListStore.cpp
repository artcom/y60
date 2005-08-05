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
//   $RCSfile: JSListStore.cpp,v $
//   $Author: martin $
//   $Revision: 1.4 $
//   $Date: 2005/04/21 16:25:02 $
//
//
//=============================================================================

#include "JSListStore.h"
#include "jsgtk.h"
#include "JSTreeIter.h"
#include <y60/JScppUtils.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

typedef Gtk::ListStore NATIVE;
typedef Glib::RefPtr<NATIVE> OWNERPTR;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::ListStore@") + as_string(obj);
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        OWNERPTR myOwner = JSListStore::getJSWrapper(cx, obj).getOwner();
        myOwner->clear();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
insert(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        OWNERPTR myOwner = JSListStore::getJSWrapper(cx, obj).getOwner();
        Ptr<Gtk::TreeIter> myInsertPosition;
        convertFrom(cx, argv[0], myInsertPosition);
        Ptr<Gtk::TreeIter> myNewRow = Ptr<Gtk::TreeIter>(new Gtk::TreeIter(myOwner->insert(*myInsertPosition)));
        *rval = as_jsval(cx, myNewRow, &(*myNewRow));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
insert_after(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        OWNERPTR myOwner = JSListStore::getJSWrapper(cx, obj).getOwner();
        Ptr<Gtk::TreeIter> myInsertPosition;
        convertFrom(cx, argv[0], myInsertPosition);
        Ptr<Gtk::TreeIter> myNewRow = Ptr<Gtk::TreeIter>(new Gtk::TreeIter(myOwner->insert_after(*myInsertPosition)));
        *rval = as_jsval(cx, myNewRow, &(*myNewRow));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
erase(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        OWNERPTR myOwner = JSListStore::getJSWrapper(cx, obj).getOwner();
        Ptr<Gtk::TreeIter> myInsertPosition;
        convertFrom(cx, argv[0], myInsertPosition);
        Ptr<Gtk::TreeIter> myNewRow = Ptr<Gtk::TreeIter>(new Gtk::TreeIter(myOwner->erase(*myInsertPosition)));
        *rval = as_jsval(cx, myNewRow, &(*myNewRow));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
append(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        OWNERPTR myOwner = JSListStore::getJSWrapper(cx, obj).getOwner();
        Ptr<Gtk::TreeIter> myNewRow = Ptr<Gtk::TreeIter>(new Gtk::TreeIter(myOwner->append()));
        *rval = as_jsval(cx, myNewRow, &(*myNewRow));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSListStore::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name           native         nargs
        {"toString",      toString,      0},
        {"clear",         clear,         0},
        {"append",        append,        0},
        {"insert",        insert,        1},
        {"insert_after",  insert_after,  1},
        {"erase",         erase,         1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSListStore::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSListStore::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    OWNERPTR myOwner = JSListStore::getJSWrapper(cx, obj).getOwner();
    return getPropertySwitch(myOwner, theID, cx, obj, id, vp);
}

JSBool
JSListStore::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    OWNERPTR myOwner = JSListStore::getJSWrapper(cx, obj).getOwner();
    return setPropertySwitch(myOwner, theID, cx, obj, id, vp);
}

JSBool
JSListStore::getPropertySwitch(OWNERPTR theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSListStore::getProperty: index %d out of range", theID);
            return JS_FALSE;
            // return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSListStore::setPropertySwitch(OWNERPTR theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSListStore::setProperty: index %d out of range", theID);
            return JS_FALSE;
            // return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}


JSBool
JSListStore::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSListStore * myNewObject = 0;

    if (argc == 1) {
        Gtk::TreeModel::ColumnRecord * myColumnRecord = 0;
        convertFrom(cx, argv[0], myColumnRecord);
        OWNERPTR newNative = Gtk::ListStore::create(*myColumnRecord);

        myNewObject = new JSListStore(newNative, 0);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected one () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSListStore::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSListStore::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
            "POST",              PROP_HTTP_POST,         Request::HTTP_POST,
            "GET",               PROP_HTTP_GET,          Request::HTTP_GET,
            "PUT",               PROP_HTTP_PUT,          Request::HTTP_PUT,
        {0}
    };
    return myProperties;
};
*/

void
JSListStore::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
                JSBASE::ClassName());
}

JSObject *
JSListStore::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSListStore::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSListStore::OWNERPTR theOwner, JSListStore::NATIVE * theNative) {
    JSObject * myReturnObject = JSListStore::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

