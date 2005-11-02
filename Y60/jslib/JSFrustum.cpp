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
//   $RCSfile: JSFrustum.cpp,v $
//   $Author: christian $
//   $Revision: 1.2 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSVector.h"
#include "JSFrustum.h"
#include "JSPlane.h"
#include <iostream>

using namespace std;

namespace jslib {

typedef asl::Frustum NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = asl::as_string(JSFrustum::getJSWrapper(cx,obj).getNative());
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

JSFunctionSpec *
JSFrustum::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {"toString",           toString,                0},
        {0}
    };
    return myFunctions;
}

#define DEFINE_FLAG(NAME) { #NAME, PROP_ ## NAME , asl::Frustum::NAME }

JSConstIntPropertySpec *
JSFrustum::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_FLAG(PERSPECTIVE),
        DEFINE_FLAG(ORTHO),
        {0}
    };
    return myProperties;
};

JSPropertySpec *
JSFrustum::Properties() {
    static JSPropertySpec myProperties[] = {
        {"left", PROP_left, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"right", PROP_right, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"top", PROP_top, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"bottom", PROP_bottom, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"near", PROP_near, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"far", PROP_far, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"left_plane", PROP_left_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"right_plane", PROP_right_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"top_plane", PROP_top_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"bottom_plane", PROP_bottom_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"near_plane", PROP_near_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"far_plane", PROP_far_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSFrustum::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSFrustum::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}


// getproperty handling
JSBool
JSFrustum::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_left:
                *vp = as_jsval(cx, getNative().getLeft());
                return JS_TRUE;
            case PROP_right:
                *vp = as_jsval(cx, getNative().getRight());
                return JS_TRUE;
            case PROP_top:
                *vp = as_jsval(cx, getNative().getTop());
                return JS_TRUE;
            case PROP_bottom:
                *vp = as_jsval(cx, getNative().getBottom());
                return JS_TRUE;
            case PROP_near:
                *vp = as_jsval(cx, getNative().getNear());
                return JS_TRUE;
            case PROP_far:
                *vp = as_jsval(cx, getNative().getFar());
                return JS_TRUE;
            case PROP_left_plane:
                *vp = as_jsval(cx, getNative().getLeftPlane());
                return JS_TRUE;
            case PROP_right_plane:
                *vp = as_jsval(cx, getNative().getRightPlane());
                return JS_TRUE;
            case PROP_top_plane:
                *vp = as_jsval(cx, getNative().getTopPlane());
                return JS_TRUE;
            case PROP_bottom_plane:
                *vp = as_jsval(cx, getNative().getBottomPlane());
                return JS_TRUE;
            case PROP_near_plane:
                *vp = as_jsval(cx, getNative().getNearPlane());
                return JS_TRUE;
            case PROP_far_plane:
                *vp = as_jsval(cx, getNative().getFarPlane());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSFrustum::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
};

// setproperty handling
JSBool
JSFrustum::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSFrustum::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
};


JSBool
JSFrustum::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    IF_NOISY2(cerr << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSFrustum * myNewObject = 0;
    JSFrustum::NativeValuePtr myNewValue = JSFrustum::NativeValuePtr(new dom::SimpleValue<asl::Frustum>(0));
    asl::Frustum & myNewFrustum = myNewValue->openWriteableValue(); // we close it only on success, otherwise we trash it anyway

    if (argc == 0) {
        // construct empty
        myNewObject = new JSFrustum(myNewValue);
    } else {
        JS_ReportError(cx,"Constructor for %s not yet implemented!",ClassName());
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        myNewValue->closeWriteableValue();
        return JS_TRUE;
    }
    return JS_FALSE;
}


JSObject *
JSFrustum::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    createClassModuleDocumentation("Math", ClassName(), Properties(), Functions(),
            ConstIntProperties(), 0, 0, 0);
    return myClass;

}

bool convertFrom(JSContext *cx, jsval theValue, asl::Frustum & theFrustum) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Frustum>::Class()) {
                theFrustum = JSClassTraits<asl::Frustum>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, const asl::Frustum & theValue) {
    JSObject * myReturnObject = JSFrustum::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSFrustum::NativeValuePtr theValue) {
    JSObject * myObject = JSFrustum::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

}

