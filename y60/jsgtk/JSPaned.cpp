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
#include "JSPaned.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Paned, asl::Ptr<Gtk::Paned>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Paned@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
add1(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc,1);
        Gtk::Widget * myWidgetToAdd;

        if (!convertFrom(cx, argv[0], myWidgetToAdd)) {
            JS_ReportError(cx, "JSPaned::add1(): argument #1 must be a widget");
            return JS_FALSE;
        }

        Gtk::Paned * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
        mySelf->add1(*myWidgetToAdd);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
add2(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc,1);
        Gtk::Widget * myWidgetToAdd;

        if (!convertFrom(cx, argv[0], myWidgetToAdd)) {
            JS_ReportError(cx, "JSPaned::add2(): argument #1 must be a widget");
            return JS_FALSE;
        }

        Gtk::Paned * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
        mySelf->add2(*myWidgetToAdd);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
pack1(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc,3);

        Gtk::Paned * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        Gtk::Widget * myWidgetToAdd;
        if (!convertFrom(cx, argv[0], myWidgetToAdd)) {
            JS_ReportError(cx, "JSPaned::pack1(): argument #1 must be a widget");
            return JS_FALSE;
        }

        bool myResizeFlag;
        if (!convertFrom(cx, argv[1], myResizeFlag)) {
            JS_ReportError(cx, "JSPaned::pack1(): argument #2 must be a bool");
            return JS_FALSE;
        }

        bool myShrinkFlag;
        if (!convertFrom(cx, argv[1], myShrinkFlag)) {
            JS_ReportError(cx, "JSPaned::pack1(): argument #3 must be a bool");
            return JS_FALSE;
        }

        mySelf->pack1(*myWidgetToAdd, myResizeFlag, myShrinkFlag);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
pack2(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc,3);

        Gtk::Paned * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        Gtk::Widget * myWidgetToAdd;
        if (!convertFrom(cx, argv[0], myWidgetToAdd)) {
            JS_ReportError(cx, "JSPaned::pack1(): argument #1 must be a widget");
            return JS_FALSE;
        }

        bool myResizeFlag;
        if (!convertFrom(cx, argv[1], myResizeFlag)) {
            JS_ReportError(cx, "JSPaned::pack1(): argument #2 must be a bool");
            return JS_FALSE;
        }

        bool myShrinkFlag;
        if (!convertFrom(cx, argv[1], myShrinkFlag)) {
            JS_ReportError(cx, "JSPaned::pack1(): argument #3 must be a bool");
            return JS_FALSE;
        }

        mySelf->pack2(*myWidgetToAdd, myResizeFlag, myShrinkFlag);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSPaned::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"add1",                 add1,                    1},
        {"add2",                 add2,                    1},
        {"pack1",                pack1,                   3},
        {"pack2",                pack2,                   3},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSPaned::Properties() {
    static JSPropertySpec myProperties[] = {
        {"position", PROP_position, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSPaned::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSPaned::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSPaned::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_position:
            *vp = as_jsval(cx, theNative.get_position());
            return JS_TRUE;
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSPaned::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_position:
            try {
                int thePosition;
                convertFrom(cx, *vp, thePosition);
                theNative.set_position(thePosition);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSPaned::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSPaned * myNewObject = 0;

    if (argc == 0) {
        newNative = 0; // Abstract
        myNewObject = new JSPaned(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSPaned::Constructor: bad parameters");
    return JS_FALSE;
}

void
JSPaned::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
                JSBASE::ClassName());
}

JSObject *
JSPaned::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSPaned::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSPaned::OWNERPTR theOwner, JSPaned::NATIVE * theNative) {
    JSObject * myReturnObject = JSPaned::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

