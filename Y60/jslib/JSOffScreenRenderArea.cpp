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
//   $RCSfile: JSStringMover.cpp,v $
//   $Author: christian $
//   $Revision: 1.10 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================


#include "JSOffScreenRenderArea.h"
#include "JSAbstractRenderWindow.h"

using namespace std;
using namespace asl;

namespace jslib {

typedef jslib::AbstractRenderWindow BASE;
typedef JSAbstractRenderWindow<OffScreenRenderArea> JSBASE;

static JSBool
renderToCanvas(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
    DOC_BEGIN("Renders my current scene onto the texture given by the target attribute of my canvas. ");
    DOC_PARAM_OPT("theCopyToImageFlag", "if true the underlying image is updated.", DOC_TYPE_BOOLEAN, false);
    DOC_END;
    
    ensureParamCount(argc, 0, 1);

    OffScreenRenderArea * myNative(0);
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

JSFunctionSpec *
JSOffScreenRenderArea::Functions() {
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"renderToCanvas",      renderToCanvas,           1},
        {"setScene",             JSBASE::setScene,          0},
        {0}
    };
    return myFunctions;
}

enum PropertyNumbers {
    PROP_BEGIN = -128,
};

JSPropertySpec *
JSOffScreenRenderArea::Properties() {
    static JSPropertySpec myProperties[] = {
        {"scene",           JSBASE::PROP_scene,             JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // node
        {"width",           JSBASE::PROP_width,             JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"height",          JSBASE::PROP_height,            JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        
        {"canvas",          JSBASE::PROP_canvas,            JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // Node
        {"renderingCaps",   JSBASE::PROP_renderingCaps,     JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // Node
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSOffScreenRenderArea::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSOffScreenRenderArea::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSOffScreenRenderArea::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}
 
// getproperty handling
JSBool
JSOffScreenRenderArea::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    switch (theID) {
        case JSBASE::PROP_renderingCaps:
        case JSBASE::PROP_canvas:
        case JSBASE::PROP_width:
        case JSBASE::PROP_height:
            return JSBASE::getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
        default:
            JS_ReportError(cx,"JSOffScreenRenderArea::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }

}


/*
template <>
struct JSClassTraits<AbstractRenderWindow> : public JSClassTraitsWrapper<OffScreenRenderArea, JSOffScreenRenderArea> {
    static JSClass * Class() {
        return JSOffScreenRenderArea::Base::Class();
    }
};
*/

// setproperty handling
JSBool
JSOffScreenRenderArea::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    switch (theID) {
        case JSBASE::PROP_canvas:
            AC_INFO << "setting canvas";
            try {
                dom::NodePtr myCanvas;
                convertFrom(cx, *vp, myCanvas);
                myObj.getNative().setCanvas(myCanvas);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;

        case JSBASE::PROP_renderingCaps:
            AC_INFO << "setting rendering caps";
            try {
                unsigned myRenderingCaps;
                convertFrom(cx, *vp, myRenderingCaps);
                myObj.getNative().setRenderingCaps(myRenderingCaps);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            JS_ReportError(cx,"JSOffScreenRenderArea::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSOffScreenRenderArea::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs a new OffScreenRenderArea.");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    
    OWNERPTR myNewObject = NATIVE::create();
    
    JSOffScreenRenderArea * myNewJSObject = new JSOffScreenRenderArea(myNewObject, &(*myNewObject));

    if (myNewJSObject) {
        JS_SetPrivate(cx,obj,myNewJSObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSOffScreenRenderArea::Constructor: error");
    return JS_FALSE;
}

JSObject *
JSOffScreenRenderArea::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), 0);
    DOC_CREATE(JSOffScreenRenderArea);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, OffScreenRenderArea *& theRenderArea) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSOffScreenRenderArea::NATIVE>::Class()) {
                theRenderArea = &(*JSClassTraits<JSOffScreenRenderArea::NATIVE>::getNativeOwner(cx,myArgument));
                return true;
            }
        }
    }
    return false;
}

} //namespace

