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

#include "CairoUtilities.h"

#include "JSCairoContext.h"

#include "JSCairoSurface.h"
#include "JSCairoPattern.h"

#include "CairoWrapper.impl"

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

template class JSWrapper<JSCairoWrapper, asl::Ptr< JSCairoWrapper >, StaticAccessProtocol>;

template <>
void CairoWrapper<cairo_t>::reference() {
    cairo_reference(_myWrapped);
}

template <>
void CairoWrapper<cairo_t>::unreference() {
    //AC_INFO << "Cairo@" << this << " reference count: " << cairo_get_reference_count(_myWrapped);
    cairo_destroy(_myWrapped);
}

template class CairoWrapper<cairo_t>;


static JSBool
checkForErrors(JSContext *theJavascriptContext, cairo_t *theContext) {
    cairo_status_t myStatus = cairo_status(theContext);
    if(myStatus != CAIRO_STATUS_SUCCESS) {
        JS_ReportError(theJavascriptContext, "cairo error: %s", cairo_status_to_string(myStatus));
        return JS_FALSE;
    }
    return JS_TRUE;
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("CairoContext@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

//
// Drawing: cairo_t
//

static JSBool
save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_save(myContext);

    return checkForErrors(cx, myContext);
}

static JSBool
restore(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_restore(myContext);

    return checkForErrors(cx, myContext);
}

static JSBool
getTarget(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_surface_t *myTarget = cairo_get_target(myContext);

    *rval = as_jsval(cx, myTarget);

    return checkForErrors(cx, myContext);
}

static JSBool
pushGroup(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_push_group(myContext);

    return checkForErrors(cx, myContext);
}

// MISSING:
// void        cairo_push_group_with_content   (cairo_t *cr,
//                                              cairo_content_t content);

static JSBool
popGroup(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_pattern_t *myResult;

    myResult = cairo_pop_group(myContext);

    *rval = as_jsval(cx, myResult);

    return checkForErrors(cx, myContext);
}

static JSBool
popGroupToSource(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_pop_group_to_source(myContext);

    return checkForErrors(cx, myContext);
}

static JSBool
getGroupTarget(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_surface_t *myResult;

    myResult = cairo_get_group_target(myContext);

    *rval = as_jsval(cx, myResult);

    return checkForErrors(cx, myContext);
}

static JSBool
setSourceRGB(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 3);

    double myR;
    convertFrom(cx, argv[0], myR);
    double myG;
    convertFrom(cx, argv[1], myG);
    double myB;
    convertFrom(cx, argv[2], myB);

    cairo_set_source_rgb(myContext, myR, myG, myB);

    return checkForErrors(cx, myContext);
}

static JSBool
setSourceRGBA(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 4);

    double myR;
    convertFrom(cx, argv[0], myR);
    double myG;
    convertFrom(cx, argv[1], myG);
    double myB;
    convertFrom(cx, argv[2], myB);
    double myA;
    convertFrom(cx, argv[3], myA);

    cairo_set_source_rgba(myContext, myR, myG, myB, myA);

    return checkForErrors(cx, myContext);
}

static JSBool
setSourceSurface(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 3);

    cairo_surface_t *mySurface;
    convertFrom(cx, argv[0], mySurface);

    double x;
    convertFrom(cx, argv[1], x);

    double y;
    convertFrom(cx, argv[2], y);

    cairo_set_source_surface(myContext, mySurface, x, y);

    return checkForErrors(cx, myContext);
}

static JSBool
setSource(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    cairo_pattern_t *myPattern;
    convertFrom(cx, argv[0], myPattern);

    cairo_set_source(myContext, myPattern);

    return checkForErrors(cx, myContext);
}

static JSBool
getSource(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_pattern_t *myPattern = cairo_get_source(myContext);

    *rval = as_jsval(cx, myPattern);

    return checkForErrors(cx, myContext);
}

static JSBool
setAntialias(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myAntialias;
    convertFrom(cx, argv[0], myAntialias);

    cairo_set_antialias(myContext, (cairo_antialias_t)myAntialias);

    return checkForErrors(cx, myContext);
}

static JSBool
getAntialias(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, cairo_get_antialias(myContext));

    return checkForErrors(cx, myContext);
}

// MISSING:
// void        cairo_set_dash                  (cairo_t *cr,
//                                              const double *dashes,
//                                              int num_dashes,
//                                              double offset);
// int         cairo_get_dash_count            (cairo_t *cr);
// void        cairo_get_dash                  (cairo_t *cr,
//                                              double *dashes,
//                                              double *offset);

static JSBool
setFillRule(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myFillRule;
    convertFrom(cx, argv[0], myFillRule);

    cairo_set_fill_rule(myContext, (cairo_fill_rule_t)myFillRule);

    return checkForErrors(cx, myContext);
}

static JSBool
getFillRule(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, cairo_get_fill_rule(myContext));

    return checkForErrors(cx, myContext);
}

static JSBool
setLineCap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myLineCap;
    convertFrom(cx, argv[0], myLineCap);

    cairo_set_line_cap(myContext, (cairo_line_cap_t)myLineCap);

    return checkForErrors(cx, myContext);
}

static JSBool
getLineCap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, cairo_get_line_cap(myContext));

    return checkForErrors(cx, myContext);
}

static JSBool
setLineJoin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myLineJoin;
    convertFrom(cx, argv[0], myLineJoin);

    cairo_set_line_join(myContext, (cairo_line_join_t)myLineJoin);

    return checkForErrors(cx, myContext);
}

static JSBool
getLineJoin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, cairo_get_line_join(myContext));

    return checkForErrors(cx, myContext);
}

static JSBool
setLineWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    double myLineWidth;
    convertFrom(cx, argv[0], myLineWidth);

    cairo_set_line_width(myContext, myLineWidth);

    return checkForErrors(cx, myContext);
}

static JSBool
getLineWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, cairo_get_line_width(myContext));

    return checkForErrors(cx, myContext);
}

static JSBool
setMiterLimit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    double myMiterLimit;
    convertFrom(cx, argv[0], myMiterLimit);

    cairo_set_miter_limit(myContext, myMiterLimit);

    return checkForErrors(cx, myContext);
}

static JSBool
getMiterLimit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, cairo_get_miter_limit(myContext));

    return checkForErrors(cx, myContext);
}

static JSBool
setOperator(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myOperator;
    convertFrom(cx, argv[0], myOperator);

    cairo_set_operator(myContext, (cairo_operator_t)myOperator);

    return checkForErrors(cx, myContext);
}

static JSBool
getOperator(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, cairo_get_operator(myContext));

    return checkForErrors(cx, myContext);
}

static JSBool
setTolerance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    double myTolerance;
    convertFrom(cx, argv[0], myTolerance);

    cairo_set_tolerance(myContext, myTolerance);

    return checkForErrors(cx, myContext);
}

static JSBool
getTolerance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, cairo_get_tolerance(myContext));

    return checkForErrors(cx, myContext);
}

static JSBool
clip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_clip(myContext);

    return checkForErrors(cx, myContext);
}

static JSBool
clipPreserve(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_clip_preserve(myContext);

    return checkForErrors(cx, myContext);
}

//static JSBool
//getClipExtents(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
//    DOC_BEGIN("");
//    DOC_END;
//    cairo_t *myContext=NULL;
//    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);
//
//    ensureParamCount(argc, 0);
//
//    double myX1, myY1, myX2, myY2;
//
//    cairo_clip_extents(myContext, &myX1, &myY1, &myX2, &myY2);
//
//    Vector4d myResult(myX1, myY1, myX2, myY2);
//    *rval = as_jsval(cx, myResult);
//
//    return JS_TRUE;
//}

static JSBool
resetClip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_reset_clip(myContext);

    return checkForErrors(cx, myContext);
}

// MISSING:
// cairo_rectangle_t;
// cairo_rectangle_list_t;
// void        cairo_rectangle_list_destroy    (cairo_rectangle_list_t *rectangle_list);
// cairo_rectangle_list_t* cairo_copy_clip_rectangle_list (cairo_t *cr);

static JSBool
fill(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_fill(myContext);

    return checkForErrors(cx, myContext);
}

static JSBool
fillPreserve(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_fill_preserve(myContext);

    return checkForErrors(cx, myContext);
}

static JSBool
getFillExtents(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    double myX1, myY1, myX2, myY2;

    cairo_fill_extents(myContext, &myX1, &myY1, &myX2, &myY2);

    Vector4d myResult(myX1, myY1, myX2, myY2);
    *rval = as_jsval(cx, myResult);

    return JS_TRUE;
}

static JSBool
inFill(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 4);

    double myX;
    convertFrom(cx, argv[0], myX);

    double myY;
    convertFrom(cx, argv[1], myY);

    *rval = cairo_in_fill(myContext, myX, myY) ? JSVAL_TRUE : JSVAL_FALSE;

    return checkForErrors(cx, myContext);
}

static JSBool
mask(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    cairo_pattern_t *myPattern;
    convertFrom(cx, argv[0], myPattern);

    cairo_mask(myContext, myPattern);

    return checkForErrors(cx, myContext);
}

static JSBool
maskSurface(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 3);

    cairo_surface_t *mySurface;
    convertFrom(cx, argv[0], mySurface);

    double mySurfaceX;
    convertFrom(cx, argv[1], mySurfaceX);

    double mySurfaceY;
    convertFrom(cx, argv[2], mySurfaceY);

    cairo_mask_surface(myContext, mySurface, mySurfaceX, mySurfaceY);

    return checkForErrors(cx, myContext);
}

static JSBool
paint(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_paint(myContext);

    return checkForErrors(cx, myContext);
}

static JSBool
paintWithAlpha(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    double myAlpha;
    convertFrom(cx, argv[0], myAlpha);

    cairo_paint_with_alpha(myContext, myAlpha);

    return checkForErrors(cx, myContext);
}

static JSBool
stroke(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_stroke(myContext);

    return checkForErrors(cx, myContext);
}

static JSBool
strokePreserve(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_stroke_preserve(myContext);

    return checkForErrors(cx, myContext);
}

static JSBool
getStrokeExtents(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    double myX1, myY1, myX2, myY2;

    cairo_stroke_extents(myContext, &myX1, &myY1, &myX2, &myY2);

    Vector4d myResult(myX1, myY1, myX2, myY2);
    *rval = as_jsval(cx, myResult);

    return JS_TRUE;
}

static JSBool
inStroke(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 4);

    double myX;
    convertFrom(cx, argv[0], myX);

    double myY;
    convertFrom(cx, argv[1], myY);

    *rval = cairo_in_stroke(myContext, myX, myY) ? JSVAL_TRUE : JSVAL_FALSE;

    return checkForErrors(cx, myContext);
}

static JSBool
copyPage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_copy_page(myContext);

    return checkForErrors(cx, myContext);
}

static JSBool
showPage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_show_page(myContext);

    return checkForErrors(cx, myContext);
}

//
// Drawing: Paths
//

// MISSING:
// cairo_path_t* cairo_copy_path               (cairo_t *cr);
// cairo_path_t* cairo_copy_path_flat          (cairo_t *cr);
// void        cairo_path_destroy              (cairo_path_t *path);
// void        cairo_append_path               (cairo_t *cr,
//                                              const cairo_path_t *path);
// void        cairo_get_current_point         (cairo_t *cr,
//                                              double *x,
//                                              double *y);
// void        cairo_new_path                  (cairo_t *cr);
// void        cairo_new_sub_path              (cairo_t *cr);
// void        cairo_close_path                (cairo_t *cr);

static JSBool
arc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("arc");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY, myRadius, myAngle1, myAngle2;
    ensureParamCount(argc, 5);

    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);
    convertFrom(cx, argv[2], myRadius);
    convertFrom(cx, argv[3], myAngle1);
    convertFrom(cx, argv[4], myAngle2);

    cairo_arc(myContext, myX, myY, myRadius, myAngle1, myAngle2);

    return checkForErrors(cx, myContext);
}

static JSBool
arcNegative(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("arc_negative");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY, myRadius, myAngle1, myAngle2;
    ensureParamCount(argc, 5);

    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);
    convertFrom(cx, argv[2], myRadius);
    convertFrom(cx, argv[3], myAngle1);
    convertFrom(cx, argv[4], myAngle2);

    cairo_arc_negative(myContext, myX, myY, myRadius, myAngle1, myAngle2);

    return checkForErrors(cx, myContext);
}

static JSBool
curveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("curve_to");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX1, myY1, myX2, myY2, myX3, myY3;
    ensureParamCount(argc, 6);

    convertFrom(cx, argv[0], myX1);
    convertFrom(cx, argv[1], myY1);
    convertFrom(cx, argv[2], myX2);
    convertFrom(cx, argv[3], myY2);
    convertFrom(cx, argv[4], myX3);
    convertFrom(cx, argv[5], myY3);

    cairo_curve_to(myContext, myX1, myY1, myX2, myY2, myX3, myY3);

    return checkForErrors(cx, myContext);
}

static JSBool
lineTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("line_to");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY;
    ensureParamCount(argc, 2);

    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);

    cairo_line_to(myContext, myX, myY);

    return checkForErrors(cx, myContext);
}

static JSBool
moveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("move_to");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY;
    ensureParamCount(argc, 2);

    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);

    cairo_move_to(myContext, myX, myY);

    return checkForErrors(cx, myContext);
}

static JSBool
rectangle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("move_to");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY, myWidth, myHeight;
    ensureParamCount(argc, 2);

    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);
    convertFrom(cx, argv[2], myWidth);
    convertFrom(cx, argv[3], myHeight);

    cairo_rectangle(myContext, myX, myY, myWidth, myHeight);

    return checkForErrors(cx, myContext);
}

// MISSING:
// void        cairo_glyph_path                (cairo_t *cr,
//                                              const cairo_glyph_t *glyphs,
//                                              int num_glyphs);
// void        cairo_text_path                 (cairo_t *cr,
//                                              const char *utf8);

static JSBool
relCurveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("rel_curve_to");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX1, myY1, myX2, myY2, myX3, myY3;
    ensureParamCount(argc, 6);

    convertFrom(cx, argv[0], myX1);
    convertFrom(cx, argv[1], myY1);
    convertFrom(cx, argv[2], myX2);
    convertFrom(cx, argv[3], myY2);
    convertFrom(cx, argv[4], myX3);
    convertFrom(cx, argv[5], myY3);

    cairo_rel_curve_to(myContext, myX1, myY1, myX2, myY2, myX3, myY3);

    return checkForErrors(cx, myContext);
}

static JSBool
relLineTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("rel_line_to");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY;
    ensureParamCount(argc, 2);

    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);

    cairo_rel_line_to(myContext, myX, myY);

    return checkForErrors(cx, myContext);
}

static JSBool
relMoveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("rel_move_to");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY;
    ensureParamCount(argc, 2);

    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);

    cairo_rel_move_to(myContext, myX, myY);

    return checkForErrors(cx, myContext);
}

//
// Drawing: Transformations
//

static JSBool
translate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY;
    ensureParamCount(argc, 2);

    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);

    cairo_translate(myContext, myX, myY);

    return checkForErrors(cx, myContext);
}

static JSBool
scale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY;
    ensureParamCount(argc, 2);

    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);

    cairo_scale(myContext, myX, myY);

    return checkForErrors(cx, myContext);
}

static JSBool
rotate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myAngle;
    ensureParamCount(argc, 1);

    convertFrom(cx, argv[0], myAngle);

    cairo_rotate(myContext, myAngle);

    return checkForErrors(cx, myContext);
}

static JSBool
transform(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    Matrix4f myMatrix;
    convertFrom(cx, argv[0], myMatrix);

    cairo_matrix_t myCairoMatrix;

    convertMatrixToCairo(myMatrix, &myCairoMatrix);

    cairo_transform(myContext, &myCairoMatrix);

    return JS_TRUE;
}

static JSBool
setMatrix(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    Matrix4f myMatrix;
    convertFrom(cx, argv[0], myMatrix);

    cairo_matrix_t myCairoMatrix;

    convertMatrixToCairo(myMatrix, &myCairoMatrix);

    cairo_set_matrix(myContext, &myCairoMatrix);

    return JS_TRUE;
}

static JSBool
getMatrix(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_matrix_t myCairoMatrix;

    cairo_get_matrix(myContext, &myCairoMatrix);

    Matrix4f myMatrix;
    convertMatrixFromCairo(myMatrix, &myCairoMatrix);

    *rval = as_jsval(cx, myMatrix);

    return JS_TRUE;
}

static JSBool
identityMatrix(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    cairo_identity_matrix(myContext);

    return JS_TRUE;
}

static JSBool
userToDevice(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    Vector2f myInput;
    convertFrom(cx, argv[0], myInput);

    double myX, myY;
    myX = myInput[0];
    myY = myInput[1];

    cairo_user_to_device(myContext, &myX, &myY);

    Vector2f myResult;
    myResult[0] = static_cast<float>(myX);
    myResult[1] = static_cast<float>(myY);

    *rval = as_jsval(cx, myResult);

    return JS_TRUE;
}

static JSBool
userToDeviceDistance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    Vector2f myInput;
    convertFrom(cx, argv[0], myInput);

    double myX, myY;
    myX = myInput[0];
    myY = myInput[1];

    cairo_user_to_device_distance(myContext, &myX, &myY);

    Vector2f myResult;
    myResult[0] = static_cast<float>(myX);
    myResult[1] = static_cast<float>(myY);

    *rval = as_jsval(cx, myResult);

    return JS_TRUE;
}

static JSBool
deviceToUser(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    Vector2f myInput;
    convertFrom(cx, argv[0], myInput);

    double myX, myY;
    myX = myInput[0];
    myY = myInput[1];

    cairo_device_to_user(myContext, &myX, &myY);

    Vector2f myResult;
    myResult[0] = static_cast<float>(myX);
    myResult[1] = static_cast<float>(myY);

    *rval = as_jsval(cx, myResult);

    return JS_TRUE;
}

static JSBool
deviceToUserDistance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    Vector2f myInput;
    convertFrom(cx, argv[0], myInput);

    double myX, myY;
    myX = myInput[0];
    myY = myInput[1];

    cairo_device_to_user(myContext, &myX, &myY);

    Vector2f myResult;
    myResult[0] = static_cast<float>(myX);
    myResult[1] = static_cast<float>(myY);

    *rval = as_jsval(cx, myResult);

    return JS_TRUE;
}

//
// Drawing: Text
//

// MISSING:
//             cairo_glyph_t;

static JSBool
selectFontFace(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 3);

    std::string myFamily;
    convertFrom(cx, argv[0], myFamily);

    int mySlant;
    convertFrom(cx, argv[1], mySlant);

    int myWeight;
    convertFrom(cx, argv[2], myWeight);

    cairo_select_font_face(myContext, myFamily.c_str(), (cairo_font_slant_t)mySlant, (cairo_font_weight_t)myWeight);

    return checkForErrors(cx, myContext);
}

static JSBool
setFontSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t *myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    double mySize;
    convertFrom(cx, argv[0], mySize);

    cairo_set_font_size(myContext, mySize);

    return checkForErrors(cx, myContext);
}

// MISSING:
// void        cairo_set_font_matrix           (cairo_t *cr,
//                                              const cairo_matrix_t *matrix);
// void        cairo_get_font_matrix           (cairo_t *cr,
//                                              cairo_matrix_t *matrix);
// void        cairo_set_font_options          (cairo_t *cr,
//                                              const cairo_font_options_t *options);
// void        cairo_get_font_options          (cairo_t *cr,
//                                              cairo_font_options_t *options);
// void        cairo_set_font_face             (cairo_t *cr,
//                                              cairo_font_face_t *font_face);
// cairo_font_face_t* cairo_get_font_face      (cairo_t *cr);
// void        cairo_set_scaled_font           (cairo_t *cr,
//                                              const cairo_scaled_font_t *scaled_font);
// cairo_scaled_font_t* cairo_get_scaled_font  (cairo_t *cr);

static JSBool
showText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_t * myContext=NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 3);

    std::string myText;
    convertFrom(cx, argv[0], myText);

    cairo_show_text(myContext, myText.c_str());

    return checkForErrors(cx, myContext);
}

// void        cairo_show_glyphs               (cairo_t *cr,
//                                              const cairo_glyph_t *glyphs,
//                                              int num_glyphs);
// void        cairo_font_extents              (cairo_t *cr,
//                                              cairo_font_extents_t *extents);
// void        cairo_text_extents              (cairo_t *cr,
//                                              const char *utf8,
//                                              cairo_text_extents_t *extents);
// void        cairo_glyph_extents             (cairo_t *cr,
//                                              const cairo_glyph_t *glyphs,
//                                              int num_glyphs,
//                                              cairo_text_extents_t *extents);


JSFunctionSpec *
cairo::JSContext::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},

        // Drawing: cairo_t
        {"save",                 save,                    0},
        {"restore",              restore,                 0},

        {"getTarget",            getTarget,               0},

        {"pushGroup",            pushGroup,               0},
        {"popGroup",             popGroup,                0},
        {"popGroupToSource",     popGroupToSource,        0},
        {"getGroupTarget",       getGroupTarget,          0},

        {"setSourceRGB",         setSourceRGB,            3},
        {"setSourceRGBA",        setSourceRGBA,           4},
        {"setSourceSurface",     setSourceSurface,        1},
        {"setSource",            setSource,               1},
        {"getSource",            getSource,               0},

        {"setAntialias",         setAntialias,            1},
        {"getAntialias",         getAntialias,            0},
        {"setFillRule",          setFillRule,             1},
        {"getFillRule",          getFillRule,             0},
        {"setLineCap",           setLineCap,              1},
        {"getLineCap",           getLineCap,              0},
        {"setLineJoin",          setLineJoin,             1},
        {"getLineJoin",          getLineJoin,             0},
        {"setLineWidth",         setLineWidth,            1},
        {"getLineWidth",         getLineWidth,            0},
        {"setMiterLimit",        setMiterLimit,           1},
        {"getMiterLimit",        getMiterLimit,           0},
        {"setOperator",          setOperator,             1},
        {"getOperator",          getOperator,             0},
        {"setTolerance",         setTolerance,            1},
        {"getTolerance",         getTolerance,            0},

        {"clip",                 clip,                    0},
        {"clipPreserve",         clipPreserve,            0},
        //{"getClipExtents",       getClipExtents,          0},
        {"resetClip",            resetClip,               0},

        {"fill",                 fill,                    0},
        {"fillPreserve",         fillPreserve,            0},
        {"getFillExtents",       getFillExtents,          0},
        {"inFill",               inFill,                  2},

        {"mask",                 mask,                    1},
        {"maskSurface",          maskSurface,             3},

        {"paint",                paint,                   0},
        {"paintWithAlpha",       paintWithAlpha,          1},

        {"stroke",               stroke,                  0},
        {"strokePreserve",       strokePreserve,          0},
        {"getStrokeExtents",     getStrokeExtents,        0},
        {"inStroke",             inStroke,                2},

        {"copyPage",             copyPage,                0},
        {"showPage",             showPage,                0},

        // Drawing: Paths
        {"arc",                  arc,                     0},
        {"arcNegative",          arcNegative,             0},
        {"curveTo",              curveTo,                 0},
        {"lineTo",               lineTo,                  0},
        {"moveTo",               moveTo,                  0},
        {"rectangle",            rectangle,               0},
        {"relCurveTo",           relCurveTo,              0},
        {"relLineTo",            relLineTo,               0},
        {"relMoveTo",            relMoveTo,               0},

        // Drawing: Transformations
        {"translate",            translate,               2},
        {"scale",                scale,                   2},
        {"rotate",               rotate,                  1},
        {"transform",            transform,               1},

        {"setMatrix",            setMatrix,               1},
        {"getMatrix",            getMatrix,               0},
        {"identityMatrix",       identityMatrix,          0},

        {"userToDevice",         userToDevice,            1},
        {"userToDeviceDistance", userToDeviceDistance,    1},
        {"deviceToUser",         deviceToUser,            1},
        {"deviceToUserDistance", deviceToUserDistance,    1},

        // Drawing: Text
        {"selectFontFace",       selectFontFace,          3},
        {"setFontSize",          setFontSize,             1},
        {"showText",             showText,                1},

        {0}
    };
    return myFunctions;
}

JSPropertySpec *
cairo::JSContext::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSBool
cairo::JSContext::getPropertySwitch(unsigned long theID, ::JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
cairo::JSContext::setPropertySwitch(unsigned long theID, ::JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
cairo::JSContext::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        ::JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
cairo::JSContext::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        ::JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
cairo::JSContext::Constructor(::JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    cairo::JSContext *myNewObject = 0;

    cairo_surface_t *mySurface = 0;

    if (argc == 1) {

        if(!convertFrom(cx, argv[0], mySurface)) {
            JS_ReportError(cx, "Need a cairo surface to construct a cairo context.");
            return JS_FALSE;
        }

        cairo_t *myCairo = cairo_create(mySurface);

        newNative = NATIVE::get(myCairo);

        cairo_destroy(myCairo);

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    JSCairoWrapper::STRONGPTR _myOwnerPtr(newNative);
    myNewObject = new cairo::JSContext(dynamic_cast_Ptr<NATIVE>(_myOwnerPtr), newNative);

    JSCairoWrapper::WEAKPTR   _mySelfPtr(_myOwnerPtr);
    newNative->setSelfPtr(_mySelfPtr);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);

        return JS_TRUE;
    }
    JS_ReportError(cx,"cairo::JSContext::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
cairo::JSContext::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        // name                id                       value
        {0}
    };
    return myProperties;
};

void
cairo::JSContext::addClassProperties(::JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("Cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
}

JSObject *
cairo::JSContext::initClass(::JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "cairo::JSContext::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(::JSContext *cx, cairo::JSContext::OWNERPTR theOwner, cairo::JSContext::NATIVE * theNative) {
    JSObject * myReturnObject = cairo::JSContext::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(::JSContext *cx, cairo::JSContext::OWNERPTR theOwner, cairo_t * theNative) {
    JSCairoWrapper *myWrapper = dynamic_cast<JSCairoWrapper*>(JSCairoWrapper::get(theNative));
    return as_jsval(cx, theOwner, myWrapper);
}

bool convertFrom(::JSContext *cx, jsval theValue, cairo::JSContext::NATIVE *& theTarget) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<cairo::JSContext::NATIVE>::Class()) {
                JSClassTraits<cairo::JSContext::NATIVE>::ScopedNativeRef myObj(cx, myArgument);
                theTarget = &myObj.getNative();
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(::JSContext *cx, jsval theValue, cairo_t *& theTarget) {
    cairo::JSContext::NATIVE *myWrapper;

    if(convertFrom(cx, theValue, myWrapper)) {
        theTarget = myWrapper->getWrapped();
        return true;
    }

    return false;
}

}
