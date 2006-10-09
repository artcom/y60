//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_JSLIB_JSRENDERWINDOW_INCLUDED_
#define _Y60_JSLIB_JSRENDERWINDOW_INCLUDED_

#include "JSWrapper.h"
#include "AbstractRenderWindow.h"
#include <y60/Viewport.h>

namespace jslib {

template<class DERIVED>
class JSAbstractRenderWindow :  public JSWrapper<DERIVED, asl::Ptr<DERIVED>, StaticAccessProtocol> {
    public:
        typedef AbstractRenderWindow NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        enum PropertyNumbers {
            PROP_BEGIN = -128,
            PROP_pause,
            PROP_eventListener,
            PROP_scene,
            PROP_canvas,
            PROP_backgroundColor,
            PROP_width,
            PROP_height,
            PROP_fixedFrameTime,
            PROP_fps,
            // renderer information
            PROP_renderingCaps,
            PROP_glVersionString,
            PROP_glVendorString,
            PROP_glRendererString,
            PROP_glExtensionStrings,
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
            // GL enums
            PROP_GL_RED,
            PROP_GL_GREEN,
            PROP_GL_BLUE,
            PROP_GL_ALPHA,
            PROP_GL_RGB,
            PROP_GL_BGR,
            PROP_GL_RGBA,
            PROP_GL_BGRA,
            PROP_GL_DEPTH_COMPONENT,
            PROP_GL_UNSIGNED_BYTE,
            PROP_GL_BYTE,
            PROP_GL_BITMAP,
            PROP_GL_UNSIGNED_SHORT,
            PROP_GL_SHORT,
            PROP_GL_UNSIGNED_INT,
            PROP_GL_INT,
            PROP_GL_FLOAT,
            PROP_GL_COLOR_BUFFER_BIT,
            PROP_GL_DEPTH_BUFFER_BIT,
            PROP_GL_STENCIL_BUFFER_BIT,
            PROP_GL_ACCUM_BUFFER_BIT,
            PROP_END
        };

        static JSBool setTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool setInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool clearTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool clearInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        saveBuffer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        getRenderer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        setSceneAndCanvas(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        hasCap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        hasCapAsString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        printStatistics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        getImagePixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        setImagePixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        getPixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        performRequest(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        addExtension(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        renderText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        setTextColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        renderTextAsImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        setTextPadding(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        setTextIndentation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        setHTextAlignment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        setVTextAlignment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        setLineHeight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        setParagraph(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        getFontMetrics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        getGlyphMetrics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        getKerning(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        hasGlyph(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        setTracking(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        playClip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        setClipLoops(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        setClipForwardDirection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        getLoops(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        runAnimations(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        isClipActive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        isCharacterActive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        stopCharacter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        activateGLContext(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        deactivateGLContext(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        clearBuffers(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        preRender(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        render(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        postRender(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSBool
        swapBuffers(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
        static JSFunctionSpec * BaseStaticFunctions();
        static JSFunctionSpec * BaseFunctions(); 
        static JSConstIntPropertySpec * ConstIntProperties(); 
        static JSPropertySpec * BaseProperties();

// These methods were declared but never defined. Ok to delete? -uz
//        static JSPropertySpec * StaticProperties();
//        static JSFunctionSpec * StaticFunctions();

        static JSBool getPropertySwitch(DERIVED & theNative, unsigned long theID,
                JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID, JSContext *cx,
                JSObject *obj, jsval id, jsval *vp);

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
    };
}

#endif
