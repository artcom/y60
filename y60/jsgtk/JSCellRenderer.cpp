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
#include "JSCellRenderer.h"

#include "JSWindow.h"
#include "JSRenderArea.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::CellRenderer, asl::Ptr<Gtk::CellRenderer>, StaticAccessProtocol>;

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
        {"TEXT",              PROP_CELL_RENDERER_TEXT,     PROP_CELL_RENDERER_TEXT},
        {"PIXBUF",            PROP_CELL_RENDERER_PIXBUF,   PROP_CELL_RENDERER_PIXBUF},
        //{"PROGRESS",          PROP_CELL_RENDERER_PROGRESS, PROP_CELL_RENDERER_PROGRESS},
        {"TOGGLE",            PROP_CELL_RENDERER_TOGGLE,   PROP_CELL_RENDERER_TOGGLE},
        //{"COMBO",             PROP_CELL_RENDERER_COMBO,    PROP_CELL_RENDERER_COMBO},
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


