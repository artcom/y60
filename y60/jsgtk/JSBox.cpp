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
#include "JSBox.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Box, asl::Ptr<Gtk::Box>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Box@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
pack_start(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1, 3);
        Gtk::Box * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        Gtk::Widget * myChildToAdd(0);
        convertFrom(cx, argv[0], myChildToAdd);

        if ( ! myChildToAdd) {
            JS_ReportError(cx,"Box::pack_start(): parameter 0 must be a widget. ");
            return JS_FALSE;
        }

        int myPackOptions = 0;
        if (argc > 1) {
            convertFrom(cx, argv[1], myPackOptions);
        }

        guint myPadding = 0;
        if (argc > 2) {
            convertFrom(cx, argv[2], myPadding);
        }

        switch (argc) {
            case 1:
                myNative->pack_start( * myChildToAdd);
                return JS_TRUE;
            case 2:
                myNative->pack_start( * myChildToAdd, static_cast<Gtk::PackOptions>(myPackOptions));
                return JS_TRUE;
            case 3:
                myNative->pack_start( * myChildToAdd, static_cast<Gtk::PackOptions>(myPackOptions), myPadding);
                return JS_TRUE;
            default:
                return JS_FALSE;
        }
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
pack_end(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1, 3);
        Gtk::Box * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        Gtk::Widget * myChildToAdd(0);
        convertFrom(cx, argv[0], myChildToAdd);

        if ( ! myChildToAdd) {
            JS_ReportError(cx,"Box::pack_end(): parameter 0 must be a widget. ");
            return JS_FALSE;
        }

        int myPackOptions = 0;
        if (argc > 1) {
            convertFrom(cx, argv[1], myPackOptions);
        }

        guint myPadding = 0;
        if (argc > 2) {
            convertFrom(cx, argv[2], myPadding);
        }

        switch (argc) {
            case 1:
                myNative->pack_end( * myChildToAdd);
                return JS_TRUE;
            case 2:
                myNative->pack_end( * myChildToAdd, static_cast<Gtk::PackOptions>(myPackOptions));
                return JS_TRUE;
            case 3:
                myNative->pack_end( * myChildToAdd, static_cast<Gtk::PackOptions>(myPackOptions), myPadding);
                return JS_TRUE;
            default:
                return JS_FALSE;
        }
    } HANDLE_CPP_EXCEPTION;
}


static JSBool
reorder_child(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        Gtk::Box * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        Gtk::Widget * myChildToMove(0);
        convertFrom(cx, argv[0], myChildToMove);

        int myNewPosition;
        convertFrom(cx, argv[1], myNewPosition);

        myNative->reorder_child( * myChildToMove, myNewPosition);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSBox::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"pack_start",           pack_start,              3},
        {"pack_end",             pack_end,                3},
        {"reorder_child",        reorder_child,           2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSBox::Properties() {
    static JSPropertySpec myProperties[] = {
        {"homogeneous", PROP_homogeneous, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"spacing",     PROP_spacing, JSPROP_ENUMERATE|JSPROP_PERMANENT},
//        {"URL", PROP_URL, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSBox::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSBox::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSBox::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_homogeneous:
            *vp = as_jsval(cx, theNative.get_homogeneous());
            return JS_TRUE;
        case PROP_spacing:
            *vp = as_jsval(cx, theNative.get_spacing());
            return JS_TRUE;
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSBox::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_homogeneous:
            try {
                bool myHomogeneousFlag;
                convertFrom(cx, *vp, myHomogeneousFlag);
                theNative.set_homogeneous(myHomogeneousFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_spacing:
            try {
                int mySpacing;
                convertFrom(cx, *vp, mySpacing);
                theNative.set_spacing(mySpacing);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSBox::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSBox * myNewObject = 0;

    if (argc == 0) {
        newNative = 0; // Abstract
        myNewObject = new JSBox(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSBox::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSBox::ConstIntProperties() {

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
JSBox::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
                JSBASE::ClassName());
}

JSObject *
JSBox::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSBox::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSBox::OWNERPTR theOwner, JSBox::NATIVE * theNative) {
    JSObject * myReturnObject = JSBox::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}
