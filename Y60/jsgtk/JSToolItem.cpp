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
//   $RCSfile: JSToolItem.cpp,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/11/26 13:01:25 $
//
//
//=============================================================================

#include "JSToolItem.h"
#include "JSTooltips.h"
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
    std::string myStringRep = string("Gtk::ToolItem@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}
static JSBool
set_tooltip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2,3);
        Gtk::Tooltips * theToolTips;
        Glib::ustring tipText;
        Glib::ustring privateText;
        
        if (!convertFrom(cx, argv[0], theToolTips)) {
            JS_ReportError(cx, "JSToolItem::set_tooltip(): argument #1 must be a Tooltip");
            return JS_FALSE;
        }
        if (!convertFrom(cx, argv[1], tipText)) {
            JS_ReportError(cx, "JSTooltips::set_tooltip(): argument #2 must be a string");
            return JS_FALSE;
        }
        if (argc ==3) {
            if (!convertFrom(cx, argv[2], privateText)) {
                JS_ReportError(cx, "JSTooltips::set_tooltip(): optional argument #3 must be a string.");
                return JS_FALSE;
            }
        }

        // native method call
        Gtk::ToolItem * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        if (argc==2) {
            myNative->set_tooltip(*theToolTips, tipText);
        } else {
            myNative->set_tooltip(*theToolTips, tipText, privateText);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


JSFunctionSpec *
JSToolItem::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"set_tooltip",          set_tooltip,             0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSToolItem::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSToolItem::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSToolItem::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSToolItem::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSToolItem::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSToolItem::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSToolItem * myNewObject = 0;

    if (argc == 0) {
        newNative = new Gtk::ToolItem();
        myNewObject = new JSToolItem(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSToolItem::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSToolItem::ConstIntProperties() {

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
JSToolItem::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSBASE::ClassName());
}

JSObject *
JSToolItem::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSToolItem::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSToolItem::OWNERPTR theOwner, JSToolItem::NATIVE * theNative) {
    JSObject * myReturnObject = JSToolItem::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}






