
#include <string>

#include <y60/Image.h>

#include <y60/JScppUtils.h>
#include <y60/JSNode.h>

#include "JSCairoContext.h"

using namespace std;
using namespace asl;
using namespace y60;
using namespace jslib;

using namespace Cairo;

static JSBool
checkForErrors(JSContext *theJavascriptContext, Context *theContext) {
    ErrorStatus myStatus = theContext->get_status();
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
setSourceRGBA(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return JS_TRUE;
}

static JSBool
getAntialias(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
}

static JSBool
setAntialias(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
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

static JSBool
stroke(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("cairo_new_path");
    DOC_END;
    Cairo::Context * myContext;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myContext);
    myContext->stroke();
    
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
        // ingo end
        
        // sebastian begin
        {"stroke",               stroke,                  0},
        {"arc",                  arc,                     0},
        {"arcNegative",          arcNegative,             0}, 
        {"curveTo",              curveTo,                 0},
        {"lineTo",               lineTo,                  0},
        {"moveTo",               moveTo,                  0}, 
        {"rectangle",            rectangle,               0},
        {"relCurveTo",           relCurveTo,              0},
        {"relLineTo",            relLineTo,               0},
        {"relMoveTo",            relMoveTo,               0},
        {"stroke",               stroke,                  0},
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
                                                myImageWidth, myImageHeight, myImageWidth);
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
/*
JSConstIntPropertySpec *
JSCairoContext::ConstIntProperties() {

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
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
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

