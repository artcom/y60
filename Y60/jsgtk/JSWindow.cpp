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
//   $RCSfile: JSWindow.cpp,v $
//   $Author: christian $
//   $Revision: 1.10 $
//   $Date: 2005/04/24 17:54:19 $
//
//
//=============================================================================

#include "JSWindow.h"
#include "jsgtk.h"
#include <y60/JScppUtils.h>
#include <iostream>
#include <gdkmm/general.h>

using namespace std;
using namespace asl;

namespace jslib {

typedef Gtk::Window NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = JSWindow::getJSWrapper(cx,obj).getNative().get_title();
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
Resize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        // native method call
        Gtk::Window * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        int theWidth;
        int theHeight;
        convertFrom(cx, argv[0], theWidth);
        convertFrom(cx, argv[1], theHeight);
        myNative->resize(theWidth, theHeight);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
Raise(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Gtk::Window * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        myNative->raise();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION
}

static JSBool
Present(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Gtk::Window * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        myNative->present();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION
}

static JSBool
SetTransientFor(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Gtk::Window * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        Gtk::Window * myParent(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myParent);

        myNative->set_transient_for( * myParent);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION
}

JSFunctionSpec *
JSWindow::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"resize",               Resize,                  2},
        {"raise",                Raise,                   0},
        {"present",              Present,                 0},
        {"set_transient_for",    SetTransientFor,         1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSWindow::Properties() {
    static JSPropertySpec myProperties[] = {
        {"title",           PROP_title,            JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"decorated",       PROP_decorated,        JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"screenSize",      PROP_screenSize,       JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"position",        PROP_position,         JSPROP_ENUMERATE|JSPROP_PERMANENT}, // Vector2i
        {"size",            PROP_size,             JSPROP_ENUMERATE|JSPROP_PERMANENT}, // Vector2i
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSWindow::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSWindow::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSWindow::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_title:
            *vp = as_jsval(cx, theNative.get_title());
            return JS_TRUE;
        case PROP_decorated:
            *vp = as_jsval(cx, theNative.get_decorated());
            return JS_TRUE;
        case PROP_position:
            {
                int myX;
                int myY;
                theNative.get_position(myX, myY);
                *vp = as_jsval(cx, Vector2i(myX, myY));
                return JS_TRUE;
            }
        case PROP_size:
            {
                int myWidth;
                int myHeight;
                theNative.get_size(myWidth, myHeight);
                *vp = as_jsval(cx, Vector2i(myWidth, myHeight));
                return JS_TRUE;
            }
        case PROP_screenSize:
            *vp = as_jsval(cx, Vector2i(Gdk::screen_width(), Gdk::screen_height()));
            return JS_TRUE;
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSWindow::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    try {
        switch (theID) {
            case PROP_title:
                {
                    Glib::ustring theTitle;
                    convertFrom(cx, *vp, theTitle);
                    theNative.set_title(theTitle);
                    return JS_TRUE;
                }
            case PROP_decorated:
                {
                    bool theFlag;
                    convertFrom(cx, *vp, theFlag);
                    theNative.set_decorated(theFlag);
                    return JS_TRUE;
                }
            case PROP_position:
                {
                    Vector2i thePosition;
                    convertFrom(cx, *vp, thePosition);
                    theNative.move(thePosition[0], thePosition[1]);
                    return JS_TRUE;
                }
            case PROP_size:
                {
                    Vector2i theSize;
                    convertFrom(cx, *vp, theSize);
                    theNative.resize(theSize[0], theSize[1]);
                    return JS_TRUE;
                }
            default:
                return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
        }
    } HANDLE_CPP_EXCEPTION;
}


JSBool
JSWindow::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSWindow * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
        newNative->set_events(Gdk::ALL_EVENTS_MASK);

        myNewObject = new JSWindow(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSWindow::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSWindow::ConstIntProperties() {

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
JSWindow::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
            JSBASE::ClassName());
}

JSObject *
JSWindow::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSWindow::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSWindow::OWNERPTR theOwner, JSWindow::NATIVE * theNative) {
    JSObject * myReturnObject = JSWindow::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}
