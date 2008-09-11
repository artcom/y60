//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSTOGGLETOOLBUTTON_INCLUDED_
#define _Y60_ACGTKSHELL_JSTOGGLETOOLBUTTON_INCLUDED_

#include "JSToolButton.h"
#include <y60/jsbase/JSWrapper.h>
#include <gtkmm/toggletoolbutton.h>

#include <asl/base/string_functions.h>

namespace jslib {

class JSToggleToolButton : public JSWrapper<Gtk::ToggleToolButton, asl::Ptr<Gtk::ToggleToolButton>, StaticAccessProtocol> {
        JSToggleToolButton();  // hide default constructor
    typedef JSToolButton JSBASE;
    public:
        virtual ~JSToggleToolButton() {
        }
        typedef Gtk::ToggleToolButton NATIVE;
        typedef asl::Ptr<Gtk::ToggleToolButton> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "ToggleToolButton";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
             PROP_active = JSBASE::PROP_END,
             PROP_signal_toggled,
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

        JSToggleToolButton(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSToggleToolButton & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSToggleToolButton &>(JSToggleToolButton::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSToggleToolButton::NATIVE>
    : public JSClassTraitsWrapper<JSToggleToolButton::NATIVE, JSToggleToolButton> {};

jsval as_jsval(JSContext *cx, JSToggleToolButton::OWNERPTR theOwner, JSToggleToolButton::NATIVE * theToggleToolButton);

} // namespace

#endif



