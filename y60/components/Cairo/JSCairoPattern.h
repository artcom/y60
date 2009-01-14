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

#ifndef _Y60_CAIRO_JSCAIROPATTERN_INCLUDED_
#define _Y60_CAIRO_JSCAIROPATTERN_INCLUDED_

#include <asl/dom/Nodes.h>

#include <y60/jsbase/JSWrapper.h>

extern "C" {
#include <cairo.h>
}

#include "CairoWrapper.h"

namespace jslib {

    typedef CairoWrapper<cairo_pattern_t> JSCairoPatternWrapper;

    class JSCairoPattern : public JSWrapper<JSCairoPatternWrapper, asl::Ptr< JSCairoPatternWrapper >, StaticAccessProtocol> {
        JSCairoPattern();  // hide default constructor
    public:

        virtual ~JSCairoPattern() {
        }

        typedef JSCairoPatternWrapper NATIVE;
        typedef asl::Ptr< JSCairoPatternWrapper > OWNERPTR;

        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "CairoPattern";
        }

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
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

        JSCairoPattern(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
            { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSCairoPattern & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCairoPattern &>(JSCairoPattern::getJSWrapper(cx,obj));
        }

    };

    template <>
    struct JSClassTraits<JSCairoPattern::NATIVE>
        : public JSClassTraitsWrapper<JSCairoPattern::NATIVE, JSCairoPattern> {};

    Y60_CAIRO_EXPORT jsval as_jsval(JSContext *cx, JSCairoPattern::OWNERPTR theOwner, JSCairoPattern::NATIVE * thePattern);
    Y60_CAIRO_EXPORT jsval as_jsval(JSContext *cx, JSCairoPattern::OWNERPTR theOwner, cairo_pattern_t * thePattern);
    inline jsval as_jsval(JSContext *cx, cairo_pattern_t * thePattern)
    {
        JSCairoPatternWrapper* theOwner = JSCairoPatternWrapper::get(thePattern);
        assert(theOwner);
        return as_jsval(cx,theOwner->getWrappedPtr(),thePattern);
    }
    
    Y60_CAIRO_EXPORT bool convertFrom(JSContext *cx, jsval theValue, JSCairoPattern::NATIVE *& theTarget);
    Y60_CAIRO_EXPORT bool convertFrom(JSContext *cx, jsval theValue, cairo_pattern_t *& thePattern);
}

#endif /* !_Y60_CAIRO_JSCAIROPATTERN_INCLUDED_ */
