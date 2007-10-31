//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSResizeableRaster.h"
#include "JSNode.h"
#include "JSVector.h"
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

    return Method<NATIVE>::call(&NATIVE::getPixel,cx,obj,argc,argv,rval);
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

    dom::ResizeableRaster & myObject = JSClassTraits<dom::ResizeableRaster>::openNativeRef(cx, obj);
    try {
        if (!JSVAL_IS_OBJECT(argv[0])) {
            throw BadArgumentException("pasteRaster: argument 0 is not a raster",PLUS_FILE_LINE); 
        }
        const dom::ResizeableRaster & mySourceRaster = JSClassTraits<dom::ResizeableRaster>::getNativeRef(cx, JSVAL_TO_OBJECT(argv[0]));
        asl::AC_OFFSET_TYPE myArg[8]={0,0,0,0,0,0,0,0};
        for (int i = 0; i < argc-1; ++i) {
            if (!convertFrom(cx,argv[i+1],myArg[i])) {
                throw BadArgumentException(std::string("pasteRaster: argument ")+asl::as_string(i+1)+" is not a valid number",PLUS_FILE_LINE); 
            }
        }
        const dom::ValueBase & myRasterValue = dynamic_cast<const dom::ValueBase &>(mySourceRaster);
        myObject.pasteRaster(myRasterValue, myArg[0], myArg[1], myArg[2], myArg[3], myArg[4], myArg[5], myArg[6], myArg[7]);
    } HANDLE_CPP_EXCEPTION;

    JSClassTraits<dom::ResizeableRaster>::closeNativeRef(cx, obj);
    return JS_TRUE;
}

static JSBool
clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Clears the raster to black.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::clear,cx,obj,argc,argv,rval);
}

static JSBool
save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Saves the raster to a file.");
    DOC_PARAM("theFilename", "Filename and path where to save the image. The image format is automatically determined by the file-extension.", DOC_TYPE_STRING);
    DOC_END;
    JS_ReportError(cx, "save(): not yet implemented");
    return JS_FALSE;

    try {
        if (argc != 1) {
			JS_ReportError(cx, "save(): expects at one argument: file name.");
            return JS_FALSE;
        }

        string myFileName;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[1], myFileName)) {
            JS_ReportError(cx, "saveImage(): argument #2 must be a string. (theFilename)");
            return JS_FALSE;
        }
/*
        PLAnyBmp myPLBitmap;
        
        PLPixelFormat myPixelFormat;
        if (!mapPixelEncodingToFormat(getRasterEncoding(), myPixelFormat)) {
              throw ImageException(std::string("Image::convertToPLBmp(): Unsupported Encoding: ") +
                      asl::as_string(getRasterEncoding()), PLUS_FILE_LINE);
        }
        myPLBitmap.Create(myWidth, myHeight, myPixelFormat, NULL, 0, PLPoint(72, 72));
        PLBYTE **myLineArray = myPLBitmap.GetLineArray();
        int myBytesPerLine = myPixelFormat.GetBitsPerPixel() * myWidth / 8;
        const unsigned char *myData = getRasterPtr()->pixels().begin();
        for (unsigend y=0; y < myHeight; ++y) {
            memcpy(myLineArray[y], myData + myBytesPerLine * y, myBytesPerLine);
        }

        string myImagePath = toLowerCase(myFileName);
        string::size_type pos = string::npos;

        pos = myImagePath.find_last_of(".");
        if ( pos != string::npos) {
            string myExtension = myImagePath.substr(pos);

            if (myExtension == ".jpg" || myExtension == ".jpeg") {
                // [jb] PLJPEGEncoder expects images to have alpha (32bit),
                //      this hack stuffs the pixelformat with the missing 8bit:
                PLAnyBmp myTmpBmp;
                myTmpBmp.CreateCopy(myPLBitmap, PLPixelFormat::X8B8G8R8);
                PLJPEGEncoder myJPEGEncoder;
                myJPEGEncoder.MakeFileFromBmp(Path(theImagePath, UTF8).toLocale().c_str(), &myTmpBmp);
                return JS_TRUE;
            } else if (myExtension == ".tif" || myExtension == ".tiff") {
                PLTIFFEncoder myTiffEncoder;
                myTiffEncoder.MakeFileFromBmp(Path(myFileName, UTF8).toLocale().c_str(), &myPLBitmap);
                return JS_TRUE;
            }
        }

        PLPNGEncoder myPNGEncoder;
        myPNGEncoder.MakeFileFromBmp(Path(myFileName, UTF8).toLocale().c_str(), &myPLBitmap);
*/
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
        {"randomize",   randomize,        2},
        {"resize",      resize,           2},
        {"resample",    resample,         2},
        {"clear",       clear,            0},
        {"save",        save,             1},
        {0}
    };
    return myFunctions;
}
enum PropertyNumbers {PROP_width = -100,
                      PROP_height = -101};

JSPropertySpec *
JSResizeableRaster::Properties() {
    static JSPropertySpec myProperties[] = {
        {"width", PROP_width, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {"height", PROP_height, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
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
        IF_NOISY_Y(AC_TRACE << "JSResizeableRaster::setPropertyIndex theIndex =" << theIndex << " myArg: " << (void*)myArg.getNativePtr() << endl);
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
