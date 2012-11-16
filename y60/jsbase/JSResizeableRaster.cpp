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

// own header
#include "JSResizeableRaster.h"

#include "JSNode.h"
#include "JSVector.h"
#include "JSBlock.h"
#include "JSWrapper.impl"

#include <iostream>

using namespace std;

namespace jslib {
typedef dom::ResizeableRaster NATIVE;
typedef JSWrapper<NATIVE,dom::ValuePtr> Base;

template class JSWrapper<dom::ResizeableRaster,
                       dom::ValuePtr,
                       VectorValueAccessProtocol>;

static JSBool
fillRect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Fills a recangular region of the raster with a given color.");
    DOC_PARAM("xmin", "left", DOC_TYPE_INTEGER);
    DOC_PARAM("ymin", "top", DOC_TYPE_INTEGER);
    DOC_PARAM("xmax", "right", DOC_TYPE_INTEGER);
    DOC_PARAM("ymax", "bottom", DOC_TYPE_INTEGER);
    DOC_PARAM("theColor", "RGBA colors between zero and one", DOC_TYPE_VECTOR4F);
    DOC_END;

    return Method<NATIVE>::call(&NATIVE::fillRect,cx,obj,argc,argv,rval);
}

static JSBool
fillRectAlpha(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Fills a recangular region of the raster with alpha.");
    DOC_PARAM("xmin", "left", DOC_TYPE_INTEGER);
    DOC_PARAM("ymin", "top", DOC_TYPE_INTEGER);
    DOC_PARAM("xmax", "right", DOC_TYPE_INTEGER);
    DOC_PARAM("ymax", "bottom", DOC_TYPE_INTEGER);
    DOC_PARAM("theAlpha", "the Alpha value between zero and one", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theAlphas", "the Alpha values between zero and one", DOC_TYPE_VECTOROFFLOAT);
    DOC_END;
    NativeRef<dom::ResizeableRaster> myNativeRef(cx, obj);

    try {
        ensureParamCount(argc, 5);
        int myXMin;
        convertFrom(cx, argv[0], myXMin);
        int myYMin;
        convertFrom(cx, argv[1], myYMin);
        int myXMax;
        convertFrom(cx, argv[2], myXMax);
        int myYMax;
        convertFrom(cx, argv[3], myYMax);
        if (JSVAL_IS_OBJECT(argv[4])) {
            std::vector<float> myAlphas;
            if ( ! convertFrom(cx, argv[4], myAlphas)) {
                JS_ReportError(cx, "argument 4 must be a vector of floats");
                return JS_FALSE;
            }
            myNativeRef.getValue().fillRectAlpha(myXMin, myYMin, myXMax, myYMax, myAlphas);
            return JS_TRUE;
        } else {
            float myAlpha;
            if ( ! convertFrom(cx, argv[4], myAlpha)) {
                JS_ReportError(cx, "argument 4 must be a float");
                return JS_FALSE;
            }
            myNativeRef.getValue().fillRectAlpha(myXMin, myYMin, myXMax, myYMax, myAlpha);
            return JS_TRUE;
        }
    } HANDLE_CPP_EXCEPTION;
    
}

static JSBool
randomize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Fills the raster with random noise.");
    DOC_PARAM("theMinColor", "the lower color bound (per channel)", DOC_TYPE_VECTOR4F);
    DOC_PARAM("theMaxColor", "the upper color bound (per channel)", DOC_TYPE_VECTOR4F);
    DOC_END;

    return Method<NATIVE>::call(&NATIVE::randomize,cx,obj,argc,argv,rval);
}

static JSBool
getPixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get color of pixel.");
    DOC_PARAM("x", "x pos", DOC_TYPE_INTEGER);
    DOC_PARAM("y", "y pos", DOC_TYPE_INTEGER);
    DOC_RVAL("Color", DOC_TYPE_VECTOR4F);
    DOC_END;
    NativeRef<dom::ResizeableRaster> myNativeRef(cx,obj);
    int myXPos;
    convertFrom(cx, argv[0], myXPos);
    int myYPos;
    convertFrom(cx, argv[1], myYPos);
    if (myXPos < 0 || myYPos < 0 || myXPos >= static_cast<int>(myNativeRef.getValue().width()) ||
        myYPos >= static_cast<int>(myNativeRef.getValue().height()))
    {
        *rval = JSVAL_NULL;
        return JS_TRUE;
    } else {
        return Method<NATIVE>::call(&NATIVE::getPixel,cx,obj,argc,argv,rval);
    }
}

static JSBool
setPixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set color of pixel.");
    DOC_PARAM("x", "x pos", DOC_TYPE_INTEGER);
    DOC_PARAM("y", "y pos", DOC_TYPE_INTEGER);
    DOC_PARAM("theColor", "RGBA colors between zero and one", DOC_TYPE_VECTOR4F);
    DOC_END;

    return Method<NATIVE>::call(&NATIVE::setPixel,cx,obj,argc,argv,rval);
}

static JSBool
resize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Destructive resize of the raster; the content is undefined after the operation, use 'resample' to create a smaller or larger version of the raster.");

    DOC_PARAM("theWidth", "New width of the raster", DOC_TYPE_INTEGER);
    DOC_PARAM("theHeight", "New height of the raster", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::resize,cx,obj,argc,argv,rval);
}

static JSBool
resample(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Resizes the raster using bilinear resampling. Does not work on DXT compressed formats. Top and left edge of the sampling area are the top resp. left edge of the source area. The bottom and right edge of the source raster are never reached during resampling. Therefore it is possible to create a mosaic of resampled images by using a one pixel overlap for the source rectangles.");
    DOC_PARAM("theWidth", "New width of the raster", DOC_TYPE_INTEGER);
    DOC_PARAM("theHeight", "New height of the raster", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::resample,cx,obj,argc,argv,rval);
}

static JSBool
assignBlock(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("assigns a block of raw memory as new pixel data; width and height of the raster are set to argument values; the pixelformat and the width/height must match the raw data to get the expected result");
    DOC_PARAM("theWidth", "New width of the raster", DOC_TYPE_INTEGER);
    DOC_PARAM("theHeight", "New height of the raster", DOC_TYPE_INTEGER);
    DOC_PARAM("thePixels", "Block of raw memory containing pixel data", DOC_TYPE_BLOCK);
    DOC_END;
    NativeRef<dom::ResizeableRaster> myNativeRef(cx, obj);
    try {
        ensureParamCount(argc, 3);
        asl::AC_SIZE_TYPE myWidth;
        convertFrom(cx, argv[0], myWidth);
        asl::AC_SIZE_TYPE myHeight;
        convertFrom(cx, argv[1], myHeight);
        asl::Block * myPixels = 0;
        convertFrom(cx, argv[1], myPixels);
        myNativeRef.getValue().assign(myWidth, myHeight, *myPixels);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
pasteRaster(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("pastes a rectangular region from another raster into a rectangular region of this raster; when source rectangle and target rectangle differ, bilinear resampling will be performed. When 0 or no argument is passed in the width or height arguments, then the width or height is the 'rest' of the region from the specified origin to the rightmost or bottommost coordinate is used");
    DOC_PARAM("theSource", "The source raster ", DOC_TYPE_RASTER);
    DOC_PARAM_OPT("sourceX", "left bound of the source rectangle in the source raster", DOC_TYPE_INTEGER, 0);
    DOC_PARAM_OPT("sourceY", "top bound of the source rectangle in the source raster", DOC_TYPE_INTEGER, 0);
    DOC_PARAM_OPT("sourceWidth", "width of the source rectangle in the source raster", DOC_TYPE_INTEGER,"theSource.width-sourceX");
    DOC_PARAM_OPT("sourceHeight", "height of the source rectangle in the source raster", DOC_TYPE_INTEGER,"theSource.height-sourceX");
    DOC_PARAM_OPT("targetX", "left bound of the target rectangle in the raster", DOC_TYPE_INTEGER, 0);
    DOC_PARAM_OPT("targetY", "top bound of the target rectangle in the raster", DOC_TYPE_INTEGER, 0);
    DOC_PARAM_OPT("targetWidth", "width of the target rectangle in the raster", DOC_TYPE_INTEGER,"this.width-targetX");
    DOC_PARAM_OPT("targetHeight", "height of the target rectangle in the raster", DOC_TYPE_INTEGER,"this.height-targetY");
    DOC_END;

    if (argc == 0) {
        JS_ReportError(cx, "pasteRaster(): need at least 1 argument");
        return JS_FALSE;
    }

    NativeRef<dom::ResizeableRaster> myNativeRef(cx,obj);
    try {
        if (!JSVAL_IS_OBJECT(argv[0])) {
            throw BadArgumentException("pasteRaster: argument 0 is not a raster",PLUS_FILE_LINE);
        }
        const dom::ResizeableRaster & mySourceRaster = JSClassTraits<dom::ResizeableRaster>::getNativeRef(cx, JSVAL_TO_OBJECT(argv[0]));
        asl::AC_OFFSET_TYPE myArg[8]={0,0,0,0,0,0,0,0};
        for (uintN i = 0; i < argc-1; ++i) {
            if (!convertFrom(cx,argv[i+1],myArg[i])) {
                throw BadArgumentException(std::string("pasteRaster: argument ")+asl::as_string(i+1)+" is not a valid number",PLUS_FILE_LINE);
            }
        }
        const dom::ValueBase & myRasterValue = dynamic_cast<const dom::ValueBase &>(mySourceRaster);
        myNativeRef.getValue().pasteRaster(myRasterValue, myArg[0], myArg[1], myArg[2], myArg[3], myArg[4], myArg[5], myArg[6], myArg[7]);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Clears the raster to black.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::clear,cx,obj,argc,argv,rval);
}

static JSBool
save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Saves the raw raster data to a file without any metainformation; to save as .jpeg or other standard image format, use saveImage().");
    DOC_PARAM("theFilename", "Filename and path where to save the image.", DOC_TYPE_STRING);
    DOC_END;
    //JS_ReportError(cx, "save(): not yet implemented, use saveImage instead");
    return JS_FALSE;

/*
    try {
        const dom::ResizeableRaster & mySourceRaster = JSClassTraits<dom::ResizeableRaster>::getNativeRef(cx, obj);
       if (argc != 1) {
           JS_ReportError(cx, "raster.save(): expects at one argument: file name.");
            return JS_FALSE;
        }

        string myFileName;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[1], myFileName)) {
            JS_ReportError(cx, "raster.save: argument #1 must be a string. (theFilename)");
            return JS_FALSE;
        }
        asl::WriteableFile myFile(myFileName);
        if (!myFile) {
            JS_ReportError(cx, "raster.save: could not open file '%s'", myFileName.c_str());
            return JS_FALSE;
        }
        myFile.WriteableStream::append(mySourceRaster.pixels());
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
*/
}

static JSBool
load(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("loads a block of raw data as new pixel data; width and height of the raster are set to argument values; the pixelformat and the width/height must match the raw data to get the expected result");
    DOC_PARAM("theWidth", "New width of the raster", DOC_TYPE_INTEGER);
    DOC_PARAM("theHeight", "New height of the raster", DOC_TYPE_INTEGER);
    DOC_PARAM("theFileName", "raw file containing pixel data", DOC_TYPE_STRING);
    DOC_END;
    NativeRef<dom::ResizeableRaster> myNativeRef(cx, obj);
    try {
        ensureParamCount(argc, 3);
        asl::AC_SIZE_TYPE myWidth;
        convertFrom(cx, argv[0], myWidth);
        asl::AC_SIZE_TYPE myHeight;
        convertFrom(cx, argv[1], myHeight);
        string myFileName;
        convertFrom(cx, argv[2], myFileName);
        asl::ReadableFile myFile(myFileName);
        if (!myFile) {
            JS_ReportError(cx, "raster.load: could not open file '%s'", myFileName.c_str());
            JSClassTraits<dom::ResizeableRaster>::closeNativeRef(cx, obj);
            return JS_FALSE;
        }
        myNativeRef.getValue().resize(myWidth, myHeight);
        if (myNativeRef.getValue().pixels().size() == myFile.size()) {
            myFile.readBytes(myNativeRef.getValue().pixels().begin(), myNativeRef.getValue().pixels().size(), 0);
            JSClassTraits<dom::ResizeableRaster>::closeNativeRef(cx, obj);
        } else {
            JS_ReportError(cx, "raster.load: could not read pixels from file '%s', file size = %d, pixel map size = %d",
                           myFileName.c_str(), myFile.size(), myNativeRef.getValue().pixels().size());
            JSClassTraits<dom::ResizeableRaster>::closeNativeRef(cx, obj);
            return JS_FALSE;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


JSFunctionSpec *
JSResizeableRaster::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name         native          nargs    */
        {"getPixel",    getPixel,         2},
        {"setPixel",    setPixel,         3},
        {"fillRect",    fillRect,         5},
        {"fillRectAlpha", fillRectAlpha,  5},
        {"randomize",   randomize,        2},
        {"resize",      resize,           2},
        {"resample",    resample,         2},
        {"pasteRaster", pasteRaster,      9},
        {"clear",       clear,            0},
        {"save",        save,             1},
        {"load",        load,             3},
        {"assignBlock", assignBlock,      3},
        {0}
    };
    return myFunctions;
}
enum PropertyNumbers {PROP_width = -100,
                      PROP_height = -101,
                      PROP_size = -102};

JSPropertySpec *
JSResizeableRaster::Properties() {
    static JSPropertySpec myProperties[] = {
        {"width", PROP_width, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {"height", PROP_height, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {"size", PROP_size, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSResizeableRaster::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSResizeableRaster::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSResizeableRaster::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSResizeableRaster::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_width:
                *vp = as_jsval(cx, getNative().width());
                return JS_TRUE;
            case PROP_height:
                *vp = as_jsval(cx, getNative().height());
                return JS_TRUE;
            case PROP_size:
                *vp = as_jsval(cx, getNative().getSize());
                {AC_DEBUG << "getNative().getSize() = " << getNative().getSize();}
                return JS_TRUE;
             default:
                JS_ReportError(cx,"JSResizeableRaster::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}
/*
JSBool JSResizeableRaster::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    *vp = as_jsval(cx, getNative().getElement(theIndex));
    return JS_TRUE;
}
*/
// setproperty handling
JSBool JSResizeableRaster::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JS_ReportError(cx,"JSResizeableRaster::setPropertySwitch: index %d out of range", theID);
    return JS_FALSE;
}
/*
JSBool JSResizeableRaster::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    dom::ValuePtr myArg;
    if (convertFrom(cx, *vp, myArg)) {
        IF_NOISY_Y(AC_TRACE << "JSResizeableRaster::setPropertyIndex theIndex =" << theIndex << " myArg: " << (void*)myArg.get() << endl);
        openNative().setElement(theIndex,*myArg);
        closeNative();
        return JS_TRUE;
    }
    return JS_TRUE;
}
*/
JSBool
JSResizeableRaster::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new resizable raster");
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSResizeableRaster * myNewObject = 0;
    if (argc == 0) {
        myNewObject=new JSResizeableRaster();
    } else {
        JS_ReportError(cx,"Constructor for %s: superflous argument ignored",ClassName());
        myNewObject=new JSResizeableRaster();
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSResizeableRaster::Constructor: bad parameters");
    return JS_FALSE;
}


jsval as_jsval(JSContext *cx, dom::ValuePtr theValuePtr, dom::ResizeableRaster *) {
    JSObject * myObject = JSResizeableRaster::Construct(cx, theValuePtr);
    return OBJECT_TO_JSVAL(myObject);
}

}
