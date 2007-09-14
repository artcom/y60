#include <string>

#include <y60/Image.h>

#include <y60/JScppUtils.h>
#include <y60/JSNode.h>
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
// cairo_status_t cairo_pattern_status         (cairo_pattern_t *pattern);
// enum        cairo_extend_t;
// void        cairo_pattern_set_extend        (cairo_pattern_t *pattern,
//                                              cairo_extend_t extend);
// cairo_extend_t cairo_pattern_get_extend     (cairo_pattern_t *pattern);
// enum        cairo_filter_t;
// void        cairo_pattern_set_filter        (cairo_pattern_t *pattern,
//                                              cairo_filter_t filter);
// cairo_filter_t cairo_pattern_get_filter     (cairo_pattern_t *pattern);
// void        cairo_pattern_set_matrix        (cairo_pattern_t *pattern,
//                                              const cairo_matrix_t *matrix);
// void        cairo_pattern_get_matrix        (cairo_pattern_t *pattern,
//                                              cairo_matrix_t *matrix);
// enum        cairo_pattern_type_t;
// cairo_pattern_type_t cairo_pattern_get_type (cairo_pattern_t *pattern);

JSFunctionSpec *
JSCairoPattern::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},

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

    NATIVE * newNative = 0;

    JSCairoPattern * myNewObject = 0;

    if (argc == 1) {

        Cairo::RefPtr<Cairo::Surface> *mySurface;
        convertFrom(cx, argv[0], mySurface);

        cairo_pattern_t *myCairoPattern = cairo_pattern_create_for_surface((*mySurface)->cobj());

        Cairo::Pattern *myCairommPattern = new Cairo::Pattern(myCairoPattern);

        newNative = new Cairo::RefPtr<Cairo::Pattern>(myCairommPattern);

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
