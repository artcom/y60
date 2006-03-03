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
//   $RCSfile: JSDragButton.cpp,v $
//   $Author: martin $
//   $Revision: 1.11 $
//   $Date: 2005/04/21 16:25:02 $
//
//
//=============================================================================

#include "JSDragButton.h"
#include "JSSignal0.h"
#include "JSSignal2.h"
#include "JSSignal3.h"
#include "jsgtk.h"
#include <y60/JScppUtils.h>
#include <y60/JSAbstractRenderWindow.h>
#include <iostream>

using namespace std;
using namespace asl;
using namespace acgtk;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = "DragButton";
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

JSFunctionSpec *
JSDragButton::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSDragButton::Properties() {
    static JSPropertySpec myProperties[] = {
//        {"URL", PROP_URL, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_drag_start", PROP_signal_drag_start, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_drag", PROP_signal_drag, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_drag_done", PROP_signal_drag_done, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSDragButton::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSDragButton::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSDragButton::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_signal_drag_start:
            {
                JSSignal3<void,double, double, unsigned int>::OWNERPTR mySignal( new
                        JSSignal3<void,double, double, unsigned int>::NATIVE(theNative.signal_drag_start()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_drag:
            {
                JSSignal2<void,double, double>::OWNERPTR mySignal( new
                        JSSignal2<void,double, double>::NATIVE(theNative.signal_drag()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        case PROP_signal_drag_done:
            {
                JSSignal0<void>::OWNERPTR mySignal( new
                        JSSignal0<void>::NATIVE(theNative.signal_drag_done()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSDragButton::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSDragButton::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;
    JSDragButton * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
    } else if (argc == 1) {
        std::string myIconFile;
        if ( ! convertFrom(cx, argv[0], myIconFile)) {
            JS_ReportError(cx,"Constructor for %s: Argument 0 must be a string",ClassName());
            return JS_FALSE;
        }
        std::string myIconWithPath = searchFileRelativeToJSInclude(cx, obj, argc, argv, myIconFile);
        newNative = new NATIVE(myIconWithPath);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none or one, got %d",ClassName(), argc);
        return JS_FALSE;
    }
    OWNERPTR newOwner(newNative);
    myNewObject = new JSDragButton(newOwner, newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSDragButton::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSDragButton::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
            "POST",              PROP_HTTP_POST,         Request::HTTP_POST,
            "GET",               PROP_HTTP_GET,          Request::HTTP_GET,
            "PUT",               PROP_HTTP_PUT,          Request::HTTP_PUT,
        {0}
    };
    return myProperties;
};
*/

void
JSDragButton::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);

    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    //TODO - multiple inheritance
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSBASE::ClassName());
}

JSObject *
JSDragButton::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        // add JSAbstractRenderWindow static props
        // XXX JSA_AddFunctions(cx, myConstructorFuncObj, JSY60BASE::BaseStaticFunctions());
        // XXX JSA_DefineConstInts(cx, myConstructorFuncObj, JSY60BASE::ConstIntProperties());
    } else {
        cerr << "JSDragButton::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSDragButton::NATIVE * theNative) {
    JSObject * myReturnObject = JSDragButton::Construct(cx, JSDragButton::OWNERPTR(0), theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<DragButton> & theDragButton) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSDragButton::NATIVE >::Class()) {
                theDragButton = JSClassTraits<JSDragButton::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

}

