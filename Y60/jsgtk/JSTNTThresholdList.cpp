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
//   $RCSfile: JSTNTThresholdList.cpp,v $
//   $Author: martin $
//   $Revision: 1.9 $
//   $Date: 2005/04/21 16:25:03 $
//
//
//=============================================================================

#include "JSTNTThresholdList.h"
#include "JSTreeIter.h"
#include "JSSignalProxies.h"
#include "jsgtk.h"
#include <y60/JSNode.h>
#include <y60/JScppUtils.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {


static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::TNTThresholdList@") + as_string(obj);
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
refresh(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    
    try {

        ensureParamCount(argc, 1);

        acgtk::TNTThresholdList * myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        dom::NodePtr myPaletteNode;
        if ( ! convertFrom(cx, argv[0], myPaletteNode)) {
            JS_ReportError(cx, "TNTThresholdList::refresh(): Argument 0 must be a xml node.");
            return JS_FALSE;
        }

        myNative->refresh(myPaletteNode);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
select(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    
    try {

        ensureParamCount(argc, 1);

        acgtk::TNTThresholdList * myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        dom::NodePtr myPaletteItem;
        if ( ! convertFrom(cx, argv[0], myPaletteItem)) {
            JS_ReportError(cx, "TNTThresholdList::refresh(): Argument 0 must be a xml node.");
            return JS_FALSE;
        }

        myNative->select(myPaletteItem);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    
    try {
        ensureParamCount(argc, 0);

        acgtk::TNTThresholdList * myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        
        myNative->clear();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


JSFunctionSpec *
JSTNTThresholdList::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                      native                    nargs
        {"toString",                 toString,                 0},
        {"refresh",                  refresh,                  1},
        {"select",                   select,                   1},
        {"clear",                    clear,                    0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSTNTThresholdList::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSTNTThresholdList::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSTNTThresholdList::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSTNTThresholdList::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSTNTThresholdList::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSTNTThresholdList::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSTNTThresholdList * myNewObject = 0;

    try {
        if (argc == 0) {
            newNative = new NATIVE();
        } else {
            JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
            return JS_FALSE;
        }

        myNewObject = new JSTNTThresholdList(OWNERPTR(newNative), newNative);

        if (myNewObject) {
            JS_SetPrivate(cx,obj,myNewObject);
            return JS_TRUE;
        }
    } HANDLE_CPP_EXCEPTION;
    JS_ReportError(cx,"JSTNTThresholdList::Constructor: bad parameters");
    return JS_FALSE;
}

enum ColumnProperties {
    PROP_COL_NAME,
    PROP_COL_INDEX,
    PROP_COL_LOWER_THRESHOLD,
    PROP_COL_UPPER_THRESHOLD
};

#define DEFINE_COL_PROP(theName) \
    {"COL_" #theName, PROP_COL_ ## theName, acgtk::TNTThresholdList::COL_ ## theName}

JSConstIntPropertySpec *
JSTNTThresholdList::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_COL_PROP( NAME ),
        DEFINE_COL_PROP( INDEX ),
        DEFINE_COL_PROP( LOWER_THRESHOLD ),
        DEFINE_COL_PROP( UPPER_THRESHOLD ),
        {0}
    };
    return myProperties;
};

void
JSTNTThresholdList::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            ConstIntProperties(), 0, 0, JSBASE::ClassName());
}

JSObject *
JSTNTThresholdList::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSTNTThresholdList::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSTNTThresholdList::OWNERPTR theOwner, JSTNTThresholdList::NATIVE * theNative) {
    JSObject * myReturnObject = JSTNTThresholdList::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

