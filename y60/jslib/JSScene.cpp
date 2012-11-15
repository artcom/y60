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
#include "JSScene.h"

#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSNodeList.h>
#include <y60/jsbase/JSLine.h>
#include <y60/jsbase/JSBox.h>
#include <y60/jsbase/JSSphere.h>
#include <y60/jsbase/JSMatrix.h>
#include "JSintersection_functions.h"
#include "JSApp.h"
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

#include <asl/base/Logger.h>
#include <y60/scene/IProgressNotifier.h>
#include <y60/modelling/modelling_functions.h>
#include <y60/scene/TextureManager.h>
#include <y60/scene/ShapeBuilder.h>
#include <y60/scene/Body.h>
#include <y60/video/Movie.h>
#include <y60/video/Capture.h>

using namespace y60;
using namespace asl;
using namespace std;

namespace jslib {

template class JSWrapper<y60::Scene, asl::Ptr<y60::Scene, dom::ThreadingModel>, StaticAccessProtocol>;

class ProgressCallback : public y60::IProgressNotifier {
    public:
        ProgressCallback(JSContext * theContext, JSObject * theTarget, const string & theHandler)
            : _jsContext(theContext), _jsTarget(theTarget), _handlerName(theHandler)
        {};

        virtual void onProgress(float theProgress, const std::string & theMessage="") {
            jsval myVal;
            /*JSBool bOK =*/ JS_GetProperty(_jsContext, _jsTarget, _handlerName.c_str(), &myVal);
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

dom::NodePtr
getSingleViewport(const y60::Scene & theNative) {
    if (theNative.getCanvasRoot()->childNodesLength(CANVAS_NODE_NAME) != 1) {
        throw asl::Exception("single-viewport convenience call used when multiple canvases exist!",
                PLUS_FILE_LINE);
    }
    dom::NodePtr myCanvas = theNative.getCanvasRoot()->childNode(CANVAS_NODE_NAME);
    if (myCanvas->childNodesLength(VIEWPORT_NODE_NAME) != 1) {
        throw asl::Exception("single-viewport convenience call used when multiple viewports exist!",
                PLUS_FILE_LINE);
    }
    return myCanvas->childNode(VIEWPORT_NODE_NAME);
}

JSBool
toString(JSContext *cx, JSObject *obj, uintn argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a string representation of the scene's dom.");
    DOC_END;
    std::string myStringRep = asl::as_string(*(JSScene::getJSWrapper(cx,obj).getNative().getSceneDom()));
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}


static JSBool
pickBody(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a body");
    DOC_PARAM("theX", "", DOC_TYPE_INTEGER);
    DOC_PARAM("theY", "", DOC_TYPE_INTEGER);
    DOC_PARAM("theCanvas", "", DOC_TYPE_NODE);
    DOC_RVAL("body", DOC_TYPE_NODE)
    DOC_END;
    try {
        MAKE_SCOPE_TIMER(pickBody);
        ensureParamCount(argc, 2, 3);

        unsigned int theX = 0;
        unsigned int theY = 0;
        dom::NodePtr theCanvas;
        dom::NodePtr myPickedBody;
        convertFrom(cx, argv[0], theX);
        convertFrom(cx, argv[1], theY);
        if (argc > 2 && convertFrom(cx, argv[2], theCanvas)) {
            myPickedBody = JSScene::getJSWrapper(cx,obj).getNative().pickBody(theX, theY, theCanvas);
        } else {
            myPickedBody = JSScene::getJSWrapper(cx,obj).getNative().pickBody(theX, theY);
        }
        *rval = as_jsval(cx, myPickedBody);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getPickedBodyInformation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns information about intersection with nearest body");
    DOC_PARAM("theX", "", DOC_TYPE_INTEGER);
    DOC_PARAM("theY", "", DOC_TYPE_INTEGER);
    DOC_PARAM("theCanvas", "", DOC_TYPE_NODE);
    DOC_RVAL("IntersectionInfo", DOC_TYPE_OBJECT);
    DOC_END;
    try {
        MAKE_SCOPE_TIMER(pickBody);
        ensureParamCount(argc, 2, 3);
        unsigned int theX = 0;
        unsigned int theY = 0;
        dom::NodePtr myCanvas;
        y60::IntersectionInfo myIntersection;
        convertFrom(cx, argv[0], theX);
        convertFrom(cx, argv[1], theY);
        if (argc > 2 && convertFrom(cx, argv[2], myCanvas)) {
            JSScene::getJSWrapper(cx,obj).getNative().getPickedBodyInformation(theX, theY, myIntersection, myCanvas);
        } else {
            JSScene::getJSWrapper(cx,obj).getNative().getPickedBodyInformation(theX, theY, myIntersection);
        }
        *rval = as_jsval(cx, myIntersection);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getPickedBodiesInformation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns information about intersection bodies");
    DOC_PARAM("theX", "", DOC_TYPE_INTEGER);
    DOC_PARAM("theY", "", DOC_TYPE_INTEGER);
    DOC_PARAM("theCanvas", "", DOC_TYPE_NODE);
    DOC_RVAL("IntersectionInfoVector", DOC_TYPE_ARRAY);
    DOC_END;
    try {
        MAKE_SCOPE_TIMER(pickBody);
        ensureParamCount(argc, 2, 3);
        unsigned int theX = 0;
        unsigned int theY = 0;
        dom::NodePtr myCanvas;
        y60::IntersectionInfoVector myIntersections;
        convertFrom(cx, argv[0], theX);
        convertFrom(cx, argv[1], theY);
        if (argc > 2 && convertFrom(cx, argv[2], myCanvas)) {
            JSScene::getJSWrapper(cx,obj).getNative().getPickedBodiesInformation(theX, theY, myIntersections, myCanvas);
        } else {
            JSScene::getJSWrapper(cx,obj).getNative().getPickedBodiesInformation(theX, theY, myIntersections);
        }
        *rval = as_jsval(cx, myIntersections);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
pickBodyBySweepingSphereFromBodies(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a body");
    DOC_PARAM("theX", "", DOC_TYPE_INTEGER);
    DOC_PARAM("theY", "", DOC_TYPE_INTEGER);
    DOC_PARAM("theSphereRadius", "", DOC_TYPE_FLOAT);
    DOC_PARAM("theCanvas", "", DOC_TYPE_NODE);
    DOC_RVAL("body", DOC_TYPE_NODE)
    DOC_END;
    try {
        MAKE_SCOPE_TIMER(pickBodyBySweepingSphereFromBodies);
        ensureParamCount(argc, 3, 4);

        unsigned int theX = 0;
        unsigned int theY = 0;
        float theSphereRadius = 0;
        dom::NodePtr theCanvas;
        dom::NodePtr myPickedBody;
        convertFrom(cx, argv[0], theX);
        convertFrom(cx, argv[1], theY);
        convertFrom(cx, argv[2], theSphereRadius);
        if (argc > 3 && convertFrom(cx, argv[3], theCanvas)) {
            myPickedBody = JSScene::getJSWrapper(cx,obj).getNative().pickBodyBySweepingSphereFromBodies(theX, theY, theSphereRadius, theCanvas);
        } else {
            myPickedBody = JSScene::getJSWrapper(cx,obj).getNative().pickBodyBySweepingSphereFromBodies(theX, theY, theSphereRadius);
        }
        *rval = as_jsval(cx, myPickedBody);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
intersectBodies(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the intersections of a body with a given line, line segment, ray or box.");
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);    DOC_PARAM("theLine", "", DOC_TYPE_LINE);
    DOC_RESET;
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);
    DOC_PARAM("theLineSegment", "", DOC_TYPE_LINESEGMENT);
    DOC_RESET;
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);
    DOC_PARAM("theRay", "", DOC_TYPE_RAY);
    DOC_RESET;
    DOC_PARAM("theBody", "", DOC_TYPE_NODE);
    DOC_PARAM("theBox", "", DOC_TYPE_BOX3F);
    DOC_RVAL("IntersectionInfoVector", DOC_TYPE_ARRAY);
    DOC_PARAM_OPT("theIntersectInvisibleBodysFlag", "Intersect invisible bodies, default is true.", DOC_TYPE_BOOLEAN, true);

    DOC_END;
    try {
        ensureParamCount(argc, 2, 3);

        dom::NodePtr myBodies;
        y60::IntersectionInfoVector myIntersections;
        convertFrom(cx, argv[0], myBodies);
        bool theIntersectInvisibleBodysFlag = true;
        if (argc >= 2) {
            convertFrom(cx, argv[2], theIntersectInvisibleBodysFlag);
        }
        if (JSLineSegment::matchesClassOf(cx, argv[1])) {
            asl::LineSegment<float> myStick;
            convertFrom(cx, argv[1], myStick);
            y60::Scene::intersectBodies(myBodies, myStick, myIntersections, theIntersectInvisibleBodysFlag);
        } else if (JSLine::matchesClassOf(cx, argv[1])) {
            asl::Line<float> myLine;
            convertFrom(cx, argv[1], myLine);
            y60::Scene::intersectBodies(myBodies, myLine, myIntersections, theIntersectInvisibleBodysFlag);
        } else if (JSRay::matchesClassOf(cx, argv[1])) {
            asl::Ray<float> myRay;
            convertFrom(cx, argv[1], myRay);
            y60::Scene::intersectBodies(myBodies, myRay, myIntersections, theIntersectInvisibleBodysFlag);
        } else if (JSBox3f::matchesClassOf(cx, argv[1])) {
            asl::Box3<float> myBox;
            convertFrom(cx, argv[1], myBox);
            y60::Scene::intersectBodies(myBodies, myBox, myIntersections, theIntersectInvisibleBodysFlag);
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
intersectBodyCenters(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns all bodies where the center of the bounding box is inside the given box");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        dom::NodePtr myBodies;
        y60::IntersectionInfoVector myIntersections;
        convertFrom(cx, argv[0], myBodies);

        asl::Box3<float> myBox;
        convertFrom(cx, argv[1], myBox);
        y60::Scene::intersectBodyCenters(myBodies, myBox, myIntersections);

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
optimize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Optimizes the scene to one body and one shape.");
    DOC_PARAM_OPT("theRootNode", "A root node in the world, where the optimizer should start.", DOC_TYPE_NODE, 0);
    DOC_END;
     try {
        ensureParamCount(argc, 0, 1);
        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        dom::NodePtr myRootNode;
        if (argc >= 1) {
            if (!convertFrom(cx, argv[0], myRootNode)) {
                JS_ReportError(cx, "JSScene::optimize(): argument #1 must be a node (rootnode)");
                return JS_FALSE;
            }
        }

        myNative->optimize(myRootNode);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

/*static JSBool
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
*/
static JSBool
collectGarbage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Removes all dangeling and non referenced nodes from the scene.");
    DOC_END;
    ensureParamCount(argc, 0, 0);
    return Method<NATIVE>::call(&NATIVE::collectGarbage,cx,obj,argc,argv,rval);
}

static JSBool
loadMovieFrame(JSContext *cx, JSObject *obj, uintn argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Updates a movie node.");
    DOC_PARAM("theMovieNode", "The movie node to update.", DOC_TYPE_NODE);
    DOC_END;
    try {
        if (argc < 1) {
            throw asl::Exception(string("Not enough arguments"));
        }

        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        dom::NodePtr myNode;
        if (!convertFrom(cx, argv[0], myNode)) {
            JS_ReportError(cx, "JSScene::loadMovieFrame(): argument #1 must be a node (movienode)");
            return JS_FALSE;
        }

        ensureParamCount(argc, 1);
        myNative->getTextureManager()->loadMovieFrame(myNode->getFacade<Movie>());
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
ensureMovieFramecount(JSContext *cx, JSObject *obj, uintn argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Ensure a movie node has the correct framecount.");
    DOC_PARAM("theMovieNode", "The movie node to ensure.", DOC_TYPE_NODE);
    DOC_END;
    try {
        ensureParamCount(argc, 1);

        JSScene::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        dom::NodePtr myNode;
        convertFrom(cx, argv[0], myNode);
        myNode->getFacade<Movie>()->ensureMovieFramecount();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}



static JSBool
loadCaptureFrame(JSContext *cx, JSObject *obj, uintn argc, jsval *argv, jsval *rval) {
  DOC_BEGIN("Updates a capture node.");
  DOC_PARAM("theCaptureNode", "Capture node to update", DOC_TYPE_NODE);
    DOC_END;
    try {

      ensureParamCount(argc, 1);

      JSScene::OWNERPTR myNative;
      convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

      dom::NodePtr myNode;
      convertFrom(cx, argv[0], myNode);

      myNative->getTextureManager()->loadCaptureFrame(myNode->getFacade<Capture>());
      return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


enum PropertyNumbers {
    PROP_dom = -100,
    PROP_statistics,
    PROP_cameras,
    PROP_world,
    PROP_worlds,
    PROP_canvases,
    PROP_canvas,
    PROP_records,
    PROP_viewport,
    PROP_overlays,
    PROP_underlays,
    PROP_materials,
    PROP_lightsources,
    PROP_animations,
    PROP_characters,
    PROP_shapes,
    PROP_textures,
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
        {"intersectBodies",        intersectBodies,        3},
        {"intersectBodyCenters",   intersectBodyCenters,   2},
        {"collideWithBodies",      collideWithBodies,      3},
        {"collideWithBodiesOnce",  collideWithBodiesOnce,  3},
//        {"createStubs",         createStubs,         0},
        {0}
    };
    return myFunctions;
}
JSFunctionSpec *
JSScene::Functions() {
    AC_DEBUG << "Accessing functions for class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                   native               nargs    */
        {"update",                update,              1},
        {"updateAllModified",     updateAllModified,   1},
        {"clear",                 clear,               0},
        {"optimize",              optimize,            1},
        {"collectGarbage",        collectGarbage,      0},
        {"bodyVolume",            bodyVolume,          1},
        {"save",                  save,                3},
        {"saveWithCatalog",       saveWithCatalog,     3},
        {"setup",                 setup,               0},
        {"getWorldSize",          getWorldSize,        1},
        {"loadMovieFrame",        loadMovieFrame,      1},
        {"ensureMovieFramecount", ensureMovieFramecount,1},
        {"loadCaptureFrame",      loadCaptureFrame,    2},
        {"pickBody",              pickBody,              3},
        {"getPickedBodyInformation",   getPickedBodyInformation, 3},
        {"getPickedBodiesInformation", getPickedBodiesInformation, 3},
        {"pickBodyBySweepingSphereFromBodies",     pickBodyBySweepingSphereFromBodies,   4},
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
        {"worlds",       PROP_worlds,       JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"canvases",     PROP_canvases,     JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"records",      PROP_records,      JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"canvas",       PROP_canvas,       JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"viewport",     PROP_viewport,     JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"overlays",     PROP_overlays,     JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"underlays",    PROP_underlays,    JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"materials",    PROP_materials,    JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"lightsources", PROP_lightsources, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"animations",   PROP_animations,   JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"characters",   PROP_characters,   JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"shapes",       PROP_shapes,       JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
        {"textures",     PROP_textures,     JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED | JSPROP_READONLY},
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
    asl::Dashboard & myDashboard = getDashboard();
    unsigned long myRenderedPrimitives = myDashboard.getCounterValue("TransparentPrimitives") + myDashboard.getCounterValue("OpaquePrimitives");
    double myElapsedGC =  myDashboard.getTimer("gc")->getLastElapsed().millis();
    if (!JS_DefineProperty(cx, myReturnObject, "renderedPrimitives", as_jsval(cx, myRenderedPrimitives),  0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "renderedVertices", as_jsval(cx, myDashboard.getCounterValue("Vertices")), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "materialChange", as_jsval(cx, myDashboard.getCounterValue("materialChange")), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "activeLights", as_jsval(cx, myDashboard.getCounterValue("ActiveLights")), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "bodies", as_jsval(cx, myDashboard.getCounterValue("RenderedBodies")), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "activeWorlds", as_jsval(cx, myDashboard.getCounterValue("ActiveWorlds")),  0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "overlays", as_jsval(cx, myDashboard.getCounterValue("Overlays")),  0,0, JSPROP_ENUMERATE)) return JS_FALSE;
    if (!JS_DefineProperty(cx, myReturnObject, "gc", as_jsval(cx, myElapsedGC),  0,0, JSPROP_ENUMERATE)) return JS_FALSE;
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
        case PROP_worlds:
        	*vp = as_jsval(cx, getNative().getWorldsRoot());
        	return JS_TRUE;
        case PROP_canvases:
            *vp = as_jsval(cx, getNative().getCanvasRoot());
            return JS_TRUE;
        case PROP_records:
            *vp = as_jsval(cx, getNative().getRecordsRoot());
            return JS_TRUE;
         case PROP_canvas:
            *vp = as_jsval(cx, getNative().getCanvasRoot()->childNode(CANVAS_NODE_NAME));
            return JS_TRUE;
        case PROP_viewport:
            *vp = as_jsval(cx, getSingleViewport(getNative()));
            return JS_TRUE;
        case PROP_underlays:
        {
            dom::NodePtr myViewPort = getSingleViewport(getNative());
            dom::NodePtr myUnderlays = myViewPort->childNode(UNDERLAY_LIST_NAME);
            if (!myUnderlays) {
                myUnderlays = myViewPort->appendChild(dom::Element(UNDERLAY_LIST_NAME));
            }
            *vp = as_jsval(cx, myUnderlays);
            return JS_TRUE;
        }
        case PROP_overlays:
            *vp = as_jsval(cx, getSingleViewport(getNative())->childNode(OVERLAY_LIST_NAME));
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
        case PROP_textures:
            *vp = as_jsval(cx, getNative().getTexturesRoot());
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
    DOC_PARAM_OPT("loadLazy", "load only catalog and root node, children only on demand", DOC_TYPE_BOOLEAN,"");
    DOC_RESET;
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_PARAM("theTarget", "Target for ProgressCallback", DOC_TYPE_STRING);
    DOC_PARAM("theFunction", "Progress callback function name", DOC_TYPE_STRING);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    asl::Ptr<y60::Scene, dom::ThreadingModel> myNewPtr = OWNERPTR();

    JSScene * myNewObject = 0;

    try {
        if (!isCalledForConversion(cx, argc,argv)) {
            if (argc == 0) {
                AC_INFO << "no filename, creating scene stubs";
                PackageManagerPtr myPackageManager = JSApp::getPackageManager();
                myNewPtr = y60::Scene::createStubs(myPackageManager);
            }
            asl::Ptr<ProgressCallback> myCallback;
            if (argc >= 3) {
                JSObject * myTarget = JSVAL_TO_OBJECT(argv[1]);
                string myHandler;
                convertFrom(cx, argv[2], myHandler);
                myCallback = asl::Ptr<ProgressCallback>(new ProgressCallback(cx, myTarget,
                        myHandler));
            }

            if (argc >= 1) {
                std::string myFilename = as_string(cx, argv[0]);
                bool lazyFlag = false;
                if (argc == 2) {
                    convertFrom(cx, argv[0], lazyFlag);
                }

                PackageManagerPtr myPackageManager = JSApp::getPackageManager();
                AC_INFO << "Loading Scene " << getFilenamePart(myFilename) << " from "
                        << getDirectoryPart(myFilename)<<", lazy="<<lazyFlag;
                myPackageManager->add(asl::getDirectoryPart(myFilename));
                myNewPtr = y60::Scene::load(getFilenamePart(myFilename), myPackageManager,
                            myCallback, true, lazyFlag);
            }

        }
        myNewObject = new JSScene(myNewPtr, myNewPtr.get());
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
        JSObject * myReturnObject = JSScene::asJSVal(cx, theScene, theScene.get() );
        return OBJECT_TO_JSVAL(myReturnObject);
    }
    return JSVAL_NULL;
}


JSBool
JSScene::save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Saves the scene to a file.");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_PARAM("theBinaryFlag", "", DOC_TYPE_BOOLEAN);
    DOC_PARAM("theGreedyFlag", "", DOC_TYPE_BOOLEAN);
    DOC_END;
    ensureParamCount(argc, 1, 3);
    return Method<JSScene::NATIVE>::call(&JSScene::NATIVE::save,cx,obj,argc,argv,rval);
}

JSBool
JSScene::saveWithCatalog(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Saves the scene to a file.");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_PARAM("theCatalogFilenname", "", DOC_TYPE_STRING);
    DOC_PARAM("theBinaryFlag", "", DOC_TYPE_BOOLEAN);
    DOC_END;
    ensureParamCount(argc, 3, 3);
    return Method<JSScene::NATIVE>::call(&JSScene::NATIVE::saveWithCatalog,cx,obj,argc,argv,rval);
}

} // namespace

