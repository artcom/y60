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

        static JSBool setTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets a javascript line to be executed after a certain time");
            DOC_PARAM("theJavaScriptCommand", "", DOC_TYPE_STRING);
            DOC_PARAM("theMilliseconds", "", DOC_TYPE_FLOAT);
            DOC_RVAL("theTimeoutId", DOC_TYPE_INTEGER);
            DOC_END;
            typedef long (DERIVED::*MyMethod)(const std::string &, float);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setTimeout,cx,obj,argc,argv,rval);
        }

        static JSBool setInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets a javascript line to be executed in a certain interval until clearInterval() is called.");
            DOC_PARAM("theJavaScriptCommand", "", DOC_TYPE_STRING);
            DOC_PARAM("theMilliseconds", "", DOC_TYPE_FLOAT);
            DOC_RVAL("theTimeoutId", DOC_TYPE_INTEGER);
            DOC_END;
            typedef long (DERIVED::*MyMethod)(const std::string &, float);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setInterval,cx,obj,argc,argv,rval);
        }

        static JSBool clearTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Stops a timeout from beeing executed");
            DOC_PARAM("theTimeoutId", "", DOC_TYPE_INTEGER);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(long);
            return Method<DERIVED>::call((MyMethod)&DERIVED::clearTimeout,cx,obj,argc,argv,rval);
        }

        static JSBool clearInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Stops an interval from beeing executed");
            DOC_PARAM("theTimeoutId", "", DOC_TYPE_INTEGER);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(long);
            return Method<DERIVED>::call((MyMethod)&DERIVED::clearInterval,cx,obj,argc,argv,rval);
        }

        static JSBool
        saveBuffer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Saves a screenshot to a given file");
            DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(const std::string & );
            return Method<DERIVED>::call((MyMethod)&DERIVED::saveBuffer,cx,obj,argc,argv,rval);
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
            DOC_PARAM("theCapability", "Capability to test for", DOC_TYPE_ENUMERATION);
            DOC_RVAL("theResult", DOC_TYPE_BOOLEAN);
            DOC_END;
            typedef bool (DERIVED::*MyMethod)(unsigned);
            return Method<DERIVED>::call((MyMethod)&DERIVED::hasCap,cx,obj,argc,argv,rval);
        }
        static JSBool
        hasCapAsString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Checks if the renderer has a certain capability." \
                      "Use OpenGL queryOGLExtension command is called with the capability string.");
            DOC_PARAM("theCapability", "Capability to test for", DOC_TYPE_STRING);
            DOC_RVAL("theResult", DOC_TYPE_BOOLEAN);
            DOC_END;
            typedef bool (DERIVED::*MyMethod)(const std::string &);
            return Method<DERIVED>::call((MyMethod)&DERIVED::hasCapAsString,cx,obj,argc,argv,rval);
        }

        static JSBool
        printStatistics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Prints render statistics and profiling information to the console");
            DOC_END;
            typedef void (DERIVED::*MyMethod)(void);
            return Method<DERIVED>::call((MyMethod)&DERIVED::printStatistics,cx,obj,argc,argv,rval);
        }
        static JSBool
        getImagePixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Returns the color of a pixel at position x, y of an image");
            DOC_PARAM("theImageNode", "", DOC_TYPE_NODE);
            DOC_PARAM("theXPosition", "", DOC_TYPE_INTEGER);
            DOC_PARAM("theYPosition", "", DOC_TYPE_INTEGER);
            DOC_RVAL("theColor", DOC_TYPE_VECTOR4F);
            DOC_END;
            typedef asl::Vector4i (DERIVED::*MyMethod)(dom::NodePtr, unsigned long, unsigned long);
            return Method<DERIVED>::call((MyMethod)&DERIVED::getImagePixel,cx,obj,argc,argv,rval);
        }
        static JSBool
        setImagePixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets the color of a pixel at position x, y in an image");
            DOC_PARAM("theImageNode", "", DOC_TYPE_NODE);
            DOC_PARAM("theXPosition", "", DOC_TYPE_INTEGER);
            DOC_PARAM("theYPosition", "", DOC_TYPE_INTEGER);
            DOC_PARAM("theColor", "", DOC_TYPE_VECTOR4F);
            DOC_RVAL("theSuccess", DOC_TYPE_BOOLEAN);
            DOC_END;
            typedef bool (DERIVED::*MyMethod)(dom::NodePtr, unsigned long, unsigned long, const asl::Vector4i &);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setImagePixel,cx,obj,argc,argv,rval);
        }

        static JSBool
        performRequest(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Performs an http request");
            DOC_PARAM("theRequest", "", DOC_TYPE_OBJECT);
            DOC_END;
            if (argc == 1) {
                if (!JSRequestWrapper::matchesClassOf(cx, argv[0])) {
                    JS_ReportError(cx,"JSRenderWindow::performRequest: bad argument type #0");
                    return JS_FALSE;
                }
                typedef void (DERIVED::*MyMethod)(const JSRequestPtr &);
                return Method<DERIVED>::call((MyMethod)&DERIVED::performRequest,cx,obj,argc,argv,rval);
            }
            JS_ReportError(cx,"JSRenderWindow::performRequest: bad number of arguments, 1 expected");
            return JS_FALSE;
        }

        static JSBool
        addExtension(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Adds a renderextension object to the window");
            DOC_PARAM("theRenderExtension", "", DOC_TYPE_OBJECT);
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
            DOC_PARAM("thePosition in Pixel", "", DOC_TYPE_VECTOR2F);
            DOC_PARAM("theTextString", "", DOC_TYPE_STRING);
            DOC_PARAM_OPT("theFontName", "An OpenGL compiled font name", DOC_TYPE_STRING, "Screen15");
            DOC_END;
            try {
                ensureParamCount(argc, 2, 3);

                asl::Vector2f thePixelPosition;
                if (!convertFrom(cx, argv[0], thePixelPosition)) {
                    JS_ReportError(cx,"JSRenderWindow::renderText: parameter 1 must be a Vector2f");
                    return JS_FALSE;
                }

                std::string theString;
                if (!convertFrom(cx, argv[1], theString)) {
                    JS_ReportError(cx,"JSRenderWindow::renderText: parameter 2 must be a string");
                    return JS_FALSE;
                }

                if (argc == 2) {
                    jslib::JSClassTraits<DERIVED>::openNativeRef(cx, obj).renderText(thePixelPosition, theString);
                    jslib::JSClassTraits<DERIVED>::closeNativeRef(cx,obj);
                } else {
                    std::string theFont;
                    if (!convertFrom(cx, argv[2], theFont)) {
                        JS_ReportError(cx,"JSRenderWindow::renderText: parameter 3 must be a fontname");
                        return JS_FALSE;
                    }
                    jslib::JSClassTraits<DERIVED>::openNativeRef(cx, obj).renderText(thePixelPosition, theString, theFont);
                    jslib::JSClassTraits<DERIVED>::closeNativeRef(cx,obj);
                }

                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        }
        static JSBool
        setTextColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets the text foreground and background color");
            DOC_PARAM("theTextColor", "", DOC_TYPE_VECTOR4F);
            DOC_PARAM_OPT("theBackGroundColor", "", DOC_TYPE_VECTOR4F, "[1,1,1,1]");
            DOC_END;
            try {
                ensureParamCount(argc, 1, 2);

                asl::Vector4f theTextColor;
                if (!convertFrom(cx, argv[0], theTextColor)) {
                    JS_ReportError(cx,"JSRenderWindow::setTextColor: parameter 1 must be a Vector4f (textcolor)");
                    return JS_FALSE;
                }

                if (argc == 1) {
                    jslib::JSClassTraits<DERIVED>::openNativeRef(cx, obj).setTextColor(theTextColor);
                    jslib::JSClassTraits<DERIVED>::closeNativeRef(cx,obj);
                } else {
                    asl::Vector4f theBackColor;
                    if (!convertFrom(cx, argv[1], theBackColor)) {
                        JS_ReportError(cx,"JSRenderWindow::renderText: parameter 2 must be a Vector4f (backcolor)");
                        return JS_FALSE;
                    }
                    jslib::JSClassTraits<DERIVED>::openNativeRef(cx, obj).setTextColor(theTextColor, theBackColor);
                    jslib::JSClassTraits<DERIVED>::closeNativeRef(cx,obj);
                }

                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        }
        static JSBool
        renderTextAsImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Renders a text into a target image node");
            DOC_PARAM("theImageNode", "", DOC_TYPE_NODE);
            DOC_PARAM("theTextString", "", DOC_TYPE_STRING);
            DOC_PARAM("theFontName", "", DOC_TYPE_STRING);
            DOC_PARAM("theTextBoxWidth", "", DOC_TYPE_INTEGER);
            DOC_PARAM("theTextBoxHeight", "", DOC_TYPE_INTEGER);
            DOC_RVAL("theFilledTextBoxSize", DOC_TYPE_VECTOR2I);
            DOC_END;
            typedef asl::Vector2i (DERIVED::*MyMethod)(dom::NodePtr, const std::string &, const std::string &, const unsigned int &, const unsigned int &);
            return Method<DERIVED>::call((MyMethod)&DERIVED::renderTextAsImage,cx,obj,argc,argv,rval);
        }
        static JSBool
        setTextStyle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets style for text paragraphs (normal, bold, italic, bolditalic)" \
                      "See Renderer static properties for possible constants");
            DOC_PARAM("theHorizontalAlignment", "", DOC_TYPE_ENUMERATION);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(unsigned);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setTextStyle,cx,obj,argc,argv,rval);
        }
        static JSBool
        setTextPadding(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets padding in pixels around text paragraphs");
            DOC_PARAM("theTopPadding", "", DOC_TYPE_INTEGER);
            DOC_PARAM("theBottomPadding", "", DOC_TYPE_INTEGER);
            DOC_PARAM("theLeftPadding", "", DOC_TYPE_INTEGER);
            DOC_PARAM("theRightPadding", "", DOC_TYPE_INTEGER);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(int, int, int, int);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setTextPadding,cx,obj,argc,argv,rval);
        }
        static JSBool
        setHTextAlignment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets horizontal alignment for text paragraphs" \
                      "See Renderer static properties for possible constants");
            DOC_PARAM("theHorizontalAlignment", "", DOC_TYPE_ENUMERATION);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(unsigned);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setHTextAlignment,cx,obj,argc,argv,rval);
        }
        static JSBool
        setVTextAlignment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets vertical alignment for text paragraphs" \
                      "See Renderer static properties for possible constants");
            DOC_PARAM("theVerticalAlignment", "", DOC_TYPE_ENUMERATION);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(unsigned);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setVTextAlignment,cx,obj,argc,argv,rval);
        }
        static JSBool
        setLineHeight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets line height in pixel for text paragraphs");
            DOC_PARAM("theLineHeight", "", DOC_TYPE_INTEGER);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(unsigned);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setLineHeight,cx,obj,argc,argv,rval);
        }
        static JSBool
        setParagraph(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets top and bottom margins in pixel for text paragraphs");
            DOC_PARAM("theTopMargin", "", DOC_TYPE_INTEGER);
            DOC_PARAM("theBottomMargin", "", DOC_TYPE_INTEGER);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(unsigned, unsigned);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setParagraph,cx,obj,argc,argv,rval);
        }
        static JSBool
        getGlyphMetrics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Gets metrics of a glyph of the given font");
            DOC_PARAM("theFont", "Font name", DOC_TYPE_STRING);
            DOC_PARAM("theGlyph", "Glyph", DOC_TYPE_STRING);
            DOC_END;
            if (argc == 2) {
                std::string myFontName;
                if (!convertFrom(cx,argv[0],myFontName)) {
                    JS_ReportError(cx,"JSRenderWindow::getGlyphMetrics: parameter 0 must be a string");
                    return JS_FALSE;
                }
                std::string myCharacter;
                if (!convertFrom(cx,argv[1],myCharacter)) {
                    JS_ReportError(cx,"JSRenderWindow::getGlyphMetrics: parameter 1 must be a string");
                    return JS_FALSE;
                }

                asl::Box2f myGlyphBox;
                double myAdvance;
#if 1
                jslib::JSClassTraits<DERIVED>::openNativeRef(cx, obj).getGlyphMetrics(myFontName, myCharacter, myGlyphBox, myAdvance);
                jslib::JSClassTraits<DERIVED>::closeNativeRef(cx,obj);
#else
                jslib::JSClassTraits<DERIVED>::ScopedNativeRef myObj(cx, obj);
                myObj.getNative().getGlyphMetrics(myFontName, myCharacter, myGlyphBox, myAdvance);
#endif

                JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
                *rval = OBJECT_TO_JSVAL(myReturnObject);
                if (!JS_DefineProperty(cx, myReturnObject, "min",    as_jsval(cx, myGlyphBox.getMin()), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
                if (!JS_DefineProperty(cx, myReturnObject, "max",    as_jsval(cx, myGlyphBox.getMax()), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
                if (!JS_DefineProperty(cx, myReturnObject, "advance", as_jsval(cx, myAdvance), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;

                return JS_TRUE;
            }
            JS_ReportError(cx,"JSRenderWindow::getGlyphMetrics: bad number of arguments, 2 expected");
            return JS_FALSE;
        }
        static JSBool
        getKerning(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Get kerning for two glyphs of the given font");
            DOC_PARAM("theFont", "Font name", DOC_TYPE_STRING);
            DOC_PARAM("theGlyph0", "First glyph", DOC_TYPE_STRING);
            DOC_PARAM("theGlyph1", "Second glyph", DOC_TYPE_STRING);
            DOC_END;
            typedef double (DERIVED::*MyMethod)(const std::string &, const std::string &, const std::string &);
            return Method<DERIVED>::call((MyMethod)&DERIVED::getKerning,cx,obj,argc,argv,rval);
        }
        static JSBool
        setTracking(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Set font tracking");
            DOC_PARAM("theTracking", "Tracking value", DOC_TYPE_FLOAT);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(float);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setTracking,cx,obj,argc,argv,rval);
        }

        static JSBool
        playClip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Plays an animation clip within character at a given time");
            DOC_PARAM("thePlayTime", "", DOC_TYPE_FLOAT);
            DOC_PARAM("theCharacterName", "", DOC_TYPE_STRING);
            DOC_PARAM("theClipName", "", DOC_TYPE_STRING);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(float, const std::string &, const std::string &);
            return Method<DERIVED>::call((MyMethod)&DERIVED::playClip,cx,obj,argc,argv,rval);
        }
        static JSBool
        setClipLoops(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets the number of loops of a clip within character");
            DOC_PARAM("theCharacterName", "", DOC_TYPE_STRING);
            DOC_PARAM("theClipName", "", DOC_TYPE_STRING);
            DOC_PARAM("theNumberOfLoops", "", DOC_TYPE_INTEGER);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(const std::string &, const std::string &, unsigned);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setClipLoops,cx,obj,argc,argv,rval);
        }
        static JSBool
        setClipForwardDirection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Sets the direction of a clip within character");
            DOC_PARAM("theCharacterName", "", DOC_TYPE_STRING);
            DOC_PARAM("theClipName", "", DOC_TYPE_STRING);
            DOC_PARAM("theDirection", "", DOC_TYPE_BOOLEAN);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(const std::string &, const std::string &, bool);
            return Method<DERIVED>::call((MyMethod)&DERIVED::setClipForwardDirection,cx,obj,argc,argv,rval);
        }
        static JSBool
        getLoops(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Returns the number of loops of a clip within character");
            DOC_PARAM("theCharacterName", "", DOC_TYPE_STRING);
            DOC_PARAM("theClipName", "", DOC_TYPE_STRING);
            DOC_RVAL("numberOfLoops", DOC_TYPE_INTEGER);
            DOC_END;
            typedef unsigned (DERIVED::*MyMethod)(const std::string &, const std::string &);
            return Method<DERIVED>::call((MyMethod)&DERIVED::getLoops,cx,obj,argc,argv,rval);
        }
        static JSBool
        runAnimations(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Runs all global animations with a given time.");
            DOC_PARAM("theTime", "", DOC_TYPE_FLOAT);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(float);
            return Method<DERIVED>::call((MyMethod)&DERIVED::runAnimations,cx,obj,argc,argv,rval);
        }
        static JSBool
        isClipActive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Returns if a clip within character with a given name is active");
            DOC_PARAM("theCharacterName", "", DOC_TYPE_STRING);
            DOC_PARAM("theClipName", "", DOC_TYPE_STRING);
            DOC_RVAL("isActive", DOC_TYPE_BOOLEAN);
            DOC_END;
            typedef bool (DERIVED::*MyMethod)(const std::string &, const std::string &);
            return Method<DERIVED>::call((MyMethod)&DERIVED::isClipActive,cx,obj,argc,argv,rval);
        }
        static JSBool
        isCharacterActive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Returns if a character with a given name is active");
            DOC_PARAM("theCharacterName", "", DOC_TYPE_STRING);
            DOC_RVAL("isActive", DOC_TYPE_BOOLEAN);
            DOC_END;
            typedef bool (DERIVED::*MyMethod)(const std::string &);
            return Method<DERIVED>::call((MyMethod)&DERIVED::isCharacterActive,cx,obj,argc,argv,rval);
        }
        static JSBool
        stopCharacter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Stops an animated character with a given name");
            DOC_PARAM("theCharacterName", "", DOC_TYPE_STRING);
            DOC_END;
            typedef void (DERIVED::*MyMethod)(const std::string &);
            return Method<DERIVED>::call((MyMethod)&DERIVED::stopCharacter,cx,obj,argc,argv,rval);
        }
        static JSBool
        activateGLContext(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Use this function to switch the OpenGL context to an offscreen windows.");
            DOC_END;
            DERIVED * mySelf;
            convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
            mySelf->activateGLContext();
            return JS_TRUE;
        }
        static JSBool
        deactivateGLContext(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Use this function to switch of an OpenGL context of an offscreen window.");
            DOC_END;
            DERIVED * mySelf;
            convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
            mySelf->deactivateGLContext();
            return JS_TRUE;
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
                {"saveBuffer",         saveBuffer,               2},
                {"getRenderer",        getRenderer,              0},
                {"addExtension",       addExtension,             1},
                {"hasCap",             hasCap,                   1},
                {"hasCapAsString",     hasCapAsString,           1},
                {"printStatistics",    printStatistics,          2}, // base class
                {"getImagePixel",      getImagePixel,            3},
                {"setImagePixel",      setImagePixel,            4},
                {"performRequest",     performRequest,           1},
                // text rendering
                {"renderText",         renderText,               3},
                {"setTextColor",       setTextColor,             2},
                {"renderTextAsImage",  renderTextAsImage,        5},
                {"setTextStyle",       setTextStyle,             1},
                {"setTextPadding",     setTextPadding,           4},
                {"setHTextAlignment",  setHTextAlignment,        1},
                {"setVTextAlignment",  setVTextAlignment,        1},
                {"setLineHeight",      setLineHeight,            1},
                {"setParagraph",       setParagraph,             2},
                {"getGlyphMetrics",    getGlyphMetrics,          7},
                {"getKerning",         getKerning,               3},
                {"setTracking",        setTracking,              1},
                // animations
                {"runAnimations",      runAnimations,            1},
                {"playClip",           playClip,                 3},
                {"setClipLoops",       setClipLoops,             3},
                {"setClipForwardDirection", setClipForwardDirection,3},
                {"getLoops",           getLoops,                 2},
                {"isClipActive",       isClipActive,             2},
                {"isCharacterActive",  isCharacterActive,        1},
                {"stopCharacter",      stopCharacter,            1},
                {"activateGLContext",  activateGLContext,        0},
                {"deactivateGLContext",  deactivateGLContext,    0},
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
                {"scene",           PROP_scene,             JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED}, // node
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
                    {
                        typedef void (DERIVED::*MyMethod)(bool);
                        return Method<DERIVED>::call((MyMethod)&DERIVED::setPause, cx, obj, 1, vp, &dummy);
                    }
                case PROP_eventListener:
                    {
                        JSObject * myListener;
                        if (convertFrom(cx, *vp, myListener)) {
                            theNative.setJSContext(cx);
                            theNative.setEventListener( myListener);
                            return JS_TRUE;
                        }
                        return JS_FALSE;
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
                case PROP_scene: {
                    y60::ScenePtr myScene(0);
                    if (*vp == JSVAL_NULL || !convertFrom(cx, *vp, myScene)) {
                        throw JSArgMismatch("Failed to convert argument to type y60::Scene", PLUS_FILE_LINE);
                    }
                    if (theNative.setScene(myScene)) {
                        return JS_TRUE;
                    } else {
                        return JS_FALSE;
                    }
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
                            asl::Vector3f myColor;
                            if (!convertFrom(cx, *vp, myColor)) {
                                JS_ReportError(cx,"JSAbstractRenderWindow::set background color must be Vector3f or Vector4f.");
                                return JS_FALSE;
                            }
                            theColor[0] = myColor[0];
                            theColor[1] = myColor[1];
                            theColor[2] = myColor[2];
                            theColor[3] = 1;
                        }
                        theNative.getCanvas()->dom::Node::getFacade<y60::Canvas>()->y60::Canvas::set<y60::CanvasBackgroundColorTag>(theColor);
                        return JS_TRUE;
                    }
                case PROP_canvas:
                    {
                        dom::NodePtr myCanvasNode(0);
                        if (convertFrom(cx, *vp, myCanvasNode)) {
                            theNative.setCanvas(myCanvasNode);
                            return JS_TRUE;
                        }
                        return JS_FALSE;
                    }
                case PROP_renderingCaps:
                    unsigned myFlags;
                    if (convertFrom(cx, *vp, myFlags)) {
                        theNative.setRenderingCaps( myFlags );
                        return JS_TRUE;
                    }
                    return JS_FALSE;
                case PROP_fixedFrameTime:
                    {
                        typedef void (DERIVED::*MyMethod)(const float &);
                        return Method<DERIVED>::call((MyMethod)&DERIVED::setFixedDeltaT, cx, obj, 1, vp, &dummy);
                    }
                default:
                    JS_ReportError(cx,"JSAbstractRenderWindow::setPropertySwitch: index %d out of range", theID);
                    return JS_FALSE;
            }
        }
    };
}

#endif



