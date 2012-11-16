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
#include "JSGdkEvent.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>
#include <gdk/gdkkeysyms.h>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<GdkEvent, asl::Ptr<GdkEvent>, StaticAccessProtocol>;

JSBool
JSGdkEvent::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSGdkEvent * myNewObject = 0;

    if (argc == 0) {
        newNative = new GdkEvent();
        myNewObject = new JSGdkEvent(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSGdkEvent::Constructor: bad parameters");
    return JS_FALSE;
}

enum PropertyNumbers {
    // Gtk::AttachOptions
    PROP_type,
    PROP_x,
    PROP_y,
    PROP_state,
    PROP_button,
    PROP_keyval,
    // Gdk Event types
    PROP_GDK_BUTTON_PRESS,
    PROP_GDK_2BUTTON_PRESS,
    PROP_GDK_3BUTTON_PRESS,
    PROP_GDK_BUTTON_RELEASE,
    PROP_GDK_KEY_PRESS,
    PROP_GDK_KEY_RELEASE,
    PROP_GDK_ENTER_NOTIFY,
    PROP_GDK_LEAVE_NOTIFY,
    // Gdk state masks
    PROP_GDK_SHIFT_MASK,
    PROP_GDK_LOCK_MASK,
    PROP_GDK_CONTROL_MASK,
    PROP_GDK_MOD1_MASK,
    PROP_GDK_MOD2_MASK,
    PROP_GDK_MOD3_MASK,
    PROP_GDK_MOD4_MASK,
    PROP_GDK_MOD5_MASK,
    PROP_GDK_BUTTON1_MASK,
    PROP_GDK_BUTTON2_MASK,
    PROP_GDK_BUTTON3_MASK,
    PROP_GDK_BUTTON4_MASK,
    PROP_GDK_BUTTON5_MASK,
    PROP_GDK_RELEASE_MASK,
    PROP_GDK_MODIFIER_MASK,
    PROP_GDK_ENTER_NOTIFY_MASK,
    PROP_GDK_LEAVE_NOTIFY_MASK,

    // keysyms (generated using generate-keysyms.sh)
#   include "JSGdkKeysymPropertyNumbers.txt"

    PROP_END
};

JSPropertySpec *
JSGdkEvent::Properties() {
    static JSPropertySpec myProperties[] = {
        {"type",    PROP_type, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"x",       PROP_x, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"y",       PROP_y, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"state",   PROP_state, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"button",  PROP_button, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"keyval",  PROP_keyval, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

#define DEFINE_GDK_PROP( NAME ) { #NAME, PROP_ ## NAME, NAME}

JSConstIntPropertySpec *
JSGdkEvent::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        // types
        DEFINE_GDK_PROP(GDK_BUTTON_PRESS),
        DEFINE_GDK_PROP(GDK_2BUTTON_PRESS),
        DEFINE_GDK_PROP(GDK_3BUTTON_PRESS),
        DEFINE_GDK_PROP(GDK_BUTTON_RELEASE),
        DEFINE_GDK_PROP(GDK_KEY_PRESS),
        DEFINE_GDK_PROP(GDK_KEY_RELEASE),
        DEFINE_GDK_PROP(GDK_ENTER_NOTIFY),
        DEFINE_GDK_PROP(GDK_LEAVE_NOTIFY),
        // state mask
        DEFINE_GDK_PROP(GDK_SHIFT_MASK),
        DEFINE_GDK_PROP(GDK_LOCK_MASK),
        DEFINE_GDK_PROP(GDK_CONTROL_MASK),
        DEFINE_GDK_PROP(GDK_MOD1_MASK),
        DEFINE_GDK_PROP(GDK_MOD2_MASK),
        DEFINE_GDK_PROP(GDK_MOD3_MASK),
        DEFINE_GDK_PROP(GDK_MOD4_MASK),
        DEFINE_GDK_PROP(GDK_MOD5_MASK),
        DEFINE_GDK_PROP(GDK_BUTTON1_MASK),
        DEFINE_GDK_PROP(GDK_BUTTON2_MASK),
        DEFINE_GDK_PROP(GDK_BUTTON3_MASK),
        DEFINE_GDK_PROP(GDK_BUTTON4_MASK),
        DEFINE_GDK_PROP(GDK_BUTTON5_MASK),
        DEFINE_GDK_PROP(GDK_RELEASE_MASK),
        DEFINE_GDK_PROP(GDK_MODIFIER_MASK),
        DEFINE_GDK_PROP(GDK_ENTER_NOTIFY_MASK),
        DEFINE_GDK_PROP(GDK_LEAVE_NOTIFY_MASK),

        // keysyms (generated using generate-keysyms.sh)
#       include "JSGdkKeysymPropertyDefinitions.txt"

        {0}
    };
    return myProperties;
};


// getproperty handling
JSBool
JSGdkEvent::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    // common properties:
    switch (theID) {
        case PROP_type:
            *vp = as_jsval(cx, getNative().type);
            return JS_TRUE;
    }
    // type dependent properties
    switch (getNative().type) {
        case GDK_BUTTON_PRESS:
        case GDK_2BUTTON_PRESS:
        case GDK_3BUTTON_PRESS:
        case GDK_BUTTON_RELEASE:
            {
                const GdkEventButton * myEvent = reinterpret_cast<const GdkEventButton*>(&getNative());
                switch (theID) {
                    case PROP_x:
                        *vp = as_jsval(cx, myEvent->x);
                        return JS_TRUE;
                    case PROP_y:
                        *vp = as_jsval(cx, myEvent->y);
                        return JS_TRUE;
                    case PROP_state:
                        *vp = as_jsval(cx, myEvent->state);
                        return JS_TRUE;
                    case PROP_button:
                        *vp = as_jsval(cx, myEvent->button);
                        return JS_TRUE;
                }
            }
            break;
        case GDK_MOTION_NOTIFY:
            {
                const GdkEventMotion * myEvent = reinterpret_cast<const GdkEventMotion*>(&getNative());
                switch (theID) {
                    case PROP_x:
                        *vp = as_jsval(cx, myEvent->x);
                        return JS_TRUE;
                    case PROP_y:
                        *vp = as_jsval(cx, myEvent->y);
                        return JS_TRUE;
                    case PROP_state:
                        *vp = as_jsval(cx, myEvent->state);
                        return JS_TRUE;
                }
            }
            break;
        case GDK_KEY_PRESS:
        case GDK_KEY_RELEASE:
            {
                const GdkEventKey * myEvent = reinterpret_cast<const GdkEventKey*>(&getNative());
                switch (theID) {
                    case PROP_keyval:
                        *vp = as_jsval(cx, int( myEvent->keyval));
                        return JS_TRUE;
                    case PROP_state:
                        *vp = as_jsval(cx, myEvent->state);
                        return JS_TRUE;
                }
            }
        default:
            break;
    }
    JS_ReportError(cx,"JSGdkEvent::getProperty: index %d out of range", theID);
    return JS_FALSE;
}

JSObject *
JSGdkEvent::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties() ,0);
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSGdkEvent::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    createClassModuleDocumentation("gtk", ClassName(), Properties(), 0, ConstIntProperties(), 0, 0, 0);
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSGdkEvent::OWNERPTR theOwner, JSGdkEvent::NATIVE * theNative) {
    JSObject * myReturnObject = JSGdkEvent::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, GdkEvent * theNative) {
    JSObject * myReturnObject = JSGdkEvent::Construct(cx, JSGdkEvent::OWNERPTR(), theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, GdkEventButton * theNative) {
    return as_jsval(cx, reinterpret_cast<GdkEvent*>(theNative));
}

jsval as_jsval(JSContext *cx, GdkEventMotion * theNative) {
    return as_jsval(cx, reinterpret_cast<GdkEvent*>(theNative));
}

jsval as_jsval(JSContext *cx, GdkEventKey * theNative) {
    return as_jsval(cx, reinterpret_cast<GdkEvent*>(theNative));
}

jsval as_jsval(JSContext *cx, GdkEventCrossing * theNative) {
    return as_jsval(cx, reinterpret_cast<GdkEvent*>(theNative));
}

jsval as_jsval(JSContext *cx, GdkEventAny * theNative) {
    return as_jsval(cx, reinterpret_cast<GdkEvent*>(theNative));
}

jsval as_jsval(JSContext *cx, GdkEventFocus * theNative) {
    return as_jsval(cx, reinterpret_cast<GdkEvent*>(theNative));
}

}
