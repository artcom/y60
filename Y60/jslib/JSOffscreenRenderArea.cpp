//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSStringMover.cpp,v $
//   $Author: christian $
//   $Revision: 1.10 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================


#include "JSOffscreenRenderArea.h"
#include "JSAbstractRenderWindow.h"
//#include "AbstractRenderWindow.h"

using namespace std;
using namespace asl;

namespace jslib {

template <>
struct JSClassTraits<AbstractRenderWindow> : public JSClassTraitsWrapper<OffscreenRenderArea, JSOffscreenRenderArea> {
    static JSClass * Class() {
        return JSOffscreenRenderArea::Base::Class();
    }
};

typedef jslib::AbstractRenderWindow BASE;

static JSBool
renderToCanvas(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
    DOC_BEGIN("Renders my current scene onto the texture given by the target attribute of my canvas. ");
    DOC_PARAM_OPT("theCopyToImageFlag", "if true the underlying image is updated.", DOC_TYPE_BOOLEAN, false);
    DOC_END;

    ensureParamCount(argc, 0, 1);

    OffscreenRenderArea * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
    if (argc > 0) {
        bool myCopyToImageFlag;
        convertFrom(cx, argv[0], myCopyToImageFlag);
        myNative->renderToCanvas(myCopyToImageFlag);
    } else {
        myNative->renderToCanvas();
    }
    return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("sets the width of the offscreen area. TODO what exactly is this ?");
        DOC_PARAM("theWidth", "", DOC_TYPE_INTEGER);
        DOC_END;
        ensureParamCount(argc, 1);
        OffscreenRenderArea * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        
        unsigned myWidth;
        convertFrom(cx, argv[0], myWidth);
        myNative->setWidth(myWidth);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
downloadFromViewport(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("downloads the content of the offscreen area to an image");
        DOC_PARAM("theImage", "", DOC_TYPE_NODE);
        DOC_END;
        ensureParamCount(argc, 1);
        OffscreenRenderArea * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        
        dom::NodePtr myImageNode(0);
        if( ! convertFrom(cx, argv[0], myImageNode)) {
            JS_ReportError(cx, "OffscreenRenderArea::downloadFromViewport(): argument #0 must be an image node.");
            return JS_FALSE;
        }
        myNative->downloadFromViewport(myImageNode);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setHeight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("sets the height of the offscreen area. TODO what exactly is this ?");
        DOC_PARAM("theHeight", "", DOC_TYPE_INTEGER);
        DOC_END;
        ensureParamCount(argc, 1);
        OffscreenRenderArea * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        
        unsigned myHeight;
        convertFrom(cx, argv[0], myHeight);
        myNative->setHeight(myHeight);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSOffscreenRenderArea::Functions() {
    static JSFunctionSpec myFunctions[] = {
        // name                  native               nargs
        {"renderToCanvas",       renderToCanvas,       1},
        {"setWidth",             setWidth,             1},
        {"setHeight",            setHeight,            1},
        {"downloadFromViewport", downloadFromViewport, 1},
        {0}
    };
    return myFunctions;
}

enum PropertyNumbers {
    PROP_BEGIN = -128
};

JSPropertySpec *
JSOffscreenRenderArea::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSOffscreenRenderArea::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSOffscreenRenderArea::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSOffscreenRenderArea::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSOffscreenRenderArea::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSOffscreenRenderArea::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default: // delegate to base class
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }

}

// setproperty handling
JSBool
JSOffscreenRenderArea::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSOffscreenRenderArea::setPropertySwitch(NATIVE & theNative, unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch(theID) {
        case 0:
        default: // delegate to base class
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSOffscreenRenderArea::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs a new OffscreenRenderArea.");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }


    OWNERPTR myNewObject = NATIVE::create();

    JSOffscreenRenderArea * myNewJSObject = new JSOffscreenRenderArea(myNewObject, &(*myNewObject));

    if (myNewJSObject) {
        JS_SetPrivate(cx,obj,myNewJSObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSOffscreenRenderArea::Constructor: error");
    return JS_FALSE;
}


void
JSOffscreenRenderArea::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, JSBASE::BaseFunctions());
    JSA_AddProperties(cx, theClassProto, JSBASE::BaseProperties());
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
}

JSObject *
JSOffscreenRenderArea::initClass(JSContext *cx, JSObject *theGlobalObject) {
    //JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), 0);
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, 0);
    if (myClass) {
        addClassProperties(cx, myClass);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), & myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_AddFunctions(cx, myConstructorFuncObj, JSBASE::BaseStaticFunctions());
        JSA_DefineConstInts(cx, myConstructorFuncObj, JSBASE::ConstIntProperties());
    } else {
        cerr << "JSOffscreenRenderArea::initClass: constructor function object not "
             << "found, could not initialize static members" << endl;

    }
    DOC_CREATE(JSOffscreenRenderArea);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, OffscreenRenderArea *& theRenderArea) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSOffscreenRenderArea::NATIVE>::Class()) {
                theRenderArea = &(*JSClassTraits<JSOffscreenRenderArea::NATIVE>::getNativeOwner(cx,myArgument));
                return true;
            }
        }
    }
    return false;
}

} //namespace

