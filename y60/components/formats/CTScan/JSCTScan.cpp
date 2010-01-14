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
//
//   $RCSfile: JSCTScan.cpp,v $
//   $Author: christian $
//   $Revision: 1.12 $
//   $Date: 2005/04/28 17:12:56 $
//
//
//=============================================================================

#include "JSCTScan.h"

#include <y60/jslib/JSApp.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSBox.h>
#include <y60/jsbase/JSNode.h>
#include <y60/image/Image.h>
#include <y60/jslib/JSScene.h>
#include <y60/jsbase/JSSphere.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSPlane.h>
#include <y60/jsbase/JSQuaternion.h>
#include <y60/jsbase/JSMatrix.h>
#include <asl/dom/Nodes.h>

#include <asl/zip/PackageManager.h>
#include <jsgtk/JSSignal2.h>
#include <asl/raster/raster.h>
#include <y60/jsbase/JSResizeableRaster.h>

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
    *rval = as_jsval(cx, "CTScan");
    return JS_TRUE;
}

static JSBool
loadSphere(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        if (argc != 1) {
            JS_ReportError(cx, "JSCTScan::loadSphere(): Wrong number of arguments, "
                               "expected 1 (theSize), got %d.", argc);
            return JS_FALSE;
        }
        asl::Vector3i mySize;
        if (!JSVAL_IS_NULL(argv[0])) {
            convertFrom(cx, argv[0], mySize);
        }
        CTScan & myCTScan = myObj.getNative();
        int slicesLoaded = myCTScan.loadSphere(mySize);
        *rval = as_jsval(cx, slicesLoaded);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
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
getVoxelAspect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::getVoxelAspect,cx,obj,argc,argv,rval);
}

static JSBool
getValueRange(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::getValueRange,cx,obj,argc,argv,rval);
}

static JSBool
getVoxelPlane(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::getVoxelPlane,cx,obj,argc,argv,rval);
}

static JSBool
getModelViewMatrix(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::getModelViewMatrix,cx,obj,argc,argv,rval);
}

static JSBool
getOccurringValueRange(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::getOccurringValueRange,cx,obj,argc,argv,rval);
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
computeProfile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);

        ensureParamCount(argc, 1);

        std::vector<Point3i> myVoxelPositions;

        if (!convertFrom(cx, argv[0], myVoxelPositions)) {
            JS_ReportError(cx, "JSCTScan::computeProfile(): 1 argument must be an array of Points");
            return JS_FALSE;
        }
        std::vector<Signed32> myProfile;
        std::vector<asl::Point3i> mySampledPoints;
        CTScan & myCTScan = myObj.getNative();
        myCTScan.computeProfile(myVoxelPositions, myProfile, mySampledPoints);
        // now return both arrays as a JS object with the properties .values and .points
        JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
        if (!JS_DefineProperty(cx, myReturnObject, "points", as_jsval(cx, mySampledPoints), 0,0, JSPROP_ENUMERATE)) {
            JS_ReportError(cx, "JSCTScan::computeProfile(): could not create 'points' property ");
            return JS_FALSE;
        }
        if (!JS_DefineProperty(cx, myReturnObject, "values", as_jsval(cx, myProfile), 0,0, JSPROP_ENUMERATE)) {
            JS_ReportError(cx, "JSCTScan::computeProfile(): could not create 'values' property ");
            return JS_FALSE;
        }

        *rval = OBJECT_TO_JSVAL(myReturnObject);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
reconstructToImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        if (argc < 3 || argc > 4) {
            JS_ReportError(cx, "JSCTScan::reconstructToImage(): Wrong number of arguments, "
                               "expected 3-3 (Orientation, SliceIndex, Image Node, interpolate), got %d.", argc);
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

        dom::NodePtr myImageNode;
        convertFrom(cx, argv[2], myImageNode);
        CTScan & myCTScan = myObj.getNative();

        int myOrientationEnum;
        asl::Quaternionf myOrientationQuaternion;
        if (!convertFrom(cx, argv[0], myOrientationEnum)) {
            if (!convertFrom(cx, argv[0], myOrientationQuaternion)) {
                JS_ReportError(cx, "JSCTScan::reconstructToImage(): Argument #0 must be an Orientation or a Quaternion");
                return JS_FALSE;
            }
            asl::Vector3f mySlicePosition;
            if (!convertFrom(cx, argv[1], mySlicePosition)) {
                JS_ReportError(cx, "JSCTScan::reconstructToImage(): Argument #1 must be an Vector3f for arbitrary angles");
                return JS_FALSE;
            }
            bool myInterpolateFlag = true;
            if (! JSVAL_IS_VOID(argv[3])) {
                convertFrom(cx, argv[3], myInterpolateFlag);
            }
            myCTScan.reconstructToImage(myOrientationQuaternion, mySlicePosition, myImageNode, myInterpolateFlag);
        } else {
            int mySlice = 0;
            convertFrom(cx, argv[1], mySlice);
            myCTScan.reconstructToImage(static_cast<CTScan::Orientation>(myOrientationEnum), mySlice, myImageNode);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getReconstructionDimensions(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        if (argc != 1) {
            JS_ReportError(cx, "JSCTScan::getReconstructionDimensions(): Wrong number of arguments, "
                               "expected 1 (Orientation), got %d.", argc);
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "JSCTScan::getReconstructionDimensions(): Argument #0 is undefined");
            return JS_FALSE;
        }
        CTScan & myCTScan = myObj.getNative();
        Vector3i myDimensions;

        int myOrientationEnum;
        asl::Quaternionf myOrientationQuaternion;
        if (!convertFrom(cx, argv[0], myOrientationEnum)) {
            if (!convertFrom(cx, argv[0], myOrientationQuaternion)) {
                JS_ReportError(cx, "JSCTScan::getReconstructionDimensions(): Argument #0 must be an Orientation or a Quaternion");
                return JS_FALSE;
            }
            myDimensions = myCTScan.getReconstructionDimensions(myOrientationQuaternion);
        } else {
            myDimensions = myCTScan.getReconstructionDimensions(static_cast<CTScan::Orientation>(myOrientationEnum));
        }
        *rval = as_jsval(cx, Vector3f(float(myDimensions[0]), float(myDimensions[1]), float(myDimensions[2])));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getSliceThickness(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::getSliceThickness,cx,obj,argc,argv,rval);
}


static JSBool
countTrianglesGlobal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        CTScan & myCTScan = myObj.getNative();

        ensureParamCount(argc, 5);

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
        bool myCloseAtClippingBoxFlag;
        convertFrom(cx, argv[4], myCloseAtClippingBoxFlag);

        Vector2i myCount = myCTScan.countTrianglesGlobal(myVoxelBox, myThresholdMin, myThresholdMax, myDownSampleRate,
                myCloseAtClippingBoxFlag);
        *rval = as_jsval(cx, myCount);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
countTrianglesInVolumeMeasurement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        CTScan & myCTScan = myObj.getNative();

        ensureParamCount(argc, 5);

        // have to convert a Box3f into a Box3i because Box3i isn't available in JS
        Box3f myFloatBox;
        convertFrom(cx, argv[0], myFloatBox);
        Box3i myVoxelBox(static_cast<int>( myFloatBox[Box3f::MIN][0]),
                         static_cast<int>( myFloatBox[Box3f::MIN][1]),
                         static_cast<int>( myFloatBox[Box3f::MIN][2]),
                         static_cast<int>( myFloatBox[Box3f::MAX][0]),
                         static_cast<int>( myFloatBox[Box3f::MAX][1]),
                         static_cast<int>( myFloatBox[Box3f::MAX][2]));

        dom::NodePtr myThresholdPalette;
        convertFrom(cx, argv[1], myThresholdPalette);

        dom::NodePtr myMeasurementNode;
        convertFrom(cx, argv[2], myMeasurementNode);

        int myDownSampleRate;
        convertFrom(cx, argv[3], myDownSampleRate);

        bool myCloseAtClippingBoxFlag;
        convertFrom(cx, argv[4], myCloseAtClippingBoxFlag);

        Vector2i myCount = myCTScan.countTrianglesInVolumeMeasurement(myVoxelBox, myMeasurementNode,
                myThresholdPalette, myDownSampleRate, myCloseAtClippingBoxFlag);
        *rval = as_jsval(cx, myCount);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
polygonizeGlobal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        CTScan & myCTScan = myObj.getNative();

        ensureParamCount(argc, 6, 8);

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

        bool myCloseAtClippingBoxFlag;
        convertFrom(cx, argv[4], myCloseAtClippingBoxFlag);

        bool myCreateNormalsFlag;
        convertFrom(cx, argv[5], myCreateNormalsFlag);

        unsigned myNumVertices = 0;
        if (argc > 6) {
            convertFrom(cx, argv[6], myNumVertices);
        }
        unsigned myNumTriangles = 0;
        if (argc > 7) {
            convertFrom(cx, argv[7], myNumTriangles);
        }

        ScenePtr myScene = myCTScan.polygonizeGlobal(myVoxelBox, myThresholdMin, myThresholdMax, myDownSampleRate,
                myCloseAtClippingBoxFlag, myCreateNormalsFlag, JSApp::getPackageManager(), myNumVertices, myNumTriangles);
        *rval = as_jsval(cx, myScene);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
polygonizeVolumeMeasurement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<CTScan>::ScopedNativeRef myObj(cx, obj);
        CTScan & myCTScan = myObj.getNative();

        ensureParamCount(argc, 6, 8);

        // have to convert a Box3f into a Box3i because Box3i isn't available in JS
        Box3f myFloatBox;
        convertFrom(cx, argv[0], myFloatBox);
        Box3i myVoxelBox(static_cast<int>( myFloatBox[Box3f::MIN][0]),
                         static_cast<int>( myFloatBox[Box3f::MIN][1]),
                         static_cast<int>( myFloatBox[Box3f::MIN][2]),
                         static_cast<int>( myFloatBox[Box3f::MAX][0]),
                         static_cast<int>( myFloatBox[Box3f::MAX][1]),
                         static_cast<int>( myFloatBox[Box3f::MAX][2]));

        dom::NodePtr myThresholdPalette;
        convertFrom(cx, argv[1], myThresholdPalette);

        dom::NodePtr myMeasurementNode;
        convertFrom(cx, argv[2], myMeasurementNode);

        int myDownSampleRate;
        convertFrom(cx, argv[3], myDownSampleRate);

        bool myCloseAtClippingBoxFlag;
        convertFrom(cx, argv[4], myCloseAtClippingBoxFlag);

        bool myCreateNormalsFlag;
        convertFrom(cx, argv[5], myCreateNormalsFlag);

        unsigned myNumVertices = 0;
        if (argc > 6) {
            convertFrom(cx, argv[6], myNumVertices);
        }
        unsigned myNumTriangles = 0;
        if (argc > 7) {
            convertFrom(cx, argv[7], myNumTriangles);
        }

        ScenePtr myScene = myCTScan.polygonizeVolumeMeasurement(myVoxelBox, myMeasurementNode, myThresholdPalette,
                myDownSampleRate, myCloseAtClippingBoxFlag, myCreateNormalsFlag, JSApp::getPackageManager(), myNumVertices,
                myNumTriangles);
        *rval = as_jsval(cx, myScene);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
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

        dom::NodePtr myImage = CTScan::createRGBAImage(myImagesNode, myWidth, myHeight, myValue);

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

        CTScan::resizeVoxelVolume(myVoxelVolume, myDirtyBox);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
copyCanvasToVoxelVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 5);

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

        CTScan::copyCanvasToVoxelVolume(myMeasurement, myCanvasImage, myDirtyBox, myOrientation, myPaletteNode);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
copyVoxelVolumeToCanvas(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
       ensureParamCount(argc, 7);

        dom::NodePtr myMeasurement;
        convertFrom(cx, argv[0], myMeasurement);

        unsigned myGlobalThresholdIndex;
        convertFrom(cx, argv[1], myGlobalThresholdIndex);

        dom::NodePtr myCanvas;
        convertFrom(cx, argv[2], myCanvas);

        dom::NodePtr myReconstructedImage;
        convertFrom(cx, argv[3], myReconstructedImage);

        unsigned mySliceIndex;
        convertFrom(cx, argv[4], mySliceIndex);

        unsigned mySliceOrientationInt;
        convertFrom(cx, argv[5], mySliceOrientationInt);
        CTScan::Orientation myOrientation = static_cast<CTScan::Orientation>( mySliceOrientationInt );

        dom::NodePtr myPaletteNode;
        convertFrom(cx, argv[6], myPaletteNode);

        CTScan::copyVoxelVolumeToCanvas(myMeasurement, myGlobalThresholdIndex, myCanvas, myReconstructedImage,
                mySliceIndex, myOrientation, myPaletteNode);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

static JSBool
applyBrush(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 5);
        dom::NodePtr myCanvasImage;
        convertFrom(cx, argv[0], myCanvasImage);

        int myXPos;
        convertFrom(cx, argv[1], myXPos);

        int myYPos;
        convertFrom(cx, argv[2], myYPos);

        dom::NodePtr myBrushImage;
        convertFrom(cx, argv[3], myBrushImage);

        Vector4f myColor;
        convertFrom(cx, argv[4], myColor);

        CTScan::applyBrush(myCanvasImage, myXPos, myYPos, myBrushImage, myColor);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

static JSBool
create3DTexture(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCTScan::NATIVE>::call(&JSCTScan::NATIVE::create3DTexture,cx,obj,argc,argv,rval);
}

static JSBool
renderTransferFunction(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ensureParamCount(argc, 2);

        dom::NodePtr myTransferFunction;
        convertFrom(cx, argv[0], myTransferFunction);

        dom::NodePtr myTargetImage;
        convertFrom(cx, argv[1], myTargetImage);

        CTScan::renderTransferFunction(myTransferFunction, myTargetImage);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

#define DEFINE_ORIENTATION_PROP(NAME) { #NAME, PROP_ ## NAME , CTScan::NAME }

JSConstIntPropertySpec *
JSCTScan::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_ORIENTATION_PROP(IDENTITY),
        DEFINE_ORIENTATION_PROP(Y2Z),
        DEFINE_ORIENTATION_PROP(X2Z),
        DEFINE_ORIENTATION_PROP(ARBITRARY),
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
        {"loadSphere",           loadSphere,              0},
//        {"renderToImage",        renderToImage,           2},
        {"reconstructToImage",   reconstructToImage,      4},
        {"clear",                clear,                   0},
        {"verifyCompleteness",   verifyCompleteness,      0},
        {"getModelViewMatrix",   getModelViewMatrix,      1},
        {"getVoxelPlane",        getVoxelPlane,           2},
        {"setVoxelSize",         setVoxelSize,            1},
        {"getVoxelSize",         getVoxelSize,            0},
        {"getVoxelAspect",       getVoxelAspect,          0},
        {"getVoxelDimensions",   getVoxelDimensions,      0},
        {"getValueRange",        getValueRange,           0},
        {"getOccurringValueRange", getOccurringValueRange,0},
        {"polygonizeGlobal",     polygonizeGlobal,        7},
        {"polygonizeVolumeMeasurement", polygonizeVolumeMeasurement,        7},
        {"countTrianglesGlobal", countTrianglesGlobal,    4},
        {"countTrianglesInVolumeMeasurement", countTrianglesInVolumeMeasurement,    4},
        {"create3DTexture",      create3DTexture,         2},
        {"computeHistogram",     computeHistogram,        1},
        {"computeProfile",       computeProfile,          1},
        {"getReconstructionDimensions", getReconstructionDimensions, 1},
        {"getSliceThickness",    getSliceThickness, 1},
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
        {"copyCanvasToVoxelVolume", copyCanvasToVoxelVolume, 5},
        {"copyVoxelVolumeToCanvas", copyVoxelVolumeToCanvas, 6},
        {"applyBrush",              applyBrush,              5},
        {"renderTransferFunction",  renderTransferFunction,  2},
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
                JSSignal2<bool,double,Glib::ustring>::OWNERPTR mySignal( new
                        JSSignal2<bool,double,Glib::ustring>::NATIVE(getNative().signal_progress()));
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

    JSCTScan * myNewObject = new JSCTScan(myNewNative, myNewNative.get());
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
    JSObject * myReturnObject = JSCTScan::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSCTScan::OWNERPTR theOwner, JSCTScan::NATIVE * theCTScan) {
    JSObject * myObject = JSCTScan::Construct(cx, theOwner, theCTScan);
    return OBJECT_TO_JSVAL(myObject);
}

}
