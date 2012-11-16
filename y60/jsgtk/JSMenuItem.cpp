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
#include "JSMenuItem.h"

#include "JSBin.h"
#include "JSSignalProxy0.h"
#include "JSMenu.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::MenuItem, asl::Ptr<Gtk::MenuItem>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::MenuItem@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSMenuItem::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSMenuItem::Properties() {
    static JSPropertySpec myProperties[] = {
        {"signal_activate", PROP_signal_activate, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_activate_item", PROP_signal_activate_item, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"submenu", PROP_submenu, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSMenuItem::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSMenuItem::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSMenuItem::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_signal_activate:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_activate()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_activate_item:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_activate_item()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_submenu:
            {
                Gtk::Menu * mySubmenu = theNative.get_submenu();
                *vp = jslib::as_jsval(cx, mySubmenu);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSMenuItem::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_submenu:
            {
                Gtk::Menu * mySubMenu = 0;
                convertFrom(cx, *vp, mySubMenu);
                if (mySubMenu) {
                    theNative.set_submenu(*mySubMenu);
                } else {
                    theNative.remove_submenu();
                }
                return JS_TRUE;
            }
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSMenuItem::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSMenuItem * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
    } else if (argc == 1) {
        Gtk::Widget * myWidget = 0;

        // argument must be a widget or a string
        if (!convertFrom(cx, argv[0], myWidget)) {
            Glib::ustring myString;
            if (!convertFrom(cx, argv[0], myString)) {
                JS_ReportError(cx, "JSMenuItem::JSMenuItem(): Argument must be a widget or a string.");
                return JS_FALSE;
            } else {
                newNative = new NATIVE(myString);
            }
        } else {
            newNative = new NATIVE(*myWidget);
        }
    } else if (argc == 2) {
        Glib::ustring myString;
        bool myMnemonicFlag;

        if (!convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx, "JSMenuItem::JSMenuItem(): First argument must be a string.");
            return JS_FALSE;
        }
        if (!convertFrom(cx, argv[1], myMnemonicFlag)) {
            JS_ReportError(cx, "JSMenuItem::JSMenuItem(): Second argument must be a bool.");
            return JS_FALSE;
        }
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }
    myNewObject = new JSMenuItem(OWNERPTR(newNative), newNative);
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSMenuItem::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSBin::ConstIntProperties() {

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
JSMenuItem::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddProperties(cx, theClassProto, Properties());
    JSA_AddFunctions(cx, theClassProto, Functions());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
                JSBASE::ClassName());
}

JSObject *
JSMenuItem::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSMenuItem::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSMenuItem::OWNERPTR theOwner, JSMenuItem::NATIVE * theNative) {
    JSObject * myReturnObject = JSMenuItem::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}
