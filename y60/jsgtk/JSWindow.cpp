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
#include "JSWindow.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>
#include <gdkmm/general.h>

#ifdef WIN32
#include <gdk/gdkwin32.h>
#endif

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Window, asl::Ptr<Gtk::Window>, StaticAccessProtocol>;

typedef Gtk::Window NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = JSWindow::getJSWrapper(cx,obj).getNative().get_title();
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
Resize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2, 3);
        // native method call
        Gtk::Window * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        int theWidth;
        int theHeight;
        convertFrom(cx, argv[0], theWidth);
        convertFrom(cx, argv[1], theHeight);
        myNative->resize(theWidth, theHeight);
        if (argc == 3 ) {
            bool myFullScreenFlag = false;
            convertFrom(cx, argv[2], myFullScreenFlag);
            if (myFullScreenFlag) {
                myNative->fullscreen();
            } else {
                myNative->unfullscreen();
            }
        }
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
#ifdef WIN32
        HWND myWindow = reinterpret_cast<HWND>(GDK_WINDOW_HWND(myNative->get_window()->gobj()));
        ShowWindow(myWindow, SW_SHOW);
        SetForegroundWindow(myWindow);
#else
        myNative->raise();
#endif
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
        if (!convertFrom(cx, argv[0], myParent)) {
            JS_ReportError(cx,"set_transient_for: Parent is not a Window.");
            return JS_FALSE;
        }

        myNative->set_transient_for( * myParent);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION
}

static JSBool
SetDefaultSize(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        Gtk::Window * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        int myWidth;
        convertFrom(cx, argv[0], myWidth);

        int myHeight;
        convertFrom(cx, argv[1], myHeight);

        myNative->set_default_size( myWidth, myHeight);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION
}

static JSBool
SetIconFromFile(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Gtk::Window * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        Glib::ustring myString;
        if ( ! convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx,"SetIconFromFile: argument 0 must be a string");
            return JS_FALSE;
        }
        std::string myImageWithPath = searchFileRelativeToJSInclude(cx, obj, argc, argv, myString);
        myNative->set_icon_from_file( myImageWithPath );

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
        {"set_default_size",     SetDefaultSize,          2},
        {"set_icon_from_file",   SetIconFromFile,         1},
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
        {"type_hint",       PROP_type_hint,        JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"modal",           PROP_modal,            JSPROP_ENUMERATE|JSPROP_PERMANENT},
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
        case PROP_type_hint:
            *vp = as_jsval(cx, static_cast<int>(theNative.get_type_hint()));
            return JS_TRUE;
        case PROP_modal:
            *vp = as_jsval(cx, theNative.get_modal());
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
            case PROP_modal:
                {
                    bool myModelFlag = false;
                    convertFrom(cx, *vp, myModelFlag);
                    theNative.set_modal(myModelFlag);
                    return JS_TRUE;
                }
            case PROP_type_hint:
                {
                    int myTypeHint;
                    convertFrom(cx, *vp, myTypeHint);
                    theNative.set_type_hint(static_cast<Gdk::WindowTypeHint>(myTypeHint));
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
