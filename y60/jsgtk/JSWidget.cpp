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
*/

// own header
#include "JSWidget.h"

#include "JSWindow.h"
#include "JSRenderArea.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>
#include "JSSignalProxies.h"
#include "JSGdkCursor.h"

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Widget, asl::Ptr<Gtk::Widget>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = std::string("GtkWidget@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
grab_focus(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);
        myWidget->grab_focus();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
queue_draw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);
        myWidget->queue_draw();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
set_size_request(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        Gtk::Widget * myWidget=0;
        int theWidth;
        int theHeight;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);
        convertFrom(cx, argv[0], theWidth);
        convertFrom(cx, argv[1], theHeight);
        myWidget->set_size_request(theWidth, theHeight);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
show(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);
        myWidget->show();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
hide(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);
        myWidget->hide();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
show_all(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);
        myWidget->show_all();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
hide_all(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);
        myWidget->hide_all();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
process_updates(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);
        bool update_children;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), update_children);
        myWidget->get_window()->process_updates(update_children);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
set_cursor(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0, 1);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        if (argc == 0 || argv[0] == JSVAL_NULL || argv[0] == JSVAL_VOID) {
            myWidget->get_window()->set_cursor();
        } else {
            Gdk::Cursor * myCursor;
            if ( ! convertFrom(cx, argv[0], myCursor)) {
                JS_ReportError(cx,"JSWidget::set_cursor(): parameter 0 must be a GdkCursor");
            }
            myWidget->get_window()->set_cursor(*myCursor);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
set_manage(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);
        myWidget->set_manage();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
set_events(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        asl::Unsigned32 myEventMask;
        convertFrom(cx, argv[0], myEventMask);
        myWidget->set_events(Gdk::EventMask(myEventMask));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
add_events(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        asl::Unsigned32 myEventMask;
        convertFrom(cx, argv[0], myEventMask);
        myWidget->add_events(Gdk::EventMask(myEventMask));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
modify_fg(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        int myState;
        if ( ! convertFrom(cx, argv[0], myState)) {
             JS_ReportError(cx,"JSWidget::modify_fg(): parameter 0 must be a StateType");
        }
        asl::Vector4f myColor;
        if ( ! convertFrom(cx, argv[1], myColor)) {
             JS_ReportError(cx,"JSWidget::modify_fg(): parameter 1 must be a Vector4f");
        }
        Gdk::Color myGdkColor;
        myGdkColor.set_rgb_p(myColor[0], myColor[1], myColor[2]);
        myWidget->modify_fg(Gtk::StateType(myState), myGdkColor);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
unset_fg(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        int myState;
        if ( ! convertFrom(cx, argv[0], myState)) {
             JS_ReportError(cx,"JSWidget::unset_fg(): parameter 0 must be a StateType");
        }
        myWidget->unset_fg(Gtk::StateType(myState));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
modify_bg(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        int myState;
        if ( ! convertFrom(cx, argv[0], myState)) {
             JS_ReportError(cx,"JSWidget::modify_fg(): parameter 0 must be a StateType");
        }
        asl::Vector4f myColor;
        if ( ! convertFrom(cx, argv[1], myColor)) {
             JS_ReportError(cx,"JSWidget::modify_fg(): parameter 1 must be a Vector4f");
        }
        Gdk::Color myGdkColor;
        myGdkColor.set_rgb_p(myColor[0], myColor[1], myColor[2]);
        myWidget->modify_bg(Gtk::StateType(myState), myGdkColor);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
unset_bg(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        int myState;
        if ( ! convertFrom(cx, argv[0], myState)) {
             JS_ReportError(cx,"JSWidget::unset_bg(): parameter 0 must be a StateType");
        }
        myWidget->unset_bg(Gtk::StateType(myState));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
modify_text(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        int myState;
        if ( ! convertFrom(cx, argv[0], myState)) {
             JS_ReportError(cx,"JSWidget::modify_fg(): parameter 0 must be a StateType");
        }
        asl::Vector4f myColor;
        if ( ! convertFrom(cx, argv[1], myColor)) {
             JS_ReportError(cx,"JSWidget::modify_fg(): parameter 1 must be a Vector4f");
        }
        Gdk::Color myGdkColor;
        myGdkColor.set_rgb_p(myColor[0], myColor[1], myColor[2]);
        myWidget->modify_text(Gtk::StateType(myState), myGdkColor);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
unset_text(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        int myState;
        if ( ! convertFrom(cx, argv[0], myState)) {
             JS_ReportError(cx,"JSWidget::unset_fg(): parameter 0 must be a StateType");
        }
        myWidget->unset_text(Gtk::StateType(myState));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
modify_base(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        int myState;
        if ( ! convertFrom(cx, argv[0], myState)) {
             JS_ReportError(cx,"JSWidget::modify_fg(): parameter 0 must be a StateType");
        }
        asl::Vector4f myColor;
        if ( ! convertFrom(cx, argv[1], myColor)) {
             JS_ReportError(cx,"JSWidget::modify_fg(): parameter 1 must be a Vector4f");
        }
        Gdk::Color myGdkColor;
        myGdkColor.set_rgb_p(myColor[0], myColor[1], myColor[2]);
        myWidget->modify_base(Gtk::StateType(myState), myGdkColor);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
unset_base(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);

        int myState;
        if ( ! convertFrom(cx, argv[0], myState)) {
             JS_ReportError(cx,"JSWidget::unset_fg(): parameter 0 must be a StateType");
        }
        myWidget->unset_base(Gtk::StateType(myState));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

/*
static JSBool
reparent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        Gtk::Widget * myWidget=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myWidget);
        Gtk::Widget * myNewParent=0;
        convertFrom(cx, argv[0], myNewParent);
        myWidget->reparent(*myNewParent);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
*/

JSFunctionSpec *
JSWidget::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"show",                 show,                    0},
        {"hide",                 hide,                    0},
        {"show_all",             show_all,                0},
        {"hide_all",             hide_all,                0},
        {"queue_draw",           queue_draw,              0},
        {"grab_focus",           grab_focus,              0},
        {"process_updates",      process_updates,         1},
        {"set_size_request",     set_size_request,        2},
        {"set_cursor",           set_cursor,              1},
        {"set_manage",           set_manage,              0},
        {"set_events",           set_events,              1},
        {"add_events",           add_events,              1},
        {"modify_fg",            modify_fg,               2},
        {"modify_bg",            modify_bg,               2},
        {"modify_text",          modify_text,             2},
        {"modify_base",          modify_base,             2},
        {"unset_fg",             unset_fg,                1},
        {"unset_bg",             unset_bg,                1},
        {"unset_text",           unset_text,              1},
        {"unset_base",           unset_base,              1},
        {0}
    };
    return myFunctions;
}

#define DEFINE_PROPERTY( NAME ) \
    { #NAME, PROP_ ## NAME,  JSPROP_ENUMERATE|JSPROP_PERMANENT}

#define DEFINE_RO_PROPERTY( NAME ) \
    { #NAME, PROP_ ## NAME,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT}

JSPropertySpec *
JSWidget::Properties() {
    static JSPropertySpec myProperties[] = {
        DEFINE_PROPERTY(name),
        DEFINE_RO_PROPERTY(is_realized),
        DEFINE_RO_PROPERTY(is_visible),
        DEFINE_RO_PROPERTY(is_mapped),
        DEFINE_RO_PROPERTY(focus),
        DEFINE_RO_PROPERTY(parent),
        DEFINE_RO_PROPERTY(width),
        DEFINE_RO_PROPERTY(height),
        DEFINE_PROPERTY(sensitive),
        DEFINE_PROPERTY(state),
        DEFINE_RO_PROPERTY(signal_show),
        DEFINE_RO_PROPERTY(signal_hide),
        DEFINE_RO_PROPERTY(signal_map),
        DEFINE_RO_PROPERTY(signal_unmap),
        DEFINE_RO_PROPERTY(signal_realize),
        DEFINE_RO_PROPERTY(signal_unrealize),
        DEFINE_RO_PROPERTY(signal_button_press_event),
        DEFINE_RO_PROPERTY(signal_button_release_event),
        DEFINE_RO_PROPERTY(signal_motion_notify_event),
        DEFINE_RO_PROPERTY(signal_key_press_event),
        DEFINE_RO_PROPERTY(signal_key_release_event),
        DEFINE_RO_PROPERTY(signal_enter_notify_event),
        DEFINE_RO_PROPERTY(signal_leave_notify_event),
        DEFINE_RO_PROPERTY(signal_focus_in_event),
        DEFINE_RO_PROPERTY(signal_focus_out_event),
        DEFINE_RO_PROPERTY(signal_delete_event),
        DEFINE_PROPERTY(flags),
        {0}
    };
    return myProperties;
}
// getproperty handling
JSBool
JSWidget::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_name:
            *vp = as_jsval(cx, theNative.get_name());
            return JS_TRUE;
        case PROP_is_realized:
            *vp = as_jsval(cx, theNative.is_realized());
            return JS_TRUE;
        case PROP_is_visible:
            *vp = as_jsval(cx, theNative.is_visible());
            return JS_TRUE;
        case PROP_is_mapped:
            *vp = as_jsval(cx, theNative.is_mapped());
            return JS_TRUE;
        case PROP_sensitive:
            *vp = as_jsval(cx, theNative.sensitive());
            return JS_TRUE;
        case PROP_flags:
            *vp = as_jsval(cx, static_cast<int>(theNative.get_flags()));
            return JS_TRUE;
        case PROP_state:
            *vp = as_jsval(cx, static_cast<int>(theNative.get_state()));
            return JS_TRUE;
        case PROP_focus:
            *vp = as_jsval(cx, theNative.has_focus());
            return JS_TRUE;
        case PROP_signal_show:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_show()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_hide:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_hide()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_map:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_map()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_unmap:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_unmap()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_realize:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_realize()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_unrealize:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal( new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_unrealize()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_button_press_event:
            {
                JSSignalProxy1<bool, GdkEventButton*>::OWNERPTR mySignal( new
                        JSSignalProxy1<bool, GdkEventButton*>::NATIVE(theNative.signal_button_press_event()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_button_release_event:
            {
                JSSignalProxy1<bool, GdkEventButton*>::OWNERPTR mySignal( new
                        JSSignalProxy1<bool, GdkEventButton*>::NATIVE(theNative.signal_button_release_event()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_motion_notify_event:
            {
                JSSignalProxy1<bool, GdkEventMotion*>::OWNERPTR mySignal( new
                        JSSignalProxy1<bool, GdkEventMotion*>::NATIVE(theNative.signal_motion_notify_event()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_key_press_event:
            {
                JSSignalProxy1<bool, GdkEventKey*>::OWNERPTR mySignal( new
                        JSSignalProxy1<bool, GdkEventKey*>::NATIVE(theNative.signal_key_press_event()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }

        case PROP_signal_key_release_event:
            {
                JSSignalProxy1<bool, GdkEventKey*>::OWNERPTR mySignal( new
                        JSSignalProxy1<bool, GdkEventKey*>::NATIVE(theNative.signal_key_release_event()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_enter_notify_event:
            {
                JSSignalProxy1<bool, GdkEventCrossing*>::OWNERPTR mySignal( new
                        JSSignalProxy1<bool, GdkEventCrossing*>::NATIVE(theNative.signal_enter_notify_event()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }

        case PROP_signal_leave_notify_event:
            {
                JSSignalProxy1<bool, GdkEventCrossing*>::OWNERPTR mySignal( new
                        JSSignalProxy1<bool, GdkEventCrossing*>::NATIVE(theNative.signal_leave_notify_event()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_focus_in_event:
            {
                JSSignalProxy1<bool, GdkEventFocus*>::OWNERPTR mySignal( new
                        JSSignalProxy1<bool, GdkEventFocus*>::NATIVE(theNative.signal_focus_in_event()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_focus_out_event:
            {
                JSSignalProxy1<bool, GdkEventFocus*>::OWNERPTR mySignal( new
                        JSSignalProxy1<bool, GdkEventFocus*>::NATIVE(theNative.signal_focus_out_event()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_delete_event:
            {
                JSSignalProxy1<bool, GdkEventAny*>::OWNERPTR mySignal( new
                        JSSignalProxy1<bool, GdkEventAny*>::NATIVE(theNative.signal_delete_event()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_parent:
            *vp = as_jsval(cx, theNative.get_parent());
            return JS_TRUE;
        case PROP_width:
            *vp = as_jsval(cx, theNative.get_width());
            return JS_TRUE;
        case PROP_height:
            *vp = as_jsval(cx, theNative.get_height());
            return JS_TRUE;

        default:
            JS_ReportError(cx,"JSWidget::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSWidget::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

// setproperty handling
JSBool
JSWidget::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_name:
            try {
                Glib::ustring myName;
                convertFrom(cx, *vp, myName);
                theNative.set_name(myName);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_sensitive:
            try {
                bool mySensitivityFlag;
                convertFrom(cx, *vp, mySensitivityFlag);
                theNative.set_sensitive(mySensitivityFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_flags:
            try {
                int myFlags;
                convertFrom(cx, *vp, myFlags);
                theNative.set_flags(static_cast<Gtk::WidgetFlags>(myFlags));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_state:
            try {
                int myFlags;
                convertFrom(cx, *vp, myFlags);
                theNative.set_state(static_cast<Gtk::StateType>(myFlags));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            JS_ReportError(cx,"JSWidget::setProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSWidget::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSWidget::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    JS_ReportError(cx,"%s is abstract and can not be instatiated", ClassName() );
    return JS_FALSE;
}

void
JSWidget::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0, 0);
}

JSObject *
JSWidget::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, 0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        /*JSObject * myConstructorFuncObj =*/ JSVAL_TO_OBJECT(myConstructorFuncObjVal);
    } else {
        cerr << "JSWidget::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSWidget::OWNERPTR theOwner, JSWidget::NATIVE * theNative) {
    JSObject * myReturnObject = JSWidget::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

