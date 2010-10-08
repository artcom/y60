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
#include <y60/jsbase/JSVector.h>

#include <y60/jsbase/JSWrapper.impl>

#include "JSSurface.h"

#include "CairoWrapper.impl"

using namespace std;
using namespace asl;
using namespace y60;
using namespace dom;

namespace jslib {

template class JSWrapper<JSCairoSurfaceWrapper, asl::Ptr< JSCairoSurfaceWrapper >, StaticAccessProtocol>;

template <>
void CairoWrapper<cairo_surface_t>::reference() {
    cairo_surface_reference(_myWrapped);
}

template <>
void CairoWrapper<cairo_surface_t>::unreference() {
    //AC_INFO << "CairoSurface@" << this << " reference count: " << cairo_surface_get_reference_count(_myWrapped);
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

static JSBool
flush(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_surface_t *mySurface = NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), mySurface);

    ensureParamCount(argc, 0);

    cairo_surface_flush(mySurface);

    return checkForErrors(cx, mySurface);
}

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

static JSBool
writeToPNG(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_surface_t *mySurface = NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), mySurface);

    ensureParamCount(argc, 1);

    std::string myPath;
    convertFrom(cx, argv[0], myPath);

    cairo_surface_write_to_png(mySurface, myPath.c_str());

    return checkForErrors(cx, mySurface);
}

static JSBool
getWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_surface_t *mySurface = NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), mySurface);

    ensureParamCount(argc, 0);

    int myWidth = cairo_image_surface_get_width(mySurface);

    *rval = as_jsval(cx, myWidth);

    return checkForErrors(cx, mySurface);
}

static JSBool
getHeight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    cairo_surface_t *mySurface = NULL;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), mySurface);

    ensureParamCount(argc, 0);

    int myHeight = cairo_image_surface_get_height(mySurface);

    *rval = as_jsval(cx, myHeight);

    return checkForErrors(cx, mySurface);
}

// XXX hack to allow triggering texture upload -ingo
static JSBool
triggerUpload(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    ensureParamCount(argc, 0);

    cairo::JSSurface *mySurface = reinterpret_cast<cairo::JSSurface*>(JS_GetPrivate(cx, obj));

    mySurface->doTriggerUpload();

    return JS_TRUE;
}

void
cairo::JSSurface::doTriggerUpload() {
    //_myImageNode->getFacade<y60::Image>()->triggerUpload();
}

JSFunctionSpec *
cairo::JSSurface::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},

        {"flush",                flush,                   0},

        {"getWidth",             getWidth,                0},
        {"getHeight",            getHeight,               0},

        {"writeToPNG",           writeToPNG,              1},

        // XXX hack to allow triggering texture upload -ingo
        {"triggerUpload",        triggerUpload,           0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
cairo::JSSurface::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSBool
cairo::JSSurface::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
cairo::JSSurface::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
cairo::JSSurface::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
cairo::JSSurface::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

void
cairo::JSSurface::convertRGBAtoBGRA(ResizeableRasterPtr theOld, ResizeableRasterPtr theNew) {
    ReadableBlock  &myOld = theOld->pixels();
    WriteableBlock &myNew = theNew->pixels();

    if(myOld.size() != myNew.size()) {
        AC_FATAL << "Trying to convert between rasters with unequal sizes.";
    }

    const unsigned char *src = const_cast<unsigned char*>(myOld.begin());
    unsigned char *dst = myNew.begin();
    unsigned int l = myOld.size();
    unsigned int i;
    for(i = 0; i < l; i += 4) {
        dst[i+0] = src[i+2];
        dst[i+1] = src[i+1];
        dst[i+2] = src[i+0];
        dst[i+3] = src[i+3];
    }
}

JSBool
cairo::JSSurface::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative;

    cairo::JSSurface * myNewObject = 0;

    dom::NodePtr myImageNode;
    std::string  myPath;

    if (argc == 1) {

        if(convertFrom(cx, argv[0], myImageNode)) {

            ImagePtr myImage = myImageNode->getFacade<Image>();

            int myWidth = myImage->get<ImageWidthTag>();
            int myHeight = myImage->get<ImageHeightTag>();
            string myPixelFormat = myImage->get<RasterPixelFormatTag>();

            ResizeableRasterPtr myRaster = myImage->getRasterPtr();
            unsigned char *myData = myRaster->pixels().begin();

            int myStride;
            cairo_format_t myFormat;

            if (myPixelFormat == "BGRA") {
                myStride = myWidth * 4;
                myFormat = CAIRO_FORMAT_ARGB32;
            } else if (myPixelFormat == "RGBA") {
                AC_WARNING << "Converting raster for image " << myImage->get<NameTag>() << " to BGRA";
                myImage->createRaster(myWidth, myHeight, 1, y60::BGRA);
                convertRGBAtoBGRA(myRaster, myImage->getRasterPtr());
                myRaster = myImage->getRasterPtr();
                myData = myRaster->pixels().begin();
                myStride = myWidth * 4;
                myFormat = CAIRO_FORMAT_ARGB32;
            } else {
                AC_ERROR << "Pixel format of image " << myImage->get<NameTag>() << " not supported by JSCairo: " << myPixelFormat;
                throw UnsupportedPixelFormat("Pixel format not supported by JSCairo: " + myPixelFormat, PLUS_FILE_LINE);
            }

            cairo_surface_t *mySurface = cairo_image_surface_create_for_data(myData, myFormat, myWidth, myHeight, myStride);

            newNative = NATIVE::get(mySurface);

            cairo_surface_destroy(mySurface);

        } else if(convertFrom(cx, argv[0], myPath)) {

            cairo_surface_t *mySurface = cairo_image_surface_create_from_png(myPath.c_str());

            newNative = NATIVE::get(mySurface);

            cairo_surface_destroy(mySurface);

        } else {
            JS_ReportError(cx, "Need an image node or png path to construct a cairo context.");
            return JS_FALSE;
        }

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    JSCairoSurfaceWrapper::STRONGPTR _myOwnerPtr(newNative);
    myNewObject = new cairo::JSSurface(dynamic_cast_Ptr<NATIVE>(_myOwnerPtr), newNative);

    JSCairoSurfaceWrapper::WEAKPTR   _mySelfPtr(_myOwnerPtr);
    newNative->setSelfPtr(_mySelfPtr);

    myNewObject->_myImageNode = myImageNode;

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);

        return JS_TRUE;
    }
    JS_ReportError(cx,"cairo::JSSurface::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
cairo::JSSurface::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        // name                id                       value

        {0}
    };
    return myProperties;
};

void
cairo::JSSurface::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("Cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
}

JSObject *
cairo::JSSurface::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "cairo::JSSurface::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, cairo::JSSurface::OWNERPTR theOwner, cairo::JSSurface::NATIVE * theNative) {
    JSObject * myReturnObject = cairo::JSSurface::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, cairo::JSSurface::OWNERPTR theOwner, cairo_surface_t * theNative) {
    JSCairoSurfaceWrapper *myWrapper = dynamic_cast<JSCairoSurfaceWrapper*>(JSCairoSurfaceWrapper::get(theNative));
    return as_jsval(cx, theOwner, myWrapper);
}

bool convertFrom(JSContext *cx, jsval theValue, cairo::JSSurface::NATIVE *& theTarget) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<cairo::JSSurface::NATIVE>::Class()) {
                JSClassTraits<cairo::JSSurface::NATIVE>::ScopedNativeRef myObj(cx, myArgument);
                theTarget = &myObj.getNative();
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, cairo_surface_t *& theTarget) {
    cairo::JSSurface::NATIVE *myWrapper;

    if(convertFrom(cx, theValue, myWrapper)) {
        theTarget = myWrapper->getWrapped();
        return true;
    }

    return false;
}

}

