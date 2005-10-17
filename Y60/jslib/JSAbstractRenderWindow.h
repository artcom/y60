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
            PROP_frameRate,
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
            // static properties
            PROP_TEXT_STATISTIC,
            PROP_RENDERED_STATISTIC,
            //
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
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setTimeout,cx,obj,argc,argv,rval);
        }

        static JSBool setInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setInterval,cx,obj,argc,argv,rval);
        }

        static JSBool setObjectTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setObjectTimeout,cx,obj,argc,argv,rval);
        }

        static JSBool setObjectInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setObjectInterval,cx,obj,argc,argv,rval);
        }

        static JSBool clearTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::clearTimeout,cx,obj,argc,argv,rval);
        }

        static JSBool clearInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::clearInterval,cx,obj,argc,argv,rval);
        }

        static JSBool setScene(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets the scene object to be rendered by this window.");
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
                //ENSURE_ARG(y60::ScenePtr, myScene, 0);

                bool myRetVal = mySelf->setScene(myScene);
                *rval = as_jsval(cx, myRetVal);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        }

        static JSBool
        getWorldSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::getWorldSize,cx,obj,argc,argv,rval);
        }

        static JSBool
        saveBuffer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::saveBuffer,cx,obj,argc,argv,rval);
        }
        static JSBool
        getRenderer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            try {
                DERIVED * mySelf;
                convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
                y60::RendererPtr myRetVal = mySelf->getRenderer();
                *rval = as_jsval(cx, myRetVal);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
            //return Method<NATIVE>::call(&NATIVE::getRenderer,cx,obj,argc,argv,rval);
        }
        static JSBool
        hasCap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::hasCap,cx,obj,argc,argv,rval);
        }
        static JSBool
        hasCapAsString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::hasCapAsString,cx,obj,argc,argv,rval);
        }

        static JSBool
        printStatistics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::printStatistics,cx,obj,argc,argv,rval);
        }
        static JSBool
        loadMovieFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            try {
                bool mySuccess = Method<NATIVE>::call(&NATIVE::loadMovieFrame,cx,obj,argc,argv,rval);
                if (mySuccess) {
                    *rval = as_jsval(cx, true);
                } else {
                    *rval = as_jsval(cx, false);
                }
            } catch(...) {
                *rval = as_jsval(cx, false);
            }
            return JS_TRUE;

        }
        static JSBool
        loadCaptureFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            try {
                bool mySuccess = Method<NATIVE>::call(&NATIVE::loadCaptureFrame,cx,obj,argc,argv,rval);
                if (mySuccess) {
                    *rval = as_jsval(cx, true);
                } else {
                    *rval = as_jsval(cx, false);
                }
            } catch(...) {
                *rval = as_jsval(cx, false);
            }
            return JS_TRUE;

        }
        static JSBool
        getImagePixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::getImagePixel,cx,obj,argc,argv,rval);
        }
        static JSBool
        setImagePixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setImagePixel,cx,obj,argc,argv,rval);
        }

        static JSBool
        performRequest(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
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
            DOC_BEGIN("");
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
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::renderText,cx,obj,argc,argv,rval);
        }
        static JSBool
        setTextColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setTextColor,cx,obj,argc,argv,rval);
        }
        static JSBool
        renderTextAsImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::renderTextAsImage,cx,obj,argc,argv,rval);
        }
        static JSBool
        setTextStyle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setTextStyle,cx,obj,argc,argv,rval);
        }
        static JSBool
        setTextPadding(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setTextPadding,cx,obj,argc,argv,rval);
        }
        static JSBool
        setHTextAlignment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setHTextAlignment,cx,obj,argc,argv,rval);
        }
        static JSBool
        setVTextAlignment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setVTextAlignment,cx,obj,argc,argv,rval);
        }
        static JSBool
        setLineHeight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setLineHeight,cx,obj,argc,argv,rval);
        }
        static JSBool
        setParagraph(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setParagraph,cx,obj,argc,argv,rval);
        }

        static JSBool
        playClip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::playClip,cx,obj,argc,argv,rval);
        }
        static JSBool
        setClipLoops(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setClipLoops,cx,obj,argc,argv,rval);
        }
        static JSBool
        setClipForwardDirection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::setClipForwardDirection,cx,obj,argc,argv,rval);
        }
        static JSBool
        getLoops(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::getLoops,cx,obj,argc,argv,rval);
        }
        static JSBool
        runAnimations(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::runAnimations,cx,obj,argc,argv,rval);
        }
        static JSBool
        isClipActive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::isClipActive,cx,obj,argc,argv,rval);
        }
        static JSBool
        isCharacterActive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            return Method<NATIVE>::call(&NATIVE::isCharacterActive,cx,obj,argc,argv,rval);
        }
        static JSBool
        stopCharacter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
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
                {"setObjectTimeout",   setObjectTimeout,         3},
                {"setObjectInterval",  setObjectInterval,        3},
                {"clearTimeout",       clearTimeout,             1},
                {"clearInterval",      clearInterval,            1},
                {"setScene",           setScene,                 1},
                {"getWorldSize",       getWorldSize,             1},
                {"saveBuffer",         saveBuffer,               2},
                {"getRenderer",        getRenderer,              0},
                {"addExtension",       addExtension,             1},
                {"hasCap",             hasCap,                   1},
                {"hasCapAsString",     hasCapAsString,           1},
                {"printStatistics",    printStatistics,          2}, // base class
                {"loadMovieFrame",     loadMovieFrame,           1},
                {"loadCaptureFrame",   loadCaptureFrame,         1},
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
                "TEXT_STATISTIC",    PROP_TEXT_STATISTIC,     NATIVE::TEXT_STATISTIC,
                "RENDERED_STATISTIC",PROP_RENDERED_STATISTIC, NATIVE::RENDERED_STATISTIC,
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
                {"frameRate",       PROP_frameRate,         JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // double
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
                case PROP_frameRate:
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



