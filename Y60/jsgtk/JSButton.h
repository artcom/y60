//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSButton.h,v $
//   $Author: david $
//   $Revision: 1.4 $
//   $Date: 2005/02/03 15:57:49 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSBUTTON_INCLUDED_
#define _Y60_ACGTKSHELL_JSBUTTON_INCLUDED_

#include "JSBin.h"
#include <y60/JSWrapper.h>
#include <gtkmm/button.h>

#include <asl/string_functions.h>

namespace jslib {

class JSButton : public JSWrapper<Gtk::Button, asl::Ptr<Gtk::Button>, StaticAccessProtocol> {
        JSButton();  // hide default constructor
    typedef JSBin JSBASE;
    public:
        virtual ~JSButton() {
        }
        typedef Gtk::Button NATIVE;
        typedef asl::Ptr<Gtk::Button> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Button";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
             PROP_relief = JSBASE::PROP_END,
             PROP_label,
             PROP_use_underline,
             PROP_use_stock,
             PROP_focus_on_click,
             PROP_alignment,
             PROP_image,
             PROP_signal_pressed,
             PROP_signal_released,
             PROP_signal_clicked,
             PROP_signal_enter,
             PROP_signal_leave,
             PROP_signal_activate,
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

        JSButton(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSButton & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSButton &>(JSButton::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSButton::NATIVE>
    : public JSClassTraitsWrapper<JSButton::NATIVE, JSButton> {};

jsval as_jsval(JSContext *cx, JSButton::OWNERPTR theOwner, JSButton::NATIVE * theButton);

} // namespace

#endif



