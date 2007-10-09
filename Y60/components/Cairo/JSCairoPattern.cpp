//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include <string>

#include <y60/Image.h>

#include <y60/JScppUtils.h>
#include <y60/JSNode.h>
#include <y60/JSMatrix.h>
#include <y60/JSVector.h>

#include <y60/JSWrapper.impl>

#include "JSCairoSurface.h"
#include "JSCairoPattern.h"

using namespace std;
using namespace asl;
using namespace y60;
using namespace jslib;

template class JSWrapper<Cairo::RefPtr<Cairo::Pattern>, Ptr< Cairo::RefPtr<Cairo::Pattern> >, StaticAccessProtocol>;

static JSBool
checkForErrors(JSContext *theJavascriptPattern, Cairo::RefPtr<Cairo::Pattern> *thePattern) {
    Cairo::ErrorStatus myStatus = (*thePattern)->get_status();
    if(myStatus != CAIRO_STATUS_SUCCESS) {
        JS_ReportError(theJavascriptPattern, "cairo error: %s", cairo_status_to_string(myStatus));
        return JS_FALSE;
    }
    return JS_TRUE;
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Cairo::RefPtr<Cairo::Pattern>@") + as_string(obj);
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
    Cairo::RefPtr<Cairo::Pattern> *myPattern(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myPattern);

    ensureParamCount(argc, 1);

    int myExtend;
    convertFrom(cx, argv[0], myExtend);

    cairo_pattern_t *myCairoPattern = (*myPattern)->cobj();

    cairo_pattern_set_extend(myCairoPattern, (cairo_extend_t)myExtend);

    return JS_TRUE;
}

static JSBool
getExtend(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::RefPtr<Cairo::Pattern> *myPattern(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myPattern);

    ensureParamCount(argc, 0);

    cairo_pattern_t *myCairoPattern = (*myPattern)->cobj();

    cairo_extend_t myExtend = cairo_pattern_get_extend(myCairoPattern);

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
    Cairo::RefPtr<Cairo::Pattern> *myPattern(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myPattern);

    ensureParamCount(argc, 1);

    Matrix4f myMatrix;
    convertFrom(cx, argv[0], myMatrix);

    const cairo_matrix_t myCairoMatrix = {
        myMatrix[0][0], myMatrix[0][1],
        myMatrix[1][0], myMatrix[1][1],
        myMatrix[3][0], myMatrix[3][1]
    };

    (*myPattern)->set_matrix(myCairoMatrix);

    return JS_TRUE;
}

static JSBool
getMatrix(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::RefPtr<Cairo::Pattern> *myPattern(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myPattern);

    ensureParamCount(argc, 0);

    cairo_matrix_t myCairoMatrix;

    (*myPattern)->get_matrix(myCairoMatrix);

    Matrix4f myMatrix;
    myMatrix[0][0] = myCairoMatrix.xx;
    myMatrix[0][1] = myCairoMatrix.yx;
    myMatrix[0][2] = 0.0;
    myMatrix[0][3] = 0.0;

    myMatrix[1][0] = myCairoMatrix.xy;
    myMatrix[1][1] = myCairoMatrix.yy;
    myMatrix[1][2] = 0.0;
    myMatrix[1][3] = 0.0;

    myMatrix[2][0] = 0.0;
    myMatrix[2][1] = 0.0;
    myMatrix[2][2] = 1.0;
    myMatrix[2][3] = 0.0;

    myMatrix[3][0] = myCairoMatrix.x0;
    myMatrix[3][1] = myCairoMatrix.y0;
    myMatrix[3][2] = 0.0;
    myMatrix[3][3] = 1.0;

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

    NATIVE * newNative = new NATIVE(0);

    JSCairoPattern * myNewObject = 0;

    if (argc == 1) {

        Cairo::RefPtr<Cairo::Surface> *mySurface;
        convertFrom(cx, argv[0], mySurface);

        (*newNative) = Cairo::SurfacePattern::create(*mySurface);

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    myNewObject = new JSCairoPattern(OWNERPTR(newNative), newNative);

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
        {"EXTEND_NONE",   PROP_EXTEND_NONE,   Cairo::EXTEND_NONE},
        {"EXTEND_REPEAT", PROP_EXTEND_REPEAT, Cairo::EXTEND_REPEAT},
        {0}
    };
    return myProperties;
};

void
JSCairoPattern::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
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

namespace jslib {
jsval as_jsval(JSContext *cx, JSCairoPattern::OWNERPTR theOwner, JSCairoPattern::NATIVE * theNative) {
    JSObject * myReturnObject = JSCairoPattern::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
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
}

