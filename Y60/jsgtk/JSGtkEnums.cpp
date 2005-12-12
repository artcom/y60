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
//   $RCSfile: JSGtkEnums.cpp,v $
//   $Author: david $
//   $Revision: 1.4 $
//   $Date: 2005/03/03 17:28:53 $
//
//
//=============================================================================

#include "JSGtkEnums.h"
#include "jsgtk.h"
#include <y60/JScppUtils.h>
#include <iostream>
#include <gtkmm/box.h>

using namespace std;
using namespace asl;

namespace jslib {
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


