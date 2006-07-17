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
//   $RCSfile: JSScene.cpp,v $
//   $Author: christian $
//   $Revision: 1.6 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSVector.h"
#include "JSNode.h"
#include "JSNodeList.h"
#include "JSLine.h"
#include "JSBox.h"
#include "JSSphere.h"
#include "JSMatrix.h"
#include "JSintersection_functions.h"
#include "JSScene.h"
#include "JSApp.h"
#include <iostream>

#include <asl/Logger.h>
#include <y60/IProgressNotifier.h>
#include <y60/modelling_functions.h>
#include <y60/TextureManager.h>
#include <y60/ShapeBuilder.h>
#include <y60/Body.h>
#include <y60/Movie.h>
#include <y60/Capture.h>

using namespace y60;
using namespace asl;
using namespace std;

namespace jslib {

class ProgressCallback : public y60::IProgressNotifier {
    public:
        ProgressCallback(JSContext * theContext, JSObject * theTarget, const string & theHandler)
            : _jsContext(theContext), _jsTarget(theTarget), _handlerName(theHandler)
        {};

        virtual void onProgress(float theProgress, const std::string & theMessage="") {
            jsval myVal;
            JSBool bOK = JS_GetProperty(_jsContext, _jsTarget, _handlerName.c_str(), &myVal);
            if (myVal == JSVAL_VOID) {
                AC_WARNING << "JS event handler for event '" << _handlerName << "' missing." << endl;
                return;
            }

            // call the function
            jsval argv[2], rval;
            argv[0] = as_jsval(_jsContext, theProgress);
            argv[1] = as_jsval(_jsContext, theMessage);

            JSBool ok = JSA_CallFunctionName(_jsContext, _jsTarget, _handlerName.c_str(), 2, argv, &rval);
            if (!ok) {
                AC_WARNING << "could not call progress callback " << _handlerName << endl;
            }
            return;
        }
    private:
        JSContext * _jsContext;
        JSObject * _jsTarget;
        string _handlerName;
};


typedef y60::Scene NATIVE;

JSBool
toString(JSContext *cx, JSObject *obj, uintn argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a string representation of the scene's dom.");
    DOC_END;
    std::string myStringRep = asl::as_string(*(JSScene::getJSWrapper(cx,obj).getNative().getSceneDom()));
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSBool
loadMovieFrame(JSContext *cx, JSObject *obj, uintn argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Updates a movie node.");
    DOC_PARAM("theMovieNode", "The movie node to update.", DOC_TYPE_NODE);
    DOC_PARAM_OPT("theCurrentTime", "The time for which the matching frame should be loaded.", DOC_TYPE_FLOAT, 0);
    DOC_END;
    try {
        if (argc < 1) {
            throw asl::Exception(string("Not enough arguments"));
        }

        dom::NodePtr myNode;
        convertFrom(cx, argv[0], myNode);

        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        if (argc == 1) {
            myNative->getTextureManager()->loadMovieFrame(myNode->getFacade<Movie>());
        } else {
            float myTime;
            convertFrom(cx, argv[1], myTime);
            myNative->getTextureManager()->loadMovieFrame(myNode->getFacade<Movie>(), myTime);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSBool
loadCaptureFrame(JSContext *cx, JSObject *obj, uintn argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Updates a capture node.");
    DOC_PARAM("theCaptureNode", "Capture node to update", DOC_TYPE_NODE);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        dom::NodePtr myNode;
        convertFrom(cx, argv[0], myNode);

        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        myNative->getTextureManager()->loadCaptureFrame(myNode->getFacade<Capture>());
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
intersectBodies(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the intersections of a body with a given line, line segment or ray.");
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);
    DOC_PARAM("theLine", "", DOC_TYPE_LINE);
    DOC_RESET;
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);
    DOC_PARAM("theLineSegment", "", DOC_TYPE_LINESEGMENT);
    DOC_RESET;
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);
    DOC_PARAM("theRay", "", DOC_TYPE_RAY);
    DOC_RESET;
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);
    DOC_PARAM("theBox", "", DOC_TYPE_BOX3F);
    DOC_RVAL("IntersectionInfoVector", DOC_TYPE_ARRAY)
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        dom::NodePtr myBodies;
        y60::IntersectionInfoVector myIntersections;
        convertFrom(cx, argv[0], myBodies);
        if (JSLineSegment::matchesClassOf(cx, argv[1])) {
            asl::LineSegment<float> myStick;
            convertFrom(cx, argv[1], myStick);
            y60::Scene::intersectBodies(myBodies, myStick, myIntersections);
        } else if (JSLine::matchesClassOf(cx, argv[1])) {
            asl::Line<float> myLine;
            convertFrom(cx, argv[1], myLine);
            y60::Scene::intersectBodies(myBodies, myLine, myIntersections);
        } else if (JSRay::matchesClassOf(cx, argv[1])) {
            asl::Ray<float> myRay;
            convertFrom(cx, argv[1], myRay);
            y60::Scene::intersectBodies(myBodies, myRay, myIntersections);
        } else if (JSBox3f::matchesClassOf(cx, argv[1])) {
            asl::Box3<float> myBox;
            convertFrom(cx, argv[1], myBox);
            y60::Scene::intersectBodies(myBodies, myBox, myIntersections);
        } 
        else {
            JS_ReportError(cx,"JSScene::intersectBodies: bad argument type #1");
            return JS_FALSE;
        }
        *rval = as_jsval(cx, myIntersections);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
collideWithBodies(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the collisions detected by recursively testing a sphere with a given motion vector and a body node.");
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);
    DOC_PARAM("theSphere", "", DOC_TYPE_SPHERE);
    DOC_PARAM("theMotionVector", "", DOC_TYPE_VECTOR3F);
    DOC_RVAL("CollisionInfoVector", DOC_TYPE_ARRAY)
    DOC_END;
    try {
        ensureParamCount(argc, 3);
        dom::NodePtr myBodies;
        asl::Sphere<float> mySphere;
        asl::Vector3<float> myMotion;
        y60::CollisionInfoVector myCollisions;
        convertFrom(cx, argv[0], myBodies);
        convertFrom(cx, argv[1], mySphere);
        convertFrom(cx, argv[2], myMotion);
        y60::Scene::collideWithBodies(myBodies, mySphere, myMotion, myCollisions);
        *rval = as_jsval(cx, myCollisions);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
collideWithBodiesOnce(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the first collision detected by recursively testing a sphere with a given motion vector and a body node.");
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);
    DOC_PARAM("theSphere", "", DOC_TYPE_SPHERE);
    DOC_PARAM("theMotionVector", "", DOC_TYPE_VECTOR3F);
    DOC_RVAL("CollisionInfo", DOC_TYPE_OBJECT)
    DOC_END;
    try {
        ensureParamCount(argc, 3);
        dom::NodePtr myBodies;
        asl::Sphere<float> mySphere;
        asl::Vector3<float> myMotion;
        y60::CollisionInfo myCollision;
        convertFrom(cx, argv[0], myBodies);
        convertFrom(cx, argv[1], mySphere);
        convertFrom(cx, argv[2], myMotion);
        y60::Scene::collideWithBodies(myBodies, mySphere, myMotion, myCollision);
        *rval = as_jsval(cx, myCollision);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
update(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Updates the components of the scene that are set in theUpdateFlags.");
    DOC_END;
    JSBool mySuccess = Method<NATIVE>::call(&NATIVE::update,cx,obj,argc,argv,rval);
    //*rval = as_jsval(cx, mySuccess);
    return mySuccess;
}

static JSBool
updateAllModified(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Updates all modified parts of the scene. You need this, if you implement your own render loop");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::updateAllModified,cx,obj,argc,argv,rval);
}

static JSBool
bodyVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Calculates the volume of a given body.");
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);
    DOC_RVAL("Volume", DOC_TYPE_FLOAT)
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        dom::NodePtr myBodyNode;
        convertFrom(cx, argv[0], myBodyNode);

        double myVolume = myBodyNode->getFacade<y60::Body>()->calculateVolume(*myNative);
        *rval = as_jsval(cx, myVolume);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
CreateLambertMaterial(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates an untextured lambert shaded materail.");
    DOC_PARAM_OPT("theDiffuseColor", "", DOC_TYPE_VECTOR4F, "[1,1,1,1]");
    DOC_PARAM_OPT("theAmbientColor", "", DOC_TYPE_VECTOR4F, "[0,0,0,1]");
    DOC_RVAL("theMaterialNode", DOC_TYPE_NODE)
    DOC_END;
    try {
        ensureParamCount(argc, 0, 2);
        JSScene::OWNERPTR myNative;

        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        dom::NodePtr myResult;
        if (argc == 0) {
            myResult = y60::createLambertMaterial(myNative);
        } else {
            asl::Vector4f myDiffuseColor;
            convertFrom(cx, argv[0], myDiffuseColor);
            if (argc == 1) {
                myResult = y60::createLambertMaterial(myNative, myDiffuseColor);
            } else {
                asl::Vector4f myAmbientColor;
                convertFrom(cx, argv[1], myAmbientColor);
                myResult = y60::createLambertMaterial(myNative, myDiffuseColor, myAmbientColor);
            }
        }

        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
CreateTexturedMaterial(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a textured, unlit material.");
    DOC_PARAM("theTextureFilename", "", DOC_TYPE_STRING);
    DOC_RESET;
    DOC_PARAM("theImageNode", "A image node that should be attached to the material", DOC_TYPE_NODE);
    DOC_RVAL("theMaterialNode", DOC_TYPE_NODE)
    DOC_END;
    try {
        ensureParamCount(argc, 1, 1);
        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        dom::NodePtr myResult;
        dom::NodePtr myImageNode;
        if (convertFrom(cx, argv[0], myImageNode)) {
            myResult = y60::createUnlitTexturedMaterial(myNative, myImageNode);
        } else {
            string myTextureFilename;
            if (convertFrom(cx, argv[0], myTextureFilename)) {
                myResult = y60::createUnlitTexturedMaterial(myNative, myTextureFilename);
            } else {
                JS_ReportError(cx, "JSScene::createTexturedMaterial(): argument #1 must be a string (File path) or a image node");
                return JS_FALSE;
            }
        }
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
CreateColorMaterial(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates an untextured and unlit colored material.");
    DOC_PARAM_OPT("theColor", "", DOC_TYPE_VECTOR4F, "[1,1,1,1]");
    DOC_RVAL("theMaterialNode", DOC_TYPE_NODE)
    DOC_END;
    try {
        ensureParamCount(argc, 0, 1);
        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        dom::NodePtr myResult;
        if (argc == 0) {
            myResult = y60::createColorMaterial(myNative);
        } else {
            asl::Vector4f myColor;
            convertFrom(cx, argv[0], myColor);
            myResult = y60::createColorMaterial(myNative, myColor);
        }

        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
CreateQuadShape(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a single quad.");
    DOC_PARAM("theMaterial", "", DOC_TYPE_NODE);
    DOC_PARAM("theTopLeftCorner", "", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theBottomRightCorner", "", DOC_TYPE_VECTOR3F);
    DOC_RVAL("theQuadShape", DOC_TYPE_NODE)
    DOC_END;
    try {
        ensureParamCount(argc, 3, 3);
        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        dom::NodePtr myMaterial;
        convertFrom(cx, argv[0], myMaterial);
        asl::Vector3f myTopLeftCorner;
        convertFrom(cx, argv[1], myTopLeftCorner);
        asl::Vector3f myBottomRightCorner;
        convertFrom(cx, argv[2], myBottomRightCorner);
        dom::NodePtr myResult = y60::createQuad(myNative, myMaterial->getAttributeString("id"), myTopLeftCorner, myBottomRightCorner);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
CreateBody(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a body inside the scene");
    DOC_PARAM("theShape", "A shape node inside the scene for this body", DOC_TYPE_NODE);
    DOC_PARAM_OPT("theParent", "A parent node inside the world hierarchy", DOC_TYPE_NODE, "toplevel node");
    DOC_RVAL("The new body", DOC_TYPE_NODE)
    DOC_END;
    try {
        ensureParamCount(argc, 1, 2);
        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        dom::NodePtr myShapeNode;
        convertFrom(cx, argv[0], myShapeNode);
        dom::NodePtr myParent = myNative->getWorldRoot();
        if (argc == 2) {
            convertFrom(cx, argv[1], myParent);
        }

        //using the facade we make sure we generate an ID if none was given
        ShapePtr myShape = myShapeNode->getFacade<Shape>();

        dom::NodePtr myResult = y60::createBody(myParent, myShape->get<IdTag>());
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
CreateImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates an image inside the scene");
    DOC_PARAM("theImageSource", "Path to image file", DOC_TYPE_NODE);
    DOC_RESET;
    DOC_PARAM("theWidth", "Image width", DOC_TYPE_INTEGER);
    DOC_PARAM("theHeight", "Image height", DOC_TYPE_INTEGER);
    DOC_PARAM("thePixelEncoding", "Pixel encoding", DOC_TYPE_STRING);
    //
    DOC_RVAL("The new image", DOC_TYPE_NODE)
    DOC_END;
    try {
        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        if (argc == 1) {
            std::string myImageSrc;
            if (!convertFrom(cx, argv[0], myImageSrc)) {
                JS_ReportError(cx, "JSScene::createImage(): argument #1 must be a string (File path)");
                return JS_FALSE;
            }

            dom::NodePtr myResult = myNative->getImagesRoot()->appendChild(
                dom::NodePtr(new dom::Element("image")));
            myResult->appendAttribute(IMAGE_SRC_ATTRIB, myImageSrc);
            *rval = as_jsval(cx, myResult);
        } else if (argc ==3) {
            unsigned myWidth;
            if (!convertFrom(cx, argv[0], myWidth)) {
                JS_ReportError(cx, "JSScene::createImage(): argument #1 must be a int (imagewidth)");
                return JS_FALSE;
            }
            unsigned myHeight;
            if (!convertFrom(cx, argv[1], myHeight)) {
                JS_ReportError(cx, "JSScene::createImage(): argument #2 must be a int (imageheight)");
                return JS_FALSE;
            }
            std::string myPixelEncoding;
            if (!convertFrom(cx, argv[2], myPixelEncoding)) {
                JS_ReportError(cx, "JSScene::createImage(): argument #3 must be a string (pixelencoding)");
                return JS_FALSE;
            }
            myPixelEncoding = asl::toUpperCase(myPixelEncoding);
            dom::NodePtr myResult = myNative->getImagesRoot()->appendChild(
                dom::NodePtr(new dom::Element("image")));
            y60::ImagePtr myImage = myResult->getFacade<y60::Image>();
            myImage->createRaster(myWidth, myHeight, 1, 
                PixelEncoding(getEnumFromString(myPixelEncoding, PixelEncodingString)));
            memset(myImage->getRasterPtr()->pixels().begin(), 0, myImage->getRasterPtr()->pixels().size());
            *rval = as_jsval(cx, myResult);

        } else {
            throw asl::Exception(string("Not enough arguments, must be 1 (filename) or 3(width,height,encoding)."));
        }
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getWorldSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the world's size including camera position.");
    DOC_PARAM("theCamera", "", DOC_TYPE_OBJECT);
    DOC_RVAL("theSize", DOC_TYPE_FLOAT)
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::getWorldSize,cx,obj,argc,argv,rval);
}

static JSBool
clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Empties the scene.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::clear,cx,obj,argc,argv,rval);
}

static JSBool
setup(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Setup the scene.");
    DOC_END;
    ensureParamCount(argc, 0, 0);
    return Method<NATIVE>::call(&NATIVE::setup,cx,obj,argc,argv,rval);
}

static JSBool
createStubs(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Create a minimal empty world.");
    DOC_END;
    try {
        ensureParamCount(argc, 0, 0);
        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        PackageManagerPtr myPackageManager = JSApp::getPackageManager();
        myNative->createStubs(myPackageManager);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

enum PropertyNumbers {
    PROP_dom = -100,
    PROP_statistics,
    PROP_cameras,
    PROP_world,
    PROP_canvases,
    PROP_canvas,
    PROP_viewport,
    PROP_overlays,
    PROP_underlays,
    PROP_materials,
    PROP_lightsources,
    PROP_animations,
    PROP_characters,
    PROP_shapes,
    PROP_images,
    PROP_document,
    PROP_MATERIALS,
    PROP_SHAPES,
    PROP_ANIMATIONS,
    PROP_ANIMATIONS_LOAD,
    PROP_WORLD,
    PROP_IMAGES,
    PROP_ALL,
    PROP_END
};

JSScene::~JSScene() {
    AC_TRACE << "JSScene DTOR " << this << endl;
}

JSFunctionSpec *
JSScene::StaticFunctions() {
    AC_DEBUG << "Accessing Static Functions to Class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                    native          nargs
        {"intersectBodies",        intersectBodies,        2},
        {"collideWithBodies",      collideWithBodies,      3},
        {"collideWithBodiesOnce",  collideWithBodiesOnce,  3},
        {0}
    };
    return myFunctions;
}
JSFunctionSpec *
JSScene::Functions() {
    AC_DEBUG << "Accessing functions for class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                 native               nargs    */
        {"update",              update,              1},
        {"updateAllModified",   updateAllModified,   1},            
        {"clear",               clear,               0},
        {"bodyVolume",          bodyVolume,          1},
        {"save",                save,                2},
        {"setup",               setup,                0},
        {"createStubs",         createStubs,         0},
        {"createLambertMaterial", CreateLambertMaterial, 2},
        {"createColorMaterial",   CreateColorMaterial,   1},
        {"createTexturedMaterial",CreateTexturedMaterial,1},
        {"createBody",            CreateBody,            2},
        {"createImage",           CreateImage,           3},
        {"createQuadShape",       CreateQuadShape,       3},
        {"loadMovieFrame",        loadMovieFrame,        1},
        {"loadCaptureFrame",      loadCaptureFrame,      1},
        {"getWorldSize",          getWorldSize,          1},
        {0}
    };
    return myFunctions;
}

#define DEFINE_SCENE_FLAG(NAME) { #NAME, PROP_ ## NAME , y60::Scene::NAME }

JSConstIntPropertySpec *
JSScene::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_SCENE_FLAG(MATERIALS),
        DEFINE_SCENE_FLAG(SHAPES),
        DEFINE_SCENE_FLAG(ANIMATIONS),
        DEFINE_SCENE_FLAG(ANIMATIONS_LOAD),
        DEFINE_SCENE_FLAG(WORLD),
        DEFINE_SCENE_FLAG(ALL),
        {0}
    };
    return myProperties;
};

JSPropertySpec *
JSScene::Properties() {
    static JSPropertySpec myProperties[] = {
        {"statistics",   PROP_statistics,   JSPROP_ENUMERATE | JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"dom",          PROP_dom,          JSPROP_ENUMERATE | JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"document",     PROP_document,     JSPROP_ENUMERATE | JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"cameras",      PROP_cameras,      JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"world",        PROP_world,        JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"canvases",     PROP_canvases,     JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"canvas",       PROP_canvas,       JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"viewport",     PROP_viewport,     JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"overlays",     PROP_overlays,     JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"underlays",    PROP_underlays,    JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"materials",    PROP_materials,    JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"lightsources", PROP_lightsources, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"animations",   PROP_animations,   JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"characters",   PROP_characters,   JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"shapes",       PROP_shapes,       JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"images",       PROP_images,       JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSScene::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSBool
JSScene::getStatistics(JSContext *cx, jsval *vp) {
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    Scene::Statistics myStatistics = getNative().getStatistics();
    asl::Dashboard & myDashboard = getDashboard();
    unsigned long myRenderedPrimitives = myDashboard.getCounterValue("TransparentPrimitives") + myDashboard.getCounterValue("OpaquePrimitives");
    if (!JS_DefineProperty(cx, myReturnObject, "primitives", as_jsval(cx, myStatistics.primitiveCount), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "renderedPrimitives", as_jsval(cx, myRenderedPrimitives),  0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "vertices", as_jsval(cx, myStatistics.vertexCount), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "renderedVertices", as_jsval(cx, myDashboard.getCounterValue("Vertices")), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "materials", as_jsval(cx, myStatistics.materialCount), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "lights", as_jsval(cx, myStatistics.lightCount), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "activeLights", as_jsval(cx, myDashboard.getCounterValue("ActiveLights")), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "bodies", as_jsval(cx, myDashboard.getCounterValue("RenderedBodies")), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "worldNodes", as_jsval(cx, myDashboard.getCounterValue("WorldNodes")),  0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "overlays", as_jsval(cx, myDashboard.getCounterValue("Overlays")),  0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    *vp = rval;
    return JS_TRUE;
}

// getproperty handling
JSBool
JSScene::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_statistics:
            return getStatistics(cx, vp);
        case PROP_cameras:
            {
                dom::NodePtr myNode = dom::NodePtr(new dom::Node());
                myNode->childNodes().setShell(0); // make sure our list does not clear parent pointers of appended nodes
                getNative().collectCameras(myNode->childNodes());
                *vp = as_jsval(cx, myNode, &myNode->childNodes());
                return JS_TRUE;
            }
        case PROP_dom:
            *vp = as_jsval(cx, getNative().getSceneDom()->childNode(SCENE_ROOT_NAME));
            return JS_TRUE;
        case PROP_document:
            *vp = as_jsval(cx, static_cast_Ptr<dom::Node>(getNative().getSceneDom()));
            return JS_TRUE;
        case PROP_world:
            *vp = as_jsval(cx, getNative().getWorldRoot());
            return JS_TRUE;
        case PROP_canvases:
            *vp = as_jsval(cx, getNative().getCanvasRoot());
            return JS_TRUE;
        case PROP_canvas:
            *vp = as_jsval(cx, getNative().getCanvasRoot()->childNode(CANVAS_NODE_NAME));
            return JS_TRUE;
        case PROP_viewport:
            *vp = as_jsval(cx, getNative().getCanvasRoot()->childNode(CANVAS_NODE_NAME)->childNode(VIEWPORT_NODE_NAME));
            return JS_TRUE;
        case PROP_underlays:
        {
            dom::NodePtr myViewPort = getNative().getCanvasRoot()->childNode(CANVAS_NODE_NAME)->childNode(VIEWPORT_NODE_NAME);
            dom::NodePtr myUnderlays = myViewPort->childNode(UNDERLAY_LIST_NAME);
            if (!myUnderlays) {
                myUnderlays = myViewPort->appendChild(dom::Element(UNDERLAY_LIST_NAME));
            }
            *vp = as_jsval(cx, myUnderlays);
            return JS_TRUE;
        }
        case PROP_overlays:
            *vp = as_jsval(cx, getNative().getCanvasRoot()->childNode(CANVAS_NODE_NAME)->childNode(VIEWPORT_NODE_NAME)->childNode(OVERLAY_LIST_NAME));
            return JS_TRUE;
        case PROP_materials:
            *vp = as_jsval(cx, getNative().getSceneDom()->childNode(SCENE_ROOT_NAME)->childNode(MATERIAL_LIST_NAME));
            return JS_TRUE;
        case PROP_lightsources:
            *vp = as_jsval(cx, getNative().getSceneDom()->childNode(SCENE_ROOT_NAME)->childNode(LIGHTSOURCE_LIST_NAME));
            return JS_TRUE;
        case PROP_animations:
            *vp = as_jsval(cx, getNative().getSceneDom()->childNode(SCENE_ROOT_NAME)->childNode(ANIMATION_LIST_NAME));
            return JS_TRUE;
        case PROP_characters:
            *vp = as_jsval(cx, getNative().getSceneDom()->childNode(SCENE_ROOT_NAME)->childNode(CHARACTER_LIST_NAME));
            return JS_TRUE;
        case PROP_shapes:
            *vp = as_jsval(cx, getNative().getShapesRoot());
            return JS_TRUE;
        case PROP_images:
            *vp = as_jsval(cx, getNative().getImagesRoot());
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSScene::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
};

// setproperty handling
JSBool
JSScene::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSScene::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
};

JSBool
JSScene::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a scene from the given file or an empty scene if no file is given.");
    DOC_RESET;
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_RESET;
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_PARAM("theTarget", "Target for ProgressCallback", DOC_TYPE_STRING);
    DOC_PARAM("theFunction", "Progress callback function name", DOC_TYPE_STRING);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    asl::Ptr<y60::Scene, dom::ThreadingModel> myNewPtr = OWNERPTR(0);;

    //OWNERPTR myNewPtr = OWNERPTR(0);
    //OWNERPTR myNewPtr = OWNERPTR(new y60::Scene());
    JSScene * myNewObject;//=new JSScene(myNewPtr, &(*myNewPtr));

    try {
        asl::Ptr<ProgressCallback> myCallback;
        if (argc >= 3) {
            JSObject * myTarget = JSVAL_TO_OBJECT(argv[1]);
            string myHandler;
            convertFrom(cx, argv[2], myHandler);
            myCallback = asl::Ptr<ProgressCallback>(new ProgressCallback(cx, myTarget, myHandler));
        }

        if (argc >= 1) {
            if (argv[0] == JSVAL_NULL) {
                AC_INFO << "no filename, creating scene stubs";
                PackageManagerPtr myPackageManager = JSApp::getPackageManager();
                myNewPtr = y60::Scene::createStubs(myPackageManager);
            } else {
                std::string myFilename = as_string(cx, argv[0]);
                PackageManagerPtr myPackageManager = JSApp::getPackageManager();
                AC_INFO << "Loading Scene " << getFilenamePart(myFilename) << " from " << getDirectoryPart(myFilename);
                myPackageManager->add(asl::getDirectoryPart(myFilename));
                //myNewPtr->load(getFilenamePart(myFilename), myPackageManager, myCallback);
                myNewPtr = y60::Scene::load(getFilenamePart(myFilename), myPackageManager, myCallback);
            }
        }

        myNewObject = new JSScene(myNewPtr, &(*myNewPtr));
        if (myNewObject) {
            JS_SetPrivate(cx,obj,myNewObject);
            return JS_TRUE;
        }
    } HANDLE_CPP_EXCEPTION;


    JS_ReportError(cx,"JSScene::Constructor: cannot be constructed.");
    return JS_FALSE;
}


JSObject *
JSScene::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
        Properties(), Functions(), ConstIntProperties(), 0, StaticFunctions());
    DOC_CREATE(JSScene);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<y60::Scene, dom::ThreadingModel> & theScene) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<y60::Scene>::Class()) {
                theScene = JSClassTraits<y60::Scene>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, asl::Ptr<y60::Scene, dom::ThreadingModel> theScene) {
    if (theScene) {
        JSObject * myReturnObject = JSScene::Construct(cx, theScene, &(*theScene));
        return OBJECT_TO_JSVAL(myReturnObject);
    }
    return JSVAL_NULL;
}


JSBool
JSScene::save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Saves the scene to a file.");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_PARAM("theBinaryFlag", "", DOC_TYPE_BOOLEAN);
    DOC_END;
    ensureParamCount(argc, 1, 2);
    return Method<JSScene::NATIVE>::call(&JSScene::NATIVE::save,cx,obj,argc,argv,rval);
}

} // namespace

