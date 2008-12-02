//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSTreeModel.h"
#include "JSTreeIter.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::TreeModel, Glib::RefPtr<Gtk::TreeModel>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = std::string("GtkTreeModel@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
GetIter(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);

        Glib::RefPtr<Gtk::TreeModel> myOwner(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

        Glib::ustring myPathString;
        convertFrom(cx, argv[0], myPathString);

        Gtk::TreeIter myIt = myOwner->get_iter(myPathString);
        if (myIt) {
            JSTreeIter::OWNERPTR myIterator = JSTreeIter::OWNERPTR( new Gtk::TreeIter( myIt ));
            *rval = as_jsval(cx, myIterator, & (* myIterator));
        } else {
            *rval = JSVAL_NULL;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION
}

JSFunctionSpec *
JSTreeModel::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"get_iter",             GetIter,                 1},
        {0}
    };
    return myFunctions;
}


JSPropertySpec *
JSTreeModel::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}
// getproperty handling
JSBool
JSTreeModel::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSTreeModel::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSTreeModel::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

// setproperty handling
JSBool
JSTreeModel::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSTreeModel::setProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSTreeModel::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSTreeModel::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    JS_ReportError(cx,"%s is abstract and can not be instatiated", ClassName() );
    return JS_FALSE;
}

void
JSTreeModel::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, 0);
}

JSObject *
JSTreeModel::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, 0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        //JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
    } else {
        cerr << "JSTreeModel::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSTreeModel::OWNERPTR theOwner, JSTreeModel::NATIVE * theNative) {
    JSObject * myReturnObject = JSTreeModel::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}


