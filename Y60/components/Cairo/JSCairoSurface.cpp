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

#include "JSCairoSurface.h"

#include "CairoWrapper.impl"

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

template class JSWrapper<JSCairoSurfaceWrapper, asl::Ptr< JSCairoSurfaceWrapper >, StaticAccessProtocol>;

template <>
void CairoWrapper<cairo_surface_t>::reference() {
    cairo_surface_reference(_myWrapped);
}

template <>
void CairoWrapper<cairo_surface_t>::unreference() {
    // AC_INFO << "CairoSurface reference count: " << cairo_surface_get_reference_count(_myWrapped);
    cairo_surface_destroy(_myWrapped);
}

template class CairoWrapper<cairo_surface_t>;

static JSBool
checkForErrors(JSContext *theJavascriptSurface, cairo_surface_t *theSurface) {
    cairo_status_t myStatus = cairo_surface_status(theSurface);
    if(myStatus != CAIRO_STATUS_SUCCESS) {
        JS_ReportError(theJavascriptSurface, "cairo error: %s", cairo_status_to_string(myStatus));
        return JS_FALSE;
    }
    return JS_TRUE;
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("CairoSurface@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

// Surfaces: cairo_surface_t

// MISSING:
// cairo_surface_t* cairo_surface_create_similar
//                                             (cairo_surface_t *other,
//                                              cairo_content_t content,
//                                              int width,
//                                              int height);
// cairo_status_t cairo_surface_status         (cairo_surface_t *surface);
// void        cairo_surface_finish            (cairo_surface_t *surface);
// void        cairo_surface_flush             (cairo_surface_t *surface);
// void        cairo_surface_get_font_options  (cairo_surface_t *surface,
//                                              cairo_font_options_t *options);
// cairo_content_t cairo_surface_get_content   (cairo_surface_t *surface);
// void        cairo_surface_mark_dirty        (cairo_surface_t *surface);
// void        cairo_surface_mark_dirty_rectangle
//                                             (cairo_surface_t *surface,
//                                              int x,
//                                              int y,
//                                              int width,
//                                              int height);
// void        cairo_surface_set_device_offset (cairo_surface_t *surface,
//                                              double x_offset,
//                                              double y_offset);
// void        cairo_surface_get_device_offset (cairo_surface_t *surface,
//                                              double *x_offset,
//                                              double *y_offset);
// void        cairo_surface_set_fallback_resolution
//                                             (cairo_surface_t *surface,
//                                              double x_pixels_per_inch,
//                                              double y_pixels_per_inch);
// enum        cairo_surface_type_t;
// cairo_surface_type_t cairo_surface_get_type (cairo_surface_t *surface);


// XXX hack to allow triggering texture upload -ingo
static JSBool
triggerUpload(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    ensureParamCount(argc, 0);

    JSCairoSurface *mySurface = reinterpret_cast<JSCairoSurface*>(JS_GetPrivate(cx, obj));

    mySurface->doTriggerUpload();

    return JS_TRUE;
}

void
JSCairoSurface::doTriggerUpload() {
    _myImageNode->getFacade<y60::Image>()->triggerUpload();
}

JSFunctionSpec *
JSCairoSurface::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},

        // XXX hack to allow triggering texture upload -ingo
        {"triggerUpload",        triggerUpload,           0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSCairoSurface::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSBool
JSCairoSurface::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoSurface::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoSurface::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoSurface::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoSurface::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative;

    JSCairoSurface * myNewObject = 0;

    dom::NodePtr myImageNode;

    if (argc == 1) {

        if(!convertFrom(cx, argv[0], myImageNode)) {
            JS_ReportError(cx, "Need an image node to construct a cairo context.");
            return JS_FALSE;
        }

        ImagePtr myImage = myImageNode->getFacade<Image>();

        int myWidth = myImage->get<ImageWidthTag>();
        int myHeight = myImage->get<ImageHeightTag>();
        string myPixelFormat = myImage->get<RasterPixelFormatTag>();

        unsigned char *myData = myImage->getRasterPtr()->pixels().begin();

        int myStride;
        cairo_format_t myFormat;

        if (myPixelFormat == "BGRA") {
            myStride = myWidth * 4;
            myFormat = CAIRO_FORMAT_ARGB32;
        } else if (myPixelFormat == "RGBA") {
            myStride = myWidth * 4;
            myFormat = CAIRO_FORMAT_ARGB32;
        } else {
            AC_ERROR << "Pixel format not supported by JSCairo: " << myPixelFormat;
            throw UnsupportedPixelFormat("Pixel format not supported by JSCairo: " + myPixelFormat, PLUS_FILE_LINE)
;
        }

        cairo_surface_t *mySurface = cairo_image_surface_create_for_data(myData, myFormat, myWidth, myHeight, myStride);

        newNative = NATIVE::get(mySurface);

        cairo_surface_destroy(mySurface);

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    JSCairoSurfaceWrapper::STRONGPTR _myOwnerPtr(newNative);
    myNewObject = new JSCairoSurface(dynamic_cast_Ptr<NATIVE>(_myOwnerPtr), newNative);

    JSCairoSurfaceWrapper::WEAKPTR   _mySelfPtr(_myOwnerPtr);
    newNative->setSelfPtr(_mySelfPtr);

    myNewObject->_myImageNode = myImageNode;

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);

        return JS_TRUE;
    }
    JS_ReportError(cx,"JSCairoSurface::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSCairoSurface::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        // name                id                       value

        {0}
    };
    return myProperties;
};

void
JSCairoSurface::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
}

JSObject *
JSCairoSurface::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSCairoSurface::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSCairoSurface::OWNERPTR theOwner, JSCairoSurface::NATIVE * theNative) {
    JSObject * myReturnObject = JSCairoSurface::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSCairoSurface::OWNERPTR theOwner, cairo_surface_t * theNative) {
    JSCairoSurfaceWrapper *myWrapper = dynamic_cast<JSCairoSurfaceWrapper*>(JSCairoSurfaceWrapper::get(theNative));
    return as_jsval(cx, theOwner, myWrapper);
}

bool convertFrom(JSContext *cx, jsval theValue, JSCairoSurface::NATIVE *& theTarget) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSCairoSurface::NATIVE>::Class()) {
                JSClassTraits<JSCairoSurface::NATIVE>::ScopedNativeRef myObj(cx, myArgument);
                theTarget = &myObj.getNative();
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, cairo_surface_t *& theTarget) {
    JSCairoSurface::NATIVE *myWrapper;

    if(convertFrom(cx, theValue, myWrapper)) {
        theTarget = myWrapper->getWrapped();
        return true;
    }

    return false;
}

}

