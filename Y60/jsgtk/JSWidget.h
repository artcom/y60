//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSWIDGET_INCLUDED_
#define _Y60_ACGTKSHELL_JSWIDGET_INCLUDED_

#include "JSRenderArea.h"

#include <y60/JSWrapper.h>
#include <gtkmm/widget.h>
#include <y60/JSAbstractRenderWindow.h>
#include <asl/string_functions.h>

namespace jslib {

class JSWidget : public JSWrapper<Gtk::Widget, asl::Ptr<Gtk::Widget>, StaticAccessProtocol> {
        JSWidget();  // hide default constructor
    public:
        virtual ~JSWidget() {
        }
        typedef Gtk::Widget NATIVE;
        typedef asl::Ptr<Gtk::Widget> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Widget";
        }

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            // TODO: since JSRenderArea inherits properites from both JSWidget and
            //       JSAbstractRenderWindow, the property ids may not overlap.
            //       A better solution would be to use different property get/set methods.
            PROP_BEGIN = JSAbstractRenderWindow<JSRenderArea>::PROP_END,
            PROP_name,
            PROP_is_realized,
            PROP_is_visible,
            PROP_is_mapped,
            PROP_sensitive,
            PROP_focus,
            PROP_parent,
            PROP_width,
            PROP_height,
            PROP_state,
            PROP_signal_show,
            PROP_signal_hide,
            PROP_signal_map,
            PROP_signal_unmap,
            PROP_signal_realize,
            PROP_signal_unrealize,
            PROP_signal_button_press_event,
            PROP_signal_button_release_event,
            PROP_signal_motion_notify_event,
            PROP_signal_key_press_event,
            PROP_signal_key_release_event,
            PROP_signal_enter_notify_event,
            PROP_signal_leave_notify_event,
            PROP_signal_focus_in_event,
            PROP_signal_focus_out_event,
            PROP_signal_delete_event,
            PROP_flags,
            PROP_END,
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

        JSWidget(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        // static JSConstIntPropertySpec * ConstIntProperties();
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSWidget & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSWidget &>(JSWidget::getJSWrapper(cx,obj));
        }
    private:

};

template <>
struct JSClassTraits<JSWidget::NATIVE>
    : public JSClassTraitsWrapper<JSWidget::NATIVE, JSWidget> {};

jsval as_jsval(JSContext *cx, JSWidget::OWNERPTR theOwner, JSWidget::NATIVE * theWidget);

}

#endif


