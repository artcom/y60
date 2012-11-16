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
#include "JSFrustum.h"

#include "JSVector.h"
#include "JSPlane.h"
#include "JSMatrix.h"
#include "JSWrapper.impl"

#include <y60/base/DataTypes.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace dom;

namespace jslib {

typedef asl::Frustum NATIVE;

template class JSWrapper<asl::Frustum>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a string representation of the frustrum");
    DOC_RVAL("The string", DOC_TYPE_STRING);
    DOC_END;
    std::string myStringRep = asl::as_string(JSFrustum::getJSWrapper(cx,obj).getNative());
    * rval = as_jsval(cx, myStringRep);
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

#define DEFINE_FLAG(NAME) { #NAME, PROP_ ## NAME , asl::NAME }

JSConstIntPropertySpec *
JSFrustum::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_FLAG(PERSPECTIVE),
        DEFINE_FLAG(ORTHONORMAL),
        {0}
    };
    return myProperties;
};

JSPropertySpec *
JSFrustum::Properties() {
    static JSPropertySpec myProperties[] = {
        {"left", PROP_left, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"right", PROP_right, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"top", PROP_top, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"bottom", PROP_bottom, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"near", PROP_near, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"far", PROP_far, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},

        {"hshift", PROP_hshift, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"vshift", PROP_vshift, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},

        {"hfov", PROP_hfov, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"vfov", PROP_vfov, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"width", PROP_width, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"height", PROP_height, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},

        {"type", PROP_type, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},

        {"left_plane", PROP_left_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"right_plane", PROP_right_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"top_plane", PROP_top_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"bottom_plane", PROP_bottom_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"near_plane", PROP_near_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"far_plane", PROP_far_plane, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},

        {"projectionmatrix", PROP_projectionmatrix, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
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
            case PROP_projectionmatrix:
                *vp = as_jsval(cx, getNative().getProjectionMatrix());
                return JS_TRUE;
            case PROP_width:
                *vp = as_jsval(cx, getNative().getWidth());
                return JS_TRUE;
            case PROP_height:
                *vp = as_jsval(cx, getNative().getHeight());
                return JS_TRUE;
            case PROP_hfov:
                *vp = as_jsval(cx, getNative().getHFov());
                return JS_TRUE;
            case PROP_vfov:
                *vp = as_jsval(cx, getNative().getVFov());
                return JS_TRUE;
            case PROP_hshift:
                *vp = as_jsval(cx, getNative().getHShift());
                return JS_TRUE;
            case PROP_vshift:
                *vp = as_jsval(cx, getNative().getVShift());
                return JS_TRUE;
            case PROP_type:
                *vp = as_jsval(cx, getNative().getType());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSFrustum::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
};

// setproperty handling
JSBool
JSFrustum::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    switch (theID) {
        case PROP_left:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setLeft(myValue);
                return JS_TRUE;
            }
        case PROP_right:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setRight(myValue);
                return JS_TRUE;
            }
        case PROP_top:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setTop(myValue);
                return JS_TRUE;
            }
        case PROP_bottom:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setBottom(myValue);
                return JS_TRUE;
            }
        case PROP_near:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setNear(myValue);
                return JS_TRUE;
            }
        case PROP_far:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setFar(myValue);
                return JS_TRUE;
            }
        case PROP_width:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setWidth(myValue);
                return JS_TRUE;
            }
        case PROP_height:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setHeight(myValue);
                return JS_TRUE;
            }
        case PROP_hfov:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setHFov(myValue);
                return JS_TRUE;
            }
        case PROP_vfov:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setVFov(myValue);
                return JS_TRUE;
            }
        case PROP_hshift:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setHShift(myValue);
                return JS_TRUE;
            }
        case PROP_vshift:
            {
                float myValue;
                convertFrom(cx, * vp, myValue);
                myObj.getNative().setVShift(myValue);
                return JS_TRUE;
            }
        case PROP_type:
            {
                ProjectionType myProjection(ORTHONORMAL);
                convertFrom(cx, * vp, myProjection);
                myObj.getNative().setType(myProjection);
                return JS_TRUE;
            }
        default:
            JS_ReportError(cx,"JSFrustum::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
};


JSBool
JSFrustum::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new empty frustrum");
    DOC_END;
    IF_NOISY2(cerr << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSFrustum * myNewObject = 0;
    JSFrustum::NativeValuePtr myNewValue = JSFrustum::NativeValuePtr(new FrustumValue(0));
    asl::Frustum & myNewFrustum = myNewValue->openWriteableValue(); // we close it only on success, otherwise we trash it anyway

    if (argc == 0) {
        // construct empty
        myNewObject = new JSFrustum(myNewValue);
    } else if (argc == 1) {
        if ( convertFrom(cx, argv[0], myNewFrustum ) ) {
            myNewObject = new JSFrustum( myNewValue );
        }
    } else {
        JS_ReportError(cx,"%s has only default and copy constructor.",ClassName());
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

bool convertFrom(JSContext *cx, jsval theValue, JSFrustum::NativeValuePtr & theValuePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Frustum>::Class()) {
                theValuePtr = JSClassTraits<asl::Frustum>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
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

