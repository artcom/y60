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

#ifndef _Y60_CAIRO_JSCAIROSURFACE_INCLUDED_
#define _Y60_CAIRO_JSCAIROSURFACE_INCLUDED_

#include <cassert>

#include <asl/dom/Nodes.h>
#include <asl/dom/Value.h>

#include <y60/jsbase/JSWrapper.h>

extern "C" {
#include <cairo.h>
}

#include "CairoWrapper.h"


namespace jslib {

    typedef CairoWrapper<cairo_surface_t> JSCairoSurfaceWrapper;

    namespace cairo {

        class JSSurface : public JSWrapper<JSCairoSurfaceWrapper, asl::Ptr< JSCairoSurfaceWrapper >, StaticAccessProtocol> {
            JSSurface();  // hide default constructor
            public:

            virtual ~JSSurface() {
            }

            typedef JSCairoSurfaceWrapper NATIVE;
            typedef asl::Ptr< JSCairoSurfaceWrapper > OWNERPTR;

            typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

            static const char * ClassName() {
                return "Surface";
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
                    ::JSContext *cx, JSObject *obj, jsval id, jsval *vp);
            virtual JSBool getPropertySwitch(unsigned long theID, ::JSContext *cx, JSObject *obj, jsval id, jsval *vp);
            static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID, ::JSContext *cx,
                    JSObject *obj, jsval id, jsval *vp);
            virtual JSBool setPropertySwitch(unsigned long theID, ::JSContext *cx, JSObject *obj, jsval id, jsval *vp);

            static JSBool
                Constructor(::JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

            static
                JSObject * Construct(::JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
                    return Base::Construct(cx, theOwner, theNative);
                }

            JSSurface(OWNERPTR theOwner, NATIVE * theNative)
                : Base(theOwner, theNative)
            { }

            static JSConstIntPropertySpec * ConstIntProperties();
            static JSObject * initClass(::JSContext *cx, JSObject *theGlobalObject);
            static void addClassProperties(::JSContext * cx, JSObject * theClassProto);

            static JSSurface & getObject(::JSContext *cx, JSObject * obj) {
                return dynamic_cast<JSSurface &>(JSSurface::getJSWrapper(cx,obj));
            }

            // XXX hack to allow triggering texture upload -ingo
            void doTriggerUpload();

            DEFINE_EXCEPTION(UnsupportedPixelFormat, asl::Exception);

            private:
            dom::NodePtr _myImageNode;

            static void convertRGBAtoBGRA(dom::ResizeableRasterPtr theOld, dom::ResizeableRasterPtr theNew);
        };
    };

    template <>
    struct JSClassTraits<cairo::JSSurface::NATIVE>
        : public JSClassTraitsWrapper<cairo::JSSurface::NATIVE, cairo::JSSurface> {};

    jsval as_jsval(JSContext *cx, cairo::JSSurface::OWNERPTR theOwner, cairo::JSSurface::NATIVE * theSurface);
    jsval as_jsval(JSContext *cx, cairo::JSSurface::OWNERPTR theOwner, cairo_surface_t * theSurface);
    inline jsval as_jsval(JSContext *cx, cairo_surface_t * theSurface)
    {
        JSCairoSurfaceWrapper* theOwner = JSCairoSurfaceWrapper::get(theSurface);
        assert(theOwner);
        return as_jsval(cx,theOwner->getWrappedPtr(),theSurface);
    }

    bool convertFrom(JSContext *cx, jsval theValue, cairo::JSSurface::NATIVE *& theTarget);
    bool convertFrom(JSContext *cx, jsval theValue, cairo_surface_t *& theTarget);
}

#endif /* !_Y60_CAIRO_JSCAIROSURFACE_INCLUDED_ */
