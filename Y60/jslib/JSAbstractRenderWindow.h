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
//   $RCSfile: JSAbstractRenderWindow.h,v $
//   $Author: christian $
//   $Revision: 1.29 $
//   $Date: 2005/04/28 17:12:58 $
//
//   Description: Base class wrapper for AbstractRenderWindow
//   TODO: this is not (yet) used by the SDL-based app
//
//=============================================================================

#ifndef _Y60_JSLIB_JSRENDERWINDOW_INCLUDED_
#define _Y60_JSLIB_JSRENDERWINDOW_INCLUDED_

#include "JSWrapper.h"
#include "AbstractRenderWindow.h"
#include "JSRenderer.h"
#include "JSintersection_functions.h"
#include "IScriptablePlugin.h"
#include "JSScriptablePlugin.h"
#include "JSScene.h"
#include "IRendererExtension.h"

#include <y60/Viewport.h>
#include <y60/ScopedGLContext.h>
#include <asl/string_functions.h>
#include <asl/os_functions.h>

namespace jslib {

// TODO:  move more non-templatized implementation to this base class
class JSAbstractRenderWindowBase : public JSWrapper<AbstractRenderWindow,
                                   asl::Ptr<AbstractRenderWindow>,
                                            StaticAccessProtocol>
{
    public:
        enum PropertyNumbers {
            PROP_BEGIN = -128,
            PROP_pause,
            PROP_eventListener,
            PROP_scene,
            PROP_canvas,
            PROP_backgroundColor,
            PROP_width,
            PROP_height,
            PROP_renderingCaps,
            PROP_fixedFrameTime,
            PROP_fps,
            // convenience attributes when in single-viewport mode
            PROP_camera,
            PROP_projectionmatrix,
            PROP_wireframe,
            PROP_drawnormals,
            PROP_lighting,
            PROP_flatshading,
            PROP_texturing,
            PROP_backfaceCulling,
            PROP_culling,
            PROP_debugCulling,
            PROP_END
        };
};

template<class DERIVED>
class JSAbstractRenderWindow : public JSAbstractRenderWindowBase
{
    public:
        typedef AbstractRenderWindow NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static JSBool setTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets a javascript line to be executed after a certain time");
            DOC_PARAM("theJavaScriptCommand", DOC_TYPE_STRING);
            DOC_PARAM("theMilliseconds", DOC_TYPE_FLOAT);
            DOC_RVAL("theTimeoutId", DOC_TYPE_INTEGER);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setTimeout,cx,obj,argc,argv,rval);
        }

        static JSBool setInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets a javascript line to be executed in a certain interval until clearInterval() is called.");
            DOC_PARAM("theJavaScriptCommand", DOC_TYPE_STRING);
            DOC_PARAM("theMilliseconds", DOC_TYPE_FLOAT);
            DOC_RVAL("theTimeoutId", DOC_TYPE_INTEGER);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setInterval,cx,obj,argc,argv,rval);
        }

        static JSBool clearTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Stops a timeout from beeing executed");
            DOC_PARAM("theTimeoutId", DOC_TYPE_INTEGER);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::clearTimeout,cx,obj,argc,argv,rval);
        }

        static JSBool clearInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Stops an interval from beeing executed");
            DOC_PARAM("theTimeoutId", DOC_TYPE_INTEGER);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::clearInterval,cx,obj,argc,argv,rval);
        }

        static JSBool setScene(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets the scene object to be rendered by this window, the current scene will be thrown away.");
            DOC_PARAM("theScene", DOC_TYPE_OBJECT);
            DOC_RVAL("theSucceededFlag", DOC_TYPE_BOOLEAN);
            DOC_END;
            try {
                DERIVED * mySelf;
                convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

                y60::ScenePtr myScene(0);
                if (argv[0] != JSVAL_NULL && argv[0] != JSVAL_VOID) {
                    if ( ! convertFrom(cx, argv[0], myScene)) {
                        throw JSArgMismatch("Failed to convert argument 0 to type y60::Scene", PLUS_FILE_LINE);
                    }
                }

                bool myRetVal = mySelf->setScene(myScene);
                *rval = as_jsval(cx, myRetVal);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        }

        static JSBool
        saveBuffer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Saves a screenshot to a given file");
            DOC_PARAM("theFilename", DOC_TYPE_STRING);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::saveBuffer,cx,obj,argc,argv,rval);
        }
        static JSBool
        getRenderer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Returns the renderer currently attached to this window");
            DOC_RVAL("theRenderer", DOC_TYPE_OBJECT);
            DOC_END;
            try {
                DERIVED * mySelf;
                convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
                y60::RendererPtr myRetVal = mySelf->getRenderer();
                *rval = as_jsval(cx, myRetVal);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        }
        static JSBool
        hasCap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Checks if the renderer has a certain capability." \
                      "Possible Capability constants are static properties of the Renderer object");
            DOC_PARAM("theCapability", DOC_TYPE_ENUMERATION);
            DOC_RVAL("theResult", DOC_TYPE_BOOLEAN);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::hasCap,cx,obj,argc,argv,rval);
        }
        static JSBool
        hasCapAsString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Checks if the renderer has a certain capability." \
                      "Use OpenGL queryOGLExtension command is called with the capability string.");
            DOC_PARAM("theCapability", DOC_TYPE_STRING);
            DOC_RVAL("theResult", DOC_TYPE_BOOLEAN);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::hasCapAsString,cx,obj,argc,argv,rval);
        }

        static JSBool
        printStatistics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("prints render statistics and profiling information to the console");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::printStatistics,cx,obj,argc,argv,rval);
        }
        static JSBool
        getImagePixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Returns the color of a pixel at position x, y of an image");
            DOC_PARAM("theImageNode", DOC_TYPE_NODE);
            DOC_PARAM("theXPosition", DOC_TYPE_INTEGER);
            DOC_PARAM("theYPosition", DOC_TYPE_INTEGER);
            DOC_RVAL("theColor", DOC_TYPE_VECTOR4F);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::getImagePixel,cx,obj,argc,argv,rval);
        }
        static JSBool
        setImagePixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets the color of a pixel at position x, y in an image");
            DOC_PARAM("theImageNode", DOC_TYPE_NODE);
            DOC_PARAM("theXPosition", DOC_TYPE_INTEGER);
            DOC_PARAM("theYPosition", DOC_TYPE_INTEGER);
            DOC_PARAM("theColor", DOC_TYPE_VECTOR4F);
            DOC_RVAL("theSuccess", DOC_TYPE_BOOLEAN);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setImagePixel,cx,obj,argc,argv,rval);
        }

        static JSBool
        performRequest(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("performs an http request");
            DOC_PARAM("theRequest", DOC_TYPE_OBJECT);
            DOC_END;
            if (argc == 1) {
                if (!JSRequestWrapper::matchesClassOf(cx, argv[0])) {
                    JS_ReportError(cx,"JSRenderWindow::performRequest: bad argument type #0");
                    return JS_FALSE;
                }
                typedef void (NATIVE::*MyMethod)(const JSRequestPtr &);
                return Method<NATIVE>::call((MyMethod)&NATIVE::performRequest,cx,obj,argc,argv,rval);
            }
            JS_ReportError(cx,"JSRenderWindow::performRequest: bad number of arguments, 1 expected");
            return JS_FALSE;
        }

        static JSBool
        addExtension(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Adds a renderextension object to the window");
            DOC_PARAM("theRenderExtension", DOC_TYPE_OBJECT);
            DOC_END;
            try {
                if (argc == 1) {
                    IScriptablePluginPtr myScriptablePlugin;
                    if (!convertFrom(cx, argv[0], myScriptablePlugin)) {
                        JS_ReportError(cx,"JSRenderWindow::addExtension: parameter 1 must be a plugin");
                        return JS_FALSE;
                    }

                    y60::IRendererExtensionPtr myExtension = dynamic_cast_Ptr<y60::IRendererExtension>(myScriptablePlugin);
                    if (!myExtension) {
                        JS_ReportError(cx, "Plug-In does not implement y60::IRendererExtension");
                        return JS_FALSE;
                    }
                    jslib::JSClassTraits<DERIVED>::openNativeRef(cx, obj).addExtension(myExtension);
                    jslib::JSClassTraits<DERIVED>::closeNativeRef(cx,obj);
                    return JS_TRUE;
                }
                JS_ReportError(cx,"JSRenderWindow::addExtension: bad number of arguments, 1 expected");
                return JS_FALSE;
            } HANDLE_CPP_EXCEPTION;
        }

        static JSBool
        renderText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Renders a text onto the screen");
            DOC_PARAM("thePosition in Pixel", DOC_TYPE_VECTOR2F);
            DOC_PARAM("theTextString", DOC_TYPE_STRING);
            DOC_PARAM("theFontName", DOC_TYPE_STRING);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::renderText,cx,obj,argc,argv,rval);
        }
        static JSBool
        setTextColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets the text foreground and background color");
            DOC_PARAM("theTextColor", DOC_TYPE_VECTOR4F);
            DOC_PARAM("theBackGroundColor", DOC_TYPE_VECTOR4F);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setTextColor,cx,obj,argc,argv,rval);
        }
        static JSBool
        renderTextAsImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Renders a text into a target image node");
            DOC_PARAM("theImageNode", DOC_TYPE_NODE);
            DOC_PARAM("theTextString", DOC_TYPE_STRING);
            DOC_PARAM("theFontName", DOC_TYPE_STRING);
            DOC_PARAM("theTextBoxWidth", DOC_TYPE_INTEGER);
            DOC_PARAM("theTextBoxHeight", DOC_TYPE_INTEGER);
            DOC_RVAL("theFilledTextBoxSize", DOC_TYPE_VECTOR2I);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::renderTextAsImage,cx,obj,argc,argv,rval);
        }
        static JSBool
        setTextStyle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets style for text paragraphs (normal, bold, italic, bolditalic)" \
                      "See Renderer static properties for possible constants");
            DOC_PARAM("theHorizontalAlignment", DOC_TYPE_ENUMERATION);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setTextStyle,cx,obj,argc,argv,rval);
        }
        static JSBool
        setTextPadding(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets padding in pixels around text paragraphs");
            DOC_PARAM("theTopPadding", DOC_TYPE_INTEGER);
            DOC_PARAM("theBottomPadding", DOC_TYPE_INTEGER);
            DOC_PARAM("theLeftPadding", DOC_TYPE_INTEGER);
            DOC_PARAM("theRightPadding", DOC_TYPE_INTEGER);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setTextPadding,cx,obj,argc,argv,rval);
        }
        static JSBool
        setHTextAlignment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets horizontal alignment for text paragraphs" \
                      "See Renderer static properties for possible constants");
            DOC_PARAM("theHorizontalAlignment", DOC_TYPE_ENUMERATION);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setHTextAlignment,cx,obj,argc,argv,rval);
        }
        static JSBool
        setVTextAlignment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets vertical alignment for text paragraphs" \
                      "See Renderer static properties for possible constants");
            DOC_PARAM("theVerticalAlignment", DOC_TYPE_ENUMERATION);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setVTextAlignment,cx,obj,argc,argv,rval);
        }
        static JSBool
        setLineHeight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets line height in pixel for text paragraphs");
            DOC_PARAM("theLineHeight", DOC_TYPE_INTEGER);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setLineHeight,cx,obj,argc,argv,rval);
        }
        static JSBool
        setParagraph(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets top and bottom margins in pixel for text paragraphs");
            DOC_PARAM("theTopMargin", DOC_TYPE_INTEGER);
            DOC_PARAM("theBottomMargin", DOC_TYPE_INTEGER);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setParagraph,cx,obj,argc,argv,rval);
        }

        static JSBool
        playClip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Plays an animation clip within character at a given time");
            DOC_PARAM("thePlayTime", DOC_TYPE_FLOAT);
            DOC_PARAM("theCharacterName", DOC_TYPE_STRING);
            DOC_PARAM("theClipName", DOC_TYPE_STRING);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::playClip,cx,obj,argc,argv,rval);
        }
        static JSBool
        setClipLoops(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets the number of loops of a clip within character");
            DOC_PARAM("theCharacterName", DOC_TYPE_STRING);
            DOC_PARAM("theClipName", DOC_TYPE_STRING);
            DOC_PARAM("theNumberOfLoops", DOC_TYPE_INTEGER);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setClipLoops,cx,obj,argc,argv,rval);
        }
        static JSBool
        setClipForwardDirection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets the direction of a clip within character");
            DOC_PARAM("theCharacterName", DOC_TYPE_STRING);
            DOC_PARAM("theClipName", DOC_TYPE_STRING);
            DOC_PARAM("theDirection", DOC_TYPE_BOOLEAN);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setClipForwardDirection,cx,obj,argc,argv,rval);
        }
        static JSBool
        getLoops(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Returns the number of loops of a clip within character");
            DOC_PARAM("theCharacterName", DOC_TYPE_STRING);
            DOC_PARAM("theClipName", DOC_TYPE_STRING);
            DOC_RVAL("numberOfLoops", DOC_TYPE_INTEGER);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::getLoops,cx,obj,argc,argv,rval);
        }
        static JSBool
        runAnimations(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Runs all global animations with a given time.");
            DOC_PARAM("theTime", DOC_TYPE_FLOAT);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::runAnimations,cx,obj,argc,argv,rval);
        }
        static JSBool
        isClipActive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Returns if a clip within character with a given name is active");
            DOC_PARAM("theCharacterName", DOC_TYPE_STRING);
            DOC_PARAM("theClipName", DOC_TYPE_STRING);
            DOC_RVAL("isActive", DOC_TYPE_BOOLEAN);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::isClipActive,cx,obj,argc,argv,rval);
        }
        static JSBool
        isCharacterActive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Returns if a character with a given name is active");
            DOC_PARAM("theCharacterName", DOC_TYPE_STRING);
            DOC_RVAL("isActive", DOC_TYPE_BOOLEAN);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::isCharacterActive,cx,obj,argc,argv,rval);
        }
        static JSBool
        stopCharacter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Stops an animated character with a given name");
            DOC_PARAM("theCharacterName", DOC_TYPE_STRING);
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::stopCharacter,cx,obj,argc,argv,rval);
        }
        static JSFunctionSpec * BaseStaticFunctions() {
            AC_DEBUG << "Registering class '"<<ClassName()<<"'";
            static JSFunctionSpec myFunctions[] = {
                // name                    native          nargs
                {0}
            };
            return myFunctions;
        }
        static JSFunctionSpec * BaseFunctions() {
            static JSFunctionSpec myFunctions[] = {
                // name                  native                   nargs
                {"setTimeout",         setTimeout,               2},
                {"setInterval",        setInterval,              2},
                {"clearTimeout",       clearTimeout,             1},
                {"clearInterval",      clearInterval,            1},
                {"setScene",           setScene,                 1},
                {"saveBuffer",         saveBuffer,               2},
                {"getRenderer",        getRenderer,              0},
                {"addExtension",       addExtension,             1},
                {"hasCap",             hasCap,                   1},
                {"hasCapAsString",     hasCapAsString,           1},
                {"printStatistics",    printStatistics,          2}, // base class
                {"getImagePixel",      getImagePixel,            3},
                {"setImagePixel",      setImagePixel,            4},
                {"performRequest",     performRequest,           1},
                {"renderText",         renderText,               3},
                {"setTextColor",       setTextColor,             2},
                {"renderTextAsImage",  renderTextAsImage,        5},
                {"setTextStyle",       setTextStyle,             1},
                {"setTextPadding",     setTextPadding,           4},
                {"setHTextAlignment",  setHTextAlignment,        1},
                {"setVTextAlignment",  setVTextAlignment,        1},
                {"setLineHeight",      setLineHeight,            1},
                {"setParagraph",       setParagraph,             2},
                {"runAnimations",      runAnimations,            1},
                {"playClip",           playClip,                 3},
                {"setClipLoops",       setClipLoops,             3},
                {"setClipForwardDirection", setClipForwardDirection,3},
                {"getLoops",           getLoops,                 2},
                {"isClipActive",       isClipActive,             2},
                {"isCharacterActive",  isCharacterActive,        1},
                {"stopCharacter",      stopCharacter,            1},
                {0}
            };
            return myFunctions;
        }

        static JSConstIntPropertySpec * ConstIntProperties() {
            static JSConstIntPropertySpec myProperties[] = {
                {0}
            };
            return myProperties;
        };

        static JSPropertySpec * BaseProperties() {
            static JSPropertySpec myProperties[] = {
                {"scene",           PROP_scene,             JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // node
                {"width",           PROP_width,             JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"height",          PROP_height,            JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"pause",           PROP_pause,             JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // boolean
                {"eventListener",   PROP_eventListener,     JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"canvas",          PROP_canvas,            JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // Node
                {"camera",          PROP_camera,            JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // Node
                {"projectionmatrix",PROP_projectionmatrix,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY|JSPROP_SHARED},
                {"renderingCaps",   PROP_renderingCaps,     JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // unsined int
                {"fixedFrameTime",  PROP_fixedFrameTime,    JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // unsined int
                {"fps",             PROP_fps,               JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // double
                {"wireframe",       PROP_wireframe,         JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"drawnormals",     PROP_drawnormals,       JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"lighting",        PROP_lighting,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"flatshading",     PROP_flatshading,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"texturing",       PROP_texturing,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"backfaceCulling", PROP_backfaceCulling,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"culling",         PROP_culling,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"backgroundColor", PROP_backgroundColor,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {"debugCulling",    PROP_debugCulling,  JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
                {0}
            };
            return myProperties;
        }

        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();


        static JSBool getPropertySwitch(DERIVED & theNative, unsigned long theID,
                JSContext *cx, JSObject *obj, jsval id, jsval *vp)
        {
            switch (theID) {
                case PROP_pause:
                    *vp = as_jsval(cx, theNative.getPause()); //TODO: remove global
                    return JS_TRUE;
                case PROP_scene:
                    *vp = as_jsval(cx, theNative.getCurrentScene());
                    return JS_TRUE;
                case PROP_eventListener:
                    *vp = as_jsval(cx, theNative.getEventListener());
                    return JS_TRUE;
                case PROP_width:
                    *vp = as_jsval(cx, theNative.getWidth());
                    return JS_TRUE;
                case PROP_height:
                    *vp = as_jsval(cx, theNative.getHeight());
                    return JS_TRUE;
                case PROP_projectionmatrix:
                    *vp = as_jsval(cx, theNative.getSingleViewport()->y60::Viewport::get<y60::ProjectionMatrixTag>());
                    return JS_TRUE;
                case PROP_camera:
                    {
                        y60::ViewportPtr myViewport = theNative.getSingleViewport();
                        *vp = as_jsval(cx, myViewport->getNode().getElementById(
                                    myViewport->y60::Viewport::get<y60::CameraTag>()));
                        return JS_TRUE;
                    }
                case PROP_flatshading:
                    *vp = as_jsval(cx, theNative.getSingleViewport()->y60::Viewport::get<y60::ViewportFlatshadingTag>());
                    return JS_TRUE;
                case PROP_texturing:
                    *vp = as_jsval(cx, theNative.getSingleViewport()->y60::Viewport::get<y60::ViewportTexturingTag>());
                    return JS_TRUE;
                case PROP_wireframe:
                    *vp = as_jsval(cx, theNative.getSingleViewport()->y60::Viewport::get<y60::ViewportWireframeTag>());
                    return JS_TRUE;
                case PROP_drawnormals:
                    *vp = as_jsval(cx, theNative.getSingleViewport()->y60::Viewport::get<y60::ViewportDrawNormalsTag>());
                    return JS_TRUE;
                case PROP_backfaceCulling:
                    *vp = as_jsval(cx, theNative.getSingleViewport()->y60::Viewport::get<y60::ViewportBackfaceCullingTag>());
                    return JS_TRUE;
                case PROP_culling:
                    *vp = as_jsval(cx, theNative.getSingleViewport()->y60::Viewport::get<y60::ViewportCullingTag>());
                    return JS_TRUE;
                case PROP_debugCulling:
                    *vp = as_jsval(cx, theNative.getSingleViewport()->y60::Viewport::get<y60::ViewportDebugCullingTag>());
                    return JS_TRUE;
                case PROP_lighting:
                    *vp = as_jsval(cx, theNative.getSingleViewport()->y60::Viewport::get<y60::ViewportLightingTag>());
                    return JS_TRUE;
                case PROP_backgroundColor:
                    {
                        y60::CanvasPtr myCanvas = theNative.getCanvas()->dom::Node::getFacade<y60::Canvas>();
                        *vp = as_jsval(cx, myCanvas->y60::Canvas::get<y60::CanvasBackgroundColorTag>());
                        return JS_TRUE;
                    }
                case PROP_canvas:
                    *vp = as_jsval(cx, theNative.getCanvas());
                    return JS_TRUE;
                case PROP_renderingCaps:
                    *vp = as_jsval(cx, theNative.getRenderingCaps());
                    return JS_TRUE;
                case PROP_fixedFrameTime:
                    *vp = as_jsval(cx, theNative.getFixedDeltaT());
                    return JS_TRUE;
                case PROP_fps:
                    *vp = as_jsval(cx, theNative.getFrameRate());
                    return JS_TRUE;
                default:
                    JS_ReportError(cx,"JSAbstractRenderWindow::getProperty: index %d out of range", theID);
                    return JS_FALSE;
            }
        }

        template <class T>
        static JSBool setViewportAttribute(NATIVE & theNative, JSContext * cx, jsval * vp) {
            y60::ViewportPtr myViewport = theNative.getSingleViewport();
            bool theFlag;
            if (!convertFrom(cx, *vp, theFlag)) {
                JS_ReportError(cx,"JSAbstractRenderWindow::setViewportAttribute(): could not convert parameter to required type.");
                return JS_FALSE;
            }
            myViewport->y60::Viewport::set<T>(theFlag);
            return JS_TRUE;
        }

        static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID, JSContext *cx,
                JSObject *obj, jsval id, jsval *vp)
        {
            jsval dummy;
            switch (theID) {
                case PROP_pause:
                    return Method<NATIVE>::call(&NATIVE::setPause, cx, obj, 1, vp, &dummy);
                case PROP_eventListener:
                    {
                        jslib::JSClassTraits<DERIVED>::openNativeRef(cx, obj).setJSContext(cx);
                        JSBool myRetVal = Method<NATIVE>::call(&NATIVE::setEventListener, cx, obj, 1, vp, &dummy);
                        jslib::JSClassTraits<DERIVED>::closeNativeRef(cx,obj);
                        return myRetVal;
                    }
                case PROP_camera:
                    {
                        y60::ViewportPtr myViewport = theNative.getSingleViewport();
                        dom::NodePtr myCameraNode;
                        if (!convertFrom(cx, *vp, myCameraNode)) {
                            JS_ReportError(cx,"JSAbstractRenderWindow::set camera: rvalue is not a node");
                            return JS_FALSE;
                        }
                        myViewport->y60::Viewport::set<y60::CameraTag>(
                                myCameraNode->getAttributeString(y60::ID_ATTRIB)
                        );
                        return JS_TRUE;
                    }
                case PROP_lighting:
                    return setViewportAttribute<y60::ViewportLightingTag>(theNative, cx, vp);
                case PROP_flatshading:
                    return setViewportAttribute<y60::ViewportFlatshadingTag>(theNative, cx, vp);
                case PROP_backfaceCulling:
                    return setViewportAttribute<y60::ViewportBackfaceCullingTag>(theNative, cx, vp);
                case PROP_culling:
                    return setViewportAttribute<y60::ViewportCullingTag>(theNative, cx, vp);
                case PROP_debugCulling:
                    return setViewportAttribute<y60::ViewportDebugCullingTag>(theNative, cx, vp);
                case PROP_wireframe:
                    return setViewportAttribute<y60::ViewportWireframeTag>(theNative, cx, vp);
                case PROP_drawnormals:
                    return setViewportAttribute<y60::ViewportDrawNormalsTag>(theNative, cx, vp);
                case PROP_texturing:
                    return setViewportAttribute<y60::ViewportTexturingTag>(theNative, cx, vp);
                case PROP_backgroundColor:
                    {
                        y60::CanvasBackgroundColorTag::TYPE theColor;
                        if (!convertFrom(cx, *vp, theColor)) {
                            JS_ReportError(cx,"JSAbstractRenderWindow::set background color: could not convert rvalue to Vector4f.");
                            return JS_FALSE;
                        }
                        theNative.getCanvas()->dom::Node::getFacade<y60::Canvas>()->y60::Canvas::set<y60::CanvasBackgroundColorTag>(theColor);
                        return JS_TRUE;
                    }
                case PROP_canvas:
                    return Method<NATIVE>::call(&NATIVE::setCanvas, cx, obj, 1, vp, &dummy);
                case PROP_renderingCaps:
                    return Method<NATIVE>::call(&NATIVE::setRenderingCaps, cx, obj, 1, vp, &dummy);
                case PROP_fixedFrameTime:
                    return Method<NATIVE>::call(&NATIVE::setFixedDeltaT, cx, obj, 1, vp, &dummy);
                default:
                    JS_ReportError(cx,"JSAbstractRenderWindow::setPropertySwitch: index %d out of range", theID);
                    return JS_FALSE;
            }
        }
    };
}

#endif



