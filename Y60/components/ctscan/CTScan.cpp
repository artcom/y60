//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: CTScan.cpp,v $
//   $Author: martin $
//   $Revision: 1.22 $
//   $Date: 2005/04/25 11:02:50 $
//
//
//=============================================================================

#include "CTScan.h"
#include "PLFile.h"
#include "DicomFile.h"
#include "MarchingCubes.h"

#include <y60/WorldBuilder.h>
#include <y60/TransformBuilder.h>
#include <y60/BodyBuilder.h>
#include <y60/MaterialBuilder.h>
#include <y60/CameraBuilder.h>
#include <y60/Camera.h>
#include <y60/Image.h>
#include <y60/Facades.h>

#include <paintlib/plexcept.h>
#include <asl/MappedBlock.h>
#include <asl/Logger.h>
#include <asl/PackageManager.h>
#include <asl/Box.h>

#include <y60/property_functions.h>
#include <y60/PropertyNames.h>

#include <y60/PLFilterResizePadded.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#include <paintlib/plpixelformat.h>
#include <paintlib/planybmp.h>
#include <paintlib/plsubbmp.h>
#include <paintlib/plpngenc.h>
#include <asl/Assure.h>
#include <fstream>
#include <algorithm>

using namespace asl;
using namespace std;
using namespace dom;
using namespace y60;

#define DB(x) // x;

namespace y60 {

CTScan::CTScan() : _myEncoding(y60::GRAY), _myDefaultWindow(128.0f, 256.0f), _myVoxelSize(0,0,0) {
}

CTScan::~CTScan() {
}

int 
CTScan::loadSlices(asl::PackageManager & thePackageManager, const std::string & theSubDir, const std::string& thePackage) {
    clear();
    vector<string> myFileList(thePackageManager.listFiles(theSubDir,thePackage));
    AC_TRACE << "found " << myFileList.size() << " files in " << thePackage << ":" << theSubDir;    

    if ( myFileList.empty()) {           
        throw CTScanException("Files in " + theSubDir + " not found \n", PLUS_FILE_LINE);
    } 

    int totalFileCount = myFileList.size();

    std::map<float, ResizeableRasterPtr> mySortedSlices;

    // take a representative file from the middle
    CTFilePtr representativeFile;
    int representativeFileIndex = totalFileCount/2;

    int myProgressTotal = totalFileCount;
    int myProgressCounter = 0;
    
    for (int i = 0; i < totalFileCount; ++i) {
        AC_TRACE << "reading '" << myFileList[i] << "'";    
        CTFilePtr myFile;
        int myFrameCount = 1;
        try {
            Ptr<ReadableBlock> myBlock = thePackageManager.openFile(myFileList[i], thePackage);

            string myExtension = toLowerCase(getExtension(myFileList[i]));
            if (myExtension == "dcm") {
                myFile = CTFilePtr(new DicomFile(myBlock, myFileList[i]));
            } else if (myExtension == "xml") {
                continue; // skip embedded xml files.
            } else {
                myFile = CTFilePtr(new PLFile(myBlock, myFileList[i]));
            }
            myFrameCount = myFile->getFrameCount();
        } catch (ImageLoaderException &) {
            myFrameCount = 0;
        }
    
        // adjust the progress total if more than one frame is in the file
        myProgressTotal += myFrameCount -1;

        for (int j = 0; j < myFrameCount; ++j) {
            float myZPos = myFile->getZPos(j);
            dom::ResizeableRasterPtr myRaster = myFile->getRaster(j);
            mySortedSlices.insert(make_pair(myZPos, myRaster));

            // Progress notification
            _myProgressSignal.emit(double(++myProgressCounter)/double(myProgressTotal), "loading");
        }
        if ( i <= representativeFileIndex || !representativeFile ) {
            representativeFile = myFile;
        }
    }
    // copy the sorted slices to an array
    std::map<float, ResizeableRasterPtr>::iterator it = mySortedSlices.begin();
    for (; it != mySortedSlices.end(); ++it) {
        _mySlices.push_back(it->second);
    }
    AC_TRACE << "done, read " << _mySlices.size() << " slices";
    if (!_mySlices.empty()) {
        // extract information from the representative slice
        _myDefaultWindow = representativeFile->getDefaultWindow(); 
        _myVoxelSize = representativeFile->getVoxelSize(); 
        _myEncoding = representativeFile->getEncoding(); 
    }
    //allocateStencils();
    
    findOccurringValueRange();
    
    return _mySlices.size();
}

int 
CTScan::loadSphere(int size) {
    clear();
    for (int i = 0; i < size; ++i) {
        ResizeableRasterPtr myRaster = 
            dynamic_cast_Ptr<dom::ResizeableRaster>(createRasterValue(y60::GRAY, size, size));

        Unsigned8 * myPixels = myRaster->pixels().begin();
        Point3f myCenter(float(size)/2.0f, float(size)/2.0f, float(size)/2.0f);
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                float myDist = asl::distance(myCenter, Point3f(float(x),float(y),float(i))) / (size/2);
                if (myDist > 1.0f) {
                    myPixels[x+y*size] = 0;
                } else {
                    myPixels[x+y*size] = Unsigned8((1.f - myDist) * 255.0f); 
                }
            }
        }
        
        _mySlices.push_back(myRaster); 
    }
    _myDefaultWindow = Vector2f(0,255);
    _myVoxelSize = Vector3f(0.001f,0.001f,0.001f);
    _myEncoding = y60::GRAY; 
    return _mySlices.size();
}

    
int
CTScan::setSlices(std::vector<dom::ResizeableRasterPtr> theSlices) {
    _mySlices = theSlices;
    return _mySlices.size();
}

void
CTScan::clear() {
    _mySlices.clear();
    _myState = LOADING;
}

const asl::Vector2f &
CTScan::getDefaultWindow() const {
    return _myDefaultWindow;
};

bool
CTScan::verifyCompleteness() {
    _myMinZ = INT_MAX;
    _myMaxZ = -INT_MAX;
    for (int i = 0; i < _mySlices.size(); ++i) {
        if (_mySlices[i] && _myMinZ == INT_MAX) {
            _myMinZ = i;
        }
        if (_mySlices[i]) {
            _myMaxZ = i;
        }
        if (!_mySlices[i] && _myMinZ != INT_MAX) {
            // missing slice
            _myState = BROKEN;
        }
    }
    if (_myMinZ == INT_MAX) {
        _myState = BROKEN;
    }
    if (_myState == LOADING) {
        _myState = COMPLETE;
    }
    return (_myState == COMPLETE);
}

asl::Vector3i
CTScan::getVoxelDimensions() const {
    if (!_mySlices.empty()) {
        return Vector3i( _mySlices[0]->width(),
                         _mySlices[0]->height(),
                         _mySlices.size());
    } else {
        return Vector3i(0,0,0);
    }
}

asl::Vector3f
CTScan::getVoxelSize() const {
    return _myVoxelSize;
}

asl::Vector3f
CTScan::getVoxelAspect() const {
    asl::Vector3f myVoxelSize = getVoxelSize();
    return Vector3f(1.0, myVoxelSize[1] / myVoxelSize[0], myVoxelSize[2] / myVoxelSize[0]);
}

void 
CTScan::setVoxelSize(const Vector3f & theSize) {
    _myVoxelSize = theSize;
}

void 
CTScan::findOccurringValueRange() {
    std::vector<unsigned> myHistogram;
    asl::Box3i myBox;
    myBox.makeEmpty();
    myBox.extendBy(Vector3i(0, 0, 0));
    myBox.extendBy(getVoxelDimensions());
    computeHistogram(myBox, myHistogram, false);

    std::vector<unsigned>::iterator myMinIt = find_if( myHistogram.begin(), myHistogram.end(), 
                                            std::bind2nd( std::not_equal_to<unsigned>(), 0) );
    if (myMinIt == myHistogram.end()) {
        throw CTScanException("All values occur zero times.", PLUS_FILE_LINE);
    }

    unsigned myMinIndex = distance( myHistogram.begin(), myMinIt);
    Vector2d myValueRange = getValueRange();
    _myOccurringValueRange[0] = myValueRange[0] + myMinIndex;

    std::vector<unsigned>::reverse_iterator myMaxIt = find_if( myHistogram.rbegin(), myHistogram.rend(), 
                                            bind2nd( not_equal_to<unsigned>(), 0) );
    if (myMaxIt == myHistogram.rend()) {
        throw CTScanException("All values occur zero times.", PLUS_FILE_LINE);
    }

    unsigned myMaxIndex = distance(myMaxIt, myHistogram.rend());
    myMaxIndex -= 1; // should be ok to subtract one here ... otherwise the above exception would have
                     // been thrown
    _myOccurringValueRange[1] = myValueRange[0] + myMaxIndex;
    AC_TRACE << "occurring value range: " << _myOccurringValueRange;
}

asl::Vector2d
CTScan::getOccurringValueRange() {
    return _myOccurringValueRange;
}

asl::Vector2d
CTScan::getValueRange() {
    switch (_myEncoding) {
        case y60::GRAY:
            return Vector2d(double( NumericTraits<unsigned char>::min()),
                            double( NumericTraits<unsigned char>::max()));
        case y60::GRAY16:
           return Vector2d(double( NumericTraits<unsigned short>::min()),
                            double( NumericTraits<unsigned short>::max()));
        case y60::GRAYS16:
            return Vector2d(double( NumericTraits<short>::min()),
                            double( NumericTraits<short>::max()));
        default:
            throw CTScanException("Unhandled voxel type in CTScan::getValueRange()", PLUS_FILE_LINE);

    }

    return Vector2d(0.0, 0.0);
}

template <class VoxelT, class SegmentationPolicy>
bool
CTScan::applyMarchingCubes(const asl::Box3i & theVoxelBox, int theDownSampleRate,
                             bool theCreateNormalsFlag, ScenePtr theScene, 
                             SegmentationPolicy & theSegmentizer,
                             unsigned int theNumVertices, unsigned int theNumTriangles)
{
    if (theNumVertices == 0 || theNumTriangles == 0) {
        // estimation not done yet - force it
        countMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theSegmentizer, theNumVertices, theNumTriangles); 
    }
    
    SceneBuilderPtr mySceneBuilder = theScene->getSceneBuilder();
    std::string myMaterialId = setupMaterial( mySceneBuilder, theCreateNormalsFlag);
    ExportShapePolicy<VoxelT, SegmentationPolicy > 
                myVertexStore(mySceneBuilder, "polygonal_shape", myMaterialId,
                theNumVertices, theNumTriangles, theCreateNormalsFlag);
    
    // here we go ...
    MarchingCubes<VoxelT, ExportShapePolicy<VoxelT, SegmentationPolicy >, SegmentationPolicy >
           myMarcher(theDownSampleRate, myVertexStore, theSegmentizer, this);

    myMarcher.setBox(theVoxelBox);
    bool isDone = myMarcher.march();

    setupScene(theScene, myVertexStore.getShapeNode()->getAttributeString("id"));
    asl::Box3f myBox;
    for (int i = 0; i < 6; ++i) {
        myBox[i/3][i%3] = float(theVoxelBox[i/3][i%3]) * _myVoxelSize[i%3];        
    }
    resetCamera(theScene, myBox);
    return isDone;
}

template <class VoxelT, class SegmentationPolicy>
bool
CTScan::countMarchingCubes(const asl::Box3i & theVoxelBox, int theDownSampleRate,
                           SegmentationPolicy & theSegmentizer,
                           unsigned int & theVertexCount, unsigned int & theTriangleCount)
{
    // here we go ...
    CountPolygonPolicy<VoxelT, SegmentationPolicy > myPolygonCounter;

    MarchingCubes<VoxelT,
                  CountPolygonPolicy<VoxelT, SegmentationPolicy >,
                  SegmentationPolicy >
           myMarcher(theDownSampleRate, myPolygonCounter, theSegmentizer, this);

    myMarcher.setBox(theVoxelBox);
    AC_INFO << "starting dry run";
    bool isDone = myMarcher.march();
    theVertexCount = myPolygonCounter.getVertexCount();
    theTriangleCount = myPolygonCounter.getTriangleCount();
    // AC_WARNING << "Triangles:"  << theTriangleCount << " Vertices:" << theVertexCount;
    return isDone;
}
/** Counts triangles and vertices that would be produced in a real run.
 *
 * @return a Vector of which x is VertexCount and y is TriangleCount or (-1, -1) 
           if the operation was aborted by the user
 */
asl::Vector2i
CTScan::countTrianglesGlobal(const asl::Box3i & theVoxelBox, 
                       double theThresholdMin, 
                       double theThresholdMax, 
                       int theDownSampleRate)
{
    unsigned int myVertexCount;
    unsigned int myTriangleCount;
    bool isDone;
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char Voxel;
                GlobalThresholdSegmentationPolicy<Voxel> mySegmentizer((Voxel)theThresholdMin, (Voxel)theThresholdMax);
                isDone = countMarchingCubes<Voxel>(theVoxelBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAY16:
            {
                typedef unsigned short Voxel;
                GlobalThresholdSegmentationPolicy<Voxel> mySegmentizer((Voxel)theThresholdMin, (Voxel)theThresholdMax);
                isDone = countMarchingCubes<Voxel>(theVoxelBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAYS16:
            {
                typedef short Voxel;
                GlobalThresholdSegmentationPolicy<Voxel> mySegmentizer((Voxel)theThresholdMin, (Voxel)theThresholdMax);
                isDone = countMarchingCubes<Voxel>(theVoxelBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        default:
            throw CTScanException("Unhandled voxel type in CTScan::countTriangles()", PLUS_FILE_LINE);
    }
    if (isDone) {
        return asl::Vector2i(myVertexCount, myTriangleCount);
    } else {
        return asl::Vector2i(-1, -1);
    }
}

asl::Vector2i
CTScan::countTrianglesInVolumeMeasurement(const asl::Box3i & theVoxelBox, dom::NodePtr theVolumeNode,
                                          dom::NodePtr theThresholdPalette, int theDownSampleRate)
{
    unsigned int myVertexCount;
    unsigned int myTriangleCount;

    Box3i myBoundingBox = theVoxelBox;
    prepareBox(myBoundingBox); 

    bool isDone;
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char Voxel;
                PerVoxelThresholdSegmentationPolicy<Voxel> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                isDone = countMarchingCubes<Voxel>(myBoundingBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAY16:
            {
                typedef unsigned short Voxel;
                PerVoxelThresholdSegmentationPolicy<Voxel> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                isDone = countMarchingCubes<Voxel>(myBoundingBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAYS16:
            {
                typedef short Voxel;
                PerVoxelThresholdSegmentationPolicy<Voxel> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                isDone = countMarchingCubes<Voxel>(myBoundingBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        default:
            throw CTScanException("Unhandled voxel type in CTScan::countTriangles()", PLUS_FILE_LINE);
    }
    if (isDone) {
        return asl::Vector2i(myVertexCount, myTriangleCount);
    } else {
        return asl::Vector2i(-1, -1);
    }
}
ScenePtr
CTScan::polygonizeGlobal(const asl::Box3i & theVoxelBox, double theThresholdMin, double theThresholdMax, 
                   int theDownSampleRate, bool theCreateNormalsFlag, PackageManagerPtr thePackageManager, 
                   unsigned int theNumVertices, unsigned int theNumTriangles)
{
    asl::Time myStartTime;

    ScenePtr myScene(new Scene);
    myScene->createStubs(thePackageManager);
    bool isDone;
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                GlobalThresholdSegmentationPolicy<VoxelT> mySegmentizer( (VoxelT)theThresholdMin, (VoxelT)theThresholdMax);
                isDone = applyMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                GlobalThresholdSegmentationPolicy<VoxelT> mySegmentizer( (VoxelT)theThresholdMin, (VoxelT)theThresholdMax);
                isDone = applyMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                GlobalThresholdSegmentationPolicy<VoxelT> mySegmentizer( (VoxelT)theThresholdMin, (VoxelT)theThresholdMax);
                isDone = applyMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        default:
            throw CTScanException("Unhandled voxel type in CTScan::polygonize()", PLUS_FILE_LINE);
    }
    asl::Time myEndTime;
    AC_INFO << "===> Marching Cubes took " << myEndTime - myStartTime << " seconds.";
    if ( isDone ) {
        return myScene;
    } else {
        return ScenePtr(0);
    }
}

void CTScan::prepareBox(asl::Box3i & theVoxelBox) {
    theVoxelBox[asl::Box3i::MIN][0] = asl::maximum(int(theVoxelBox[asl::Box3i::MIN][0]), 0);
    theVoxelBox[asl::Box3i::MIN][1] = asl::maximum(int(theVoxelBox[asl::Box3i::MIN][1]), 0);
    theVoxelBox[asl::Box3i::MIN][2] = asl::maximum(int(theVoxelBox[asl::Box3i::MIN][2]), 0);
    theVoxelBox[asl::Box3i::MAX][0] = asl::minimum(int(theVoxelBox[asl::Box3i::MAX][0]), getVoxelDimensions()[0]);
    theVoxelBox[asl::Box3i::MAX][1] = asl::minimum(int(theVoxelBox[asl::Box3i::MAX][1]), getVoxelDimensions()[1]);
    theVoxelBox[asl::Box3i::MAX][2] = asl::minimum(int(theVoxelBox[asl::Box3i::MAX][2]), getVoxelDimensions()[2]);
}

ScenePtr
CTScan::polygonizeVolumeMeasurement(const asl::Box3i & theVoxelBox, dom::NodePtr theVolumeNode, dom::NodePtr theThresholdPalette, 
                   int theDownSampleRate, bool theCreateNormalsFlag, PackageManagerPtr thePackageManager, 
                   unsigned int theNumVertices, unsigned int theNumTriangles)
{
    asl::Time myStartTime;
    asl::Box3i myBoundingBox = theVoxelBox;
    prepareBox(myBoundingBox);

    ScenePtr myScene(new Scene);
    myScene->createStubs(thePackageManager);    
    bool isDone;
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                PerVoxelThresholdSegmentationPolicy<VoxelT> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                isDone = applyMarchingCubes<VoxelT>(myBoundingBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                PerVoxelThresholdSegmentationPolicy<VoxelT> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                isDone = applyMarchingCubes<VoxelT>(myBoundingBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                PerVoxelThresholdSegmentationPolicy<VoxelT> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                isDone = applyMarchingCubes<VoxelT>(myBoundingBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        default:
            throw CTScanException("Unhandled voxel type in CTScan::polygonize()", PLUS_FILE_LINE);
    }
    asl::Time myEndTime;
    AC_INFO << "===> Marching Cubes took " << myEndTime - myStartTime << " seconds.";
    if ( isDone ) {
        return myScene;
    } else {
        return ScenePtr(0);
    }
}

Vector3i
CTScan::getReconstructionDimensions(const Vector3f & theOrientationVector) const {
    if (_myState != COMPLETE) {
        throw CTScanException("cannot reconstruct image when loading not complete!", PLUS_FILE_LINE);
    }
    Vector3i mySize = getVoxelDimensions();
    Orientation myOrientation;
    Vector3i myResult;

    if (asl::almostEqual(theOrientationVector, Vector3f(0,0,1))) {
        myOrientation = CTScan::IDENTITY;
    } else if (asl::almostEqual(theOrientationVector, Vector3f(1,0,0))) {
        myOrientation = CTScan::Y2Z;
    } else if (asl::almostEqual(theOrientationVector, Vector3f(0,1,0))) {
        myOrientation = CTScan::X2Z;
    } else {
        myOrientation = CTScan::ARBITRARY;
    }
    switch (myOrientation) {
        case IDENTITY:
            myResult = mySize;
            break;
        case Y2Z:
            myResult = Vector3i(mySize[0], mySize[2], mySize[1]);
            break;
        case X2Z:
            myResult = Vector3i(mySize[2], mySize[1], mySize[0]);
            break;
        default:
            {
                Box3f myBounds;
                Vector3f myInitialVector(0,0,1.0f);
                Vector3f myOrientationVector = theOrientationVector;
                Quaternionf myRotationQuaternion(myOrientationVector, myInitialVector);
                Matrix4f myScreenToVoxelProjection(myRotationQuaternion);

                Vector3f myScale, myShear, myOrientation, myPosition;
                myScreenToVoxelProjection.decompose(myScale, myShear, myOrientation, myPosition);

                Vector3i myVoxelSize = getVoxelDimensions();
                Matrix4f myVoxelToScreenProjection = myScreenToVoxelProjection;
                myVoxelToScreenProjection.invert();
                myBounds = computeProjectionBounds(myVoxelToScreenProjection);
                Vector3f myFloatSize = myBounds.getSize();
                myResult = Vector3i(int(ceil(myFloatSize[0])), int(ceil(myFloatSize[1])), int(ceil(myFloatSize[2])));
            }
            break;
    }
    return myResult;
}

template <class VoxelT>
typename VoxelT
CTScan::interpolatedValueAt(const Vector3f & thePosition) {
    Vector3i theFloorPos(int(floor(thePosition[0])), int(floor(thePosition[1])), int(floor(thePosition[2])));
    Vector3i theCeilPos = theFloorPos + Vector3i(1,1,1);
    float myValueA, myValueB;
    VoxelT myFloorValue, myCeilValue;
    float myFloorResult, myCeilResult;
    Vector3i mySize = getVoxelDimensions();
    const unsigned char * mySource;

    int myLineStride = getBytesRequired(mySize[0], _myEncoding);
    unsigned myBpp = getBytesRequired(1, _myEncoding);
    if (theFloorPos[2] >= 0 && theFloorPos[2] < _mySlices.size()) {
        myFloorValue = NumericTraits<VoxelT>::min();
        myCeilValue = NumericTraits<VoxelT>::min();
        mySource = _mySlices[int(theFloorPos[2])]->pixels().begin();
        if (isInside(theFloorPos[0], theFloorPos[1], theFloorPos[2])) {
            memcpy(&myFloorValue,
                mySource+myLineStride*theFloorPos[1] + theFloorPos[0],
                myBpp);
        }
        if (isInside(theCeilPos[0], theFloorPos[1], theFloorPos[2])) {
            memcpy(&myCeilValue,
                mySource+myLineStride*theFloorPos[1] + theCeilPos[0],
                myBpp);
        }
        myValueA = linearInterpolate(myFloorValue, myCeilValue,
            theFloorPos[0], theCeilPos[0], thePosition[0]);
        if (isInside(theFloorPos[0], theCeilPos[1], theFloorPos[2])) {
            memcpy(&myFloorValue,
                mySource+myLineStride*theCeilPos[1] + theFloorPos[0],
                myBpp);
        }
        if (isInside(theCeilPos[0], theCeilPos[1], theFloorPos[2])) {
            memcpy(&myCeilValue,
                mySource+myLineStride*theCeilPos[1] + theCeilPos[0],
                myBpp);
        }
        myValueB = linearInterpolate(myFloorValue, myCeilValue,
            theFloorPos[0], theCeilPos[0], thePosition[0]);

        myFloorResult = linearInterpolate(myValueA, myValueB, theFloorPos[1], theCeilPos[1], thePosition[1]);            
    } else {
        myFloorResult = NumericTraits<VoxelT>::min();
    }
    if (theCeilPos[2] >= 0 && theCeilPos[2] < _mySlices.size()) {
        mySource = _mySlices[int(theCeilPos[2])]->pixels().begin();
        myFloorValue = NumericTraits<VoxelT>::min();
        myCeilValue = NumericTraits<VoxelT>::min();
        if (isInside(theFloorPos[0], theFloorPos[1], theCeilPos[2])) {
            memcpy(&myFloorValue,
                mySource+myLineStride*theFloorPos[1] + theFloorPos[0],
                myBpp);
        }
        if (isInside(theCeilPos[0], theFloorPos[1], theCeilPos[2])) {
            memcpy(&myCeilValue,
                mySource+myLineStride*theFloorPos[1] + theCeilPos[0],
                myBpp);
        }
        myValueA = linearInterpolate(myFloorValue, myCeilValue,
            int(theFloorPos[0]), int(theCeilPos[0]), thePosition[0]);
        if (isInside(theFloorPos[0], theCeilPos[1], theCeilPos[2])) {
            memcpy(&myFloorValue,
                mySource+myLineStride*theCeilPos[1] + theFloorPos[0],
                myBpp);
        }
        if (isInside(theCeilPos[0], theCeilPos[1], theCeilPos[2])) {
            memcpy(&myCeilValue,
                mySource+myLineStride*theCeilPos[1] + theCeilPos[0],
                myBpp);
        }
        myValueB = linearInterpolate(myFloorValue, myCeilValue,
            int(theFloorPos[0]), int(theCeilPos[0]), thePosition[0]);
        myCeilResult = linearInterpolate(myValueA, myValueB, theFloorPos[1], theCeilPos[1], thePosition[1]);
    } else {
        myCeilResult = NumericTraits<VoxelT>::min();
    }
    return VoxelT(linearInterpolate(myFloorResult, myCeilResult, theFloorPos[2], theCeilPos[2], thePosition[2]));
}

void 
CTScan::reconstructToImage(const Vector3f & theOrientationVector, int theSliceIndex, 
        dom::NodePtr & theImageNode) {
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                reconstructToImageImpl<VoxelT>(theOrientationVector, theSliceIndex, theImageNode);
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                reconstructToImageImpl<VoxelT>(theOrientationVector, theSliceIndex, theImageNode);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                reconstructToImageImpl<VoxelT>(theOrientationVector, theSliceIndex, theImageNode);
            }
            break;
        default:
            throw CTScanException("Unhandled voxel type", PLUS_FILE_LINE);   
    }
}

template <class VoxelT>
void 
CTScan::reconstructToImageImpl(const Vector3f & theOrientationVector, int theSliceIndex, 
        dom::NodePtr & theImageNode) 
{
    if (_myState != COMPLETE) {
        throw CTScanException("cannot reconstruct image when loading not complete!", PLUS_FILE_LINE);
    }
    Vector3i mySize = getVoxelDimensions();

    int myWidth;
    int myHeight;
    int myPoTWidth;
    int myPoTHeight;
    Ptr<ReadableBlock> myPixelData;
    Orientation myOrientation;
    if (asl::almostEqual(theOrientationVector, Vector3f(0,0,1))) {
        myOrientation = CTScan::IDENTITY;
    } else if (asl::almostEqual(theOrientationVector, Vector3f(1,0,0))) {
        myOrientation = CTScan::Y2Z;
    } else if (asl::almostEqual(theOrientationVector, Vector3f(0,1,0))) {
        myOrientation = CTScan::X2Z;
    } else {
        myOrientation = CTScan::ARBITRARY;
    }

    switch (myOrientation) {        
        case IDENTITY:
            {
                myWidth = mySize[0];
                myHeight = mySize[1];
                myPoTWidth = nextPowerOfTwo(myWidth);
                myPoTHeight = nextPowerOfTwo(myHeight);

                Ptr<Block> myTarget(new Block(getBytesRequired(myPoTWidth* myPoTHeight, _myEncoding)));

                int myLineStride = getBytesRequired(myPoTWidth, _myEncoding);
                int myLineWidth = getBytesRequired(myWidth, _myEncoding);
                for (int y=0; y < myHeight; ++y) {
                    const unsigned char * mySource = _mySlices[theSliceIndex]->pixels().begin();
                    memcpy(myTarget->begin()+myLineStride*y,
                           mySource+myLineWidth*y,
                           myLineWidth);
                }
                myPixelData = myTarget; 
                break;
            }
        case Y2Z:
            {
                myWidth = mySize[0];
                myHeight = mySize[2];
                myPoTWidth = nextPowerOfTwo(myWidth);
                myPoTHeight = nextPowerOfTwo(myHeight);
                Ptr<Block> myTarget(new Block(getBytesRequired(myPoTWidth* myPoTHeight, _myEncoding)));

                int myLineStride = getBytesRequired(myPoTWidth, _myEncoding);
                int myLineWidth = getBytesRequired(myWidth, _myEncoding);
                for (int ySrc=0; ySrc < myHeight; ++ySrc) {
                    int yTarget = myHeight-ySrc-1;
                    const unsigned char * mySource = _mySlices[ySrc]->pixels().begin();
                    memcpy(myTarget->begin()+myLineStride*yTarget,
                           mySource+myLineWidth*theSliceIndex,
                           myLineWidth);
                }
                myPixelData = myTarget; 
                break;
            }
        case X2Z:
            {
                myWidth = mySize[2];
                myHeight = mySize[1];
                myPoTWidth = nextPowerOfTwo(myWidth);
                myPoTHeight = nextPowerOfTwo(myHeight);
                Ptr<Block> myTarget(new Block(getBytesRequired(myPoTWidth* myPoTHeight, _myEncoding)));
                unsigned myBpp = getBytesRequired(1, _myEncoding);
                unsigned mySourceLineOffset = myBpp*theSliceIndex;
                int myTargetLineStride = getBytesRequired(myPoTWidth, _myEncoding);
                int mySourceLineStride = getBytesRequired(mySize[0], _myEncoding);
                int mySliceCount = getSliceCount();
                for (int x=0; x < myWidth; ++x) {
                    const unsigned char * mySource = _mySlices[mySliceCount-x-1]->pixels().begin();
                    int xTarget = myWidth-x-1;
                    for (int y=0; y < myHeight; ++y) {
                        memcpy(myTarget->begin()+myTargetLineStride*y+myBpp*xTarget, 
                            mySource+mySourceLineStride*y+mySourceLineOffset, myBpp);
                    }
                }
                myPixelData = myTarget; 
                break;
            }
        case ARBITRARY:
            {                
                Box3f myBounds;
                Vector3f myInitialVector(0,0,1.0f);
                Vector3f myOrientationVector = theOrientationVector;
                Quaternionf myRotationQuaternion(myOrientationVector, myInitialVector);
                Matrix4f myScreenToVoxelProjection(myRotationQuaternion);

                Vector3f myScale, myShear, myOrientation, myPosition;
                myScreenToVoxelProjection.decompose(myScale, myShear, myOrientation, myPosition);

                Vector3i myVoxelSize = getVoxelDimensions();
                Matrix4f myVoxelToScreenProjection = myScreenToVoxelProjection;
                myVoxelToScreenProjection.invert();
                myBounds = computeProjectionBounds(myVoxelToScreenProjection);
                Vector3f mySize = myBounds.getSize();
                myWidth = int(ceil(mySize[0]));
                myHeight = int(ceil(mySize[1]));
                myPoTWidth = nextPowerOfTwo(myWidth);
                myPoTHeight = nextPowerOfTwo(myHeight);
                Ptr<Block> myTarget(new Block(getBytesRequired(myPoTWidth* myPoTHeight, _myEncoding)));                
                unsigned myBpp = getBytesRequired(1, _myEncoding);
                int myTargetLineStride = getBytesRequired(myPoTWidth, _myEncoding);
                
                float mySlicePosition = float(theSliceIndex) + myBounds[Box3f::MIN][2];
                Vector3f myLinePos = product(Point3f(myBounds[Box3f::MIN][0], myBounds[Box3f::MIN][1], mySlicePosition), myScreenToVoxelProjection);
                Vector3f mySourceDeltaU = (product(Point3f(myBounds[Box3f::MAX][0], myBounds[Box3f::MIN][1], mySlicePosition), myScreenToVoxelProjection) - myLinePos) / float(myWidth);
                Vector3f mySourceDeltaV = (product(Point3f(myBounds[Box3f::MIN][0], myBounds[Box3f::MAX][1], mySlicePosition), myScreenToVoxelProjection) - myLinePos) / float(myHeight);
                AC_TRACE << "Width: " << myWidth << ", Height: " << myHeight << ", LinePos: " << myLinePos << " DeltaU: " << mySourceDeltaU << " DeltaV " << mySourceDeltaV;

                for (int v = 0; v < myHeight; ++v) {
                    Vector3f mySourcePos = myLinePos;
                    unsigned char* myAddress = myTarget->begin()+myTargetLineStride*v;
                    for (int u = 0; u < myWidth; ++u) {                        
                        VoxelT myValue = interpolatedValueAt<VoxelT>(mySourcePos);
                        memcpy(myAddress, &myValue, myBpp);
                        mySourcePos += mySourceDeltaU;
                        myAddress += myBpp;
                    }
                    myLinePos += mySourceDeltaV;
                }
                myPixelData = myTarget; 
            }
            break;
    }
    // set the image data
    theImageNode->getFacade<Image>()->set(myPoTWidth, myPoTHeight, 1, _myEncoding, *myPixelData);
    // set the matrix to make up for the padded image
    asl::Matrix4f myScale;
    myScale.makeIdentity();
    myScale.scale(Vector3f(float(myWidth)/myPoTWidth, float(myHeight)/myPoTHeight, 1.0f)); 
    theImageNode->getFacade<y60::Image>()->set<y60::ImageMatrixTag>(myScale); 
}

Box3f
CTScan::computeProjectionBounds(const Matrix4f & theInvertedProjection) const {
    vector<Point3f> myCorners;
    Vector3i mySize = getVoxelDimensions();
    float myZMin = 0;
    float myZMax = float(mySize[2]);
    Box3f myBox;
    myBox.makeEmpty();

    //theBox[Box3f::MIN] = product(Point3f(0,0,myZMin), theInvertedProjection);
    //theBox[Box3f::MAX] = product(Point3f(0,0,myZMin), theInvertedProjection);

    // convert the 8 corners into the inverted projection space
    myBox.extendBy(product(Point3f(0,0,0), theInvertedProjection));
    myBox.extendBy(product(Point3f(float(mySize[0]),0,0), theInvertedProjection));
    myBox.extendBy(product(Point3f(0,float(mySize[1]),0), theInvertedProjection));
    myBox.extendBy(product(Point3f(float(mySize[0]),float(mySize[1]),0), theInvertedProjection));

    myBox.extendBy(product(Point3f(0,0,float(mySize[2])), theInvertedProjection));
    myBox.extendBy(product(Point3f(float(mySize[0]),0,float(mySize[2])), theInvertedProjection));
    myBox.extendBy(product(Point3f(0,float(mySize[1]),float(mySize[2])), theInvertedProjection));
    myBox.extendBy(product(Point3f(float(mySize[0]),float(mySize[1]),float(mySize[2])), theInvertedProjection));
    return myBox;
}

std::string
CTScan::setupMaterial(SceneBuilderPtr theSceneBuilder, bool theCreateNormalsFlag) {
    // instead of using a modelling function we roll our own material here,
    // beacuse we don't have a Scene object ...
    std::string myMaterialName = "mSTL"; // XXX [DS] this name should not be changed
    MaterialBuilder myMaterialBuilder(myMaterialName, false);
    theSceneBuilder->appendMaterial(myMaterialBuilder);
    myMaterialBuilder.setTransparencyFlag(false);

    if (theCreateNormalsFlag) {
        VectorOfRankedFeature myLightingFeature;
        createLightingFeature(myLightingFeature, PHONG);
        myMaterialBuilder.setType(myLightingFeature);

        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f",
                AMBIENT_PROPERTY, Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f",
                DIFFUSE_PROPERTY, Vector4f(0.8f, 0.8f, 0.8f, 1.0f));

        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f",
                EMISSIVE_PROPERTY, Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f",
                SPECULAR_PROPERTY, Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

        setPropertyValue<float>(myMaterialBuilder.getNode(), "float",
                SHININESS_PROPERTY, 0.0f);
    } else {
        VectorOfRankedFeature myLightingFeature;
        createLightingFeature(myLightingFeature, UNLIT);
        myMaterialBuilder.setType(myLightingFeature);

        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f",
                SURFACE_COLOR_PROPERTY, Vector4f(0.8f, 0.8f, 0.8f, 1.0f));
    }
    
    myMaterialBuilder.addFeature("vertexparams", VectorOfRankedFeature(10,"color"));

    string myBlendFunction = "[src_alpha, one_minus_src_alpha]";
    setPropertyValue<VectorOfString>(myMaterialBuilder.getNode(), "vectorofstring",
            BLENDFUNCTION_PROPERTY, asl::as<VectorOfString>(myBlendFunction));

    myMaterialBuilder.computeRequirements();

    return myMaterialBuilder.getNode()->getAttributeString("id");
}

void
CTScan::resetCamera(ScenePtr theScene, const asl::Box3f & theBox) {
    float myDiameter = asl::distance(theBox[0], theBox[1]);
    // Set camera to ortho and the width to the bodies bounding-box-size
    dom::NodePtr myWorld = theScene->getWorldRoot();
    dom::NodePtr myCamera = myWorld->childNode("camera");
    NodePtr myAttr = myCamera->getAttribute("width");
    if (myAttr) {
        float & myWidth = myAttr->nodeValueRefOpen<float>();
        myWidth = myDiameter;
        myAttr->nodeValueRefClose<float>();
    } else {
        myCamera->appendAttribute("width", myDiameter);
    }
    myAttr = myCamera->getAttribute("hfov");
    float & myFOV = myAttr->nodeValueRefOpen<float>();
    myFOV = 0.0;
    myAttr->nodeValueRefClose<float>();
}

void
CTScan::setupScene(ScenePtr theScene, const std::string & theShapeId) {
    dom::NodePtr myWorld = theScene->getWorldRoot();
    dom::NodePtr myCamera = myWorld->childNode("camera");
    // Move camera out of object
    NodePtr myAttr = myCamera->getAttribute("position");
    Vector3f & myPosition = myAttr->nodeValueRefOpen<asl::Vector3f>();
    myPosition = asl::Vector3f(0.0, 0.0, 100.0);
    myAttr->nodeValueRefClose<asl::Vector3f>();
    //Append a parent transform
    TransformBuilder myTranform("Main");
    myWorld->appendChild(myTranform.getNode());;
    // Append Body
    y60::BodyBuilder myBody(theShapeId, "poly_model");
    myTranform.appendObject(myBody);
}


template<class PIXEL>
void
CTScan::appendEmptySlices(asl::Block & the3DTexture, unsigned theSliceCount, int theBytesPerSlice) {
    unsigned mySamplesPerSlice = theBytesPerSlice / sizeof(PIXEL);
    std::vector<PIXEL> myEmptySlice(mySamplesPerSlice, NumericTraits<PIXEL>::min());

    for (unsigned i = 0; i < theSliceCount; ++i) {
        AC_TRACE << "appending empty slice with " << theBytesPerSlice << " bytes";
        the3DTexture.append(static_cast<void*>(&(*myEmptySlice.begin())), theBytesPerSlice);
    }
}

void
CTScan::create3DTexture(dom::NodePtr theImageNode, int theMaxTextureSize) {
    asl::Vector3i myTextureSize(getVoxelDimensions());
    for (int i = 0; i < 3; ++i) {
        myTextureSize[i] = nextPowerOfTwo(myTextureSize[i]);
        if (myTextureSize[i] > theMaxTextureSize) {
            myTextureSize[i] = theMaxTextureSize;
        }
    }
    
    asl::Block my3DTexture;
    int mySliceCount = getSliceCount();
    int myBytesPerSlice = getBytesRequired(myTextureSize[0]*myTextureSize[1], _myEncoding);
    AC_DEBUG << "creating 3D texture, size=" << myTextureSize << ", slice count=" 
        << mySliceCount << ", bytes/slice=" << myBytesPerSlice;

    switch (_myEncoding) {
        case GRAY:
            appendEmptySlices<unsigned char>(my3DTexture, 1, myBytesPerSlice);
            break;
        case GRAY16:
            appendEmptySlices<unsigned short>(my3DTexture, 1, myBytesPerSlice);
            break;
        case GRAYS16:
            appendEmptySlices<signed short>(my3DTexture, 1, myBytesPerSlice);
            break;
    }
    for (int z = 0; z < myTextureSize[2]-2; ++z) {
        int mySlice = int(floor(float(z*mySliceCount)/myTextureSize[2]));
        AC_TRACE << " adding slice " << mySlice; 
        CTScan::appendTo3DTexture(mySlice, my3DTexture, myTextureSize[0], myTextureSize[1]);
    }
    if (myTextureSize[2] > 1) {
        switch (_myEncoding) {
            case GRAY:
                appendEmptySlices<unsigned char>(my3DTexture, 1, myBytesPerSlice);
                break;
            case GRAY16:
                appendEmptySlices<unsigned short>(my3DTexture, 1, myBytesPerSlice);
                break;
            case GRAYS16:
                appendEmptySlices<signed short>(my3DTexture, 1, myBytesPerSlice);
                break;
        }
    }
    y60::ImagePtr myImage = theImageNode->getFacade<y60::Image>();
    myImage->set(myTextureSize[0], myTextureSize[1], myTextureSize[2], _myEncoding, my3DTexture);
    AC_DEBUG << "done creating 3d texture" << endl;
}

template <class VoxelT>
void
CTScan::countVoxelValues(const asl::Box3i & theVOI, std::vector<unsigned> & theHistogram,
                         bool useOccurringRange) {
    Vector2d myValueRange;
    if (useOccurringRange) {
        myValueRange = getOccurringValueRange();
    } else {
        myValueRange = getValueRange();
    }
    int myValueCount = int(myValueRange[1] - myValueRange[0]) + 1;
    int myXStart = theVOI[Box3i::MIN][0];
    int myYStart = theVOI[Box3i::MIN][1];
    int myZStart = theVOI[Box3i::MIN][2];
    int myXEnd = theVOI[Box3i::MAX][0];
    int myYEnd = theVOI[Box3i::MAX][1];
    int myZEnd = theVOI[Box3i::MAX][2];

    VoxelT myValueOffset = VoxelT(myValueRange[0]);
    int myStride = getVoxelDimensions()[0];

    theHistogram.clear();
    theHistogram.resize(myValueCount);

    VoxelT myVoxel;
    int myIndex;
    for (int i = myZStart; i < myZEnd; ++i) {
        VoxelT * mySlice = getSlicePtr<VoxelT>(i);
        for (int j = myYStart; j < myYEnd; ++j) {
            for (int k = myXStart; k < myXEnd; ++k) {
                myVoxel = * (mySlice + j * myStride + k);
                myIndex = int( myVoxel - myValueOffset);
                theHistogram[myIndex]++;
            }
        }
    }
}

void
CTScan::computeHistogram(const Box3i & theVOI, std::vector<unsigned> & theHistogram,
                         bool useOccurringRange)
{
    switch (_myEncoding) {
        case y60::GRAY:
            countVoxelValues<unsigned char>(theVOI, theHistogram, useOccurringRange);
            break;
        case y60::GRAY16:
            countVoxelValues<unsigned short>(theVOI, theHistogram, useOccurringRange);
            break;
        case y60::GRAYS16:
            countVoxelValues<short>(theVOI, theHistogram, useOccurringRange);
            break;
        default:
            throw CTScanException("Unhandled voxel type in CTScan::computeHistogram()", PLUS_FILE_LINE);
    }
    
};

void
CTScan::computeProfile(const std::vector<asl::Point3i> & thePoints, 
        std::vector<Signed16> & theProfile, std::vector<asl::Point3i> & thePointsSampled)
{
    if (thePoints.empty()) {
        return;
    }
    
    // for each segment, add the profile from [i, i+1[ (half-open interval)
    for (int i = 0; i+1 < thePoints.size(); ++i) {
        computeLineSegmentProfile(thePoints[i], thePoints[i+1], theProfile, thePointsSampled);
    }
    // now add the last point
    const Point3i & myLastPoint = thePoints[thePoints.size()-1];
    computeProfile(myLastPoint, theProfile);
    thePointsSampled.push_back(myLastPoint);
}

void
CTScan::computeProfile(const asl::Point3i & thePosition, std::vector<Signed16> & theProfile) {
    const unsigned char * myData = _mySlices[thePosition[2]]->pixels().begin();
    myData += getBytesRequired(thePosition[0], _myEncoding)
              +thePosition[1]*getBytesRequired(_mySlices[thePosition[2]]->width(), _myEncoding);
    switch (_myEncoding) {
        case y60::GRAY:
            theProfile.push_back(*myData);
            break;
        case y60::GRAY16:
            theProfile.push_back(*reinterpret_cast<const Unsigned16*>(myData));
            break;
        case y60::GRAYS16:
            theProfile.push_back(*reinterpret_cast<const Signed16*>(myData));
            break;
        default:
            throw CTScanException("Unhandled voxel type in CTScan::computeProfile()", PLUS_FILE_LINE);
    }
}

// computes the grey-scale profile of all voxels
// from [theStart, theEnd[  (without theEnd)
void
CTScan::computeLineSegmentProfile(const asl::Point3i & theStart, const asl::Point3i & theEnd, 
        std::vector<Signed16> & theProfile, std::vector<Point3i> & thePointsSampled) 
{
    // find the axis with the max delta
    int maxDelta = 0;
    int maxDeltaAxis = -1;
    for (int i = 0; i < 3; ++i) {
        int curDelta = abs(theStart[i] - theEnd[i]);
        if (curDelta > maxDelta) {
            maxDeltaAxis = i;
            maxDelta = curDelta; 
        }
    }
    if (maxDeltaAxis == -1) {
        return;
    }
    Vector3i totalDelta(theEnd-theStart);
    Vector3f myStep = Vector3f(float(totalDelta[0]) / maxDelta, 
                               float(totalDelta[1]) / maxDelta, 
                               float(totalDelta[2]) / maxDelta);

    for (int i = 0; i < maxDelta; ++i) {
        Vector3f curDelta(myStep*i);

        asl::Point3i curPoint(theStart + Vector3i(int(round(curDelta[0])), 
                                                   int(round(curDelta[1])), 
                                                   int(round(curDelta[2]))));
        computeProfile(curPoint, theProfile);
        thePointsSampled.push_back(curPoint);
    }
}

int 
CTScan::appendTo3DTexture(int theSlice, asl::Block & the3dTexture, int theXSize, int theYSize) {
    PLPixelFormat myFormat;
    if (!mapPixelEncodingToFormat(_myEncoding, myFormat)) {
        throw CTException("Unsupported pixel encoding", PLUS_FILE_LINE);
    }

    PLSubBmp myTempBitmap;
    const unsigned char * myData = _mySlices[theSlice]->pixels().begin();

    myTempBitmap.Create(_mySlices[theSlice]->width(), _mySlices[theSlice]->height(),
                        myFormat, const_cast<PLBYTE*>(myData),
                        getBytesRequired(_mySlices[theSlice]->width(), _myEncoding));
    PLAnyBmp myScaledBitmap;
    PLFilterResizeBilinear myScaler(theXSize, theYSize);
    myScaler.Apply(& myTempBitmap, & myScaledBitmap);

    PLBYTE ** myLineArray = myScaledBitmap.GetLineArray();

    ReadableBlockAdapter myAdapter(myLineArray[0],
            myLineArray[myScaledBitmap.GetHeight() - 1] + 
            myScaledBitmap.GetBytesPerLine());
    the3dTexture.append(myAdapter);
    return myAdapter.size();
}

bool 
CTScan::notifyProgress(double theProgress, const std::string & theMessage) {
    //cerr << "progress = " << theProgress << endl;
    if (_myProgressSignal.empty() ) {
        return true;
    } else {
        return _myProgressSignal.emit(theProgress, Glib::ustring(theMessage));
    }
}

NodePtr
CTScan::createGrayImage(dom::NodePtr theParent, int theWidth, int theHeight, int theValue) {
    NodePtr myRasterNode = theParent->appendChild(NodePtr(new Element("rasterofgray")));
    myRasterNode->appendChild(NodePtr(new Text()));

    ResizeableRasterPtr myRaster
        = dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());

    Block myBlock(theWidth * theHeight, theValue);
    myRaster->assign(theWidth, theHeight, myBlock);
    //AC_DEBUG << "created raster of " << myRaster->width() << "x" << myRaster->height();
    return myRasterNode;
}

dom::NodePtr
CTScan::createRGBAImage(dom::NodePtr theParent, int theWidth, int theHeight, int theValue) {
    asl::Block myBlock(theWidth * theHeight * 4, theValue);
    dom::NodePtr myImage(new dom::Element("image"));
    theParent->appendChild(myImage);
    myImage->getFacade<Image>()->set<ImageMipmapTag>(false);
    myImage->getFacade<Image>()->set(theWidth, theHeight, 1, y60::RGBA, myBlock);
    return myImage;
}

void 
CTScan::resizeVoxelVolume(dom::NodePtr theVoxelVolume, const asl::Box3f & theDirtyBox) {
    const Box3f & myOldBox = theVoxelVolume->getAttributeValue<Box3f>("boundingbox");

    Box3f myNewBox = myOldBox;
    //Box3f myDirtyBox(theDirtyBox.getMin()[0]-1, theDirtyBox.getMin()[1]-1, theDirtyBox.getMin()[2]-1,
    //    theDirtyBox.getMax()[0]+1, theDirtyBox.getMax()[1]+1, theDirtyBox.getMax()[2]+1);
    Box3f myDirtyBox(theDirtyBox);
    myNewBox.extendBy( myDirtyBox );

    //AC_INFO << "theDirtyBox = " << theDirtyBox;
    //AC_INFO << "myOldBox = " << myOldBox;
    //AC_INFO << "myNewBox = " << myNewBox;

    if (myNewBox == myOldBox) {
        //AC_INFO << "Raster box didn't change.";
        return;
    }

    const Vector3f & mySize = myNewBox.getSize();

    int myTargetWidth  = int( round(mySize[0]) );
    int myTargetHeight = int( round(mySize[1]) );
    //AC_INFO << "target width = " << myTargetWidth << " target height = " << myTargetHeight;
    int myNewRasterCount  = int( round(mySize[2]) );

    int myNewBoxBegin = int( round( myNewBox[Box3f::MIN][2] ));
    int myNewBoxEnd   = int( round( myNewBox[Box3f::MAX][2] ));
    //AC_INFO << "myNewBoxBegin = " << myNewBoxBegin;
    //AC_INFO << "myNewBoxEnd = " << myNewBoxEnd;

    int myOldBoxBegin = int( round(myOldBox[Box3f::MIN][2]) );
    int myOldBoxEnd   = int( round(myOldBox[Box3f::MAX][2]) );

    dom::NodePtr myRasters = theVoxelVolume->childNode("rasters");
    dom::NodePtr myOldRaster;
    dom::NodePtr myNewRaster;

    unsigned myXOrigin = unsigned( round(myOldBox[Box3f::MIN][0] - myNewBox[Box3f::MIN][0]));
    unsigned myYOrigin = unsigned( round(myOldBox[Box3f::MIN][1] - myNewBox[Box3f::MIN][1]));

    for(int i = myNewBoxBegin;i < myNewBoxEnd; ++i) {
        myNewRaster = CTScan::createGrayImage(myRasters, myTargetWidth, myTargetHeight, 0);
        if (i >= myOldBoxBegin && i < myOldBoxEnd) {
            //AC_INFO << "blitting to " << myXOrigin << "x" << myYOrigin << "on " << i;
            myOldRaster = myRasters->childNode(0);
            ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>(
                    myNewRaster->childNode(0)->nodeValueWrapperPtr());
            ValuePtr mySourceRaster = myOldRaster->childNode(0)->nodeValueWrapperPtr();
            myTargetRaster->pasteRaster(myXOrigin, myYOrigin, * mySourceRaster); 
            myRasters->removeChild(myOldRaster);
        } else {
            //AC_INFO << "Creating new " << i;
        }
    }

    theVoxelVolume->getAttribute("boundingbox")->nodeValueAssign(myNewBox);
}

void
CTScan::fillColorMap(dom::NodePtr thePaletteNode, ColorMap & theColorMap) {
    Vector3i myColor;
    unsigned char myIndex;
    for (unsigned i = 0; i < thePaletteNode->childNodesLength(); ++i) {
        dom::NodePtr myItem = thePaletteNode->childNode(i);
        myColor = myItem->getAttributeValue<Vector3i>("color");
        myIndex = (unsigned char)(myItem->getAttributeValue<int>("index"));
        theColorMap[myColor] = myIndex;
        //cerr << "index = " << int(myIndex) << " color = " << myColor << endl;
    }
}

void 
CTScan::applyBrush(dom::NodePtr theCanvasImage, unsigned theX, unsigned theY,
                   dom::NodePtr theBrushImage, const asl::Vector4f & theColor)
{
    ResizeableRasterPtr myCanvasRaster = dynamic_cast_Ptr<ResizeableRaster>(
            theCanvasImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());
    ResizeableRasterPtr myBrushRaster = dynamic_cast_Ptr<ResizeableRaster>(
            theBrushImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());

    int myXOffset = myBrushRaster->width() / 2;
    int myYOffset = myBrushRaster->height() / 2;

    int myXOrigin = int(theX) - myXOffset;
    int myYOrigin = int(theY) - myYOffset;
    int myXStart(0);
    int myYStart(0);

    if (myXOrigin < 0) {
        myXStart = - myXOrigin;    
    }
    if (myYOrigin < 0) {
        myYStart = - myYOrigin;
    }
    int myXEnd(myBrushRaster->width());
    int myYEnd(myBrushRaster->height());

    if (theX + myXOffset > myCanvasRaster->width()) {
        myXEnd -= theX + myXOffset - myCanvasRaster->width();
    }
    if (theY + myYOffset > myCanvasRaster->height()) {
        myYEnd -= theY + myYOffset - myCanvasRaster->height();
    }

    for (unsigned y = myYStart; y < myYEnd; ++y) {
        for (unsigned x = myXStart; x < myXEnd; ++x) {
            // AC_TRACE << "getPixel(" << x << "," << y << ")";
            asl::Vector4f myBrushPixel = myBrushRaster->getPixel(x, y);
            if (int(myBrushPixel[0]) == 255) {
                // AC_TRACE << "setPixel(" << x +myXOrigin<< "," << y +myYOrigin<< ")";
                myCanvasRaster->setPixel(myXOrigin + x, myYOrigin + y,
                        theColor[0], theColor[1], theColor[2], theColor[3]);
            }
        }    
    }
}


void 
CTScan::copyCanvasToVoxelVolume(dom::NodePtr theMeasurement, dom::NodePtr theCanvasImage,
                                const asl::Box3f & theDirtyBox, Orientation theOrientation,
                                dom::NodePtr thePaletteNode)
{
    ColorMap myColorMap;
    fillColorMap(thePaletteNode, myColorMap);

    ResizeableRasterPtr myCanvas = dynamic_cast_Ptr<ResizeableRaster>(
            theCanvasImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());

    Box3f myMeasurementBox = theMeasurement->getAttributeValue<Box3f>("boundingbox");
    //AC_WARNING << "theDirtyBox=" << theDirtyBox;
    //AC_WARNING << "myMeasurementBox=" << myMeasurementBox;

    unsigned myCanvasLineStride = getBytesRequired(myCanvas->width(), y60::RGBA);
    unsigned myBytesPerPixel = getBytesRequired(1, y60::RGBA);
    const unsigned char * mySourcePixels = myCanvas->pixels().begin();
    switch (theOrientation) {
        case CTScan::IDENTITY:
            {
                Vector3i myPixel;
                unsigned myAffectedSlice = unsigned( theDirtyBox[Box3f::MIN][2] - myMeasurementBox[Box3f::MIN][2]);
                unsigned myXStart = unsigned(theDirtyBox[Box3f::MIN][0]);
                unsigned myXEnd   = unsigned(theDirtyBox[Box3f::MAX][0]);
                unsigned myYStart = unsigned(theDirtyBox[Box3f::MIN][1]);
                unsigned myYEnd   = unsigned(theDirtyBox[Box3f::MAX][1]);
                dom::NodePtr myRasterNode = theMeasurement->childNode("rasters")->childNode(myAffectedSlice);
                if ( ! myRasterNode) {
                    throw asl::Exception("Failed to get affected raster.");
                }
                ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>(
                        myRasterNode->childNode(0)->nodeValueWrapperPtr());
                unsigned myTargetLineStride  = getBytesRequired(myTargetRaster->width(), y60::GRAY);

                unsigned char * myTargetPixels = myTargetRaster->pixels().begin();

                for (unsigned y = myYStart; y < myYEnd; ++y) {
                    for (unsigned x = myXStart; x < myXEnd; ++x) {

                        const unsigned char * myRGBPixel = & mySourcePixels[y * myCanvasLineStride + x * myBytesPerPixel];
                        myPixel = Vector3i(myRGBPixel[0], myRGBPixel[1], myRGBPixel[2]);

                        // XXX: argh! map.find() in inner loop... any ideas? [DS]
                        ColorMap::iterator myIt = myColorMap.find(myPixel);
                        if (myIt == myColorMap.end()) {
                            throw asl::Exception(string("Unknown color in canvas image: ") + as_string(myPixel),
                                                 PLUS_FILE_LINE);
                        }
                        unsigned char * myTargetPixel = & myTargetPixels[(y - int(myMeasurementBox[Box3f::MIN][1])) * myTargetLineStride + 
                                        x - int(myMeasurementBox[Box3f::MIN][0])];
                        * myTargetPixel = myIt->second;
                    }
                }
            }
            break;
        case CTScan::Y2Z: // top
            {
                Vector3i myPixel;
                unsigned myAffectedY = unsigned( theDirtyBox[Box3f::MIN][1] - myMeasurementBox[Box3f::MIN][1]);
                unsigned myXStart = unsigned(theDirtyBox[Box3f::MIN][0]);
                unsigned myXEnd   = unsigned(theDirtyBox[Box3f::MAX][0]);
                unsigned myZStart = unsigned(theDirtyBox[Box3f::MIN][2]);
                unsigned myZEnd   = unsigned(theDirtyBox[Box3f::MAX][2]);
                unsigned myCanvasHeight = theCanvasImage->getAttributeValue<unsigned>("height");

                for (unsigned z = myZStart; z < myZEnd; ++z) {
                    unsigned myCurrentSlice = z - unsigned(myMeasurementBox[Box3f::MIN][2]);
                    dom::NodePtr myRasterNode = theMeasurement->childNode("rasters")->childNode(myCurrentSlice);
                    if ( ! myRasterNode) {
                        throw asl::Exception("Failed to get affected raster.");
                    }
                    ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>(
                            myRasterNode->childNode(0)->nodeValueWrapperPtr());

                    unsigned myTargetLineStride  = getBytesRequired(myTargetRaster->width(), y60::GRAY);
                    unsigned char * myTargetPixels = myTargetRaster->pixels().begin();

                    unsigned mySourceY = myCanvasHeight - z - 1;
                    for (unsigned x = myXStart; x < myXEnd; ++x) {

                        const unsigned char * myRGBPixel = & mySourcePixels[mySourceY * myCanvasLineStride + x * myBytesPerPixel];
                        myPixel = Vector3i(myRGBPixel[0], myRGBPixel[1], myRGBPixel[2]);

                        // XXX: argh! map.find() in inner loop... any ideas? [DS]
                        ColorMap::iterator myIt = myColorMap.find(myPixel);
                        if (myIt == myColorMap.end()) {
                            throw asl::Exception(string("Unknown color in canvas image: ") + as_string(myPixel),
                                                 PLUS_FILE_LINE);
                        }
                        unsigned char * myTargetPixel = & myTargetPixels[myAffectedY * myTargetLineStride +
                                                        (x - int(myMeasurementBox[Box3f::MIN][0]))];
                        * myTargetPixel = myIt->second;
                    }
                }
            }
            break;
        case CTScan::X2Z:
            {
                Vector3i myPixel;
                unsigned myAffectedX = unsigned( theDirtyBox[Box3f::MIN][0] - myMeasurementBox[Box3f::MIN][0]);
                unsigned myYStart = unsigned(theDirtyBox[Box3f::MIN][1]);
                unsigned myYEnd   = unsigned(theDirtyBox[Box3f::MAX][1]);
                unsigned myZStart = unsigned(theDirtyBox[Box3f::MIN][2]);
                unsigned myZEnd   = unsigned(theDirtyBox[Box3f::MAX][2]);
                unsigned myCanvasWidth = theCanvasImage->getAttributeValue<unsigned>("width");
                
                for (unsigned z = myZStart; z < myZEnd; ++z) {
                    unsigned myCurrentSlice = z - unsigned(myMeasurementBox[Box3f::MIN][2]);
                    dom::NodePtr myRasterNode = theMeasurement->childNode("rasters")->childNode(myCurrentSlice);
                    if ( ! myRasterNode) {
                        throw asl::Exception("Failed to get affected raster.");
                    }
                    ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>(
                            myRasterNode->childNode(0)->nodeValueWrapperPtr());

                    unsigned myTargetLineStride  = getBytesRequired(myTargetRaster->width(), y60::GRAY);
                    unsigned char * myTargetPixels = myTargetRaster->pixels().begin();

                    for (unsigned y = myYStart; y < myYEnd; ++y) {

                        const unsigned char * myRGBPixel = & mySourcePixels[y * myCanvasLineStride + z * myBytesPerPixel];
                        myPixel = Vector3i(myRGBPixel[0], myRGBPixel[1], myRGBPixel[2]);

                        // XXX: argh! map.find() in inner loop... any ideas? [DS]
                        ColorMap::iterator myIt = myColorMap.find(myPixel);
                        if (myIt == myColorMap.end()) {
                            throw asl::Exception(string("Unknown color in canvas image: ") + as_string(myPixel),
                                                 PLUS_FILE_LINE);
                        }
                        
                        unsigned char * myTargetPixel = & myTargetPixels[(y - int(myMeasurementBox[Box3f::MIN][1])) * myTargetLineStride +
                                                        myAffectedX];
                        * myTargetPixel = myIt->second;
                    }
                }
            }
            break;
        default:
            break;

    }
}

void
CTScan::copyVoxelVolumeToCanvas(dom::NodePtr theMeasurement, dom::NodePtr theCanvas, dom::NodePtr theReconstructedImage, unsigned theSliceIndex,
                                    Orientation theOrientation, dom::NodePtr thePaletteNode) 
{
    y60::PixelEncoding myEncoding;
    myEncoding = theReconstructedImage->getFacade<Image>()->getEncoding();
    switch (myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                copyVoxelVolumeToCanvasImpl<VoxelT>(theMeasurement, theCanvas, theReconstructedImage, theSliceIndex, theOrientation, thePaletteNode);            
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                copyVoxelVolumeToCanvasImpl<VoxelT>(theMeasurement, theCanvas, theReconstructedImage, theSliceIndex, theOrientation, thePaletteNode);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                copyVoxelVolumeToCanvasImpl<VoxelT>(theMeasurement, theCanvas, theReconstructedImage, theSliceIndex, theOrientation, thePaletteNode);
            }
            break;
        default:
            throw CTScanException("Unhandled voxel type", PLUS_FILE_LINE);   
    }
}

template <class VoxelT>
unsigned char
CTScan::getSegmentationAlpha(unsigned theIndex, VoxelT theVoxel, const PaletteTable<VoxelT> & thePalette, unsigned char theAlpha) {
    if (theIndex == 0) {
        return 0;
    } else if (theVoxel < thePalette[theIndex].thresholds[0] || 
               theVoxel > thePalette[theIndex].thresholds[1]) 
    {
        return theAlpha;
    } else {
        return 255;
    }
}

template <class VoxelT>
void 
CTScan::copyVoxelVolumeToCanvasImpl(dom::NodePtr theMeasurement, dom::NodePtr theCanvas, 
                                    dom::NodePtr theReconstructedImage, unsigned theSliceIndex,
                                    Orientation theOrientation, dom::NodePtr thePaletteNode)
{
    PaletteTable<VoxelT> myPalette(255, PaletteItem<VoxelT>(Vector2<VoxelT>(-1, -1), Vector3i( -1, -1, -1)));
    unsigned myIndex;
    Vector2f myThresholdsFloat;
    Vector3i myColor;
    unsigned char myAlpha = 255;
    if (thePaletteNode->dom::Node::getAttributeValue<bool>("livesegmentation")) {
        float myFloatingAlpha = thePaletteNode->dom::Node::getAttributeValue<float>("livesegmentationalpha");
        myAlpha = static_cast<unsigned char>(myFloatingAlpha * 255.0);
    } 
    for (unsigned i = 0; i < thePaletteNode->childNodesLength(); ++i) {
        myIndex = thePaletteNode->childNode(i)->dom::Node::getAttributeValue<unsigned>("index");
        PaletteItem<VoxelT> myItem;
        myItem.color = thePaletteNode->childNode(i)->dom::Node::getAttributeValue<Vector3i>("color");
        myThresholdsFloat = thePaletteNode->childNode(i)->dom::Node::getAttributeValue<Vector2f>("threshold");
        myItem.thresholds = Vector2<VoxelT>(VoxelT(myThresholdsFloat[0]), VoxelT(myThresholdsFloat[1]));
        myPalette[myIndex] = myItem;
    }

    Box3f myBoundingBox = theMeasurement->dom::Node::getAttributeValue<Box3f>("boundingbox");
    Point3i myMin(round(myBoundingBox[Box3f::MIN][0]), 
                  round(myBoundingBox[Box3f::MIN][1]), 
                  round(myBoundingBox[Box3f::MIN][2]));
    Point3i myMax(round(myBoundingBox[Box3f::MAX][0]), 
                  round(myBoundingBox[Box3f::MAX][1]), 
                  round(myBoundingBox[Box3f::MAX][2]));

    ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>( 
            theCanvas->childNode(0)->childNode(0)->nodeValueWrapperPtr());
    unsigned myCanvasWidth = theCanvas->dom::Node::getAttributeValue<unsigned>("width");
    unsigned myCanvasHeight = theCanvas->dom::Node::getAttributeValue<unsigned>("height");

    ResizeableRasterPtr myReconstructedRaster = dynamic_cast_Ptr<ResizeableRaster>(
            theReconstructedImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());

    y60::PixelEncoding myEncoding = theReconstructedImage->dom::Node::getFacade<y60::Image>()->getEncoding();
    unsigned myRIHeight = myCanvasHeight; // use non power of two size from dom 
    unsigned myRIStride = getBytesRequired(myReconstructedRaster->width(), myEncoding);
    unsigned myRIBPP    = getBytesRequired(1, myEncoding);
    const unsigned char * myRIPixels = myReconstructedRaster->pixels().begin();


    unsigned myTargetLineStride = getBytesRequired(myCanvasWidth, y60::RGBA);
    unsigned myBytesPerPixel = getBytesRequired(1, y60::RGBA);
    unsigned char * myTargetPixels = myTargetRaster->pixels().begin();
    unsigned myPixelIndex;
    
    switch ( theOrientation) {
        case CTScan::IDENTITY: // front
            if (theSliceIndex >= myMin[2] && theSliceIndex < myMax[2]) {
                dom::NodePtr myRasterNode = theMeasurement->childNode(0)->childNode(int( theSliceIndex - myMin[2]));
                ResizeableRasterPtr mySourceRaster =
                    dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());
                unsigned mySourceLineStride  = getBytesRequired(mySourceRaster->width(), y60::GRAY);
                unsigned char * mySourcePixels = mySourceRaster->pixels().begin();

                unsigned myXStart = unsigned(myMin[0]);
                unsigned myXEnd = unsigned(myMax[0]);
                unsigned myYStart = unsigned(myMin[1]);
                unsigned myYEnd = unsigned(myMax[1]);
                Vector3i myColor;
                VoxelT myVoxel;
                for (unsigned y = myYStart; y < myYEnd; ++y) {
                    for (unsigned x = myXStart; x < myXEnd; ++x) {
                        myIndex = mySourcePixels[(y - myYStart) * mySourceLineStride + (x - myXStart)];
                        myColor = myPalette[myIndex].color;

                        myVoxel = *reinterpret_cast<const VoxelT*>(&(myRIPixels[y * myRIStride + x * myRIBPP]));
                        myPixelIndex =  y * myTargetLineStride + x * myBytesPerPixel;
                        myTargetPixels[myPixelIndex]     = myColor[0];
                        myTargetPixels[myPixelIndex + 1] = myColor[1];
                        myTargetPixels[myPixelIndex + 2] = myColor[2];
                        myTargetPixels[myPixelIndex + 3] = getSegmentationAlpha(myIndex, myVoxel, myPalette, myAlpha);
                    }
                }
            }
            break;
        case CTScan::Y2Z: // top
            if (theSliceIndex >= myMin[1] && theSliceIndex < myMax[1]) {
                unsigned mySourceY = unsigned( theSliceIndex - myMin[1]);
                unsigned myZStart = unsigned( myMin[2]);
                unsigned myZEnd = unsigned( myMax[2]);
                unsigned myXStart = unsigned(myMin[0]);
                unsigned myXEnd = unsigned(myMax[0]);
                Vector3i myColor;
                VoxelT myVoxel;
                for (unsigned z = myZStart; z < myZEnd; ++z) {
                    unsigned myCurrentSliceIndex = z - myZStart;
                    dom::NodePtr myRasterNode = theMeasurement->childNode(0)->childNode(myCurrentSliceIndex);
                    ResizeableRasterPtr mySourceRaster =
                        dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());

                    unsigned mySourceLineStride  = getBytesRequired(mySourceRaster->width(), y60::GRAY);
                    unsigned char * mySourcePixels = mySourceRaster->pixels().begin();

                    for (unsigned x = myXStart; x < myXEnd; ++x) {
                        myIndex = mySourcePixels[mySourceY * mySourceLineStride + (x - myXStart)];
                        myColor = myPalette[myIndex].color;
                        myPixelIndex = (myRIHeight - z - 1) * myRIStride + x * myRIBPP;
                        myVoxel = *reinterpret_cast<const VoxelT*>(&(myRIPixels[myPixelIndex]));
                        myPixelIndex = (myCanvasHeight - z - 1) * myTargetLineStride + x * myBytesPerPixel;
                        myTargetPixels[myPixelIndex] = myColor[0];
                        myTargetPixels[myPixelIndex + 1] = myColor[1];
                        myTargetPixels[myPixelIndex + 2] = myColor[2];
                        myTargetPixels[myPixelIndex + 3] = getSegmentationAlpha(myIndex, myVoxel, myPalette, myAlpha);
                    }
                }
            }
            break;
        case CTScan::X2Z: // side
            if (theSliceIndex >= myMin[0] && theSliceIndex < myMax[0]) {
                unsigned mySourceX = unsigned( theSliceIndex - myMin[0]);
                unsigned myZStart = unsigned( myMin[2]);
                unsigned myZEnd = unsigned( myMax[2]);
                unsigned myYStart = unsigned(myMin[1]);
                unsigned myYEnd = unsigned(myMax[1]);
                Vector3i myColor;
                VoxelT myVoxel;
                for (unsigned z = myZStart; z < myZEnd; ++z) {
                    unsigned myCurrentSliceIndex = z - myZStart;
                    dom::NodePtr myRasterNode = theMeasurement->childNode(0)->childNode(myCurrentSliceIndex);
                    ResizeableRasterPtr mySourceRaster =
                        dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());

                    unsigned mySourceLineStride  = getBytesRequired(mySourceRaster->width(), y60::GRAY);
                    unsigned char * mySourcePixels = mySourceRaster->pixels().begin();

                    for (unsigned y = myYStart; y < myYEnd; ++y) {
                        myIndex = mySourcePixels[(y - myYStart) * mySourceLineStride + mySourceX];
                        myColor = myPalette[myIndex].color;
                        myVoxel = *reinterpret_cast<const VoxelT*>(&(myRIPixels[y * myRIStride + z * myRIBPP]));
                        myPixelIndex = y * myTargetLineStride + z * myBytesPerPixel;
                        myTargetPixels[myPixelIndex] = myColor[0];
                        myTargetPixels[myPixelIndex + 1] = myColor[1];
                        myTargetPixels[myPixelIndex + 2] = myColor[2];
                        myTargetPixels[myPixelIndex + 3] = getSegmentationAlpha(myIndex, myVoxel, myPalette, myAlpha);                        
                    }
                }
            }
            break;
    }
}

}

