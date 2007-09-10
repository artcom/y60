//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_CAIRO_JSCAIROPATTERN_INCLUDED_
#define _Y60_CAIRO_JSCAIROPATTERN_INCLUDED_

#include <dom/Nodes.h>

#include <y60/JSWrapper.h>

#include <cairomm/cairomm.h>

namespace jslib {

    class JSCairoPattern : public JSWrapper<Cairo::Pattern, asl::Ptr<Cairo::Pattern>, StaticAccessProtocol> {
        JSCairoPattern();  // hide default constructor
    public:

        virtual ~JSCairoPattern() {
        }

        typedef Cairo::Pattern NATIVE;
        typedef asl::Ptr<Cairo::Pattern> OWNERPTR;

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

    private:
    };

    template <>
    struct JSClassTraits<JSCairoPattern::NATIVE>
        : public JSClassTraitsWrapper<JSCairoPattern::NATIVE, JSCairoPattern> {};

    jsval as_jsval(JSContext *cx, JSCairoPattern::OWNERPTR theOwner, JSCairoPattern::NATIVE * theButton);
    
    bool convertFrom(JSContext *cx, jsval theValue, JSCairoPattern::NATIVE *& theTarget);
}

#endif /* !_Y60_CAIRO_JSCAIROPATTERN_INCLUDED_ */
