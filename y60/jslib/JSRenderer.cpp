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
#include "JSRenderer.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSBox.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSLine.h>
#include <y60/jsbase/JSTriangle.h>
#include <y60/jsbase/JSSphere.h>
#include "JSBSpline.h"
#include "JSSvgPath.h"
#include <y60/jsbase/JSWrapper.impl>

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

template class JSWrapper<y60::Renderer,asl::Ptr<y60::Renderer>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints the index of the current renderer.");
    DOC_END;
    std::string myStringRep = std::string("Renderer@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

template <class T>
static JSBool
drawHelper(JSContext * cx, JSObject * obj, uintN argc, jsval * argv)
{
    T myObject;
    asl::Vector4f myColor(1.0f, 1.0f, 1.0f, 1.0f);
    asl::Matrix4f myMatrix;
    myMatrix.makeIdentity();
    float myLineWidth = 1.0f;
    std::string myRenderStyles = "";

    if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myObject)) {
        JS_ReportError(cx, "JSRenderer::draw: argument #1 must be a drawable element");
        return JS_FALSE;
    }
    if (argc > 1) {
        if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myColor)) {
            JS_ReportError(cx, "JSRenderer::draw: argument #2 must be a Vector4f");
            return JS_FALSE;
        }
    }
    if (argc > 2) {
        if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myMatrix)) {
            JS_ReportError(cx, "JSRenderer::draw: argument #3 must be a Matrix4f");
            return JS_FALSE;
        }
    }
    if (argc > 3) {
        if (JSVAL_IS_VOID(argv[3]) || !convertFrom(cx, argv[3], myLineWidth)) {
            JS_ReportError(cx, "JSRenderer::draw: argument #4 must be a float");
            return JS_FALSE;
        }
    }
    if (argc > 4) {
        if (JSVAL_IS_VOID(argv[4]) || !convertFrom(cx, argv[4], myRenderStyles)) {
            JS_ReportError(cx, "JSRenderer::draw: argument #5 must be a string");
            return JS_FALSE;
        }
    }

    JSRenderer::NATIVE * myNative = 0;
    if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
        JS_ReportError(cx, "JSRenderer::draw: self is not a Renderer");
        return JS_FALSE;
    }
    myNative->draw(myObject, myColor, myMatrix, myLineWidth, myRenderStyles);

    return JS_TRUE;
}

static JSBool
draw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Draws a point, line segment, triangle, sphere or box in a given "
              "color and transformation matrix.");
    DOC_PARAM("thePoint", "The Point to draw", DOC_TYPE_LINESEGMENT);
    DOC_PARAM("theColor", "Drawing color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("thTransformation", "Transformation matrix", DOC_TYPE_MATRIX4F);
    DOC_PARAM("theSize", "Point- and Linesize", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theLineSegment", "Line Segment to draw", DOC_TYPE_LINESEGMENT);
    DOC_PARAM("theColor", "Drawing color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("thTransformation", "Transformation matrix", DOC_TYPE_MATRIX4F);
    DOC_PARAM("theSize", "Point- and Linesize", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theTriangle", "", DOC_TYPE_TRIANGLE);
    DOC_PARAM("theColor", "Drawing color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("thTransformation", "Transformation matrix", DOC_TYPE_MATRIX4F);
    DOC_PARAM("theSize", "Point- and Linesize", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theSphere", "", DOC_TYPE_SPHERE);
    DOC_PARAM("theColor", "Drawing color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("thTransformation", "Transformation matrix", DOC_TYPE_MATRIX4F);
    DOC_PARAM("theSize", "Point- and Linesize", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theBox", "", DOC_TYPE_BOX3F);
    DOC_PARAM("theColor", "Drawing color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("theTransformation", "Transformation matrix", DOC_TYPE_MATRIX4F);
    DOC_PARAM("theSize", "Point- and Linesize", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theBSpline", "", DOC_TYPE_BSPLINE);
    DOC_PARAM("theColor", "Drawing color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("thTransformation", "Transformation matrix", DOC_TYPE_MATRIX4F);
    DOC_PARAM("theSize", "Point- and Linesize", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theSvgPath", "", DOC_TYPE_SVGPATH);
    DOC_PARAM("theColor", "Drawing color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("thTransformation", "Transformation matrix", DOC_TYPE_MATRIX4F);
    DOC_PARAM("theWidth", "Point- and Linesize", DOC_TYPE_FLOAT);
    DOC_END;
    try {
        ensureParamCount(argc, 1,5);

        if (JSVector<asl::Vector3f>::matchesClassOf(cx, argv[0])) {
            return drawHelper<asl::Vector3f >(cx, obj, argc, argv);
        }
        if (JSLineSegment::matchesClassOf(cx, argv[0])) {
            return drawHelper<asl::LineSegment<float> >(cx, obj, argc, argv);
        }
        if (JSTriangle::matchesClassOf(cx, argv[0])) {
            return drawHelper<asl::Triangle<float> >(cx, obj, argc, argv);
        }
        if (JSSphere::matchesClassOf(cx, argv[0])) {
            return drawHelper<asl::Sphere<float> >(cx, obj, argc, argv);
        }
        if (JSBox3f::matchesClassOf(cx, argv[0])) {
            return drawHelper<asl::Box3<float> >(cx, obj, argc, argv);
        }
        if (JSBSpline::matchesClassOf(cx, argv[0])) {
            return drawHelper<asl::BSpline<float> >(cx, obj, argc, argv);
        }
        if (JSSvgPath::matchesClassOf(cx, argv[0])) {
            return drawHelper<asl::SvgPath>(cx, obj, argc, argv);
        }
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

static JSBool
drawFrustum(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Draws a Frustum");
    DOC_PARAM("theProjectiveNode", "A camera or projector node", DOC_TYPE_NODE);
    /*
    DOC_PARAM("theAspect", "The aspect of the frustum. This is determined by "
              "the viewport in case of a camera and by the aspect of texture "
              "when using projective textures.", DOC_TYPE_VECTOR4F);
    */
    DOC_END;

    try {
        dom::NodePtr myNode;
        if ( ! convertFrom(cx, argv[0], myNode)) {
            JS_ReportError(cx, "argument 0 must be a node");
            return JS_FALSE;
        }

        /*
        float myAspect;
        if ( ! convertFrom(cx, argv[1], myAspect)) {
            JS_ReportError(cx, "argument 1 must be a float");
            return JS_FALSE;
        }
        */


        JSRenderer::NATIVE * myNative = 0;
        if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
            JS_ReportError(cx, "JSRenderer::drawFrustum: self is not a Renderer");
            return JS_FALSE;
        }
        //myNative->renderFrustum(myNode, myAspect);
        myNative->renderFrustum( myNode );

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


JSFunctionSpec *
JSRenderer::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"draw",                 draw,                    5},
        {"drawFrustum",          drawFrustum,             1},
        {0}
    };
    return myFunctions;
}

enum PropertyNumbers {
    PROP_boundingVolumeMode = -100,
    PROP_scene,
    PROP_framenumber,
// const static int for bounding volume mode
    PROP_BV_NONE,
    PROP_BV_SHAPE,
    PROP_BV_BODY,
    PROP_BV_HIERARCHY,
    PROP_MATERIALS,
    PROP_SHAPES,
    PROP_ANIMATIONS,
    PROP_ANIMATIONS_LOAD,
    PROP_WORLD,
    PROP_IMAGES,
    PROP_ALL,
    PROP_TOP_ALIGNMENT,
    PROP_BOTTOM_ALIGNMENT,
    PROP_LEFT_ALIGNMENT,
    PROP_RIGHT_ALIGNMENT,
    PROP_CENTER_ALIGNMENT,
    PROP_CUBEMAP_SUPPORT,
    PROP_TEXTURECOMPRESSION_SUPPORT,
    PROP_MULTITEXTURE_SUPPORT,
    PROP_POINT_SPRITE_SUPPORT,
    PROP_POINT_PARAMETER_SUPPORT,
    PROP_TEXTURE_3D_SUPPORT,
    PROP_FRAMEBUFFER_SUPPORT
};

#define DEFINE_PROPERTY(NAME) { #NAME, PROP_ ## NAME , NAME }

#define DEFINE_FLAG(NAME) { #NAME, PROP_ ## NAME , NAME }
#define DEFINE_RENDERING_CAPS(NAME) { #NAME, PROP_ ## NAME , y60::NAME }
#define DEFINE_SCENE_FLAG(NAME) { #NAME, PROP_ ## NAME , y60::Scene::NAME }
#define DEFINE_TEXT_FLAG(NAME) { #NAME, PROP_ ## NAME , y60::Text::NAME }
#define DEFINE_FONT_FLAG(NAME) { #NAME, PROP_ ## NAME , y60::TTFFontInfo::NAME }
#define DEFINE_TEXTRENDERER_FLAG(NAME) { #NAME, PROP_ ## NAME , y60::TextStyle::NAME }

JSConstIntPropertySpec *
JSRenderer::ConstIntProperties() {

    const unsigned short BV_NONE         = 0;
    const unsigned short BV_SHAPE        = 1;
    const unsigned short BV_BODY         = 2;
    const unsigned short BV_HIERARCHY    = 4;

    const unsigned short PROP_NORMAL     = 0;
    const unsigned short PROP_BOLD       = 1;
    const unsigned short PROP_ITALIC     = 2;
    const unsigned short PROP_BOLDITALIC = 3;

    const unsigned short PROP_NOHINTING     = 0;
    const unsigned short PROP_NATIVEHINTING = 1;
    const unsigned short PROP_AUTOHINTING   = 2;
    const unsigned short PROP_AUTOHINTING_LIGHT = 3;
    const unsigned short PROP_AUTOHINTING_MONO   = 4;
    

    static JSConstIntPropertySpec myProperties[] = {
            DEFINE_PROPERTY(BV_NONE),
            DEFINE_PROPERTY(BV_SHAPE),
            DEFINE_PROPERTY(BV_BODY),
            DEFINE_PROPERTY(BV_HIERARCHY),
            DEFINE_SCENE_FLAG(MATERIALS),  // SCENE FLAGS moved to JSScene.
            DEFINE_SCENE_FLAG(SHAPES),
            DEFINE_SCENE_FLAG(ANIMATIONS),
            DEFINE_SCENE_FLAG(ANIMATIONS_LOAD),
            DEFINE_SCENE_FLAG(WORLD),
            DEFINE_SCENE_FLAG(ALL),        // end of deprecated scene flags
            DEFINE_FONT_FLAG(NOHINTING),
            DEFINE_FONT_FLAG(NATIVEHINTING),
            DEFINE_FONT_FLAG(AUTOHINTING),
            DEFINE_FONT_FLAG(AUTOHINTING_LIGHT),
            DEFINE_FONT_FLAG(AUTOHINTING_MONO),
            DEFINE_FONT_FLAG(NORMAL),
            DEFINE_FONT_FLAG(BOLD),
            DEFINE_FONT_FLAG(ITALIC),
            DEFINE_FONT_FLAG(BOLDITALIC),
            DEFINE_TEXTRENDERER_FLAG(TOP_ALIGNMENT),
            DEFINE_TEXTRENDERER_FLAG(BOTTOM_ALIGNMENT),
            DEFINE_TEXTRENDERER_FLAG(LEFT_ALIGNMENT),
            DEFINE_TEXTRENDERER_FLAG(RIGHT_ALIGNMENT),
            DEFINE_TEXTRENDERER_FLAG(CENTER_ALIGNMENT),
            DEFINE_RENDERING_CAPS(CUBEMAP_SUPPORT),
            DEFINE_RENDERING_CAPS(TEXTURECOMPRESSION_SUPPORT),
            DEFINE_RENDERING_CAPS(MULTITEXTURE_SUPPORT),
            DEFINE_RENDERING_CAPS(POINT_SPRITE_SUPPORT),
            DEFINE_RENDERING_CAPS(POINT_PARAMETER_SUPPORT),
            DEFINE_RENDERING_CAPS(TEXTURE_3D_SUPPORT),
            DEFINE_RENDERING_CAPS(FRAMEBUFFER_SUPPORT),
        {0}
    };
    return myProperties;
};


JSPropertySpec *
JSRenderer::Properties() {
    static JSPropertySpec myProperties[] = {
        {"boundingVolumeMode",PROP_boundingVolumeMode,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"scene",           PROP_scene,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"framenumber",     PROP_framenumber,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY},
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSRenderer::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSRenderer::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSRenderer::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_boundingVolumeMode:
            *vp = as_jsval(cx, getNative().getBoundingVolumeMode());
            return JS_TRUE;
        case PROP_scene:
            *vp = as_jsval(cx, getScene().getSceneDom()->childNode("scene"));
            return JS_TRUE;
        case PROP_framenumber:
            *vp = as_jsval(cx, asl::as_string(getNative().getFrameNumber()));
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSRenderer::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSRenderer::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    jsval dummy;
    switch (theID) {
        case PROP_boundingVolumeMode:
            return Method<Renderer>::call(&Renderer::setBoundingVolumeMode, cx, obj, 1, vp, &dummy);
        default:
            JS_ReportError(cx,"JSRenderer::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSRenderer::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("NOTE: Do not create a renderer with new! Access existing" \
              "renderers by accessing the RenderWindow's renderer property.");
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Base::Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    // we never create a renderer with new, we only access existing renderers by
    // accessing the RenderWindow's renderer property.
    // so only create a dummy object here - it will be reassigned
    // in the JSWrapper::Construct method anyway
    // TODO: is there a better way to do this?
    OWNERPTR myNewPtr = OWNERPTR();
    JSRenderer * myNewObject=new JSRenderer(myNewPtr, 0);
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSRenderer::Constructor: new JSRenderer failed");
    return JS_FALSE;
}

void
JSRenderer::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
}

JSObject *
JSRenderer::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_CREATE(JSRenderer);
    /*
    if (myClass) {
        addClassProperties(cx, myClass);
    }*/
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, Renderer *& theRenderer) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRenderer::NATIVE >::Class()) {
                theRenderer = JSClassTraits<JSRenderer::NATIVE>::getNativeOwner(cx,myArgument).get();
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<Renderer> & theRenderer) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRenderer::NATIVE >::Class()) {
                theRenderer = JSClassTraits<JSRenderer::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSRenderer::OWNERPTR & theOwner) {
    JSObject * myReturnObject = JSRenderer::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

}
