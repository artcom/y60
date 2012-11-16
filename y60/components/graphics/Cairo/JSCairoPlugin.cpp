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

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>

#include "JSCairoContext.h"
#include "JSCairoSurface.h"
#include "JSCairoPattern.h"

#include "JSPangoContext.h"
#include "JSPangoFontDescription.h"
#include "JSPangoLayout.h"

namespace y60 {
    using namespace jslib;

    static JSClass Package = {
        "Package",
        JSCLASS_HAS_PRIVATE,
        JS_PropertyStub, JS_PropertyStub,
        JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,
        JS_ConvertStub, JS_FinalizeStub
    };

    class JSCairoPlugin : public asl::PlugInBase, public jslib::IScriptablePlugin {
        public:
            enum PropertyNumbers {
                PROP_ANTIALIAS_DEFAULT,
                PROP_ANTIALIAS_NONE,
                PROP_ANTIALIAS_GRAY,
                PROP_ANTIALIAS_SUBPIXEL,

                PROP_FILL_RULE_WINDING,
                PROP_FILL_RULE_EVEN_ODD,

                PROP_LINE_CAP_BUTT,
                PROP_LINE_CAP_ROUND,
                PROP_LINE_CAP_SQUARE,

                PROP_LINE_JOIN_MITER,
                PROP_LINE_JOIN_ROUND,
                PROP_LINE_JOIN_BEVEL,

                PROP_OPERATOR_CLEAR,
                PROP_OPERATOR_SOURCE,
                PROP_OPERATOR_OVER,
                PROP_OPERATOR_IN,
                PROP_OPERATOR_OUT,
                PROP_OPERATOR_ATOP,
                PROP_OPERATOR_DEST,
                PROP_OPERATOR_DEST_OVER,
                PROP_OPERATOR_DEST_IN,
                PROP_OPERATOR_DEST_OUT,
                PROP_OPERATOR_DEST_ATOP,
                PROP_OPERATOR_XOR,
                PROP_OPERATOR_ADD,
                PROP_OPERATOR_SATURATE,

                PROP_FONT_TYPE_TOY,
                PROP_FONT_TYPE_FT,
                PROP_FONT_TYPE_WIN32,
                PROP_FONT_TYPE_ATSUI,

                PROP_FONT_SLANT_NORMAL,
                PROP_FONT_SLANT_ITALIC,
                PROP_FONT_SLANT_OBLIQUE,

                PROP_FONT_WEIGHT_NORMAL,
                PROP_FONT_WEIGHT_BOLD,

                PROP_SUBPIXEL_ORDER_DEFAULT,
                PROP_SUBPIXEL_ORDER_RGB,
                PROP_SUBPIXEL_ORDER_BGR,
                PROP_SUBPIXEL_ORDER_VRGB,
                PROP_SUBPIXEL_ORDER_VBGR,

                PROP_HINT_STYLE_DEFAULT,
                PROP_HINT_STYLE_NONE,
                PROP_HINT_STYLE_SLIGHT,
                PROP_HINT_STYLE_MEDIUM,
                PROP_HINT_STYLE_FULL,

                PROP_HINT_METRICS_DEFAULT,
                PROP_HINT_METRICS_OFF,
                PROP_HINT_METRICS_ON,

                PROP_EXTEND_NONE,
                PROP_EXTEND_REPEAT,
                // PROP_EXTEND_REFLECT,
                // PROP_EXTEND_PAD,

                PROP_END
            };
            JSCairoPlugin(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}
            JSConstIntPropertySpec * ConstIntProperties() {

                static JSConstIntPropertySpec myProperties[] = {
                    // name                id                       value
                    {"ANTIALIAS_DEFAULT",  PROP_ANTIALIAS_DEFAULT,  CAIRO_ANTIALIAS_DEFAULT},
                    {"ANTIALIAS_NONE",     PROP_ANTIALIAS_NONE,     CAIRO_ANTIALIAS_NONE},
                    {"ANTIALIAS_GRAY",     PROP_ANTIALIAS_GRAY,     CAIRO_ANTIALIAS_GRAY},
                    {"ANTIALIAS_SUBPIXEL", PROP_ANTIALIAS_SUBPIXEL, CAIRO_ANTIALIAS_SUBPIXEL},

                    {"FILL_RULE_WINDING",  PROP_FILL_RULE_WINDING,  CAIRO_FILL_RULE_WINDING},
                    {"FILL_RULE_EVEN_ODD", PROP_FILL_RULE_EVEN_ODD, CAIRO_FILL_RULE_EVEN_ODD},

                    {"LINE_CAP_BUTT",      PROP_LINE_CAP_BUTT,      CAIRO_LINE_CAP_BUTT},
                    {"LINE_CAP_ROUND",     PROP_LINE_CAP_ROUND,     CAIRO_LINE_CAP_ROUND},
                    {"LINE_CAP_SQUARE",    PROP_LINE_CAP_SQUARE,    CAIRO_LINE_CAP_SQUARE},

                    {"LINE_JOIN_MITER",    PROP_LINE_JOIN_MITER,    CAIRO_LINE_JOIN_MITER},
                    {"LINE_JOIN_ROUND",    PROP_LINE_JOIN_ROUND,    CAIRO_LINE_JOIN_ROUND},
                    {"LINE_JOIN_BEVEL",    PROP_LINE_JOIN_BEVEL,    CAIRO_LINE_JOIN_BEVEL},

                    {"OPERATOR_CLEAR",     PROP_OPERATOR_CLEAR,     CAIRO_OPERATOR_CLEAR},
                    {"OPERATOR_SOURCE",    PROP_OPERATOR_SOURCE,    CAIRO_OPERATOR_SOURCE},
                    {"OPERATOR_OVER",      PROP_OPERATOR_OVER,      CAIRO_OPERATOR_OVER},
                    {"OPERATOR_IN",        PROP_OPERATOR_IN,        CAIRO_OPERATOR_IN},
                    {"OPERATOR_OUT",       PROP_OPERATOR_OUT,       CAIRO_OPERATOR_OUT},
                    {"OPERATOR_ATOP",      PROP_OPERATOR_ATOP,      CAIRO_OPERATOR_ATOP},
                    {"OPERATOR_DEST",      PROP_OPERATOR_DEST,      CAIRO_OPERATOR_DEST},
                    {"OPERATOR_DEST_OVER", PROP_OPERATOR_DEST_OVER, CAIRO_OPERATOR_DEST_OVER},
                    {"OPERATOR_DEST_IN",   PROP_OPERATOR_DEST_IN,   CAIRO_OPERATOR_DEST_IN},
                    {"OPERATOR_DEST_OUT",  PROP_OPERATOR_DEST_OUT,  CAIRO_OPERATOR_DEST_OUT},
                    {"OPERATOR_DEST_ATOP", PROP_OPERATOR_DEST_ATOP, CAIRO_OPERATOR_DEST_ATOP},
                    {"OPERATOR_XOR",       PROP_OPERATOR_XOR,       CAIRO_OPERATOR_XOR},
                    {"OPERATOR_ADD",       PROP_OPERATOR_ADD,       CAIRO_OPERATOR_ADD},
                    {"OPERATOR_SATURATE",  PROP_OPERATOR_SATURATE,  CAIRO_OPERATOR_SATURATE},

                    {"FONT_TYPE_TOY",      PROP_FONT_TYPE_TOY,      CAIRO_FONT_TYPE_TOY},
                    {"FONT_TYPE_FT",       PROP_FONT_TYPE_FT,       CAIRO_FONT_TYPE_FT},
                    {"FONT_TYPE_WIN32",    PROP_FONT_TYPE_WIN32,    CAIRO_FONT_TYPE_WIN32},
                    {"FONT_TYPE_ATSUI",    PROP_FONT_TYPE_ATSUI,    CAIRO_FONT_TYPE_ATSUI},

                    {"FONT_SLANT_NORMAL",  PROP_FONT_SLANT_NORMAL,          CAIRO_FONT_SLANT_NORMAL},
                    {"FONT_SLANT_ITALIC",  PROP_FONT_SLANT_ITALIC,          CAIRO_FONT_SLANT_ITALIC},
                    {"FONT_SLAND_OBLIQUE", PROP_FONT_SLANT_OBLIQUE,         CAIRO_FONT_SLANT_OBLIQUE},

                    {"FONT_WEIGHT_NORMAL", PROP_FONT_WEIGHT_NORMAL,         CAIRO_FONT_WEIGHT_NORMAL},
                    {"FONT_WEIGHT_BOLD",   PROP_FONT_WEIGHT_BOLD,           CAIRO_FONT_WEIGHT_BOLD},

                    {"SUBPIXEL_ORDER_DEFAULT", PROP_SUBPIXEL_ORDER_DEFAULT, CAIRO_SUBPIXEL_ORDER_DEFAULT},
                    {"SUBPIXEL_ORDER_RGB",     PROP_SUBPIXEL_ORDER_RGB,     CAIRO_SUBPIXEL_ORDER_RGB},
                    {"SUBPIXEL_ORDER_BGR",     PROP_SUBPIXEL_ORDER_BGR,     CAIRO_SUBPIXEL_ORDER_BGR},
                    {"SUBPIXEL_ORDER_VRGB",    PROP_SUBPIXEL_ORDER_VRGB,    CAIRO_SUBPIXEL_ORDER_VRGB},
                    {"SUBPIXEL_ORDER_VBGR",    PROP_SUBPIXEL_ORDER_VBGR,    CAIRO_SUBPIXEL_ORDER_VBGR},

                    {"HINT_STYLE_DEFAULT",     PROP_HINT_STYLE_DEFAULT,     CAIRO_HINT_STYLE_DEFAULT},
                    {"HINT_STYLE_NONE",        PROP_HINT_STYLE_NONE,        CAIRO_HINT_STYLE_NONE},
                    {"HINT_STYLE_SLIGHT",      PROP_HINT_STYLE_SLIGHT,      CAIRO_HINT_STYLE_SLIGHT},
                    {"HINT_STYLE_MEDIUM",      PROP_HINT_STYLE_MEDIUM,      CAIRO_HINT_STYLE_MEDIUM},
                    {"HINT_STYLE_FULL",        PROP_HINT_STYLE_FULL,        CAIRO_HINT_STYLE_FULL},

                    {"HINT_METRICS_DEFAULT",   PROP_HINT_METRICS_DEFAULT,   CAIRO_HINT_METRICS_DEFAULT},
                    {"HINT_METRICS_OFF",       PROP_HINT_METRICS_OFF,       CAIRO_HINT_METRICS_OFF},
                    {"HINT_METRICS_ON",        PROP_HINT_METRICS_DEFAULT,   CAIRO_HINT_METRICS_ON},

                    {"EXTEND_NONE",   PROP_EXTEND_NONE,   CAIRO_EXTEND_NONE},
                    {"EXTEND_REPEAT", PROP_EXTEND_REPEAT, CAIRO_EXTEND_REPEAT},

                    {0}
                };
                return myProperties;
            };

            virtual void initClasses(JSContext * theContext, JSObject * theGlobalObject) {
                // start cairo namespace
                JSObject *cairoNamespace = JS_DefineObject(theContext, theGlobalObject, "Cairo", &Package, NULL, JSPROP_PERMANENT | JSPROP_READONLY);
                JSA_DefineConstInts(theContext, cairoNamespace, ConstIntProperties()); 
                cairo::JSContext::initClass(theContext, cairoNamespace);
                cairo::JSSurface::initClass(theContext,  cairoNamespace);
                cairo::JSPattern::initClass(theContext, cairoNamespace);

                // start pango namespace
                JSObject *pangoNamespace = JS_DefineObject(theContext, theGlobalObject, "Pango", &Package, NULL, JSPROP_PERMANENT | JSPROP_READONLY);
                pango::JSFontDescription::initClass(theContext, pangoNamespace);
                pango::JSContext::initClass(theContext, pangoNamespace);
                pango::JSLayout::initClass(theContext, pangoNamespace);
            }

            const char * ClassName() {
                static const char * myClassName = "Cairo";
                return myClassName;
            }
    };
}

extern "C"
EXPORT asl::PlugInBase * Cairo_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::JSCairoPlugin(myDLHandle);
}
