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
#include "JSListStore.h"

#include "jsgtk.h"
#include "JSTreeIter.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::ListStore, Glib::RefPtr<Gtk::ListStore>, StaticAccessProtocol>;

typedef Gtk::ListStore NATIVE;
typedef Glib::RefPtr<NATIVE> OWNERPTR;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::ListStore@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
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
        asl::Ptr<Gtk::TreeIter> myInsertPosition;
        convertFrom(cx, argv[0], myInsertPosition);
        asl::Ptr<Gtk::TreeIter> myNewRow = asl::Ptr<Gtk::TreeIter>(new Gtk::TreeIter(myOwner->insert(*myInsertPosition)));
        *rval = as_jsval(cx, myNewRow, myNewRow.get());
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
        asl::Ptr<Gtk::TreeIter> myInsertPosition;
        convertFrom(cx, argv[0], myInsertPosition);
        asl::Ptr<Gtk::TreeIter> myNewRow = asl::Ptr<Gtk::TreeIter>(new Gtk::TreeIter(myOwner->insert_after(*myInsertPosition)));
        *rval = as_jsval(cx, myNewRow, myNewRow.get());
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
        asl::Ptr<Gtk::TreeIter> myInsertPosition;
        convertFrom(cx, argv[0], myInsertPosition);
        asl::Ptr<Gtk::TreeIter> myNewRow = asl::Ptr<Gtk::TreeIter>(new Gtk::TreeIter(myOwner->erase(*myInsertPosition)));
        *rval = as_jsval(cx, myNewRow, myNewRow.get());
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
append(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        OWNERPTR myOwner = JSListStore::getJSWrapper(cx, obj).getOwner();
        asl::Ptr<Gtk::TreeIter> myNewRow = asl::Ptr<Gtk::TreeIter>(new Gtk::TreeIter(myOwner->append()));
        *rval = as_jsval(cx, myNewRow, myNewRow.get());
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

    //NATIVE * newNative = 0;

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

