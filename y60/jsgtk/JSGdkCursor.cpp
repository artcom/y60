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
#include "JSGdkCursor.h"

#include "jsgtk.h"
#include <y60/acgtk/CustomCursors.h>
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>

#include <iostream>
#if defined(_MSC_VER)
    // supress warnings caused by gtk in vc++
    #pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gdk/gdkkeysyms.h>
#include <gtkmm/window.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gdk::Cursor, asl::Ptr<Gdk::Cursor>, StaticAccessProtocol>;

static JSBool
initCustomCursors(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    ensureParamCount(argc, 0);
    acgtk::CustomCursors::init();
    return JS_TRUE;
}

JSFunctionSpec *
JSGdkCursor::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        /* name         native          nargs    */
        {"initCustomCursors", initCustomCursors, 0},
        {0}
    };
    return myFunctions;
};

JSBool
JSGdkCursor::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSGdkCursor * myNewObject = 0;

    if(argc == 0) {
        newNative = new Gdk::Cursor;
        myNewObject = new JSGdkCursor(OWNERPTR(), newNative);
    } else if (argc == 1) {
        int myCursorId;
        if ( convertFrom(cx, argv[0], myCursorId)) {
            newNative = new Gdk::Cursor(static_cast<Gdk::CursorType>(myCursorId));
        } else {
            JS_ReportError(cx,"Constructor for %s: expected an int cursor constant, like 'GdkCursor.CROSSHAIR' ()",ClassName());
            return JS_FALSE;
        }
        myNewObject = new JSGdkCursor(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected one () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSGdkCursor::Constructor: bad parameters");
    return JS_FALSE;
}

// checkout http://www-eleves-isia.cma.fr/documentation/GtkDoc/gdk/gdk-cursors.html
// to get an overview
enum PropertyNumbers {
    PROP_X_CURSOR,
    PROP_ARROW,
    PROP_BASED_ARROW_DOWN,
    PROP_BASED_ARROW_UP,
    PROP_BOAT,
    PROP_BOGOSITY,
    PROP_BOTTOM_LEFT_CORNER,
    PROP_BOTTOM_RIGHT_CORNER,
    PROP_BOTTOM_SIDE,
    PROP_BOTTOM_TEE,
    PROP_BOX_SPIRAL,
    PROP_CENTER_PTR,
    PROP_CIRCLE,
    PROP_CLOCK,
    PROP_COFFEE_MUG,
    PROP_CROSS,
    PROP_CROSS_REVERSE,
    PROP_CROSSHAIR,
    PROP_DIAMOND_CROSS,
    PROP_DOT,
    PROP_DOTBOX,
    PROP_DOUBLE_ARROW,
    PROP_DRAFT_LARGE,
    PROP_DRAFT_SMALL,
    PROP_DRAPED_BOX,
    PROP_EXCHANGE,
    PROP_FLEUR,
    PROP_GOBBLER,
    PROP_GUMBY,
    PROP_HAND1,
    PROP_HAND2,
    PROP_HEART,
    PROP_ICON,
    PROP_IRON_CROSS,
    PROP_LEFT_PTR,
    PROP_LEFT_SIDE,
    PROP_LEFT_TEE,
    PROP_LEFTBUTTON,
    PROP_LL_ANGLE,
    PROP_LR_ANGLE,
    PROP_MAN,
    PROP_MIDDLEBUTTON,
    PROP_MOUSE,
    PROP_PENCIL,
    PROP_PIRATE,
    PROP_PLUS,
    PROP_QUESTION_ARROW,
    PROP_RIGHT_PTR,
    PROP_RIGHT_SIDE,
    PROP_RIGHT_TEE,
    PROP_RIGHTBUTTON,
    PROP_RTL_LOGO,
    PROP_SAILBOAT,
    PROP_SB_DOWN_ARROW,
    PROP_SB_H_DOUBLE_ARROW,
    PROP_SB_LEFT_ARROW,
    PROP_SB_RIGHT_ARROW,
    PROP_SB_UP_ARROW,
    PROP_SB_V_DOUBLE_ARROW,
    PROP_SHUTTLE,
    PROP_SIZING,
    PROP_SPIDER,
    PROP_SPRAYCAN,
    PROP_STAR,
    PROP_TARGET,
    PROP_TCROSS,
    PROP_TOP_LEFT_ARROW,
    PROP_TOP_LEFT_CORNER,
    PROP_TOP_RIGHT_CORNER,
    PROP_TOP_SIDE,
    PROP_TOP_TEE,
    PROP_TREK,
    PROP_UL_ANGLE,
    PROP_UMBRELLA,
    PROP_UR_ANGLE,
    PROP_WATCH,
    PROP_XTERM
};

enum StaticPropertyNumbers {
    PROP_AC_ADD_POINT,
    PROP_AC_EDIT_ANGLE,
    PROP_AC_EDIT_ANGLE1,
    PROP_AC_EDIT_ANGLE2,
    PROP_AC_EDIT_ANGLE3
};

JSPropertySpec *
JSGdkCursor::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

#define DEFINE_GDK_PROP( NAME ) { #NAME, PROP_ ## NAME, Gdk::NAME}

// checkout http://www-eleves-isia.cma.fr/documentation/GtkDoc/gdk/gdk-cursors.html
// to get an overview
JSConstIntPropertySpec *
JSGdkCursor::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        // types
        DEFINE_GDK_PROP(X_CURSOR),
        DEFINE_GDK_PROP(ARROW),
        DEFINE_GDK_PROP(BASED_ARROW_DOWN),
        DEFINE_GDK_PROP(BASED_ARROW_UP),
        DEFINE_GDK_PROP(BOAT),
        DEFINE_GDK_PROP(BOGOSITY),
        DEFINE_GDK_PROP(BOTTOM_LEFT_CORNER),
        DEFINE_GDK_PROP(BOTTOM_RIGHT_CORNER),
        DEFINE_GDK_PROP(BOTTOM_SIDE),
        DEFINE_GDK_PROP(BOTTOM_TEE),
        DEFINE_GDK_PROP(BOX_SPIRAL),
        DEFINE_GDK_PROP(CENTER_PTR),
        DEFINE_GDK_PROP(CIRCLE),
        DEFINE_GDK_PROP(CLOCK),
        DEFINE_GDK_PROP(COFFEE_MUG),
        DEFINE_GDK_PROP(CROSS),
        DEFINE_GDK_PROP(CROSS_REVERSE),
        DEFINE_GDK_PROP(CROSSHAIR),
        DEFINE_GDK_PROP(DIAMOND_CROSS),
        DEFINE_GDK_PROP(DOT),
        DEFINE_GDK_PROP(DOTBOX),
        DEFINE_GDK_PROP(DOUBLE_ARROW),
        DEFINE_GDK_PROP(DRAFT_LARGE),
        DEFINE_GDK_PROP(DRAFT_SMALL),
        DEFINE_GDK_PROP(DRAPED_BOX),
        DEFINE_GDK_PROP(EXCHANGE),
        DEFINE_GDK_PROP(FLEUR),
        DEFINE_GDK_PROP(GOBBLER),
        DEFINE_GDK_PROP(GUMBY),
        DEFINE_GDK_PROP(HAND1),
        DEFINE_GDK_PROP(HAND2),
        DEFINE_GDK_PROP(HEART),
        DEFINE_GDK_PROP(ICON),
        DEFINE_GDK_PROP(IRON_CROSS),
        DEFINE_GDK_PROP(LEFT_PTR),
        DEFINE_GDK_PROP(LEFT_SIDE),
        DEFINE_GDK_PROP(LEFT_TEE),
        DEFINE_GDK_PROP(LEFTBUTTON),
        DEFINE_GDK_PROP(LL_ANGLE),
        DEFINE_GDK_PROP(LR_ANGLE),
        DEFINE_GDK_PROP(MAN),
        DEFINE_GDK_PROP(MIDDLEBUTTON),
        DEFINE_GDK_PROP(MOUSE),
        DEFINE_GDK_PROP(PENCIL),
        DEFINE_GDK_PROP(PIRATE),
        DEFINE_GDK_PROP(PLUS),
        DEFINE_GDK_PROP(QUESTION_ARROW),
        DEFINE_GDK_PROP(RIGHT_PTR),
        DEFINE_GDK_PROP(RIGHT_SIDE),
        DEFINE_GDK_PROP(RIGHT_TEE),
        DEFINE_GDK_PROP(RIGHTBUTTON),
        DEFINE_GDK_PROP(RTL_LOGO),
        DEFINE_GDK_PROP(SAILBOAT),
        DEFINE_GDK_PROP(SB_DOWN_ARROW),
        DEFINE_GDK_PROP(SB_H_DOUBLE_ARROW),
        DEFINE_GDK_PROP(SB_LEFT_ARROW),
        DEFINE_GDK_PROP(SB_RIGHT_ARROW),
        DEFINE_GDK_PROP(SB_UP_ARROW),
        DEFINE_GDK_PROP(SB_V_DOUBLE_ARROW),
        DEFINE_GDK_PROP(SHUTTLE),
        DEFINE_GDK_PROP(SIZING),
        DEFINE_GDK_PROP(SPIDER),
        DEFINE_GDK_PROP(SPRAYCAN),
        DEFINE_GDK_PROP(STAR),
        DEFINE_GDK_PROP(TARGET),
        DEFINE_GDK_PROP(TCROSS),
        DEFINE_GDK_PROP(TOP_LEFT_ARROW),
        DEFINE_GDK_PROP(TOP_LEFT_CORNER),
        DEFINE_GDK_PROP(TOP_RIGHT_CORNER),
        DEFINE_GDK_PROP(TOP_SIDE),
        DEFINE_GDK_PROP(TOP_TEE),
        DEFINE_GDK_PROP(TREK),
        DEFINE_GDK_PROP(UL_ANGLE),
        DEFINE_GDK_PROP(UMBRELLA),
        DEFINE_GDK_PROP(UR_ANGLE),
        DEFINE_GDK_PROP(WATCH),
        DEFINE_GDK_PROP(XTERM)
    };
    return myProperties;
};

#define DISPATCH_CURSOR( theName ) \
    case PROP_ ## theName : \
        *vp = as_jsval(cx, JSGdkCursor::OWNERPTR(), & acgtk::CustomCursors::theName); \
        return JS_TRUE;


static JSBool
getStaticProperty(JSContext *cx, JSObject * obj, jsval id, jsval * vp) {
    int myID = JSVAL_TO_INT(id);
    switch (myID) {
        DISPATCH_CURSOR( AC_ADD_POINT );
        DISPATCH_CURSOR( AC_EDIT_ANGLE );
        DISPATCH_CURSOR( AC_EDIT_ANGLE1 );
        DISPATCH_CURSOR( AC_EDIT_ANGLE2 );
        DISPATCH_CURSOR( AC_EDIT_ANGLE3 );
        default:
            JS_ReportError(cx,"JSGdkCursor::getStaticProperty: index %d out of range", myID);
    }
    return JS_FALSE;
}

static JSBool
setStaticProperty(JSContext *cx, JSObject * obj, jsval id, jsval * vp) {
    int myID = JSVAL_TO_INT(id);
    switch (myID) {
        case 0:
        default:
            JS_ReportError(cx,"JSGdkCursor::setStaticProperty: index %d out of range", myID);
    }
    return JS_FALSE;
}
#define DEFINE_STATIC_PROP(theName) \
    {#theName, PROP_ ## theName, JSPROP_READONLY | JSPROP_ENUMERATE | JSPROP_PERMANENT, \
            getStaticProperty, setStaticProperty}

JSPropertySpec *
JSGdkCursor::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        DEFINE_STATIC_PROP(AC_ADD_POINT),
        DEFINE_STATIC_PROP(AC_EDIT_ANGLE),
        DEFINE_STATIC_PROP(AC_EDIT_ANGLE1),
        DEFINE_STATIC_PROP(AC_EDIT_ANGLE2),
        DEFINE_STATIC_PROP(AC_EDIT_ANGLE3),
        {0}
    };
    return myProperties;
}


// getproperty handling
JSBool
JSGdkCursor::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    // common properties:
    switch (theID) {
        case 0:
            return JS_FALSE;
    }
    JS_ReportError(cx,"JSGdkCursor::getProperty: index %d out of range", theID);
    return JS_FALSE;
}

JSObject *
JSGdkCursor::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
                                               Properties(), 0,
                                               0, StaticProperties(), StaticFunctions());
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSGdkCursor::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    createClassModuleDocumentation("gtk", ClassName(), Properties(), 0, ConstIntProperties(), 0, 0, 0);
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSGdkCursor::OWNERPTR theOwner, JSGdkCursor::NATIVE * theNative) {
    JSObject * myReturnObject = JSGdkCursor::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, JSGdkCursor::NATIVE * & theCursor) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {

            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSGdkCursor::NATIVE >::Class()) {
                typedef JSClassTraits<JSGdkCursor::NATIVE>::ScopedNativeRef NativeRef;
                NativeRef myObj(cx, myArgument);
                theCursor = & myObj.getNative();

                return true;
            }
        }
    }
    return false;
}


}
