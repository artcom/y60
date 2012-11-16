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

#ifndef _Y60_JSLIB_JSRENDERWINDOW_INCLUDED_
#define _Y60_JSLIB_JSRENDERWINDOW_INCLUDED_

#include "y60_jslib_settings.h"

#include <y60/jsbase/JSWrapper.h>
#include "AbstractRenderWindow.h"
#include <y60/scene/Viewport.h>
#include <y60/gltext/TextRenderer.h>

namespace jslib {

template<class DERIVED>
class JSAbstractRenderWindow :  public JSWrapper<DERIVED, asl::Ptr<DERIVED>, StaticAccessProtocol> {
    public:
        typedef AbstractRenderWindow NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        enum PropertyNumbers {
            PROP_BEGIN = -128,
            PROP_forceFullGC,
            PROP_visible,
            PROP_pause,
            PROP_eventListener,
            PROP_scene,
            PROP_canvas,
            PROP_backgroundColor,
            PROP_width,
            PROP_height,
            PROP_fixedFrameTime,
            PROP_fps,
            PROP_multisamples,
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
        static JSBool saveBuffer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool copyBufferToTexture(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static JSBool getRenderer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool setSceneAndCanvas(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool hasCap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
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
        renderTextAsImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        getTextGlyphPositions(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        getTextCursorPosition(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        getTextLineWidths(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        getTextMaxWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        getFontMetrics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        getGlyphMetrics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        getKerning(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool
        hasGlyph(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
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
    bool convertFrom(JSContext *cx, jsval theValue, y60::TextStyle & theTextStyle);
}

#endif
