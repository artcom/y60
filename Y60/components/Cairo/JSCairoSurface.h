//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

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

    class JSCairoSurface : public JSWrapper<JSCairoSurfaceWrapper, asl::Ptr< JSCairoSurfaceWrapper >, StaticAccessProtocol> {
        JSCairoSurface();  // hide default constructor
    public:

        virtual ~JSCairoSurface() {
        }

        typedef JSCairoSurfaceWrapper NATIVE;
        typedef asl::Ptr< JSCairoSurfaceWrapper > OWNERPTR;

        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "CairoSurface";
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

        JSCairoSurface(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
            { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSCairoSurface & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCairoSurface &>(JSCairoSurface::getJSWrapper(cx,obj));
        }

        // XXX hack to allow triggering texture upload -ingo
        void doTriggerUpload();

        DEFINE_EXCEPTION(UnsupportedPixelFormat, asl::Exception);

    private:
        dom::NodePtr _myImageNode;

        static void convertRGBAtoBGRA(dom::ResizeableRasterPtr theOld, dom::ResizeableRasterPtr theNew);
    };

    template <>
    struct JSClassTraits<JSCairoSurface::NATIVE>
        : public JSClassTraitsWrapper<JSCairoSurface::NATIVE, JSCairoSurface> {};

    jsval as_jsval(JSContext *cx, JSCairoSurface::OWNERPTR theOwner, JSCairoSurface::NATIVE * theSurface);
    jsval as_jsval(JSContext *cx, JSCairoSurface::OWNERPTR theOwner, cairo_surface_t * theSurface);
    inline jsval as_jsval(JSContext *cx, cairo_surface_t * theSurface)
    {
        JSCairoSurfaceWrapper* theOwner = JSCairoSurfaceWrapper::get(theSurface);
        assert(theOwner);
        return as_jsval(cx,theOwner->getWrappedPtr(),theSurface);
    }
    
    bool convertFrom(JSContext *cx, jsval theValue, JSCairoSurface::NATIVE *& theTarget);
    bool convertFrom(JSContext *cx, jsval theValue, cairo_surface_t *& theTarget);
}

#endif /* !_Y60_CAIRO_JSCAIROSURFACE_INCLUDED_ */
