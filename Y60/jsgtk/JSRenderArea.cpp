//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSRenderArea.h"
#include "JSWidget.h"
#include "jsgtk.h"
#include <y60/JScppUtils.h>
#include <y60/JSAbstractRenderWindow.h>
#include <y60/JSAbstractRenderWindow.impl>
#include <iostream>
#include <y60/GLContextRegistry.h>

using namespace std;
using namespace asl;

namespace jslib {

template JSAbstractRenderWindow<acgtk::RenderArea>;

typedef JSAbstractRenderWindow<acgtk::RenderArea> JSY60BASE;
typedef JSWidget JSGTKBASE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = "RenderArea";
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
setIdle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        // native method call
        acgtk::RenderArea * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        bool myIdleMode;
        convertFrom(cx, argv[0], myIdleMode);
        myNative->setIdleMode(myIdleMode);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSRenderArea::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"setIdle",              setIdle,                 1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSRenderArea::Properties() {
    static JSPropertySpec myProperties[] = {
//        {"URL", PROP_URL, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSRenderArea::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSRenderArea::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSRenderArea::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            if (theID >= JSGTKBASE::PROP_BEGIN) {
                return JSGTKBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
            } else {
                return JSY60BASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
            }
    }
}
JSBool
JSRenderArea::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            if (theID >= JSGTKBASE::PROP_BEGIN) {
                return JSGTKBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
            } else {
                return JSY60BASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
            }
    }
}

JSBool
JSRenderArea::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSRenderArea * myNewObject = 0;
    acgtk::RenderAreaPtr myRenderArea(0);
    ensureParamCount(argc, 0);

    const GLContextRegistry::ContextSet & myContextSet = GLContextRegistry::get().getContextSet();
    for (GLContextRegistry::ContextSet::const_iterator i = myContextSet.begin(); i != myContextSet.end(); ++i) {
        if (*i) {
            WeakPtr<AbstractRenderWindow> myWeaky = *i;
            asl::Ptr<AbstractRenderWindow> myContext = myWeaky.lock();
            myRenderArea = dynamic_cast_Ptr<acgtk::RenderArea>(myContext);
            if (myRenderArea && myRenderArea->is_realized()) {
                AC_INFO << "Realized Context found.";
                break;
            } else {
                myRenderArea = acgtk::RenderAreaPtr(0);
                AC_INFO << "Found unrealized Context";
            }
        }
    }

    if (!myRenderArea && myContextSet.size() > 0) {
        AC_ERROR << "Can not reuse a context although there exists one. Please realize any contexts before creating new ones! ";
    }
    newNative = new NATIVE(myRenderArea);
    OWNERPTR newOwner(newNative);
    newNative->setSelf(newOwner);
    myNewObject = new JSRenderArea(newOwner, newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSRenderArea::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSRenderArea::ConstIntProperties() {

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
JSRenderArea::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSGTKBASE::addClassProperties(cx, theClassProto);
    //JSY60BASE::addClassProperties(cx, theClassProto);

    JSA_AddFunctions(cx, theClassProto, JSY60BASE::BaseFunctions());
    JSA_AddProperties(cx, theClassProto, JSY60BASE::BaseProperties());

    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    //TODO - multiple inheritance
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSGTKBASE::ClassName());
}

JSObject *
JSRenderArea::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        // add JSAbstractRenderWindow static props
        JSA_AddFunctions(cx, myConstructorFuncObj, JSY60BASE::BaseStaticFunctions());
        JSA_DefineConstInts(cx, myConstructorFuncObj, JSY60BASE::ConstIntProperties());
    } else {
        cerr << "JSRenderArea::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSRenderArea::OWNERPTR theOwner, JSRenderArea::NATIVE * theNative) {
    JSObject * myReturnObject = JSRenderArea::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<acgtk::RenderArea> & theRenderArea) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRenderArea::NATIVE >::Class()) {
                theRenderArea = JSClassTraits<JSRenderArea::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

}

