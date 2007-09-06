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
#include <y60/JSVector.h>

#include <y60/JSWrapper.impl>

#include "JSCairoContext.h"

using namespace std;
using namespace asl;
using namespace y60;
using namespace jslib;

namespace jslib {

template class JSWrapper<Cairo::Context, Ptr<Cairo::Context>, StaticAccessProtocol>;

static JSBool
checkForErrors(JSContext *theJavascriptContext, Cairo::Context *theContext) {
    Cairo::ErrorStatus myStatus = theContext->get_status();
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
    std::string myStringRep = string("Cairo::Context@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

// INGO START

static JSBool
save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->save();

    return checkForErrors(cx, myContext);
}

static JSBool
restore(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->restore();

    return checkForErrors(cx, myContext);
}

// MISSING:
// cairo_surface_t* cairo_get_target           (cairo_t *cr);
// void        cairo_push_group                (cairo_t *cr);
// void        cairo_push_group_with_content   (cairo_t *cr,
//                                              cairo_content_t content);
// cairo_pattern_t* cairo_pop_group            (cairo_t *cr);
// void        cairo_pop_group_to_source       (cairo_t *cr);
// cairo_surface_t* cairo_get_group_target     (cairo_t *cr);

static JSBool
setSourceRGB(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 3);

    int myR;
    convertFrom(cx, argv[0], myR);
    int myG;
    convertFrom(cx, argv[1], myG);
    int myB;
    convertFrom(cx, argv[2], myB);

    myContext->set_source_rgb(myR, myG, myB);

    return checkForErrors(cx, myContext);
}

static JSBool
setSourceRGBA(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 4);

    int myR;
    convertFrom(cx, argv[0], myR);
    int myG;
    convertFrom(cx, argv[1], myG);
    int myB;
    convertFrom(cx, argv[2], myB);
    int myA;
    convertFrom(cx, argv[3], myA);

    myContext->set_source_rgba(myR, myG, myB, myA);

    return checkForErrors(cx, myContext);
}

// MISSING:
// void        cairo_set_source                (cairo_t *cr,
//                                              cairo_pattern_t *source);
// void        cairo_set_source_surface        (cairo_t *cr,
//                                              cairo_surface_t *surface,
//                                              double x,
//                                              double y);
// cairo_pattern_t* cairo_get_source           (cairo_t *cr);

static JSBool
setAntialias(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myAntialias;
    convertFrom(cx, argv[0], myAntialias);

    myContext->set_antialias((Cairo::Antialias)myAntialias);

    return checkForErrors(cx, myContext);
}

static JSBool
getAntialias(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, (int)myContext->get_antialias());

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
// enum        cairo_fill_rule_t;
// void        cairo_set_fill_rule             (cairo_t *cr,
//                                              cairo_fill_rule_t fill_rule);
// cairo_fill_rule_t cairo_get_fill_rule       (cairo_t *cr);

static JSBool
setLineCap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myLineCap;
    convertFrom(cx, argv[0], myLineCap);

    myContext->set_line_cap((Cairo::LineCap)myLineCap);

    return checkForErrors(cx, myContext);
}

static JSBool
getLineCap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, (int)myContext->get_line_cap());

    return checkForErrors(cx, myContext);
}

static JSBool
setLineJoin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myLineJoin;
    convertFrom(cx, argv[0], myLineJoin);

    myContext->set_line_join((Cairo::LineJoin)myLineJoin);

    return checkForErrors(cx, myContext);
}

static JSBool
getLineJoin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, (int)myContext->get_line_join());

    return checkForErrors(cx, myContext);
}

static JSBool
setLineWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myLineWidth;
    convertFrom(cx, argv[0], myLineWidth);

    myContext->set_line_width(myLineWidth);

    return checkForErrors(cx, myContext);
}

static JSBool
getLineWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, myContext->get_line_width());

    return checkForErrors(cx, myContext);
}

static JSBool
setMiterLimit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myMiterLimit;
    convertFrom(cx, argv[0], myMiterLimit);

    myContext->set_miter_limit(myMiterLimit);

    return checkForErrors(cx, myContext);
}

static JSBool
getMiterLimit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, myContext->get_miter_limit());

    return checkForErrors(cx, myContext);
}

static JSBool
setOperator(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    int myOperator;
    convertFrom(cx, argv[0], myOperator);

    myContext->set_operator((Cairo::Operator)myOperator);

    return checkForErrors(cx, myContext);
}

static JSBool
getOperator(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, (int)myContext->get_operator());

    return checkForErrors(cx, myContext);
}

static JSBool
setTolerance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    double myTolerance;
    convertFrom(cx, argv[0], myTolerance);

    myContext->set_tolerance(myTolerance);

    return checkForErrors(cx, myContext);
}

static JSBool
getTolerance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, myContext->get_tolerance());

    return checkForErrors(cx, myContext);
}

static JSBool
clip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->clip();

    return checkForErrors(cx, myContext);
}

static JSBool
clipPreserve(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->clip_preserve();

    return checkForErrors(cx, myContext);
}

/*
 XXX: my cairomm doesn't provide this, but the docs say it should. -ingo

static JSBool
getClipExtents(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    double myX1, myY1, myX2, myY2;
    
    myContext->get_clip_extents(myX1, myY1, myX2, myY2);
    
    Vector4d myResult(myX1, myY1, myX2, myY2);
    *rval = as_jsval(cx, myResult);

    return JS_TRUE;
}
*/

static JSBool
resetClip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->reset_clip();

    return checkForErrors(cx, myContext);
}

static JSBool
fill(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->fill();

    return checkForErrors(cx, myContext);
}

static JSBool
fillPreserve(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->fill_preserve();

    return checkForErrors(cx, myContext);
}

static JSBool
getFillExtents(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);
    
    ensureParamCount(argc, 0);
    
    double myX1, myY1, myX2, myY2;
    
    myContext->get_fill_extents(myX1, myY1, myX2, myY2);
    
    Vector4d myResult(myX1, myY1, myX2, myY2);
    *rval = as_jsval(cx, myResult);
    
    return JS_TRUE;
}

static JSBool
inFill(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 4);

    double myX;
    convertFrom(cx, argv[0], myX);

    double myY;
    convertFrom(cx, argv[1], myY);

    *rval = myContext->in_fill(myX, myY) ? JSVAL_TRUE : JSVAL_FALSE;

    return checkForErrors(cx, myContext);
}

// MISSING:
// void        cairo_mask                      (cairo_t *cr,
//                                              cairo_pattern_t *pattern);
// void        cairo_mask_surface              (cairo_t *cr,
//                                              cairo_surface_t *surface,
//                                              double surface_x,
//                                              double surface_y);

static JSBool
paint(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->paint();
    
    return checkForErrors(cx, myContext); 
}

static JSBool
paintWithAlpha(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 1);

    double myAlpha;
    convertFrom(cx, argv[0], myAlpha);

    myContext->paint_with_alpha(myAlpha);
    
    return checkForErrors(cx, myContext); 
}

static JSBool
stroke(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->stroke();
    
    return checkForErrors(cx, myContext); 
}

static JSBool
strokePreserve(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->stroke_preserve();

    return checkForErrors(cx, myContext);
}

static JSBool
getStrokeExtents(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);
    
    ensureParamCount(argc, 0);
    
    double myX1, myY1, myX2, myY2;
    
    myContext->get_stroke_extents(myX1, myY1, myX2, myY2);
    
    Vector4d myResult(myX1, myY1, myX2, myY2);
    *rval = as_jsval(cx, myResult);
    
    return JS_TRUE;
}

static JSBool
inStroke(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 4);

    double myX;
    convertFrom(cx, argv[0], myX);

    double myY;
    convertFrom(cx, argv[1], myY);

    *rval = myContext->in_stroke(myX, myY) ? JSVAL_TRUE : JSVAL_FALSE;

    return checkForErrors(cx, myContext);
}

static JSBool
copyPage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->copy_page();

    return checkForErrors(cx, myContext);
}

static JSBool
showPage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::Context *myContext(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    ensureParamCount(argc, 0);

    myContext->show_page();

    return checkForErrors(cx, myContext);
}

// INGO END

// SEBASTIAN START
// function list:
// cairo_path_t* cairo_copy_path               (cairo_t *cr);
// cairo_path_t* cairo_copy_path_flat          (cairo_t *cr);
// void        cairo_path_destroy              (cairo_path_t *path);
// void        cairo_append_path               (cairo_t *cr,
//                                              const cairo_path_t *path);
// void        cairo_get_current_point         (cairo_t *cr,
//                                              double *x,
//                                              double *y);

static JSBool
arc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("arc");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY, myRadius, myAngle1, myAngle2;
    ensureParamCount(argc, 5);
    
    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);
    convertFrom(cx, argv[2], myRadius);
    convertFrom(cx, argv[3], myAngle1);
    convertFrom(cx, argv[4], myAngle2);

    myContext->arc(myX, myY, myRadius, myAngle1, myAngle2);
    
    return checkForErrors(cx, myContext); 
}

static JSBool
arcNegative(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("arc_negative");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY, myRadius, myAngle1, myAngle2;
    ensureParamCount(argc, 5);
    
    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);
    convertFrom(cx, argv[2], myRadius);
    convertFrom(cx, argv[3], myAngle1);
    convertFrom(cx, argv[4], myAngle2);

    myContext->arc_negative(myX, myY, myRadius, myAngle1, myAngle2);
    
    return checkForErrors(cx, myContext); 
}

static JSBool
curveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("curve_to");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX1, myY1, myX2, myY2, myX3, myY3;
    ensureParamCount(argc, 6);
    
    convertFrom(cx, argv[0], myX1);
    convertFrom(cx, argv[1], myY1);
    convertFrom(cx, argv[2], myX2);
    convertFrom(cx, argv[3], myY2);
    convertFrom(cx, argv[4], myX3);
    convertFrom(cx, argv[5], myY3);

    myContext->curve_to(myX1, myY1, myX2, myY2, myX3, myY3);
    
    return checkForErrors(cx, myContext); 
}

static JSBool
lineTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("line_to");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY;
    ensureParamCount(argc, 2);
    
    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);

    myContext->line_to(myX, myY);
    
    return checkForErrors(cx, myContext); 
}

static JSBool
moveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("move_to");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY;
    ensureParamCount(argc, 2);
    
    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);

    myContext->move_to(myX, myY);
    
    return checkForErrors(cx, myContext); 
}

static JSBool
rectangle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("move_to");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY, myWidth, myHeight;
    ensureParamCount(argc, 2);
    
    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);
    convertFrom(cx, argv[2], myWidth);
    convertFrom(cx, argv[3], myHeight);

    myContext->rectangle(myX, myY, myWidth, myHeight);
    
    return checkForErrors(cx, myContext); 
}

// void        cairo_glyph_path                (cairo_t *cr,
//                                              const cairo_glyph_t *glyphs,
//                                              int num_glyphs);
// void        cairo_text_path                 (cairo_t *cr,
//                                              const char *utf8);

static JSBool
relCurveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("rel_curve_to");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);
   
    double myX1, myY1, myX2, myY2, myX3, myY3;
    ensureParamCount(argc, 6);
    
    convertFrom(cx, argv[0], myX1);
    convertFrom(cx, argv[1], myY1);
    convertFrom(cx, argv[2], myX2);
    convertFrom(cx, argv[3], myY2);
    convertFrom(cx, argv[4], myX3);
    convertFrom(cx, argv[5], myY3);
    
    myContext->rel_curve_to(myX1, myY1, myX2, myY2, myX3, myY3);
    
    return checkForErrors(cx, myContext); 
}

static JSBool
relLineTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("rel_line_to");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY;
    ensureParamCount(argc, 2);
    
    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);

    myContext->rel_line_to(myX, myY);
    
    return checkForErrors(cx, myContext); 
}

static JSBool
relMoveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("rel_move_to");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);

    double myX, myY;
    ensureParamCount(argc, 2);
    
    convertFrom(cx, argv[0], myX);
    convertFrom(cx, argv[1], myY);

    myContext->rel_move_to(myX, myY);
    
    return checkForErrors(cx, myContext); 
}

// SEBASTIAN END


JSFunctionSpec *
JSCairoContext::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},

        // ingo begin
        {"save",                 save,                    0},
        {"restore",              restore,                 0},
        {"setSourceRGB",         setSourceRGB,            3},
        {"setSourceRGBA",        setSourceRGBA,           4},
        {"setAntialias",         setAntialias,            1},
        {"getAntialias",         getAntialias,            0},
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

        {"paint",                paint,                   0},
        {"paintWithAlpha",       paintWithAlpha,          1},

        {"stroke",               stroke,                  0},
        {"strokePreserve",       strokePreserve,          0},
        {"getStrokeExtents",     getStrokeExtents,        0},
        {"inStroke",             inStroke,                2},

        {"copyPage",             copyPage,                0},
        {"showPage",             showPage,                0},

        // ingo end
        
        // sebastian begin
        {"arc",                  arc,                     0},
        {"arcNegative",          arcNegative,             0}, 
        {"curveTo",              curveTo,                 0},
        {"lineTo",               lineTo,                  0},
        {"moveTo",               moveTo,                  0}, 
        {"rectangle",            rectangle,               0},
        {"relCurveTo",           relCurveTo,              0},
        {"relLineTo",            relLineTo,               0},
        {"relMoveTo",            relMoveTo,               0},
        // sebastian end
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSCairoContext::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSBool
JSCairoContext::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoContext::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoContext::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoContext::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoContext::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSCairoContext * myNewObject = 0;

    if (argc == 1) {
        dom::NodePtr myImageNode;
        if(!convertFrom(cx, argv[0], myImageNode)) {
            JS_ReportError(cx, "Need an image node to construct a cairo context.");
            return JS_FALSE;
        }

        ImagePtr myImage = myImageNode->getFacade<Image>();
        int myImageWidth = myImage->get<ImageWidthTag>();
        int myImageHeight = myImage->get<ImageHeightTag>();


        unsigned char *myDataPtr = myImage->getRasterPtr()->pixels().begin();

        cairo_surface_t *myCairoSurface =
            cairo_image_surface_create_for_data(myDataPtr, CAIRO_FORMAT_ARGB32, 
                                                myImageWidth, myImageHeight, myImageWidth*4);
        cairo_t *myCairoContext =
            cairo_create(myCairoSurface);

        newNative = new Cairo::Context(myCairoContext);

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }
    myNewObject = new JSCairoContext(OWNERPTR(newNative), newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);

        return JS_TRUE;
    }
    JS_ReportError(cx,"JSCairoContext::Constructor: bad parameters");
    return JS_FALSE;
}


JSConstIntPropertySpec *
JSCairoContext::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {"ANTIALIAS_DEFAULT",  PROP_ANTIALIAS_DEFAULT,  Cairo::ANTIALIAS_DEFAULT},
        {"ANTIALIAS_NONE",     PROP_ANTIALIAS_NONE,     Cairo::ANTIALIAS_NONE},
        {"ANTIALIAS_GRAY",     PROP_ANTIALIAS_GRAY,     Cairo::ANTIALIAS_GRAY},
        {"ANTIALIAS_SUBPIXEL", PROP_ANTIALIAS_SUBPIXEL, Cairo::ANTIALIAS_SUBPIXEL},
        
        {"LINE_CAP_BUTT",      PROP_LINE_CAP_BUTT,      Cairo::LINE_CAP_BUTT  },
        {"LINE_CAP_ROUND",     PROP_LINE_CAP_ROUND,     Cairo::LINE_CAP_ROUND },
        {"LINE_CAP_SQUARE",    PROP_LINE_CAP_SQUARE,    Cairo::LINE_CAP_SQUARE},

        {"LINE_JOIN_MITER",    PROP_LINE_JOIN_MITER,    Cairo::LINE_JOIN_MITER  },
        {"LINE_JOIN_ROUND",    PROP_LINE_JOIN_ROUND,    Cairo::LINE_JOIN_ROUND },
        {"LINE_JOIN_BEVEL",    PROP_LINE_JOIN_BEVEL,    Cairo::LINE_JOIN_BEVEL},

        {"OPERATOR_CLEAR",     PROP_OPERATOR_CLEAR,     Cairo::OPERATOR_CLEAR},
        {"OPERATOR_SOURCE",    PROP_OPERATOR_SOURCE,    Cairo::OPERATOR_SOURCE},
        {"OPERATOR_OVER",      PROP_OPERATOR_OVER,      Cairo::OPERATOR_OVER},
        {"OPERATOR_IN",        PROP_OPERATOR_IN,        Cairo::OPERATOR_IN},
        {"OPERATOR_OUT",       PROP_OPERATOR_OUT,       Cairo::OPERATOR_OUT},
        {"OPERATOR_ATOP",      PROP_OPERATOR_ATOP,      Cairo::OPERATOR_ATOP},
        {"OPERATOR_DEST",      PROP_OPERATOR_DEST,      Cairo::OPERATOR_DEST},
        {"OPERATOR_DEST_OVER", PROP_OPERATOR_DEST_OVER, Cairo::OPERATOR_DEST_OVER},
        {"OPERATOR_DEST_IN",   PROP_OPERATOR_DEST_IN,   Cairo::OPERATOR_DEST_IN},
        {"OPERATOR_DEST_OUT",  PROP_OPERATOR_DEST_OUT,  Cairo::OPERATOR_DEST_OUT},
        {"OPERATOR_DEST_ATOP", PROP_OPERATOR_DEST_ATOP, Cairo::OPERATOR_DEST_ATOP},
        {"OPERATOR_XOR",       PROP_OPERATOR_XOR,       Cairo::OPERATOR_XOR},
        {"OPERATOR_ADD",       PROP_OPERATOR_ADD,       Cairo::OPERATOR_ADD},
        {"OPERATOR_SATURATE",  PROP_OPERATOR_SATURATE,  Cairo::OPERATOR_SATURATE},

        {0}
    };
    return myProperties;
};

void
JSCairoContext::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
}

JSObject *
JSCairoContext::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSCairoContext::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSCairoContext::OWNERPTR theOwner, JSCairoContext::NATIVE * theNative) {
    JSObject * myReturnObject = JSCairoContext::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, JSCairoContext::NATIVE *& theTarget) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSCairoContext::NATIVE>::Class()) {
                JSClassTraits<JSCairoContext::NATIVE>::ScopedNativeRef myObj(cx, myArgument);
                theTarget = &myObj.getNative();
                return true;
            }
        }
    }
    return false;
}

}
