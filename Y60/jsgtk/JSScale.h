//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSSCALE_INCLUDED_
#define _Y60_ACGTKSHELL_JSSCALE_INCLUDED_

#include "JSRange.h"

#include <y60/JSWrapper.h>
#include <gtkmm/scale.h>

#include <asl/string_functions.h>

namespace jslib {

class JSScale : public JSWrapper<Gtk::Scale, asl::Ptr<Gtk::Scale>, StaticAccessProtocol> {
    private:
        JSScale();  // hide default constructor
        typedef JSRange JSBASE;
    public:
        virtual ~JSScale() {
        }
        typedef Gtk::Scale NATIVE;
        typedef asl::Ptr<Gtk::Scale> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Scale";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_digits = JSBASE::PROP_END,
            PROP_draw_value,
            PROP_value_pos,
            PROP_signal_format_value,
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

        JSScale(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSScale & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSScale &>(JSScale::getJSWrapper(cx,obj));
        }

    private:
};


template <>
struct JSClassTraits<JSScale::NATIVE>
    : public JSClassTraitsWrapper<JSScale::NATIVE, JSScale> {};

jsval as_jsval(JSContext *cx, JSScale::OWNERPTR theOwner, JSScale::NATIVE * theScale);

} // namespace

#endif

