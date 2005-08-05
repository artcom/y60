//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSLabel.cpp,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2005/04/21 16:25:02 $
//
//
//=============================================================================

#include "JSLabel.h"
#include "jsgtk.h"
#include <y60/JScppUtils.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {


static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Label@") + as_string(obj);
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}
JSFunctionSpec *
JSLabel::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

#define DEFINE_PROPERTY( NAME ) \
    { #NAME, PROP_ ## NAME,  JSPROP_ENUMERATE|JSPROP_PERMANENT}

JSPropertySpec *
JSLabel::Properties() {
    static JSPropertySpec myProperties[] = {
        {"text", PROP_text, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"label", PROP_label, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        DEFINE_PROPERTY( use_markup ),
        DEFINE_PROPERTY( use_underline ),
        DEFINE_PROPERTY( justify ),
        DEFINE_PROPERTY( ellipsize ),
        DEFINE_PROPERTY( width_chars ),
        DEFINE_PROPERTY( max_width_chars ),
        DEFINE_PROPERTY( line_wrap ),
        DEFINE_PROPERTY( selectable ),
        DEFINE_PROPERTY( angle ),
        DEFINE_PROPERTY( single_line_mode ),
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSLabel::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSLabel::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSLabel::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_text:
            *vp = as_jsval(cx, theNative.get_text());
            return JS_TRUE;
        case PROP_label:
            *vp = as_jsval(cx, theNative.get_label());
            return JS_TRUE;
        case PROP_use_markup:
            *vp = as_jsval(cx, theNative.get_use_markup());
            return JS_TRUE;
        case PROP_use_underline:
            *vp = as_jsval(cx, theNative.get_use_underline());
            return JS_TRUE;
        case PROP_justify:
            *vp = as_jsval(cx, static_cast<int>(theNative.get_justify()));
            return JS_TRUE;
        /* missing in current gtkmm
        case PROP_ellipsize:
            *vp = as_jsval(cx, theNative.get_ellipsize());
            return JS_TRUE;
        case PROP_width_chars:
            *vp = as_jsval(cx, theNative.get_width_chars());
            return JS_TRUE;
        case PROP_max_width_chars:
            *vp = as_jsval(cx, theNative.get_max_width_chars());
            return JS_TRUE;
        */
        case PROP_line_wrap:
            *vp = as_jsval(cx, theNative.get_line_wrap());
            return JS_TRUE;
        case PROP_selectable:
            *vp = as_jsval(cx, theNative.get_selectable());
            return JS_TRUE;
        /* missing in current gtkmm
        case PROP_angle:
            *vp = as_jsval(cx, theNative.get_angle());
            return JS_TRUE;
        case PROP_single_line_mode:
            *vp = as_jsval(cx, theNative.get_single_line_mode());
            return JS_TRUE;
        */
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSLabel::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_text:
            try {
                Glib::ustring myText;
                convertFrom(cx, *vp, myText);
                theNative.set_text(myText);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_label:
            try {
                Glib::ustring myLabel;
                convertFrom(cx, *vp, myLabel);
                theNative.set_label(myLabel);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_use_markup:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.set_use_markup(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_use_underline:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.set_use_underline(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_justify:
            try {
                int myJustificationMode;
                convertFrom(cx, *vp, myJustificationMode);
                theNative.set_justify(static_cast<Gtk::Justification>(myJustificationMode));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        /* missing in current gtkmm
        case PROP_ellipsize:
            try {
                int myEllipsizeMode;
                convertFrom(cx, *vp, myEllipsizeMode);
                theNative.set_justify(static_cast<Pango::EllipsizeMode>(myEllipsizeMode));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_width_chars:
            try {
                int myWidthChars;
                convertFrom(cx, *vp, myWidthChars);
                theNative.set_width_chars(myWidthChars);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_max_width_chars:
            try {
                int myWidthChars;
                convertFrom(cx, *vp, myWidthChars);
                theNative.set_max_width_chars(myWidthChars);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        */
        case PROP_line_wrap:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.set_line_wrap(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_selectable:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.set_selectable(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        /* missing in current gtkmm
        case PROP_angle:
            try {
                double myAngle;
                convertFrom(cx, *vp, myAngle);
                theNative.set_angle(myAngle);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_single_line_mode:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                theNative.set_single_line_mode(myFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        */
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSLabel::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSLabel * myNewObject = 0;

    switch (argc) {
    case 0:
        newNative = new NATIVE;
        break;
    case 1:
        { 

            Glib::ustring myString;
            if ( ! convertFrom(cx, argv[0], myString)) {
                JS_ReportError(cx,"Constructor for %s: argument 0 must be a string", ClassName());
                return JS_FALSE;
            }
            newNative = new NATIVE(myString);
        }
        break;
    default:
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }
    myNewObject = new JSLabel(OWNERPTR(newNative), newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSLabel::Constructor: bad parameters");
    return JS_FALSE;
}

void
JSLabel::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
                JSBASE::ClassName());
}

JSObject *
JSLabel::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSLabel::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSLabel::OWNERPTR theOwner, JSLabel::NATIVE * theNative) {
    JSObject * myReturnObject = JSLabel::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}
