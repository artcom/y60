//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSCTScan.cpp,v $
//   $Author: christian $
//   $Revision: 1.12 $
//   $Date: 2005/04/28 17:12:56 $
//
//
//=============================================================================

#include "JSCTScan.h"

#include <y60/JSApp.h>
#include <y60/JSVector.h>
#include <y60/JSBox.h>
#include <y60/JSNode.h>
#include <y60/Image.h>
#include <y60/JSScene.h>
#include <y60/JSSphere.h>
#include <y60/JSVector.h>
#include <dom/Nodes.h>

#include <asl/PackageManager.h>
#include <jsgtk/JSSignal2.h>
#include <asl/raster.h>
#include <y60/JSResizeableRaster.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;
using namespace dom;

#define DB(x) // x
#define SEGBITMAP_TEST

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    const CTScan & myNative = JSCTScan::getJSWrapper(cx,obj).getNative();
    string myStringRep = string("CTScan ");
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
loadSlices(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        if (argc != 2) {
            JS_ReportError(cx, "JSCTScan::loadSlices(): Wrong number of arguments, "
                               "expected 2 (theSubDir, thePackage), got %d.", argc);
            return JS_FALSE;
        }
        string mySubDir;
        if (!JSVAL_IS_NULL(argv[0])) {
            convertFrom(cx, argv[0], mySubDir);
        }
        string myPackage;
        convertFrom(cx, argv[1], myPackage);
        CTScan & myCTScan = myObj.getNative();

        // [TS] Couldn't make it work in acxpshell without this... 
        PackageManager myPackageManager;   
        myPackageManager.add(*JSApp::getPackageManager());   
        myPackageManager.add(myPackage); 

        //int slicesLoaded = myCTScan.loadSlices(*JSApp::getPackageManager(), mySubDir, myPackage);
        int slicesLoaded = myCTScan.loadSlices(myPackageManager, mySubDir, myPackage);
        *rval = as_jsval(cx, slicesLoaded);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::clear,cx,obj,argc,argv,rval);
}

static JSBool
verifyCompleteness(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::verifyCompleteness,cx,obj,argc,argv,rval);
}

static JSBool
getVoxelDimensions(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::getVoxelDimensions,cx,obj,argc,argv,rval);
}

static JSBool
setVoxelSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::setVoxelSize,cx,obj,argc,argv,rval);
}

static JSBool
getVoxelSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::getVoxelSize,cx,obj,argc,argv,rval);
}

static JSBool
getValueRange(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::getValueRange,cx,obj,argc,argv,rval);
}

static JSBool
computeHistogram(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);

        ensureParamCount(argc, 1);

        // have to convert a Box3f into a Box3i because Box3i isn't available in JS
        Box3f myFloatBox;
        convertFrom(cx, argv[0], myFloatBox);
        Box3i myVoxelBox(static_cast<int>( myFloatBox[Box3f::MIN][0]),
                              static_cast<int>( myFloatBox[Box3f::MIN][1]),
                              static_cast<int>( myFloatBox[Box3f::MIN][2]),
                              static_cast<int>( myFloatBox[Box3f::MAX][0]),
                              static_cast<int>( myFloatBox[Box3f::MAX][1]),
                              static_cast<int>( myFloatBox[Box3f::MAX][2]));

        vector<unsigned> myHistogram;
        
        CTScan & myCTScan = myObj.getNative();
        myCTScan.computeHistogram(myVoxelBox, myHistogram);
         *rval = as_jsval(cx, myHistogram);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


static JSBool
reconstructToImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        if (argc != 3) {
            JS_ReportError(cx, "JSCTScan::reconstructToImage(): Wrong number of arguments, "
                               "expected 3 (Orientation, SliceIndex, Image Node), got %d.", argc);
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "JSCTScan::reconstructToImage(): Argument #0 is undefined");
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[1])) {
            JS_ReportError(cx, "JSCTScan::reconstructToImage(): Argument #1 is undefined");
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[2])) {
            JS_ReportError(cx, "JSCTScan::reconstructToImage(): Argument #2 is undefined");
            return JS_FALSE;
        }
        int myOrientation = 0;
        convertFrom(cx, argv[0], myOrientation);
        int mySlice = 0;
        convertFrom(cx, argv[1], mySlice);
        dom::NodePtr myImageNode;
        convertFrom(cx, argv[2], myImageNode);

        CTScan & myCTScan = myObj.getNative();
        myCTScan.reconstructToImage(CTScan::Orientation(myOrientation), mySlice, myImageNode);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
countTrianglesGlobal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        CTScan & myCTScan = myObj.getNative();

        ensureParamCount(argc, 4);

        // have to convert a Box3f into a Box3i because Box3i isn't available in JS
        Box3f myFloatBox;
        convertFrom(cx, argv[0], myFloatBox);
        Box3i myVoxelBox(static_cast<int>( myFloatBox[Box3f::MIN][0]),
                              static_cast<int>( myFloatBox[Box3f::MIN][1]),
                              static_cast<int>( myFloatBox[Box3f::MIN][2]),
                              static_cast<int>( myFloatBox[Box3f::MAX][0]),
                              static_cast<int>( myFloatBox[Box3f::MAX][1]),
                              static_cast<int>( myFloatBox[Box3f::MAX][2]));
        double myThresholdMin;
        convertFrom(cx, argv[1], myThresholdMin);
        double myThresholdMax;
        convertFrom(cx, argv[2], myThresholdMax);
        int myDownSampleRate;
        convertFrom(cx, argv[3], myDownSampleRate);

        Vector2i myCount = myCTScan.countTrianglesGlobal(myVoxelBox, myThresholdMin, myThresholdMax, myDownSampleRate);
        *rval = as_jsval(cx, myCount);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
countTrianglesInVolumeMeasurement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        CTScan & myCTScan = myObj.getNative();

        ensureParamCount(argc, 3);

        dom::NodePtr myThresholdPalette;
        convertFrom(cx, argv[0], myThresholdPalette);

        dom::NodePtr myMeasurementNode;
        convertFrom(cx, argv[1], myMeasurementNode);

        int myDownSampleRate;
        convertFrom(cx, argv[2], myDownSampleRate);

        Vector2i myCount = myCTScan.countTrianglesInVolumeMeasurement(myMeasurementNode, myThresholdPalette, myDownSampleRate);
        *rval = as_jsval(cx, myCount);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
polygonizeGlobal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        CTScan & myCTScan = myObj.getNative();

        ensureParamCount(argc, 5, 7);

        // have to convert a Box3f into a Box3i because Box3i isn't available in JS
        Box3f myFloatBox;
        convertFrom(cx, argv[0], myFloatBox);
        Box3i myVoxelBox(static_cast<int>( myFloatBox[Box3f::MIN][0]),
                              static_cast<int>( myFloatBox[Box3f::MIN][1]),
                              static_cast<int>( myFloatBox[Box3f::MIN][2]),
                              static_cast<int>( myFloatBox[Box3f::MAX][0]),
                              static_cast<int>( myFloatBox[Box3f::MAX][1]),
                              static_cast<int>( myFloatBox[Box3f::MAX][2]));
        double myThresholdMin;
        convertFrom(cx, argv[1], myThresholdMin);
        double myThresholdMax;
        convertFrom(cx, argv[2], myThresholdMax);

        int myDownSampleRate;
        convertFrom(cx, argv[3], myDownSampleRate);

        bool myCreateNormalsFlag;
        convertFrom(cx, argv[4], myCreateNormalsFlag);

        unsigned myNumVertices = 0;
        if (argc > 5) {
            convertFrom(cx, argv[5], myNumVertices);
        }
        unsigned myNumTriangles = 0;
        if (argc > 6) {
            convertFrom(cx, argv[6], myNumTriangles);
        }

        ScenePtr myScene = myCTScan.polygonizeGlobal(myVoxelBox, myThresholdMin, myThresholdMax, myDownSampleRate,  
            myCreateNormalsFlag, JSApp::getPackageManager(), myNumVertices, myNumTriangles);
        *rval = as_jsval(cx, myScene);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
polygonizeVolumeMeasurement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        CTScan & myCTScan = myObj.getNative();

        ensureParamCount(argc, 5, 7);

        dom::NodePtr myThresholdPalette;
        convertFrom(cx, argv[0], myThresholdPalette);

        dom::NodePtr myMeasurementNode;
        convertFrom(cx, argv[1], myMeasurementNode);

        int myDownSampleRate;
        convertFrom(cx, argv[3], myDownSampleRate);

        bool myCreateNormalsFlag;
        convertFrom(cx, argv[4], myCreateNormalsFlag);

        unsigned myNumVertices = 0;
        if (argc > 5) {
            convertFrom(cx, argv[5], myNumVertices);
        }
        unsigned myNumTriangles = 0;
        if (argc > 6) {
            convertFrom(cx, argv[6], myNumTriangles);
        }

        ScenePtr myScene = myCTScan.polygonizeVolumeMeasurement(myMeasurementNode, myThresholdPalette,
                myDownSampleRate,  myCreateNormalsFlag, JSApp::getPackageManager(), myNumVertices,
                myNumTriangles);
        *rval = as_jsval(cx, myScene);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
/*
static JSBool
getStencilRaster(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        CTScan & myCTScan = myObj.getNative();

        ensureParamCount(argc, 1);

        int mySliceNumber;
        convertFrom(cx, argv[0], mySliceNumber);
        dom::ValuePtr myRaster = dynamic_cast_Ptr<dom::ValueBase>(myCTScan.getStencil()[mySliceNumber]);
        dom::ResizeableRaster * myDummy = 0;
        *rval = as_jsval(cx, myRaster, myDummy);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
appendStencil(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        CTScan & myCTScan = myObj.getNative();

        ensureParamCount(argc, 1);

        dom::NodePtr myImage;
        convertFrom(cx, argv[0], myImage);
        myCTScan.appendStencil(myImage);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
*/
static JSBool
createRGBAImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 4);
        dom::NodePtr myImagesNode;
        convertFrom(cx, argv[0], myImagesNode);
        int myWidth;
        convertFrom(cx, argv[1], myWidth);
        int myHeight;
        convertFrom(cx, argv[2], myHeight);
        unsigned char myValue;
        convertFrom(cx, argv[3], myValue);
        asl::Block myBlock(myWidth * myHeight * 4, myValue);
        dom::NodePtr myImage(new dom::Element("image"));
        myImagesNode->appendChild(myImage);
        myImage->getFacade<Image>()->set<ImageMipmapTag>(false);
        myImage->getFacade<Image>()->set(myWidth, myHeight, 1, y60::RGBA, myBlock);
        *rval = as_jsval(cx, myImage);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
createGrayImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 4);
        dom::NodePtr myImagesNode;
        convertFrom(cx, argv[0], myImagesNode);
        int myWidth;
        convertFrom(cx, argv[1], myWidth);
        int myHeight;
        convertFrom(cx, argv[2], myHeight);
        unsigned char myValue;
        convertFrom(cx, argv[3], myValue);

        CTScan::createGrayImage(myImagesNode, myWidth, myHeight, myValue);
/*        
        asl::Block myBlock(myWidth * myHeight, myValue);
        dom::NodePtr myImage(new dom::Element("image"));
        myImagesNode->appendChild(myImage);
        myImage->getFacade<Image>()->set<ImageMipmapTag>(false);
        myImage->getFacade<Image>()->set(myWidth, myHeight, 1, y60::GRAY, myBlock);        
        *rval = as_jsval(cx, myImage);
*/        
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
resizeVoxelVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 2);
        dom::NodePtr myVoxelVolume;
        convertFrom(cx, argv[0], myVoxelVolume);
        if ( ! myVoxelVolume) {
            JS_ReportError(cx, "Argument 0 must be a Node");
            return JS_FALSE;
        } 

        Box3f myDirtyBox;
        convertFrom(cx, argv[1], myDirtyBox);

        const Box3f & myOldBox = myVoxelVolume->getAttributeValue<Box3f>("boundingbox");

        Box3f myNewBox = myOldBox;
        myNewBox.extendBy( myDirtyBox );

        if (myNewBox == myOldBox) {
            AC_INFO << "Raster box didn't change.";
            return JS_TRUE;
        }
        
        const Vector3f & mySize = myNewBox.getSize();

        int myTargetWidth  = int( mySize[0] );
        int myTargetHeight = int( mySize[1] );
        AC_INFO << "target width = " << myTargetWidth << " target height = " << myTargetHeight;
        int myNewRasterCount  = int( mySize[2] );
       
        int myNewBoxBegin = int( myNewBox[Box3f::MIN][2] );
        int myNewBoxEnd   = int( myNewBox[Box3f::MAX][2] );

        int myOldBoxBegin = int( myOldBox[Box3f::MIN][2] );
        int myOldBoxEnd   = int( myOldBox[Box3f::MAX][2] );
       
        dom::NodePtr myRasters = myVoxelVolume->childNode("rasters");
        dom::NodePtr myOldRaster;
        dom::NodePtr myNewRaster;

        unsigned myXOrigin = unsigned( myOldBox[Box3f::MIN][0] - myNewBox[Box3f::MIN][0]);
        unsigned myYOrigin = unsigned( myOldBox[Box3f::MIN][1] - myNewBox[Box3f::MIN][1]);
        AC_INFO << "blitting to " << myXOrigin << "x" << myYOrigin;

        for(int i = myNewBoxBegin;i < myNewBoxEnd; ++i) {
            if (i >= myOldBoxBegin && i < myOldBoxEnd) {
                myOldRaster = myRasters->childNode(0);
                AC_INFO << "Replacing " << i;
            } else {
                AC_INFO << "Creating new " << i;
            }
            myNewRaster = CTScan::createGrayImage(myRasters, myTargetWidth, myTargetHeight, 0);
            if (i >= myOldBoxBegin && i < myOldBoxEnd) {
                ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>(
                        myNewRaster->childNode(0)->nodeValueWrapperPtr());
                ValuePtr mySourceRaster = myOldRaster->childNode(0)->nodeValueWrapperPtr();
                myTargetRaster->pasteRaster(myXOrigin, myYOrigin, * mySourceRaster); 
                myRasters->removeChild(myOldRaster);
            }
        }

        myVoxelVolume->getAttribute("boundingbox")->nodeValueAssign(myNewBox);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

struct Vector3iCmp {
    bool operator() (const Vector3i & a, const Vector3i & b) const {
            return a[0] < b[0] || a[1] < b[1] || a[2] < b[2];
    }
};

typedef std::map<Vector3i, unsigned char, Vector3iCmp> ColorMap;

static JSBool
copyCanvasToVoxelVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 4);

        dom::NodePtr myMeasurement;
        convertFrom(cx, argv[0], myMeasurement);

        dom::NodePtr myCanvasImage;
        convertFrom(cx, argv[1], myCanvasImage);

        Box3f myDirtyBox;
        convertFrom(cx, argv[2], myDirtyBox);
        
        int mySliceOrientationInt;
        convertFrom(cx, argv[3], mySliceOrientationInt);
        CTScan::Orientation myOrientation = static_cast<CTScan::Orientation>( mySliceOrientationInt );

        dom::NodePtr myPaletteNode;
        convertFrom(cx, argv[4], myPaletteNode);

       
        ColorMap myColorMap;
        Vector3i myColor;
        unsigned char myIndex;
        for (unsigned i = 0; i < myPaletteNode->childNodesLength(); ++i) {
            dom::NodePtr myItem = myPaletteNode->childNode(i);
            myColor = myItem->getAttributeValue<Vector3i>("color");
            myIndex = (unsigned char)(myItem->getAttributeValue<int>("index"));
            myColorMap[myColor] = myIndex;
        }
        
        ResizeableRasterPtr myCanvas = dynamic_cast_Ptr<ResizeableRaster>(
                    myCanvasImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());
        
        Box3f myMeasurementBox = myMeasurement->getAttributeValue<Box3f>("boundingbox");
        
        switch (myOrientation) {
            case CTScan::IDENTITY:
                {
                    Vector4f myFloatPixel;
                    Vector3i myPixel;
                    unsigned myAffectedSlice = unsigned( myDirtyBox[Box3f::MIN][2] - myMeasurementBox[Box3f::MIN][2]);
                    unsigned myXStart = unsigned(myDirtyBox[Box3f::MIN][0]);
                    unsigned myXEnd   = unsigned(myDirtyBox[Box3f::MAX][0]);
                    unsigned myYStart = unsigned(myDirtyBox[Box3f::MIN][1]);
                    unsigned myYEnd   = unsigned(myDirtyBox[Box3f::MAX][1]);
                    AC_DEBUG << "affected slice = " << myAffectedSlice;
                    dom::NodePtr myRasterNode = myMeasurement->childNode("rasters")->childNode(myAffectedSlice);
                    if ( ! myRasterNode) {
                        throw asl::Exception("Failed to get affected raster.");
                    }
                    ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>(
                            myRasterNode->childNode(0)->nodeValueWrapperPtr());

                    for (unsigned y = myYStart; y < myYEnd; ++y) {
                        for (unsigned x = myXStart; x < myXEnd; ++x) {
                            myFloatPixel = myCanvas->getPixel(x, y);
                            myPixel = Vector3i(int( myFloatPixel[0]), int( myFloatPixel[1]), int( myFloatPixel[2]));
                            // XXX: argh! map.find() in inner loop... any ideas? [DS]
                            ColorMap::iterator myIt = myColorMap.find(myPixel);
                            if (myIt == myColorMap.end()) {
                                throw asl::Exception("Unknown color in canvas image", PLUS_FILE_LINE);
                            }
                            //cerr << int(myIt->second) << " ";
                            float myValue = float( myIt->second);
                            myTargetRaster->setPixel( x - int(myMeasurementBox[Box3f::MIN][0]),
                                                      y - int(myMeasurementBox[Box3f::MIN][1]),
                                                      myValue, myValue, myValue, myValue);
                        }
                        //cerr << endl;
                    }
                }
                break;
            case CTScan::Y2Z:
                AC_WARNING << "copyCanvasToVoxelVolume() Y2Z not implemented.";
                break;
            case CTScan::X2Z:
                AC_WARNING << "copyCanvasToVoxelVolume() X2Z not implemented.";
                break;
            default:
                break;

        }
        
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
copyVoxelVolumeToCanvas(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
       ensureParamCount(argc, 5);

        dom::NodePtr myMeasurement;
        convertFrom(cx, argv[0], myMeasurement);

        dom::NodePtr myCanvas;
        convertFrom(cx, argv[1], myCanvas);

        unsigned mySliceIndex;
        convertFrom(cx, argv[2], mySliceIndex);

        unsigned mySliceOrientationInt;
        convertFrom(cx, argv[3], mySliceOrientationInt);
        CTScan::Orientation myOrientation = static_cast<CTScan::Orientation>( mySliceOrientationInt );

        dom::NodePtr myPaletteNode;
        convertFrom(cx, argv[4], myPaletteNode);

        std::vector<Vector3i> myPalette(255, Vector3i( -1, -1, -1));
        unsigned myIndex;
        Vector3i myColor;
        for (unsigned i = 0; i < myPaletteNode->childNodesLength(); ++i) {
            myIndex = myPaletteNode->childNode(i)->getAttributeValue<unsigned>("index");
            myColor = myPaletteNode->childNode(i)->getAttributeValue<Vector3i>("color");
            myPalette[myIndex] = myColor;
        }

        Box3f myBoundingBox = myMeasurement->getAttributeValue<Box3f>("boundingbox");

        ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>( 
                    myCanvas->childNode(0)->childNode(0)->nodeValueWrapperPtr());

        switch ( myOrientation) {
            case CTScan::IDENTITY:
                //cerr << "index = " << mySliceIndex << " bbox = " << myBoundingBox[Box3f::MIN][2] << endl;
                if (mySliceIndex >= myBoundingBox[Box3f::MIN][2] && mySliceIndex < myBoundingBox[Box3f::MAX][2]) {
                    dom::NodePtr myRasterNode = myMeasurement->childNode(0)->childNode(int( mySliceIndex - myBoundingBox[Box3f::MIN][2]));
                    ResizeableRasterPtr mySourceRaster =
                            dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());
                    unsigned myXStart = unsigned(myBoundingBox[Box3f::MIN][0]);
                    unsigned myXEnd = unsigned(myBoundingBox[Box3f::MAX][0]);
                    unsigned myYStart = unsigned(myBoundingBox[Box3f::MIN][1]);
                    unsigned myYEnd = unsigned(myBoundingBox[Box3f::MAX][1]);
                    Vector4f myFloatPixel;
                    for (unsigned y = myYStart; y < myYEnd; ++y) {
                        for (unsigned x = myXStart; x < myXEnd; ++x) {
                            myFloatPixel = mySourceRaster->getPixel(x - myXStart, y - myYStart);
                            myIndex = (unsigned char)(myFloatPixel[0]);
                            myColor = myPalette[myIndex];
                            //cerr << "setPixel x = " << x << " y = " << y << endl;
                            myTargetRaster->setPixel(x, y, float(myColor[0]), float(myColor[1]), float(myColor[2]),
                                (myIndex == 0 ? 0.0 : 255.0)); // index zero is our erase color
                        }
                    }
                    //cerr << "done" << endl;
                }
                break;
            case CTScan::Y2Z:
                AC_WARNING << "copyVoxelVolumeToCanvas() Y2Z not implemented.";
                break;
            case CTScan::X2Z:
                AC_WARNING << "copyVoxelVolumeToCanvas() X2Z not implemented.";
                break;
        }

        return JS_TRUE;
        
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

static JSBool
create3DTexture(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::create3DTexture,cx,obj,argc,argv,rval);
}

#define DEFINE_ORIENTATION_PROP(NAME) { #NAME, PROP_ ## NAME , CTScan::NAME }

JSConstIntPropertySpec *
JSCTScan::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_ORIENTATION_PROP(IDENTITY),
        DEFINE_ORIENTATION_PROP(Y2Z),
        DEFINE_ORIENTATION_PROP(X2Z),
        {0}
    };
    return myProperties;
};

JSFunctionSpec *
JSCTScan::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"loadSlices",           loadSlices,              0},
//        {"renderToImage",        renderToImage,           2},
        {"reconstructToImage",   reconstructToImage,      3},
        {"clear",                clear,                   0},
        {"verifyCompleteness",   verifyCompleteness,      0},
        {"setVoxelSize",         setVoxelSize,            1},
        {"getVoxelSize",         getVoxelSize,            0},
        {"getVoxelDimensions",   getVoxelDimensions,      0},
        {"getValueRange",        getValueRange,           0},
        {"polygonizeGlobal",     polygonizeGlobal,        7},
        {"polygonizeVolumeMeasurement", polygonizeVolumeMeasurement,        7},
        {"countTrianglesGlobal", countTrianglesGlobal,    4},
        {"countTrianglesInVolumeMeasurement", countTrianglesInVolumeMeasurement,    4},
        {"create3DTexture",      create3DTexture,         2},
        {"computeHistogram",     computeHistogram,        1},
        //{"getStencilRaster",     getStencilRaster,        1},
        //{"appendStencil",        appendStencil,           1},        
        {0}
    };
    return myFunctions;
}

JSFunctionSpec *
JSCTScan::StaticFunctions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        {"createRGBAImage",         createRGBAImage,         4},
        {"createGrayImage",         createGrayImage,         4},
        {"resizeVoxelVolume",       resizeVoxelVolume,       2},
        {"copyCanvasToVoxelVolume", copyCanvasToVoxelVolume, 4},
        {"copyVoxelVolumeToCanvas", copyVoxelVolumeToCanvas, 5},
        {0}
    };
    return myFunctions;
}


JSPropertySpec *
JSCTScan::Properties() {
    static JSPropertySpec myProperties[] = {
        {"default_window", PROP_default_window, JSPROP_READONLY | JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_progress", PROP_signal_progress, JSPROP_READONLY | JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"length", PROP_length, JSPROP_READONLY | JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSCTScan::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_default_window:
            *vp = as_jsval(cx, getNative().getDefaultWindow());
            return JS_TRUE;
        case PROP_length:
            *vp = as_jsval(cx, getNative().getSliceCount());
            return JS_TRUE;
        case PROP_signal_progress:
            {
                JSSignal2<void,double,Glib::ustring>::OWNERPTR mySignal( new
                        JSSignal2<void,double,Glib::ustring>::NATIVE(getNative().signal_progress()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            JS_ReportError(cx,"JSCTScan::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSCTScan::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSCTScan::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSCTScan::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    OWNERPTR myNewNative;
    switch (argc) {
        case 0:
            myNewNative = OWNERPTR(new CTScan());
            break;
        default:
            JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected none or one "
                    "(filepath), got %d", ClassName(), argc);
            return JS_FALSE;
    }

    JSCTScan * myNewObject = new JSCTScan(myNewNative, &(*myNewNative));
    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}

JSObject *
JSCTScan::initClass(JSContext *cx, JSObject *theGlobalObject) {
    return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties(), 0, StaticFunctions());
}

bool convertFrom(JSContext *cx, jsval theValue, JSCTScan::OWNERPTR & theCTScan) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSCTScan::NATIVE >::Class()) {
                theCTScan = JSClassTraits<JSCTScan::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSCTScan::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSCTScan::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSCTScan::OWNERPTR theOwner, JSCTScan::NATIVE * theCTScan) {
    JSObject * myObject = JSCTScan::Construct(cx, theOwner, theCTScan);
    return OBJECT_TO_JSVAL(myObject);
}

}
