//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSMesh.cpp,v $
//   $Author: christian $
//   $Revision: 1.12 $
//   $Date: 2005/04/28 17:12:56 $
//
//
//=============================================================================

#include "JSMesh.h"

#include <y60/jslib/JSApp.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSBox.h>
#include <y60/jsbase/JSNode.h>
#include <y60/image/Image.h>
#include <y60/jslib/JSScene.h>
#include <y60/jsbase/JSSphere.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSMatrix.h>
#include <asl/dom/Nodes.h>

#include <asl/zip/PackageManager.h>
#include <jsgtk/JSSignal2.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) // x

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        *rval = as_jsval(cx, "Mesh");
    return JS_TRUE;
}

static JSBool
colorize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 2);
        unsigned myStartIndex;
        convertFrom(cx, argv[0], myStartIndex);
        unsigned myColorIndex;
        convertFrom(cx, argv[1], myColorIndex);
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        unsigned myNumVertices = myMesh.colorize(myStartIndex, myColorIndex);
        *rval = as_jsval(cx, myNumVertices);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
colorizeError(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 2);
        float myMaxError;
        convertFrom(cx, argv[0], myMaxError);
        unsigned myColorIndex;
        convertFrom(cx, argv[1], myColorIndex);
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        unsigned myNumVertices = myMesh.colorizeError(myMaxError, myColorIndex);
        *rval = as_jsval(cx, myNumVertices);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


static JSBool
colorizeDisconnected(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 1);
        unsigned myColorIndex;
        convertFrom(cx, argv[0], myColorIndex);
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        unsigned myNumVertices = myMesh.colorizeDisconnected(myColorIndex);
        *rval = as_jsval(cx, myNumVertices);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
computeError(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 0);
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        *rval = as_jsval(cx, myMesh.computeError());
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
removeMeshAndColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        myMesh.removeMeshAndColor();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
deleteIndicesByColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 1);
        unsigned myColorIndex;
        convertFrom(cx, argv[0], myColorIndex);
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        unsigned myNumDeleted = myMesh.deleteIndicesByColor(myColorIndex);
        *rval = as_jsval(cx, myNumDeleted);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 1);
        unsigned myColorIndex;
        convertFrom(cx, argv[0], myColorIndex);
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        myMesh.setColor(myColorIndex);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
deleteDanglingVertices(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 0);
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        unsigned myNumDeleted = myMesh.deleteDanglingVertices();
        *rval = as_jsval(cx, myNumDeleted);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
edgeCollapse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 1);
        unsigned int myPosition;
        convertFrom(cx, argv[0], myPosition);
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        myMesh.edgeCollapse(myPosition);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
collapseByError(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 1);
        float myMaxError;
        convertFrom(cx, argv[0], myMaxError);
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        unsigned myResult = myMesh.collapseByError(myMaxError);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
check(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        bool myResult = myMesh.check();
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
test(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 1);
        dom::NodePtr myNode;
        if (!convertFrom(cx, argv[0], myNode)) {
            JS_ReportError(cx,"JSCTScan::checkMesh: param 1 must be an Node (Shape)");
            return JS_FALSE;
        }
        bool myResult = Mesh::test(myNode);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
colorScreenSpaceLasso(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 3);
        dom::NodePtr myLassoBody;
        if (!convertFrom(cx, argv[0], myLassoBody)) {
            JS_ReportError(cx,"JSCTScan::colorScreenSpaceLasso: param 1 must be a body node (lasso)");
            return JS_FALSE;
        }
        dom::NodePtr myTransformationRoot;
        if (!convertFrom(cx, argv[1], myTransformationRoot)) {
            JS_ReportError(cx,"JSCTScan::colorScreenSpaceLasso: param 2 must be a body node (transformation root)");
            return JS_FALSE;
        }
        asl::Matrix4f myViewProjection;
        if (!convertFrom(cx, argv[2], myViewProjection)) {
            JS_ReportError(cx,"JSCTScan::colorScreenSpaceLasso: param 3 must be a Matrix4f (view-projection matrix)");
            return JS_FALSE;
        }
        unsigned int myColor;
        if (!convertFrom(cx, argv[3], myColor)) {
            JS_ReportError(cx,"JSCTScan::colorScreenSpaceLasso: param 4 must be an int (Color index)");
            return JS_FALSE;
        }
        unsigned myNumVertices = Mesh::colorScreenSpaceLasso(myLassoBody, myTransformationRoot, myViewProjection, myColor);
        *rval = as_jsval(cx, myNumVertices);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
colorSweptSphere(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 4);
        asl::Sphere<float> mySphere;
        if (!convertFrom(cx, argv[0], mySphere)) {
            JS_ReportError(cx,"JSCTScan::colorSweptSphere: param 1 must be a Sphere");
            return JS_FALSE;
        }
        asl::Vector3f myMotion;
        if (!convertFrom(cx, argv[1], myMotion)) {
            JS_ReportError(cx,"JSCTScan::colorSweptSphere: param 2 must be a Vector3f");
            return JS_FALSE;
        }
        dom::NodePtr myTransformationRoot;
        if (!convertFrom(cx, argv[2], myTransformationRoot)) {
            JS_ReportError(cx,"JSCTScan::colorSweptSphere: param 3 must be a Node (Transformation Node)");
            return JS_FALSE;
        }
        unsigned int myColor;
        if (!convertFrom(cx, argv[3], myColor)) {
            JS_ReportError(cx,"JSCTScan::colorSweptSphere: param 4 must be an int (Color index)");
            return JS_FALSE;
        }
        unsigned myNumVertices = Mesh::colorSweptSphere(mySphere, myMotion, myTransformationRoot, myColor);
        *rval = as_jsval(cx, myNumVertices);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSConstIntPropertySpec *
JSMesh::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {

        {0}
    };
    return myProperties;
};

JSFunctionSpec *
JSMesh::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                     native                     nargs
        {"colorize",                colorize,                   2},
        {"deleteIndicesByColor",    deleteIndicesByColor,       1},
        {"deleteDanglingVertices",  deleteDanglingVertices,     0},
        {"check",                   check,                      0},
        {"edgeCollapse",            edgeCollapse,               1},
        {"collapseByError",         collapseByError,            1},
        {"colorizeError",           colorizeError,              2},
        {"colorizeDisconnected",    colorizeDisconnected,       1},
        {"computeError",            computeError,               0},
        {"setColor",                setColor,                   1},
        {"removeMeshAndColor",      removeMeshAndColor,         0},
        {0}
    };
    return myFunctions;
}

JSFunctionSpec *
JSMesh::StaticFunctions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        {"test",                    test,                       1},
        {"colorScreenSpaceLasso",   colorScreenSpaceLasso,      4},
        {"colorSweptSphere",        colorSweptSphere,           4},
        // name                  native                   nargs
        {0}
    };
    return myFunctions;
}


JSPropertySpec *
JSMesh::Properties() {
    static JSPropertySpec myProperties[] = {
        {"signal_progress", PROP_signal_progress, JSPROP_READONLY | JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"simplify", PROP_simplify, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSMesh::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
    case PROP_signal_progress:
    {
        JSSignal2<bool,double,Glib::ustring>::OWNERPTR mySignal( new
            JSSignal2<bool,double,Glib::ustring>::NATIVE(getNative().signal_progress()));
        *vp = jslib::as_jsval(cx, mySignal);
        return JS_TRUE;
    }
    case PROP_simplify:
    {
        *vp = jslib::as_jsval(cx, getNative().getSimplifyProceed());
        return JS_TRUE;
    }
    default:
        JS_ReportError(cx,"JSMesh::getProperty: index %d out of range", theID);
        return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSMesh::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
    case PROP_simplify:
    {
        bool mySwitch;
        convertFrom(cx, *vp, mySwitch);
        JSClassTraits<Mesh>::ScopedNativeRef myObj(cx, obj);
        Mesh & myMesh = myObj.getNative();
        myMesh.setSimplifyProceed(mySwitch);
        return JS_TRUE;
    }
    default:
        JS_ReportError(cx,"JSMesh::setPropertySwitch: index %d out of range", theID);
        return JS_FALSE;
    }
}

JSBool
JSMesh::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    OWNERPTR myNewNative;
    switch (argc) {
        case 1:
            {
            dom::NodePtr myNode;
            convertFrom(cx, argv[0], myNode);
            myNewNative = OWNERPTR(new Mesh(myNode));
            }
            break;
        default:
            JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected none or one "
                    "(filepath), got %d", ClassName(), argc);
            return JS_FALSE;
    }

    JSMesh * myNewObject = new JSMesh(myNewNative, &(*myNewNative));
    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}

JSObject *
JSMesh::initClass(JSContext *cx, JSObject *theGlobalObject) {
    return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties(), 0, StaticFunctions());
}

bool convertFrom(JSContext *cx, jsval theValue, JSMesh::OWNERPTR & theMesh) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSMesh::NATIVE >::Class()) {
                theMesh = JSClassTraits<JSMesh::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSMesh::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSMesh::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSMesh::OWNERPTR theOwner, JSMesh::NATIVE * theMesh) {
    JSObject * myObject = JSMesh::Construct(cx, theOwner, theMesh);
    return OBJECT_TO_JSVAL(myObject);
}

}
