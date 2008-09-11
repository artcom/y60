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
//   $RCSfile: JSDicomImage.cpp,v $
//   $Author: christian $
//   $Revision: 1.3 $
//   $Date: 2005/04/28 17:12:57 $
//
//
//=============================================================================

#include "JSDicomImage.h"

#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSNode.h>
#include <y60/image/Image.h>
#include <asl/dom/Nodes.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;

#define DB(x) // x

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    const DicomImage & myNative = JSDicomImage::getJSWrapper(cx,obj).getNative();
    std::string myStringRep = string("DicomImage ") +
        as_string(myNative.getWidth()) + "x" + as_string(myNative.getHeight()) +
        "x" + as_string(myNative.getDepth());
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
renderToImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<DicomImage>::ScopedNativeRef myObj(cx, obj);
        if (argc != 1) {
            JS_ReportError(cx, "JSDicomImage::renderToImage(): Wrong number of arguments, "
                               "expected one (Image Node), got %d.", argc);
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "JSDicomImage::renderToImage(): Argument #0 is undefined");
            return JS_FALSE;
        }
        dom::NodePtr myImageNode;
        convertFrom(cx, argv[0], myImageNode);

        DicomImage & myDicomImage = myObj.getNative();
        const void * myRenderedData = myDicomImage.getOutputData(8);
        unsigned myDataLength = myDicomImage.getOutputDataSize(8);
        myImageNode->getFacade<y60::Image>()->set(myDicomImage.getWidth(),
                myDicomImage.getHeight(), 1, y60::GRAY,
                ReadableBlockAdapter((const unsigned char*) myRenderedData,
                                     (const unsigned char*) myRenderedData + myDataLength));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSDicomImage::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"renderToImage",        renderToImage,           1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSDicomImage::Properties() {
    static JSPropertySpec myProperties[] = {
        {"window", PROP_window, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"status", PROP_status, JSPROP_READONLY | JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSDicomImage::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_window:
            try {
                JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
                double myWidth;
                double myCenter;
                myObj.getNative().getWindow(myCenter, myWidth);
                Vector2f myRetVal = Vector2f(float(myCenter), float(myWidth));
                *vp = as_jsval(cx, myRetVal);
            } HANDLE_CPP_EXCEPTION;
        case PROP_status:
            *vp = as_jsval(cx, getNative().getStatus());
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSDicomImage::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSDicomImage::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_window:
            try {
                JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
                asl::Vector2f myWindow;
                if (!convertFrom(cx, vp[0], myWindow)) {
                    JS_ReportError(cx, "JSDicomImage::window: argument must be a Vector2f (center,width)");
                    return JS_FALSE;
                }
                myObj.getNative().setWindow(double(myWindow[0]),double(myWindow[1]));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            JS_ReportError(cx,"JSDicomImage::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSDicomImage::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    if (argc != 1) {
        JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected 1 "
            "(filename), got %d", ClassName(), argc);
        return JS_FALSE;
    }

    if (JSVAL_IS_VOID(argv[0])) {
        JS_ReportError(cx,"JSDicomImage::Constructor: bad argument #1 (undefined)");
        return JS_FALSE;
    }

    string myFilename;
    if (!convertFrom(cx, argv[0], myFilename)) {
        JS_ReportError(cx, "JSDicomImage::Constructor: argument #1 must be a filename");
        return JS_FALSE;
    }

    OWNERPTR myNewNative = OWNERPTR(new DicomImage(myFilename.c_str()));
    JSDicomImage * myNewObject = new JSDicomImage(myNewNative, &(*myNewNative));
    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}

JSObject *
JSDicomImage::initClass(JSContext *cx, JSObject *theGlobalObject) {
    return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
}

/*
bool convertFrom(JSContext *cx, jsval theValue, JSDicomImage::NATIVE & theNative) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSDicomImage::NATIVE >::Class()) {
                theNative = JSClassTraits<JSDicomImage::NATIVE>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}
*/
jsval as_jsval(JSContext *cx, JSDicomImage::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSDicomImage::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSDicomImage::OWNERPTR theOwner, JSDicomImage::NATIVE * theSerial) {
    JSObject * myObject = JSDicomImage::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}

