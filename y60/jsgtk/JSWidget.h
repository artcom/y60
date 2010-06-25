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

#ifndef _Y60_ACGTKSHELL_JSWIDGET_INCLUDED_
#define _Y60_ACGTKSHELL_JSWIDGET_INCLUDED_

#include "y60_jsgtk_settings.h"

#include "JSRenderArea.h"

#include <y60/jsbase/JSWrapper.h>
#include <y60/jslib/JSAbstractRenderWindow.h>
#include <asl/base/string_functions.h>

#if defined(_MSC_VER)
    #pragma warning(push,1)
    //#pragma warning(disable:4512 4413 4244 4251 4250)
#endif //defined(_MSC_VER)
#include <gtkmm/widget.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

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

Y60_JSGTK_DECL jsval as_jsval(JSContext *cx, JSWidget::OWNERPTR theOwner, JSWidget::NATIVE * theWidget);

}

#endif


