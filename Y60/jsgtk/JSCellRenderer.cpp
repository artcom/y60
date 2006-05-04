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
//   $RCSfile: JSCellRenderer.cpp,v $
//   $Author: christian $
//   $Revision: 1.2 $
//   $Date: 2005/02/02 14:48:38 $
//
//
//=============================================================================

#include "JSCellRenderer.h"
#include "JSWindow.h"
#include "JSRenderArea.h"
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
    std::string myStringRep = std::string("GtkCellRenderer@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSCellRenderer::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}


JSPropertySpec *
JSCellRenderer::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}
// getproperty handling
JSBool
JSCellRenderer::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    /*switch (theID) {
        default:*/
            JS_ReportError(cx,"JSCellRenderer::getProperty: index %d out of range", theID);
            return JS_FALSE;
    //}
}

JSBool
JSCellRenderer::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

// setproperty handling
JSBool
JSCellRenderer::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    /*switch (theID) {
        default:*/
            JS_ReportError(cx,"JSCellRenderer::setProperty: index %d out of range", theID);
            return JS_FALSE;
    //}
}

JSBool
JSCellRenderer::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCellRenderer::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    JS_ReportError(cx,"%s is abstract and can not be instatiated", ClassName() );
    return JS_FALSE;
}

void
JSCellRenderer::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), ConstIntProperties(), 0, 0);
}

JSConstIntPropertySpec *
JSCellRenderer::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
            "TEXT",              PROP_CELL_RENDERER_TEXT,     PROP_CELL_RENDERER_TEXT,
            "PIXBUF",            PROP_CELL_RENDERER_PIXBUF,   PROP_CELL_RENDERER_PIXBUF,
            //"PROGRESS",          PROP_CELL_RENDERER_PROGRESS, PROP_CELL_RENDERER_PROGRESS,
            "TOGGLE",            PROP_CELL_RENDERER_TOGGLE,   PROP_CELL_RENDERER_TOGGLE,
            //"COMBO",             PROP_CELL_RENDERER_COMBO,    PROP_CELL_RENDERER_COMBO,
        {0}
    };
    return myProperties;
};

JSObject *
JSCellRenderer::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, 0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSCellRenderer::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSCellRenderer::OWNERPTR theOwner, JSCellRenderer::NATIVE * theNative) {
    JSObject * myReturnObject = JSCellRenderer::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}


