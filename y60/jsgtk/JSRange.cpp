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
#include "JSRange.h"

#include "JSWidget.h"
#include "JSSignalProxies.h"
#include "jsgtk.h"
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Range, asl::Ptr<Gtk::Range>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Range@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
set_increments(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    ensureParamCount(argc, 2);

    Gtk::Range * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

    double myStep;
    convertFrom(cx, argv[0], myStep);

    double myPage;
    convertFrom(cx, argv[1], myPage);

    myNative->set_increments(myStep, myPage);
    return JS_TRUE;
}

static JSBool
set_range(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    ensureParamCount(argc, 1, 2);

    Gtk::Range * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

    double myMin;
    double myMax;
    if (argc == 1) {
        asl::Vector2f myRange;
        convertFrom(cx, argv[0], myRange);
        myMin = myRange[0];
        myMax = myRange[1];
    } else {
        convertFrom(cx, argv[0], myMin);
        convertFrom(cx, argv[1], myMax);
    }

    myNative->set_range(myMin, myMax);
    return JS_TRUE;
}

JSFunctionSpec *
JSRange::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"set_increments",       set_increments,          2},
        {"set_range",            set_range,               2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSRange::Properties() {
    static JSPropertySpec myProperties[] = {
//        {"URL", PROP_URL, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"adjustment", PROP_adjustment, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"inverted", PROP_inverted, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"update_policy", PROP_update_policy, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"value", PROP_value, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_adjust_bounds", PROP_signal_adjust_bounds, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_move_slider", PROP_signal_move_slider, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_value_changed", PROP_signal_value_changed, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSRange::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSRange::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSRange::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_adjustment:
            // TODO
            return JS_FALSE;
        case PROP_inverted:
            *vp = as_jsval(cx, theNative.get_inverted());
            return JS_TRUE;
        case PROP_update_policy:
            // TODO
            return JS_FALSE;
        case PROP_value:
            *vp = as_jsval(cx, theNative.get_value());
            return JS_TRUE;
        case PROP_signal_adjust_bounds:
            {
                JSSignalProxy1<void, double>::OWNERPTR mySignal( new
                        JSSignalProxy1<void, double>::NATIVE(theNative.signal_adjust_bounds()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
            break;
        case PROP_signal_move_slider:
            // TODO
            return JS_FALSE;
        case PROP_signal_value_changed:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_value_changed()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSRange::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_adjustment:
            // TODO
            return JS_FALSE;
        case PROP_inverted:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.set_inverted(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_update_policy:
            // TODO
            return JS_FALSE;
        case PROP_value:
            try {
                double myValue;
                convertFrom(cx, *vp, myValue);
                theNative.set_value(myValue);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSRange::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSRange * myNewObject = 0;

    if (argc == 0) {
        newNative = 0;  // Abstract
        myNewObject = new JSRange(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSRange::Constructor: bad parameters");
    return JS_FALSE;
}

void
JSRange::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
                JSBASE::ClassName());
}

JSObject *
JSRange::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSRange::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSRange::OWNERPTR theOwner, JSRange::NATIVE * theNative) {
    JSObject * myReturnObject = JSRange::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}
