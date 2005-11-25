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
//   $RCSfile: JSResizeableRaster.cpp,v $
//   $Author: pavel $
//   $Revision: 1.3 $
//   $Date: 2005/04/24 00:41:19 $
//
//
//=============================================================================

#include "JSResizeableRaster.h"
#include "JSNode.h"
#include "JSVector.h"

#include <iostream>

using namespace std;

namespace jslib {
typedef dom::ResizeableRaster NATIVE;
typedef JSWrapper<NATIVE,dom::ValuePtr> Base;

static JSBool
fillRect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Fills a recangular region of the raster with a given color.");
    DOC_PARAM("xmin", "left", DOC_TYPE_INTEGER);
    DOC_PARAM("ymin", "top", DOC_TYPE_INTEGER);
    DOC_PARAM("xmax", "right", DOC_TYPE_INTEGER);
    DOC_PARAM("ymax", "bottom", DOC_TYPE_INTEGER);
    DOC_PARAM("r", "red value", DOC_TYPE_FLOAT);
    DOC_PARAM("g", "green value", DOC_TYPE_FLOAT);
    DOC_PARAM("b", "blue value", DOC_TYPE_FLOAT);
    DOC_PARAM("a", "alpha value", DOC_TYPE_FLOAT);
    DOC_END;
//    typedef void (NATIVE::*MyMethod)(unsigned long,unsigned long, unsigned long,unsigned long, asl::Vector4f);
    typedef void (NATIVE::*MyMethod)(unsigned long,unsigned long, unsigned long,unsigned long,  float r, float g, float b, float a);
    return Method<NATIVE>::call((MyMethod)&NATIVE::fillRect,cx,obj,argc,argv,rval);
}

static JSBool
getPixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get color of pixel.");
    DOC_PARAM("x", "x pos", DOC_TYPE_INTEGER);
    DOC_PARAM("y", "y pos", DOC_TYPE_INTEGER);
    DOC_RVAL("Color", DOC_TYPE_VECTOR4F);
    DOC_END;
    typedef asl::Vector4f (NATIVE::*MyMethod)(unsigned long,unsigned long) const;
    return Method<NATIVE>::call((MyMethod)&NATIVE::getPixel,cx,obj,argc,argv,rval);
}

static JSBool
setPixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set color of pixel.");
    DOC_PARAM("x", "x pos", DOC_TYPE_INTEGER);
    DOC_PARAM("y", "y pos", DOC_TYPE_INTEGER);
    DOC_PARAM("r", "red value", DOC_TYPE_FLOAT);
    DOC_PARAM("g", "green value", DOC_TYPE_FLOAT);
    DOC_PARAM("b", "blue value", DOC_TYPE_FLOAT);
    DOC_PARAM("a", "alpha value", DOC_TYPE_FLOAT);
    DOC_END;
    //typedef void (NATIVE::*MyMethod)(unsigned long,unsigned long, asl::Vector4f);

    typedef void (NATIVE::*MyMethod)(unsigned long,unsigned long, float r, float g, float b, float a);
    return Method<NATIVE>::call((MyMethod)&NATIVE::setPixel,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSResizeableRaster::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name         native          nargs    */
        {"getPixel",    getPixel,         2},
        {"setPixel",    setPixel,         6},
        {"fillRect",    fillRect,         8},
        {0}
    };
    return myFunctions;
}
enum PropertyNumbers {PROP_width = -100,
                      PROP_height = -101};

JSPropertySpec *
JSResizeableRaster::Properties() {
    static JSPropertySpec myProperties[] = {
        {"width", PROP_width, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {"height", PROP_height, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSResizeableRaster::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSResizeableRaster::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSResizeableRaster::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSResizeableRaster::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_width:
                *vp = as_jsval(cx, getNative().width());
                return JS_TRUE;
            case PROP_height:
                *vp = as_jsval(cx, getNative().height());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSResizeableRaster::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}
/*
JSBool JSResizeableRaster::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    *vp = as_jsval(cx, getNative().getElement(theIndex));
    return JS_TRUE;
}
*/
// setproperty handling
JSBool JSResizeableRaster::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JS_ReportError(cx,"JSResizeableRaster::setPropertySwitch: index %d out of range", theID);
    return JS_FALSE;
}
/*
JSBool JSResizeableRaster::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    dom::ValuePtr myArg;
    if (convertFrom(cx, *vp, myArg)) {
        IF_NOISY_Y(AC_TRACE << "JSResizeableRaster::setPropertyIndex theIndex =" << theIndex << " myArg: " << (void*)myArg.getNativePtr() << endl);
        openNative().setElement(theIndex,*myArg);
        closeNative();
        return JS_TRUE;
    }
    return JS_TRUE;
}
*/
JSBool
JSResizeableRaster::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSResizeableRaster * myNewObject = 0;
    if (argc == 0) {
        myNewObject=new JSResizeableRaster();
    } else {
        JS_ReportError(cx,"Constructor for %s: superflous argument ignored",ClassName());
        myNewObject=new JSResizeableRaster();
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSResizeableRaster::Constructor: bad parameters");
    return JS_FALSE;
}


jsval as_jsval(JSContext *cx, dom::ValuePtr theValuePtr, dom::ResizeableRaster *) {
    JSObject * myObject = JSResizeableRaster::Construct(cx, theValuePtr);
    return OBJECT_TO_JSVAL(myObject);
}

}
