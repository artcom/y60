//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_CAIRO_JSCAIROCONTEXT_INCLUDED_
#define _Y60_CAIRO_JSCAIROCONTEXT_INCLUDED_

#include <dom/Nodes.h>

#include <y60/JSWrapper.h>

#include <cairomm/cairomm.h>

namespace jslib {

    class JSCairoContext : public JSWrapper<Cairo::RefPtr<Cairo::Context>, asl::Ptr< Cairo::RefPtr<Cairo::Context> >, StaticAccessProtocol> {
        JSCairoContext();  // hide default constructor
    public:

        virtual ~JSCairoContext() {
        }

        typedef Cairo::RefPtr<Cairo::Context> NATIVE;
        typedef asl::Ptr< Cairo::RefPtr<Cairo::Context> > OWNERPTR;

        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "CairoContext";
        }

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_ANTIALIAS_DEFAULT,
            PROP_ANTIALIAS_NONE,
            PROP_ANTIALIAS_GRAY,
            PROP_ANTIALIAS_SUBPIXEL,

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

        JSCairoContext(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
            { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSCairoContext & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCairoContext &>(JSCairoContext::getJSWrapper(cx,obj));
        }

    private:
    };

    template <>
    struct JSClassTraits<JSCairoContext::NATIVE>
        : public JSClassTraitsWrapper<JSCairoContext::NATIVE, JSCairoContext> {};

    jsval as_jsval(JSContext *cx, JSCairoContext::OWNERPTR theOwner, JSCairoContext::NATIVE * theButton);
    
    bool convertFrom(JSContext *cx, jsval theValue, JSCairoContext::NATIVE *& theTarget);
}

#endif /* !_Y60_CAIRO_JSCAIROCONTEXT_INCLUDED_ */
