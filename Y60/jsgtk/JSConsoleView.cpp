//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSConsoleView.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<acgtk::ConsoleView, asl::Ptr<acgtk::ConsoleView>,
                                       StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("acgtk::ConsoleView@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
Append(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    
    ensureParamCount(argc, 1, 2);
    
    acgtk::ConsoleView * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL( obj ), myNative);

    Glib::ustring myText;
    convertFrom(cx, argv[0], myText);

    std::string myTagName;
    if (argc == 2) {
        convertFrom(cx, argv[1], myTagName);
    }

    myNative->append( myText, myTagName );
    return JS_TRUE;
}

static JSBool
AddTag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    
    ensureParamCount(argc, 2, 3);
    
    acgtk::ConsoleView * myNative(0);
    convertFrom(cx, OBJECT_TO_JSVAL( obj ), myNative);

    std::string myTagName;
    convertFrom(cx, argv[0], myTagName);

    asl::Vector3f myForegroundColor;
    convertFrom(cx, argv[1], myForegroundColor);

    asl::Vector3f myBackgroundColor(1.0, 1.0, 1.0);
    if (argc == 3) {
        convertFrom(cx, argv[2], myBackgroundColor);
    }

    myNative->addTag( myTagName, myForegroundColor, myBackgroundColor );
    return JS_TRUE;
}

JSFunctionSpec *
JSConsoleView::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"append",               Append,                  2},
        {"addTag",               AddTag,                  3},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSConsoleView::Properties() {
    static JSPropertySpec myProperties[] = {
        {"scrollback", PROP_scrollback, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSConsoleView::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSConsoleView::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSConsoleView::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_scrollback:
            *vp = as_jsval(cx, theNative.getScrollback());
            return JS_TRUE;
        case 0:
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSConsoleView::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_scrollback:
            try {
                unsigned myLineCount;
                convertFrom(cx, *vp, myLineCount);
                theNative.setScrollback(myLineCount);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case 0:
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSConsoleView::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSConsoleView * myNewObject = 0;

    if (argc == 0) {
        newNative = new acgtk::ConsoleView();
        myNewObject = new JSConsoleView(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSConsoleView::Constructor: bad parameters");
    return JS_FALSE;
}

void
JSConsoleView::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
                JSBASE::ClassName());
}

JSObject *
JSConsoleView::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSConsoleView::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSConsoleView::OWNERPTR theOwner, JSConsoleView::NATIVE * theNative) {
    JSObject * myReturnObject = JSConsoleView::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

