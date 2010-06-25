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

// own header
#include "JSGtkEnums.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>
#if defined(_MSC_VER)
    // supress warnings caused by gtk in vc++
    #pragma warning(push,1)
    //#pragma warning(disable:4413 4244 4512 4250)
#endif //defined(_MSC_VER)
#include <gtkmm/box.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<bool, asl::Ptr<bool>, StaticAccessProtocol>;

JSBool
JSGtk::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSGtk * myNewObject = 0;

    if (argc == 0) {
        newNative = 0;  // only static props
        myNewObject = new JSGtk(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSGtk::Constructor: bad parameters");
    return JS_FALSE;
}

enum PropertyNumbers {
    // Gtk::AttachOptions
    PROP_EXPAND,
    PROP_SHRINK,
    PROP_FILL,
    // Gtk::PackType (box.h)
    PROP_PACK_SHRINK,
    PROP_PACK_EXPAND_PADDING,
    PROP_PACK_EXPAND_WIDGET,
    // Gtk::ReliefStyle
    PROP_RELIEF_NORMAL,
    PROP_RELIEF_HALF,
    PROP_RELIEF_NONE,
    // Gtk::Justification
    PROP_JUSTIFY_LEFT,
    PROP_JUSTIFY_RIGHT,
    PROP_JUSTIFY_CENTER,
    PROP_JUSTIFY_FILL,
    // Pango::EllipsizeMode
    PROP_ELLIPSIZE_NONE,
    PROP_ELLIPSIZE_START,
    PROP_ELLIPSIZE_MIDDLE,
    PROP_ELLIPSIZE_END,
    // Gtk::PositionType
    PROP_POS_LEFT,
    PROP_POS_RIGHT,
    PROP_POS_TOP,
    PROP_POS_BOTTOM,
    // Gtk::PolicyType
    PROP_POLICY_ALWAYS,
    PROP_POLICY_AUTOMATIC,
    PROP_POLICY_NEVER,
    //Gtk::ButtonBoxStyle
    PROP_BUTTONBOX_DEFAULT_STYLE,
    PROP_BUTTONBOX_SPREAD,
    PROP_BUTTONBOX_EDGE,
    PROP_BUTTONBOX_START,
    PROP_BUTTONBOX_END,
    //Gtk::ButtonBoxStyle
    PROP_TOOLBAR_ICONS,
    PROP_TOOLBAR_TEXT,
    PROP_TOOLBAR_BOTH,
    PROP_TOOLBAR_BOTH_HORIZ,
    // Gtk::ShadowType
    PROP_SHADOW_NONE,
    PROP_SHADOW_IN,
    PROP_SHADOW_OUT,
    PROP_SHADOW_ETCHED_IN,
    PROP_SHADOW_ETCHED_OUT,
    // Gtk::ArrowType
    PROP_ARROW_UP,
    PROP_ARROW_DOWN,
    PROP_ARROW_LEFT,
    PROP_ARROW_RIGHT,
    // Gtk::StateType
    PROP_STATE_NORMAL,
    PROP_STATE_ACTIVE,
    PROP_STATE_PRELIGHT,
    PROP_STATE_SELECTED,
    PROP_STATE_INSENSITIVE,
    // Gtk::WidgetFlags
    PROP_TOPLEVEL,
    PROP_NO_WINDOW,
    PROP_REALIZED,
    PROP_MAPPED,
    PROP_VISIBLE,
    PROP_SENSITIVE,
    PROP_PARENT_SENSITIVE,
    PROP_CAN_FOCUS,
    PROP_HAS_FOCUS,
    PROP_CAN_DEFAULT,
    PROP_HAS_DEFAULT,
    PROP_HAS_GRAB,
    PROP_RC_STYLE,
    PROP_COMPOSITE_CHILD,
    PROP_NO_REPARENT,
    PROP_APP_PAINTABLE,
    PROP_RECEIVES_DEFAULT,
    PROP_DOUBLE_BUFFERED,
    PROP_NO_SHOW_ALL,
    // Gtk::BuiltinIconSize
    PROP_ICON_SIZE_INVALID,
    PROP_ICON_SIZE_MENU,
    PROP_ICON_SIZE_SMALL_TOOLBAR,
    PROP_ICON_SIZE_LARGE_TOOLBAR,
    PROP_ICON_SIZE_BUTTON,
    PROP_ICON_SIZE_DND,
    PROP_ICON_SIZE_DIALOG,
    // Gdk::WindowTypeHint
    PROP_WINDOW_TYPE_HINT_NORMAL,
    PROP_WINDOW_TYPE_HINT_DIALOG,
    PROP_WINDOW_TYPE_HINT_MENU,
    PROP_WINDOW_TYPE_HINT_TOOLBAR,
    PROP_WINDOW_TYPE_HINT_SPLASHSCREEN,
    PROP_WINDOW_TYPE_HINT_UTILITY,
    PROP_WINDOW_TYPE_HINT_DOCK,
    PROP_WINDOW_TYPE_HINT_DESKTOP,
    // Gdk::EventMask
    PROP_EXPOSURE_MASK,
    PROP_POINTER_MOTION_MASK,
    PROP_POINTER_MOTION_HINT_MASK,
    PROP_BUTTON_MOTION_MASK,
    PROP_BUTTON1_MOTION_MASK,
    PROP_BUTTON2_MOTION_MASK,
    PROP_BUTTON3_MOTION_MASK,
    PROP_BUTTON_PRESS_MASK,
    PROP_BUTTON_RELEASE_MASK,
    PROP_KEY_PRESS_MASK,
    PROP_KEY_RELEASE_MASK,
    PROP_ENTER_NOTIFY_MASK,
    PROP_LEAVE_NOTIFY_MASK,
    PROP_FOCUS_CHANGE_MASK,
    PROP_STRUCTURE_MASK,
    PROP_PROPERTY_CHANGE_MASK,
    PROP_VISIBILITY_NOTIFY_MASK,
    PROP_PROXIMITY_IN_MASK,
    PROP_PROXIMITY_OUT_MASK,
    PROP_SUBSTRUCTURE_MASK,
    PROP_SCROLL_MASK,
    PROP_ALL_EVENTS_MASK,
    // === PROP_END =====
    PROP_END
};

#define DEFINE_GTK_PROP( NAME ) { #NAME, PROP_ ## NAME, Gtk::NAME}
#define DEFINE_GDK_PROP( NAME ) { #NAME, PROP_ ## NAME, Gdk::NAME}
#define DEFINE_PANGO_PROP( NAME ) { #NAME, PROP_ ## NAME, Pango::NAME}

JSConstIntPropertySpec *
JSGtk::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_GTK_PROP(EXPAND),
        DEFINE_GTK_PROP(SHRINK),
        DEFINE_GTK_PROP(FILL),

        DEFINE_GTK_PROP(PACK_EXPAND_PADDING),
        DEFINE_GTK_PROP(PACK_EXPAND_WIDGET),
        DEFINE_GTK_PROP(PACK_SHRINK),

        DEFINE_GTK_PROP(RELIEF_NORMAL),
        DEFINE_GTK_PROP(RELIEF_HALF),
        DEFINE_GTK_PROP(RELIEF_NONE),

        DEFINE_GTK_PROP(JUSTIFY_LEFT),
        DEFINE_GTK_PROP(JUSTIFY_RIGHT),
        DEFINE_GTK_PROP(JUSTIFY_CENTER),
        DEFINE_GTK_PROP(JUSTIFY_FILL),
        /*
        DEFINE_PANGO_PROP(ELLIPSIZE_NONE),
        DEFINE_PANGO_PROP(ELLIPSIZE_START),
        DEFINE_PANGO_PROP(ELLIPSIZE_MIDDLE),
        DEFINE_PANGO_PROP(ELLIPSIZE_END),
       */
        DEFINE_GTK_PROP(POS_LEFT),
        DEFINE_GTK_PROP(POS_RIGHT),
        DEFINE_GTK_PROP(POS_TOP),
        DEFINE_GTK_PROP(POS_BOTTOM),

        DEFINE_GTK_PROP(POLICY_ALWAYS),
        DEFINE_GTK_PROP(POLICY_AUTOMATIC),
        DEFINE_GTK_PROP(POLICY_NEVER),

        DEFINE_GTK_PROP(BUTTONBOX_DEFAULT_STYLE),
        DEFINE_GTK_PROP(BUTTONBOX_SPREAD),
        DEFINE_GTK_PROP(BUTTONBOX_EDGE),
        DEFINE_GTK_PROP(BUTTONBOX_START),
        DEFINE_GTK_PROP(BUTTONBOX_END),

        DEFINE_GTK_PROP(TOOLBAR_ICONS),
        DEFINE_GTK_PROP(TOOLBAR_TEXT),
        DEFINE_GTK_PROP(TOOLBAR_BOTH),
        DEFINE_GTK_PROP(TOOLBAR_BOTH_HORIZ),

        DEFINE_GTK_PROP(SHADOW_NONE),
        DEFINE_GTK_PROP(SHADOW_IN),
        DEFINE_GTK_PROP(SHADOW_OUT),
        DEFINE_GTK_PROP(SHADOW_ETCHED_IN),
        DEFINE_GTK_PROP(SHADOW_ETCHED_OUT),

        DEFINE_GTK_PROP(ARROW_UP),
        DEFINE_GTK_PROP(ARROW_DOWN),
        DEFINE_GTK_PROP(ARROW_LEFT),
        DEFINE_GTK_PROP(ARROW_RIGHT),

        DEFINE_GTK_PROP(STATE_NORMAL),
        DEFINE_GTK_PROP(STATE_ACTIVE),
        DEFINE_GTK_PROP(STATE_PRELIGHT),
        DEFINE_GTK_PROP(STATE_SELECTED),
        DEFINE_GTK_PROP(STATE_INSENSITIVE),

        DEFINE_GTK_PROP(TOPLEVEL),
        DEFINE_GTK_PROP(NO_WINDOW),
        DEFINE_GTK_PROP(REALIZED),
        DEFINE_GTK_PROP(MAPPED),
        DEFINE_GTK_PROP(VISIBLE),
        DEFINE_GTK_PROP(SENSITIVE),
        DEFINE_GTK_PROP(PARENT_SENSITIVE),
        DEFINE_GTK_PROP(CAN_FOCUS),
        DEFINE_GTK_PROP(HAS_FOCUS),
        DEFINE_GTK_PROP(CAN_DEFAULT),
        DEFINE_GTK_PROP(HAS_DEFAULT),
        DEFINE_GTK_PROP(HAS_GRAB),
        DEFINE_GTK_PROP(RC_STYLE),
        DEFINE_GTK_PROP(COMPOSITE_CHILD),
        DEFINE_GTK_PROP(NO_REPARENT),
        DEFINE_GTK_PROP(APP_PAINTABLE),
        DEFINE_GTK_PROP(RECEIVES_DEFAULT),
        DEFINE_GTK_PROP(DOUBLE_BUFFERED),
        DEFINE_GTK_PROP(NO_SHOW_ALL),

        DEFINE_GTK_PROP(ICON_SIZE_INVALID),
        DEFINE_GTK_PROP(ICON_SIZE_MENU),
        DEFINE_GTK_PROP(ICON_SIZE_SMALL_TOOLBAR),
        DEFINE_GTK_PROP(ICON_SIZE_LARGE_TOOLBAR),
        DEFINE_GTK_PROP(ICON_SIZE_BUTTON),
        DEFINE_GTK_PROP(ICON_SIZE_DND),
        DEFINE_GTK_PROP(ICON_SIZE_DIALOG),

        DEFINE_GDK_PROP(WINDOW_TYPE_HINT_NORMAL),
        DEFINE_GDK_PROP(WINDOW_TYPE_HINT_DIALOG),
        DEFINE_GDK_PROP(WINDOW_TYPE_HINT_MENU),
        DEFINE_GDK_PROP(WINDOW_TYPE_HINT_TOOLBAR),
        DEFINE_GDK_PROP(WINDOW_TYPE_HINT_SPLASHSCREEN),
        DEFINE_GDK_PROP(WINDOW_TYPE_HINT_UTILITY),
        DEFINE_GDK_PROP(WINDOW_TYPE_HINT_DOCK),
        DEFINE_GDK_PROP(WINDOW_TYPE_HINT_DESKTOP),

        DEFINE_GDK_PROP(EXPOSURE_MASK),
        DEFINE_GDK_PROP(POINTER_MOTION_MASK),
        DEFINE_GDK_PROP(POINTER_MOTION_HINT_MASK),
        DEFINE_GDK_PROP(BUTTON_MOTION_MASK),
        DEFINE_GDK_PROP(BUTTON1_MOTION_MASK),
        DEFINE_GDK_PROP(BUTTON2_MOTION_MASK),
        DEFINE_GDK_PROP(BUTTON3_MOTION_MASK),
        DEFINE_GDK_PROP(BUTTON_PRESS_MASK),
        DEFINE_GDK_PROP(BUTTON_RELEASE_MASK),
        DEFINE_GDK_PROP(KEY_PRESS_MASK),
        DEFINE_GDK_PROP(KEY_RELEASE_MASK),
        DEFINE_GDK_PROP(ENTER_NOTIFY_MASK),
        DEFINE_GDK_PROP(LEAVE_NOTIFY_MASK),
        DEFINE_GDK_PROP(FOCUS_CHANGE_MASK),
        DEFINE_GDK_PROP(STRUCTURE_MASK),
        DEFINE_GDK_PROP(PROPERTY_CHANGE_MASK),
        DEFINE_GDK_PROP(VISIBILITY_NOTIFY_MASK),
        DEFINE_GDK_PROP(PROXIMITY_IN_MASK),
        DEFINE_GDK_PROP(PROXIMITY_OUT_MASK),
        DEFINE_GDK_PROP(SUBSTRUCTURE_MASK),
        DEFINE_GDK_PROP(SCROLL_MASK),
        DEFINE_GDK_PROP(ALL_EVENTS_MASK),

        {0}
    };
    return myProperties;
};

JSObject *
JSGtk::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSGtk::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    createClassModuleDocumentation("gtk", ClassName(), 0, 0, ConstIntProperties(), 0, 0);
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSGtk::OWNERPTR theOwner, JSGtk::NATIVE * theNative) {
    JSObject * myReturnObject = JSGtk::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}


