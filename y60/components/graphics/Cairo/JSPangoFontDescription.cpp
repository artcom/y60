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

#include <string>

#include <y60/image/Image.h>

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSMatrix.h>
#include <y60/jsbase/JSVector.h>

#include <y60/jsbase/JSWrapper.impl>

#include <glib-object.h>

#include "JSPangoFontDescription.h"

#include "CairoUtilities.h"

#include "CairoWrapper.impl"

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    pango::JSFontDescription::OWNERPTR myOwner;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);
    char * theDescription = pango_font_description_to_string(myOwner->get());
    std::string myStringRep = string(theDescription);
    *rval = as_jsval(cx, myStringRep);
    g_free(theDescription);
    return JS_TRUE;
}

#if 0
static JSBool
setExtend(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_pattern_t *myPattern = NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myPattern);

    ensureParamCount(argc, 1);

    int myExtend;
    convertFrom(cx, argv[0], myExtend);

    cairo_pattern_set_extend(myPattern, (cairo_extend_t)myExtend);

    return JS_TRUE;
}

static JSBool
getExtend(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_pattern_t *myPattern = NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myPattern);

    ensureParamCount(argc, 0);

    cairo_extend_t myExtend = cairo_pattern_get_extend(myPattern);

    *rval = as_jsval(cx, (int)myExtend);

    return JS_TRUE;
}

// enum        cairo_filter_t;
// void        cairo_pattern_set_filter        (cairo_pattern_t *pattern,
//                                              cairo_filter_t filter);
// cairo_filter_t cairo_pattern_get_filter     (cairo_pattern_t *pattern);

static JSBool
setMatrix(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_pattern_t *myPattern = NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myPattern);

    ensureParamCount(argc, 1);

    Matrix4f myMatrix;
    convertFrom(cx, argv[0], myMatrix);

    cairo_matrix_t myCairoMatrix;

    convertMatrixToCairo(myMatrix, &myCairoMatrix);

    cairo_pattern_set_matrix(myPattern, &myCairoMatrix);

    return JS_TRUE;
}

static JSBool
getMatrix(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_pattern_t *myPattern = NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myPattern);

    ensureParamCount(argc, 0);

    cairo_matrix_t myCairoMatrix;

    cairo_pattern_get_matrix(myPattern, &myCairoMatrix);

    Matrix4f myMatrix;
    convertMatrixFromCairo(myMatrix, &myCairoMatrix);

    *rval = as_jsval(cx, myMatrix);

    return JS_TRUE;
}
#endif

JSFunctionSpec *
pango::JSFontDescription::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},

        {0}
    };
    return myFunctions;
}

JSPropertySpec *
pango::JSFontDescription::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSBool
pango::JSFontDescription::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
pango::JSFontDescription::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
pango::JSFontDescription::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
pango::JSFontDescription::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
pango::JSFontDescription::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    pango::JSFontDescription * myNewObject = 0;
    NATIVE * newNative = 0;

    if (argc == 0) {
        PangoFontDescription * myDescr = pango_font_description_new();
        newNative = new NATIVE(myDescr);
	} else if (argc == 1) {
        string fontDescription;
        convertFrom(cx, argv[0], fontDescription);

        PangoFontDescription * myDescr = pango_font_description_from_string(fontDescription.c_str());
        newNative = new NATIVE(myDescr);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 0 or 1: got %d",ClassName(), argc);
        return JS_FALSE;
    }

    myNewObject = new pango::JSFontDescription(OWNERPTR(newNative), newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);

        return JS_TRUE;
    }
    JS_ReportError(cx,"pango::JSFontDescription::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
pango::JSFontDescription::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        // name                id                       value
        {0}
    };
    return myProperties;
};

void
pango::JSFontDescription::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("Cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
}

JSObject *
pango::JSFontDescription::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "pango::JSFontDescription::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, pango::JSFontDescription::OWNERPTR theOwner, pango::JSFontDescription::NATIVE * theNative) {
    JSObject * myReturnObject = pango::JSFontDescription::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, pango::JSFontDescription::OWNERPTR & theOwner) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<pango::JSFontDescription::NATIVE>::Class()) {
                theOwner = pango::JSFontDescription::getJSWrapper(cx, myArgument).getOwner();
                return true;
            }
        }
    }
    return false;
}

}
