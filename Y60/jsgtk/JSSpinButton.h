//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSSPINBUTTON_INCLUDED_
#define _Y60_ACGTKSHELL_JSSPINBUTTON_INCLUDED_

#include "JSEntry.h"
#include <y60/jsbase/JSWrapper.h>
#include <gtkmm/spinbutton.h>

#include <asl/base/string_functions.h>

namespace jslib {

class JSSpinButton : public JSWrapper<Gtk::SpinButton, asl::Ptr<Gtk::SpinButton>, StaticAccessProtocol> {
        JSSpinButton();  // hide default constructor
    typedef JSEntry JSBASE;
    public:
        virtual ~JSSpinButton() {
        }
        typedef Gtk::SpinButton NATIVE;
        typedef asl::Ptr<Gtk::SpinButton> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "SpinButton";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_digits = JSBASE::PROP_END,
            PROP_increments,
            PROP_range,
            PROP_value,
            PROP_signal_value_changed,
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

        JSSpinButton(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSSpinButton & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSSpinButton &>(JSSpinButton::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSSpinButton::NATIVE>
    : public JSClassTraitsWrapper<JSSpinButton::NATIVE, JSSpinButton> {};

jsval as_jsval(JSContext *cx, JSSpinButton::OWNERPTR theOwner, JSSpinButton::NATIVE * theSpinButton);

} // namespace

#endif



