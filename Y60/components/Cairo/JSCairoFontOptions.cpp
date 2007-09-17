#include <string>

#include <y60/Image.h>

#include <y60/JScppUtils.h>
#include <y60/JSNode.h>
#include <y60/JSVector.h>

#include <y60/JSWrapper.impl>

#include "JSCairoFontOptions.h"

using namespace std;
using namespace asl;
using namespace y60;
using namespace jslib;

template class JSWrapper<Cairo::RefPtr<Cairo::FontOptions>, Ptr< Cairo::RefPtr<Cairo::FontOptions> >, StaticAccessProtocol>;

static JSBool
checkForErrors(JSContext *theJavascriptFontOptions, Cairo::RefPtr<Cairo::FontOptions> *theFontOptions) {
    Cairo::ErrorStatus myStatus = (*theFontOptions)->get_status();
    if(myStatus != CAIRO_STATUS_SUCCESS) {
        JS_ReportError(theJavascriptFontOptions, "cairo error: %s", cairo_status_to_string(myStatus));
        return JS_FALSE;
    }
    return JS_TRUE;
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Cairo::RefPtr<Cairo::FontOptions>@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

// MISSING:
// void        cairo_font_options_set_antialias
//                                             (cairo_font_options_t *options,
//                                              cairo_antialias_t antialias);
// cairo_antialias_t cairo_font_options_get_antialias
//                                             (const cairo_font_options_t *options);
// enum        cairo_subpixel_order_t;
// void        cairo_font_options_set_subpixel_order
//                                             (cairo_font_options_t *options,
//                                              cairo_subpixel_order_t subpixel_order);
// cairo_subpixel_order_t cairo_font_options_get_subpixel_order
//                                             (const cairo_font_options_t *options);
// enum        cairo_hint_style_t;
// void        cairo_font_options_set_hint_style
//                                             (cairo_font_options_t *options,
//                                              cairo_hint_style_t hint_style);
// cairo_hint_style_t cairo_font_options_get_hint_style
//                                             (const cairo_font_options_t *options);
// enum        cairo_hint_metrics_t;
// void        cairo_font_options_set_hint_metrics
//                                             (cairo_font_options_t *options,
//                                              cairo_hint_metrics_t hint_metrics);
// cairo_hint_metrics_t cairo_font_options_get_hint_metrics
//                                             (const cairo_font_options_t *options);

static JSBool
setAntialias(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::RefPtr<Cairo::FontOptions> *myFontOptions(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myFontOptions);

    ensureParamCount(argc, 1);

    int myAntialias;
    convertFrom(cx, argv[0], myAntialias);

    (*myFontOptions)->set_antialias((Cairo::Antialias)myAntialias);

    return checkForErrors(cx, myFontOptions);
}

static JSBool
getAntialias(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    Cairo::RefPtr<Cairo::FontOptions> *myFontOptions(0);
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myFontOptions);

    ensureParamCount(argc, 0);

    *rval = as_jsval(cx, (int)(*myFontOptions)->get_antialias());

    return checkForErrors(cx, myFontOptions);
}

static JSBool
setSubpixelOrder(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return JS_TRUE;
}

static JSBool
getSubpixelOrder(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return JS_TRUE;
}

static JSBool
setHintStyle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return JS_TRUE;
}

static JSBool
getHintStyle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return JS_TRUE;
}

static JSBool
setHintMetrics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return JS_TRUE;
}

static JSBool
getHintMetrics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return JS_TRUE;
}


JSFunctionSpec *
JSCairoFontOptions::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},

        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSCairoFontOptions::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSBool
JSCairoFontOptions::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoFontOptions::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoFontOptions::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoFontOptions::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoFontOptions::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSCairoFontOptions * myNewObject = 0;

    dom::NodePtr myImageNode;

    if (argc == 0) {

        Cairo::FontOptions *myCairommFontOptions = new Cairo::FontOptions();

        newNative = new Cairo::RefPtr<Cairo::FontOptions>(myCairommFontOptions);

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    myNewObject = new JSCairoFontOptions(OWNERPTR(newNative), newNative);
    myNewObject->_myImageNode = myImageNode;

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);

        return JS_TRUE;
    }
    JS_ReportError(cx,"JSCairoFontOptions::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSCairoFontOptions::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        // name                id                       value

        {0}
    };
    return myProperties;
};

void
JSCairoFontOptions::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
}

JSObject *
JSCairoFontOptions::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSCairoFontOptions::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

namespace jslib {
jsval as_jsval(JSContext *cx, JSCairoFontOptions::OWNERPTR theOwner, JSCairoFontOptions::NATIVE * theNative) {
    JSObject * myReturnObject = JSCairoFontOptions::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, JSCairoFontOptions::NATIVE *& theTarget) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSCairoFontOptions::NATIVE>::Class()) {
                JSClassTraits<JSCairoFontOptions::NATIVE>::ScopedNativeRef myObj(cx, myArgument);
                theTarget = &myObj.getNative();
                return true;
            }
        }
    }
    return false;
}
}
