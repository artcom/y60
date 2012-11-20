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
#include "JSRenderArea.h"

#include "JSWidget.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jslib/JSAbstractRenderWindow.h>
#include <y60/jslib/JSAbstractRenderWindow.impl>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>
#include <y60/jslib/GLContextRegistry.h>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<acgtk::RenderArea, asl::Ptr<acgtk::RenderArea>, StaticAccessProtocol>;
template class JSAbstractRenderWindow<acgtk::RenderArea>;

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
            if (theID >= static_cast<unsigned long>(JSGTKBASE::PROP_BEGIN)) {
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
            if (theID >= static_cast<unsigned long>(JSGTKBASE::PROP_BEGIN)) {
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
    acgtk::RenderAreaPtr myRenderArea;
    ensureParamCount(argc, 0);

    const GLContextRegistry::ContextSet & myContextSet = GLContextRegistry::get().getContextSet();
    for (GLContextRegistry::ContextSet::const_iterator i = myContextSet.begin(); i != myContextSet.end(); ++i) {
        if ( i->lock() ) {
            WeakPtr<AbstractRenderWindow> myWeaky = *i;
            asl::Ptr<AbstractRenderWindow> myContext = myWeaky.lock();
            myRenderArea = dynamic_cast_Ptr<acgtk::RenderArea>(myContext);
            if (myRenderArea && myRenderArea->is_realized()) {
                AC_INFO << "Realized Context found.";
                break;
            } else {
                myRenderArea = acgtk::RenderAreaPtr();
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

