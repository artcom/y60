//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSGradientSlider.h"
#include "JSSignal0.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;
using namespace acgtk;

namespace jslib {

template class JSWrapper<acgtk::GradientSlider, asl::Ptr<acgtk::GradientSlider>,
        StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = "GradientSlider";
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSGradientSlider::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

#define DEFINE_PROPERTY( NAME ) \
    { #NAME, PROP_ ## NAME,  JSPROP_ENUMERATE|JSPROP_PERMANENT}

#define DEFINE_RO_PROPERTY( NAME ) \
    { #NAME, PROP_ ## NAME,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT}

JSPropertySpec *
JSGradientSlider::Properties() {
    static JSPropertySpec myProperties[] = {
        DEFINE_PROPERTY(value_range),
        DEFINE_PROPERTY(transfer_function),
        DEFINE_PROPERTY(selected_color),
        DEFINE_RO_PROPERTY(signalValuesChanged),
        DEFINE_RO_PROPERTY(signalSelectionChanged),
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSGradientSlider::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSGradientSlider::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSGradientSlider::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_value_range:
            *vp = jslib::as_jsval(cx, theNative.getValueRange());
            return JS_TRUE;
        case PROP_transfer_function:
            *vp = jslib::as_jsval(cx, theNative.getTransferFunction());
            return JS_TRUE;
        case PROP_selected_color:
            *vp = jslib::as_jsval(cx, theNative.getSelectedColor());
            return JS_TRUE;
        case PROP_signalValuesChanged:
            {
                JSSignal0<void>::OWNERPTR mySignal( new
                        JSSignal0<void>::NATIVE(theNative.getSignalValuesChanged()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signalSelectionChanged:
            {
                JSSignal0<void>::OWNERPTR mySignal( new
                        JSSignal0<void>::NATIVE(theNative.getSignalSelectionChanged()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSGradientSlider::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_value_range:
            try {
                asl::Vector2f myValueRange;
                convertFrom(cx, *vp, myValueRange);
                theNative.setValueRange(myValueRange);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_transfer_function:
            try {
                dom::NodePtr myNode;
                convertFrom(cx, *vp, myNode);
                theNative.setTransferFunction(myNode);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_selected_color:
            try {
                dom::NodePtr myNode;
                convertFrom(cx, *vp, myNode);
                theNative.setSelectedColor(myNode);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSGradientSlider::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;
    JSGradientSlider * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
    } else {
        JS_ReportError(cx,"Constructor for %s takes one or zero params, got %d.",ClassName(), argc);
        return JS_FALSE;
    }
    OWNERPTR newOwner(newNative);
    myNewObject = new JSGradientSlider(newOwner, newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSGradientSlider::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSGradientSlider::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
    /*
            "MODE_THRESHOLD",    PROP_MODE_THRESHOLD, acgtk::GradientSlider::MODE_THRESHOLD,
            "MODE_CENTER_WIDTH", PROP_MODE_CENTER_WIDTH, acgtk::GradientSlider::MODE_CENTER_WIDTH,
            "MODE_LOWER_UPPER",  PROP_MODE_LOWER_UPPER, acgtk::GradientSlider::MODE_LOWER_UPPER,
    */
        {0}
    };
    return myProperties;
};

void
JSGradientSlider::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);

    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    //TODO - multiple inheritance
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSBASE::ClassName());
}

JSObject *
JSGradientSlider::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSGradientSlider::initClass: constructor function object not found, " 
             << "could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSGradientSlider::NATIVE * theNative) {
    JSObject * myReturnObject = JSGradientSlider::Construct(cx, JSGradientSlider::OWNERPTR(0), theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<GradientSlider> & theGradientSlider) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSGradientSlider::NATIVE >::Class()) {
                theGradientSlider = JSClassTraits<JSGradientSlider::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

}

