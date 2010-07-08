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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include <string>

#include <y60/image/Image.h>

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSMatrix.h>
#include <y60/jsbase/JSVector.h>

#include <y60/jsbase/JSWrapper.impl>

#include "CairoUtilities.h"

#include "JSCairoSurface.h"
#include "JSCairoPattern.h"

#include "CairoWrapper.impl"

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

template class JSWrapper<JSCairoPatternWrapper, asl::Ptr< JSCairoPatternWrapper >, StaticAccessProtocol>;

template <>
void CairoWrapper<cairo_pattern_t>::reference() {
    cairo_pattern_reference(_myWrapped);
}

template <>
void CairoWrapper<cairo_pattern_t>::unreference() {
    //AC_INFO << "CairoPattern@" << this << " reference count: " << cairo_pattern_get_reference_count(_myWrapped);
    cairo_pattern_destroy(_myWrapped);
}

template class CairoWrapper<cairo_pattern_t>;

// static JSBool
// checkForErrors(JSContext *theJavascriptPattern, cairo_pattern_t *thePattern) {
//     cairo_status_t myStatus = cairo_pattern_status(thePattern);
//     if(myStatus != CAIRO_STATUS_SUCCESS) {
//         JS_ReportError(theJavascriptPattern, "cairo error: %s", cairo_status_to_string(myStatus));
//         return JS_FALSE;
//     }
//     return JS_TRUE;
// }

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("CairoPattern@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

// Drawing: Patterns

// MISSING:
// void        cairo_pattern_add_color_stop_rgb
//                                             (cairo_pattern_t *pattern,
//                                              double offset,
//                                              double red,
//                                              double green,
//                                              double blue);
// void        cairo_pattern_add_color_stop_rgba
//                                             (cairo_pattern_t *pattern,
//                                              double offset,
//                                              double red,
//                                              double green,
//                                              double blue,
//                                              double alpha);
// cairo_status_t cairo_pattern_get_color_stop_count
//                                             (cairo_pattern_t *pattern,
//                                              int *count);
// cairo_status_t cairo_pattern_get_color_stop_rgba
//                                             (cairo_pattern_t *pattern,
//                                              int index,
//                                              double *offset,
//                                              double *red,
//                                              double *green,
//                                              double *blue,
//                                              double *alpha);
// cairo_pattern_t* cairo_pattern_create_rgb   (double red,
//                                              double green,
//                                              double blue);
// cairo_pattern_t* cairo_pattern_create_rgba  (double red,
//                                              double green,
//                                              double blue,
//                                              double alpha);
// cairo_status_t cairo_pattern_get_rgba       (cairo_pattern_t *pattern,
//                                              double *red,
//                                              double *green,
//                                              double *blue,
//                                              double *alpha);
// cairo_pattern_t* cairo_pattern_create_for_surface
//                                             (cairo_surface_t *surface);
// cairo_status_t cairo_pattern_get_surface    (cairo_pattern_t *pattern,
//                                              cairo_surface_t **surface);
// cairo_pattern_t* cairo_pattern_create_linear
//                                             (double x0,
//                                              double y0,
//                                              double x1,
//                                              double y1);
// cairo_status_t cairo_pattern_get_linear_points
//                                             (cairo_pattern_t *pattern,
//                                              double *x0,
//                                              double *y0,
//                                              double *x1,
//                                              double *y1);
// cairo_pattern_t* cairo_pattern_create_radial
//                                             (double cx0,
//                                              double cy0,
//                                              double radius0,
//                                              double cx1,
//                                              double cy1,
//                                              double radius1);
// cairo_status_t cairo_pattern_get_radial_circles
//                                             (cairo_pattern_t *pattern,
//                                              double *x0,
//                                              double *y0,
//                                              double *r0,
//                                              double *x1,
//                                              double *y1,
//                                              double *r1);

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

JSFunctionSpec *
JSCairoPattern::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},

        {"setMatrix",            setMatrix,               1},
        {"getMatrix",            getMatrix,               0},

        {"setExtend",            setExtend,               1},
        {"getExtend",            getExtend,               0},

        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSCairoPattern::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSBool
JSCairoPattern::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoPattern::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoPattern::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoPattern::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoPattern::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative;

    JSCairoPattern * myNewObject = 0;

    if (argc == 1) {

        cairo_surface_t *mySurface;
        convertFrom(cx, argv[0], mySurface);

        cairo_pattern_t *myPattern = cairo_pattern_create_for_surface(mySurface);

        newNative = NATIVE::get(myPattern);

        cairo_pattern_destroy(myPattern);

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    JSCairoPatternWrapper::STRONGPTR _myOwnerPtr(newNative);
    myNewObject = new JSCairoPattern(dynamic_cast_Ptr<NATIVE>(_myOwnerPtr), newNative);

    JSCairoPatternWrapper::WEAKPTR   _mySelfPtr(_myOwnerPtr);
    newNative->setSelfPtr(_mySelfPtr);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);

        return JS_TRUE;
    }
    JS_ReportError(cx,"JSCairoPattern::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSCairoPattern::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        // name                id                       value
        {0}
    };
    return myProperties;
};

void
JSCairoPattern::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("Cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
}

JSObject *
JSCairoPattern::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSCairoPattern::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSCairoPattern::OWNERPTR theOwner, JSCairoPattern::NATIVE * theNative) {
    JSObject * myReturnObject = JSCairoPattern::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSCairoPattern::OWNERPTR theOwner, cairo_pattern_t * theNative) {
    JSCairoPatternWrapper *myWrapper = dynamic_cast<JSCairoPatternWrapper*>(JSCairoPatternWrapper::get(theNative));
    return as_jsval(cx, theOwner, myWrapper);
}

bool convertFrom(JSContext *cx, jsval theValue, JSCairoPattern::NATIVE *& theTarget) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSCairoPattern::NATIVE>::Class()) {
                JSClassTraits<JSCairoPattern::NATIVE>::ScopedNativeRef myObj(cx, myArgument);
                theTarget = &myObj.getNative();
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, cairo_pattern_t *& theTarget) {
    JSCairoPattern::NATIVE *myWrapper;

    if(convertFrom(cx, theValue, myWrapper)) {
        theTarget = myWrapper->getWrapped();
        return true;
    }

    return false;
}

}
