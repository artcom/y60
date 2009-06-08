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

#ifndef _Y60_CAIRO_JSCAIRO_INCLUDED_
#define _Y60_CAIRO_JSCAIRO_INCLUDED_

#include <asl/dom/Nodes.h>

#include <y60/jsbase/JSWrapper.h>

extern "C" {
#include <cairo.h>
}

#include "CairoWrapper.h"

namespace jslib {

    typedef CairoWrapper<cairo_t> JSCairoWrapper;

    class JSCairo : public JSWrapper<JSCairoWrapper, asl::Ptr< JSCairoWrapper >, StaticAccessProtocol> {
        JSCairo();  // hide default constructor
    public:

        virtual ~JSCairo() {
        }

        typedef JSCairoWrapper NATIVE;
        typedef asl::Ptr< JSCairoWrapper > OWNERPTR;

        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Cairo";
        }

        static JSFunctionSpec * Functions();

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
        static JSPropertySpec * Properties();

        virtual unsigned long length() const {
            return 1;
        }

        static JSBool getPropertySwitch(NATIVE & theNative, unsigned long theID,
                                        JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID, JSContext *cx,
                                        JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
            Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSCairo(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
            { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSCairo & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCairo &>(JSCairo::getJSWrapper(cx,obj));
        }

    };

    template <>
    struct JSClassTraits<JSCairo::NATIVE>
        : public JSClassTraitsWrapper<JSCairo::NATIVE, JSCairo> {};

    jsval as_jsval(JSContext *cx, JSCairo::OWNERPTR theOwner, JSCairo::NATIVE * theContext);
    jsval as_jsval(JSContext *cx, JSCairo::OWNERPTR theOwner, cairo_t * theContext);
    
    bool convertFrom(JSContext *cx, jsval theValue, JSCairo::NATIVE *& theContext);
    bool convertFrom(JSContext *cx, jsval theValue, cairo_t *& theContext);
}

#endif /* !_Y60_CAIRO_JSCAIROCONTEXT_INCLUDED_ */
