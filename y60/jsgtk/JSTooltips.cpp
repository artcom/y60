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
#include "JSTooltips.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Tooltips, asl::Ptr<Gtk::Tooltips>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = std::string("GtkTooltips@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
force_window(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        // native method call
        Gtk::Tooltips * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        myNative->force_window();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
disable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        // native method call
        Gtk::Tooltips * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        myNative->disable();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
enable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        // native method call
        Gtk::Tooltips * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        myNative->enable();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
set_tip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2,3);
        Gtk::Widget * theWidget;
        Glib::ustring tipText;
        Glib::ustring privateText;

        if (!convertFrom(cx, argv[0], theWidget)) {
            JS_ReportError(cx, "JSTooltips::set_tip(): argument #1 must be a widget");
            return JS_FALSE;
        }
        if (!convertFrom(cx, argv[1], tipText)) {
            JS_ReportError(cx, "JSTooltips::set_tip(): argument #2 must be a string");
            return JS_FALSE;
        }
        if (argc ==3) {
            if (!convertFrom(cx, argv[2], privateText)) {
                JS_ReportError(cx, "JSTooltips::set_tip(): optional argument #3 must be a string.");
                return JS_FALSE;
            }
        }

        // native method call
        Gtk::Tooltips * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        if (argc==2) {
            myNative->set_tip(*theWidget, tipText);
        } else {
            myNative->set_tip(*theWidget, tipText, privateText);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
unset_tip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Gtk::Widget * theWidget;

        if (!convertFrom(cx, argv[0], theWidget)) {
            JS_ReportError(cx, "JSTooltips::unset_tip(): argument #1 must be a widget");
            return JS_FALSE;
        }

        // native method call
        Gtk::Tooltips * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
            myNative->unset_tip(*theWidget);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSTooltips::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                 0},
        {"force_window",         force_window,             0},
        {"disable",              disable,                  0},
        {"enable",               enable,                   0},
        {"set_tip",              set_tip,                  3},
        {"unset_tip",            unset_tip,                1},
        {0}
    };
    return myFunctions;
}


JSPropertySpec *
JSTooltips::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}
// getproperty handling
JSBool
JSTooltips::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSTooltips::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSTooltips::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

// setproperty handling
JSBool
JSTooltips::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSTooltips::setProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSTooltips::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSTooltips::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSTooltips * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
        myNewObject = new JSTooltips(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSTooltips::Constructor: bad parameters");
    return JS_FALSE;
}

void
JSTooltips::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, 0);
}

JSObject *
JSTooltips::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, 0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        //JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
    } else {
        cerr << "JSTooltips::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSTooltips::OWNERPTR theOwner, JSTooltips::NATIVE * theNative) {
    JSObject * myReturnObject = JSTooltips::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, JSTooltips::NATIVE *& theTarget) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSTooltips::NATIVE>::Class()) {
                JSClassTraits<JSTooltips::NATIVE>::ScopedNativeRef myObj(cx, myArgument);
                theTarget = &myObj.getNative();
                return true;
            }
        }
    }
    return false;
}

}


