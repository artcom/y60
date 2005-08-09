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
//   $RCSfile: JSRenderer.cpp,v $
//   $Author: christian $
//   $Revision: 1.15 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSRenderer.h"
#include "JScppUtils.h"
#include "JSBox.h"
#include "JSLine.h"
#include "JSTriangle.h"
#include "JSSphere.h"

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints the index of the current renderer.");
    DOC_END;
    std::string myStringRep = std::string("Renderer@") + as_string(obj);
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
draw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Draws a line segment, triangle, sphere or box in a given color and transformation matrix.");
    DOC_PARAM("line segment", DOC_TYPE_LINESEGMENT);
    DOC_PARAM("color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("transformation", DOC_TYPE_MATRIX4F);
    DOC_RESET;
    DOC_PARAM("triangle", DOC_TYPE_TRIANGLE);
    DOC_PARAM("color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("transformation", DOC_TYPE_MATRIX4F);
    DOC_RESET;
    DOC_PARAM("sphere", DOC_TYPE_SPHERE);
    DOC_PARAM("color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("transformation", DOC_TYPE_MATRIX4F);
    DOC_RESET;
    DOC_PARAM("box", DOC_TYPE_BOX3F);
    DOC_PARAM("color", DOC_TYPE_VECTOR4F);
    DOC_PARAM("transformation", DOC_TYPE_MATRIX4F);
    DOC_RESET;
    DOC_END;
    if (argc == 3) {
        if (JSLineSegment::matchesClassOf(cx, argv[0])) {
            typedef void (Renderer::*MyMethod)(
                const asl::LineSegment<float> &,
                const asl::Vector4f & theColor,
                const asl::Matrix4f & theTransformation);
            return Method<Renderer>::call((MyMethod)&Renderer::draw,cx,obj,argc,argv,rval);
        }
        if (JSTriangle::matchesClassOf(cx, argv[0])) {
            typedef void (Renderer::*MyMethod)(
                const asl::Triangle<float> &,
                const asl::Vector4f & theColor,
                const asl::Matrix4f & theTransformation);
            return Method<Renderer>::call((MyMethod)&Renderer::draw,cx,obj,argc,argv,rval);
        }
        if (JSSphere::matchesClassOf(cx, argv[0])) {
            typedef void (Renderer::*MyMethod)(
                const asl::Sphere<float> &,
                const asl::Vector4f & theColor,
                const asl::Matrix4f & theTransformation);
            return Method<Renderer>::call((MyMethod)&Renderer::draw,cx,obj,argc,argv,rval);
        }
        if (JSBox3f::matchesClassOf(cx, argv[0])) {
            typedef void (Renderer::*MyMethod)(
                const asl::Box3<float> &,
                const asl::Vector4f & theColor,
                const asl::Matrix4f & theTransformation);
            return Method<Renderer>::call((MyMethod)&Renderer::draw,cx,obj,argc,argv,rval);
        }
        JS_ReportError(cx,"JSRenderWindow::draw: bad argument type #0");
        return JS_FALSE;
    }
    JS_ReportError(cx,"JSRenderWindow::draw: bad number of arguments, 3 expected");
    return JS_FALSE;
}

JSFunctionSpec *
JSRenderer::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"draw",                 draw,                    3},
        {0}
    };
    return myFunctions;
}

enum PropertyNumbers {
    PROP_boundingVolumeMode = -100,
    PROP_scene,
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
    PROP_SOLID_TEXT,
    PROP_SHADED_TEXT,
    PROP_BLENDED_TEXT,
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
#define DEFINE_TEXTRENDERER_FLAG(NAME) { #NAME, PROP_ ## NAME , y60::TextRenderer::NAME }

JSConstIntPropertySpec *
JSRenderer::ConstIntProperties() {

    const unsigned short BV_NONE         = 0;
    const unsigned short BV_SHAPE        = 1;
    const unsigned short BV_BODY         = 2;
    const unsigned short BV_HIERARCHY    = 4;

    const unsigned short PROP_BOLD       = 1;
    const unsigned short PROP_ITALIC     = 2;
    const unsigned short PROP_BOLDITALIC = 3;

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
            DEFINE_SCENE_FLAG(IMAGES),
            DEFINE_SCENE_FLAG(ALL),        // end of deprecated scene flags
            DEFINE_TEXT_FLAG(SOLID_TEXT),
            DEFINE_TEXT_FLAG(SHADED_TEXT),
            DEFINE_TEXT_FLAG(BLENDED_TEXT),
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
    DOC_BEGIN("NOTE: Do not create a renderer with new! Access existing \
renderers by accessing the RenderWindow's renderer property.");
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
    OWNERPTR myNewPtr = OWNERPTR(0);
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
                theRenderer = &(*JSClassTraits<JSRenderer::NATIVE>::getNativeOwner(cx,myArgument));
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
    JSObject * myReturnObject = JSRenderer::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

}



