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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "JSOffscreenRenderArea.h"

#include "JSAbstractRenderWindow.h"
#include "JSAbstractRenderWindow.impl"
#include <y60/jsbase/JSWrapper.impl>
#include <y60/jsbase/JSNode.h>

using namespace std;
using namespace asl;


namespace jslib {

template class JSWrapper<OffscreenRenderArea, asl::Ptr<OffscreenRenderArea>,
         StaticAccessProtocol>;

template class JSAbstractRenderWindow<OffscreenRenderArea>;

typedef jslib::AbstractRenderWindow BASE;

template <>
struct JSClassTraits<OffscreenRenderArea> : public JSClassTraitsWrapper<OffscreenRenderArea, JSOffscreenRenderArea> {
    static JSClass * Class() {
        return JSOffscreenRenderArea::Base::Class();
    }
};

static JSBool
renderToCanvas(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Render scene to texture given by the target attribute of canvas. ");
    DOC_PARAM_OPT("theCopyToImageFlag", "if true the underlying image is updated.", DOC_TYPE_BOOLEAN, false);
    DOC_PARAM_OPT("myCubemapFace", "myCubemapFace.", DOC_TYPE_BOOLEAN, false);
    DOC_PARAM_OPT("theClearColorBufferFlag", "if true the color buffer is cleared.", DOC_TYPE_BOOLEAN, true);
    DOC_PARAM_OPT("theDEpthColorBufferFlag", "if true the depth buffer is cleared.", DOC_TYPE_BOOLEAN, true);

    DOC_END;
    try {
        ensureParamCount(argc, 0, 4);

        OffscreenRenderArea * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        if (argc > 0) {
            bool myCopyToImageFlag;
            convertFrom(cx, argv[0], myCopyToImageFlag);
            if (argc > 1) {
                unsigned myCubemapFace;
                convertFrom(cx, argv[1], myCubemapFace);
                if (argc > 2) {
                    bool myClearColorBufferFlag;
                    convertFrom(cx, argv[2], myClearColorBufferFlag);
                    if (argc > 3) {
                        bool myDepthColorBufferFlag;
                        convertFrom(cx, argv[3], myDepthColorBufferFlag);
                        myNative->renderToCanvas(myCopyToImageFlag, myCubemapFace, myClearColorBufferFlag, myDepthColorBufferFlag);
                        return JS_TRUE;
                    } else {
                        myNative->renderToCanvas(myCopyToImageFlag, myCubemapFace, myClearColorBufferFlag);
                        return JS_TRUE;
                    }
                } else {
                    myNative->renderToCanvas(myCopyToImageFlag, myCubemapFace);
                    return JS_TRUE;
                }
            }
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
        DOC_BEGIN("Sets the width of the offscreen area. TODO what exactly is this ?");
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
setHeight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Sets the height of the offscreen area. TODO what exactly is this ?");
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

static JSBool
setScene(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	try {
		DOC_BEGIN("Set the scene the offscreen area should render");
		DOC_PARAM("theScene", "", DOC_TYPE_NODE);
		DOC_END;

		ensureParamCount(argc, 1);
        OffscreenRenderArea * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

		y60::ScenePtr myScene;
		convertFrom(cx, argv[0], myScene);
		myNative->setScene(myScene);
		return JS_TRUE;

	} HANDLE_CPP_EXCEPTION;
}

static JSBool
setCanvas(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	try {
		DOC_BEGIN("Set the canvas the offscreen area should render to");
		DOC_PARAM("theCanvas", "", DOC_TYPE_NODE);
		DOC_END;

		ensureParamCount(argc, 1);
        OffscreenRenderArea * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

		dom::NodePtr myCanvas;
		convertFrom(cx, argv[0], myCanvas);
		myNative->setCanvas(myCanvas);
		return JS_TRUE;

	} HANDLE_CPP_EXCEPTION;
}

static JSBool
setSceneAndCanvas(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	try {
		DOC_BEGIN("Set the canvas the offscreen area should render into the given canvas");
		DOC_PARAM("theScene", "", DOC_TYPE_OBJECT);
		DOC_PARAM("theCanvas", "", DOC_TYPE_NODE);
		DOC_END;

		ensureParamCount(argc, 2);
        OffscreenRenderArea * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

		y60::ScenePtr myScene;
		convertFrom(cx, argv[0], myScene);
		dom::NodePtr myCanvas;
		convertFrom(cx, argv[1], myCanvas);
		myNative->setSceneAndCanvas(myScene, myCanvas);
		return JS_TRUE;

	} HANDLE_CPP_EXCEPTION;
}

static JSBool
downloadFromViewport(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Downloads the content of the offscreen area to a texture.");
        DOC_PARAM("theTexture", "", DOC_TYPE_NODE);
        DOC_END;
        ensureParamCount(argc, 1);
        OffscreenRenderArea * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        dom::NodePtr myTextureNode;
        if( ! convertFrom(cx, argv[0], myTextureNode)) {
            JS_ReportError(cx, "OffscreenRenderArea::downloadFromViewport(): argument #0 must be a texture node.");
            return JS_FALSE;
        }
        myNative->downloadFromViewport(myTextureNode);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
activate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Activate as render target.");
    DOC_PARAM_OPT("theCubemapFace",
                  "The cubemap face index. To fill a dynamic cubemap, call activate and render one time for each face.",
                  DOC_TYPE_INTEGER, 0);
    DOC_END;
    ensureParamCount(argc, 0, 1);
    try {
        OffscreenRenderArea * mySelf;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        if (argc == 0) {
            mySelf->activate();
            return JS_TRUE;
        }

        unsigned myCubemapFace;
        if (!convertFrom(cx, argv[0], myCubemapFace)) {
            JS_ReportError(cx, "OffscreenRenderArea::activate(): argument #0 must be an unsigned int.");
            return JS_FALSE;
        }
        mySelf->activate(myCubemapFace);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
deactivate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Deactivate as render target and optionally copy result to attached image.");
    DOC_PARAM_OPT("theCopyToImageFlag", "if true the underlying image is updated.", DOC_TYPE_BOOLEAN, false);
    DOC_END;
    try {
        ensureParamCount(argc, 0, 1);

        OffscreenRenderArea * mySelf;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        if (argc > 0) {
            bool myCopyToImageFlag;
            convertFrom(cx, argv[0], myCopyToImageFlag);
            mySelf->deactivate(myCopyToImageFlag);
        } else {
            mySelf->deactivate();
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSOffscreenRenderArea::Functions() {
    static JSFunctionSpec myFunctions[] = {
        // name                  native               nargs
        {"renderToCanvas",       renderToCanvas,       4},
        {"setWidth",             setWidth,             1},
        {"setHeight",            setHeight,            1},
        {"setScene",             setScene,             1},
        {"setCanvas",            setCanvas,            1},
        {"setSceneAndCanvas",    setSceneAndCanvas,    2},
        {"downloadFromViewport", downloadFromViewport, 1},
        {"activate",             activate,             1},
        {"deactivate",           deactivate,           1},
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

    JSOffscreenRenderArea * myNewJSObject = new JSOffscreenRenderArea(myNewObject, myNewObject.get());

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
                theRenderArea = JSClassTraits<JSOffscreenRenderArea::NATIVE>::getNativeOwner(cx,myArgument).get();
                return true;
            }
        }
    }
    return false;
}

} //namespace

