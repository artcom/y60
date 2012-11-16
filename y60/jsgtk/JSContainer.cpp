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
#include "JSContainer.h"

#include "JSWidget.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Container, asl::Ptr<Gtk::Container>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Container@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
Add(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        Gtk::Widget * myWidgetToAdd;

        for (unsigned i = 0; i < argc; ++i) {
            if (JSVAL_IS_VOID(argv[i])) {
                JS_ReportError(cx, "JSContainer::add(): Argument #%d is undefined", i + 1);
                return JS_FALSE;
            }
        }

        if (argc == 1) {
            if (!convertFrom(cx, argv[0], myWidgetToAdd)) {
                JS_ReportError(cx, "JSContainer::add(): argument #1 must be a widget");
                return JS_FALSE;
            }
        } else {
            JS_ReportError(cx, "JSContainer::add(): Wrong number of arguments, 1 expected.");
            return JS_FALSE;
        }
        Gtk::Container * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
        mySelf->add(*myWidgetToAdd);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;

}

static JSBool
Remove(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        Gtk::Widget * myWidgetToRemove;

        for (unsigned i = 0; i < argc; ++i) {
            if (JSVAL_IS_VOID(argv[i])) {
                JS_ReportError(cx, "JSContainer::remove(): Argument #%d is undefined", i + 1);
                return JS_FALSE;
            }
        }

        if (argc == 1) {
            if (!convertFrom(cx, argv[0], myWidgetToRemove)) {
                JS_ReportError(cx, "JSContainer::remove(): argument #1 must be a widget");
                return JS_FALSE;
            }
        } else {
            JS_ReportError(cx, "JSContainer::remove(): Wrong number of arguments, 1 expected.");
            return JS_FALSE;
        }
        Gtk::Container * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
        mySelf->remove(*myWidgetToRemove);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;

}

static JSBool
ShowAllChildren(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0, 1);
        bool myRecursiveFlag = true;
        if (argc == 1) {
            if (!convertFrom(cx, argv[0], myRecursiveFlag)) {
                JS_ReportError(cx, "JSContainer::add(): argument #1 must be a widget");
                return JS_FALSE;
            }
        }
        Gtk::Container * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
        mySelf->show_all_children(myRecursiveFlag);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

Gtk::Widget *
searchForChild(Gtk::Container * theContainer, const Glib::ustring & theName) {
    Glib::ListHandle<Gtk::Widget*> myChildren = theContainer->get_children();

    Glib::ListHandle<Gtk::Widget*>::iterator it = myChildren.begin();
    for (it=myChildren.begin(); it!=myChildren.end(); ++it) {
        if ((*it)->get_name() == theName) {
            return *it;
        }
        Gtk::Container * myChildContainer = dynamic_cast<Gtk::Container*>(*it);
        if (myChildContainer) {
            Gtk::Widget * myMatch = searchForChild(myChildContainer, theName);
            if (myMatch) {
                return myMatch;
            }
        }
    }
    return 0;
}

static JSBool
FindChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Glib::ustring theChildName;
        if (!convertFrom(cx, argv[0], theChildName)) {
            JS_ReportError(cx, "JSContainer::find_child(): argument #1 must be a widget name");
            return JS_FALSE;
        }
        Gtk::Container * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        Gtk::Widget * myMatch = searchForChild(mySelf, theChildName);
        if (myMatch) {
            *rval = gtk_jsval(cx, myMatch);
        } else {
            *rval = JSVAL_NULL;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSContainer::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"add",                  Add,                     1},
        {"remove",               Remove,                  1},
        {"show_all_children",    ShowAllChildren,         1},
        {"find_child",           FindChild,               1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSContainer::Properties() {
    static JSPropertySpec myProperties[] = {
        {"border_width", PROP_border_width, JSPROP_ENUMERATE|JSPROP_PERMANENT},
//        {"URL", PROP_URL, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSContainer::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSContainer::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSContainer::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_border_width:
            {
                *vp = as_jsval(cx, theNative.get_border_width());
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSContainer::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_border_width:
            {
                int myBorderWidth;
                convertFrom(cx, *vp, myBorderWidth);
                theNative.set_border_width(myBorderWidth);
                return JS_TRUE;
            }
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSContainer::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSContainer * myNewObject = 0;

    if (argc == 0) {
        newNative = 0;  // Abstract
        myNewObject = new JSContainer(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSContainer::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSContainer::ConstIntProperties() {

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
JSContainer::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
            JSBASE::ClassName());
}

JSObject *
JSContainer::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSContainer::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSContainer::OWNERPTR theOwner, JSContainer::NATIVE * theNative) {
    JSObject * myReturnObject = JSContainer::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}
