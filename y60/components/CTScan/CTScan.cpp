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
//   $RCSfile: CTScan.cpp,v $
//   $Author: martin $
//   $Revision: 1.22 $
//   $Date: 2005/04/25 11:02:50 $
//
//
//=============================================================================

#include "CTScan.h"

#include <fstream>
#include <algorithm>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/Filter/plfilterresizebilinear.h>
#include <paintlib/plpixelformat.h>
#include <paintlib/planybmp.h>
#include <paintlib/plsubbmp.h>
#include <paintlib/plpngenc.h>
#include <paintlib/plexcept.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include <asl/base/Assure.h>
#include <asl/base/MappedBlock.h>
#include <asl/base/Logger.h>
#include <asl/zip/PackageManager.h>
#include <asl/math/Box.h>
#include <asl/math/Line.h>
#include <asl/base/begin_end.h>

#include <y60/base/property_functions.h>
#include <y60/base/PropertyNames.h>
#include <y60/scene/WorldBuilder.h>
#include <y60/scene/TransformBuilder.h>
#include <y60/scene/BodyBuilder.h>
#include <y60/scene/MaterialBuilder.h>
#include <y60/scene/CameraBuilder.h>
#include <y60/scene/Camera.h>
#include <y60/image/Image.h>
#include <y60/image/PLFilterResizePadded.h>
#include <y60/scene/Facades.h>

#include "PLFile.h"
#include "DicomFile.h"
#include "MarchingCubes.h"

using namespace asl;
using namespace std;
using namespace dom;
using namespace y60;

#define DB(x) // x;

namespace y60 {

CTScan::CTScan() : _myEncoding(y60::GRAY), _myDefaultWindow(0.0f, 0.0f), _myVoxelSize(0,0,0) {
}

CTScan::~CTScan() {
}

asl::Matrix4f
CTScan::getModelViewMatrix(const asl::Quaternionf & theOrientation) {
    Matrix4f myModelView;
    myModelView.makeScaling(getVoxelAspect());

    Quaternionf myOrientation = theOrientation; // inverse camera rotation
    myOrientation.invert();

    myModelView.postMultiply(Matrix4f(myOrientation));

    Quaternionf myYFlipRotation;
    myYFlipRotation.assignFromEuler(Vector3f(float(PI),0.0f,0.0f));
    myModelView.postMultiply(Matrix4f(myYFlipRotation));

    return myModelView;
}

asl::Planef
CTScan::getVoxelPlane(const asl::Quaternionf & theOrientation, float theVoxelSlice) {
    Quaternionf myOrientation = theOrientation;

    Vector4f myVoxelNormal = product(Vector4f(0,0,1,0), myOrientation);

    Vector3f myNormal3(myVoxelNormal[0], myVoxelNormal[1], myVoxelNormal[2]);
    Planef myVoxelSpacePlane(myNormal3, -theVoxelSlice);
    return myVoxelSpacePlane;
}

int
CTScan::loadSlices(asl::PackageManager & thePackageManager, const std::string & theSubDir, const std::string& thePackage) {
    clear();
    vector<string> myFileList(thePackageManager.findFiles(theSubDir,thePackage));
    AC_TRACE << "found " << myFileList.size() << " files in " << thePackage << ":" << theSubDir;

    if ( myFileList.empty()) {
        throw CTScanException("No files in " + thePackage +"/"+theSubDir + " found \n", PLUS_FILE_LINE);
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
        if ( myFrameCount && (i <= representativeFileIndex || !representativeFile )) {
            AC_TRACE << "setting representativeFile";
            representativeFile = myFile;
        }
    }
    // copy the sorted slices to an array
    std::map<float, ResizeableRasterPtr>::iterator it = mySortedSlices.begin();
    for (; it != mySortedSlices.end(); ++it) {
        _mySlices.push_back(it->second);
    }
    AC_TRACE << "done, read " << _mySlices.size() << " slices";
    if (_mySlices.empty()) {
        throw CTScanException("No slice files in " + thePackage +"/"+theSubDir + " found \n", PLUS_FILE_LINE);
    }
    AC_TRACE << "representativeFile = " << representativeFile != 0;
        // extract information from the representative slice
        _myDefaultWindow = representativeFile->getDefaultWindow();
    AC_TRACE << "default window " << _myDefaultWindow;
    _myVoxelSize = representativeFile->getVoxelSize();
    AC_TRACE << "Voxel Size " << _myVoxelSize;
    _myEncoding = representativeFile->getEncoding();
    AC_TRACE << "Encoding " << _myEncoding;
    findOccurringValueRange();
    //allocateStencils();
    AC_TRACE << "returning " << _mySlices.size() << " slices";


    return _mySlices.size();
}

int
CTScan::loadSphere(const asl::Vector3i & size) {
    clear();
    for (int z = 0; z < size[2]; ++z) {
        ResizeableRasterPtr myRaster =
            dynamic_cast_Ptr<dom::ResizeableRaster>(createRasterValue(y60::GRAY, size[0], size[1]));

        Unsigned8 * myPixels = myRaster->pixels().begin();
        Point3f myCenter = Point3f(float(size[0])/2.0f, float(size[1])/2.0f, float(size[2])/2.0f);
        for (int y = 0; y < size[1]; ++y) {
            for (int x = 0; x < size[0]; ++x) {
                float myDist = sqrt((myCenter[0]-float(x))/float(size[0]/2) * (myCenter[0]-float(x))/float(size[0]/2)+
                                    (myCenter[1]-float(y))/float(size[1]/2) * (myCenter[1]-float(y))/float(size[1]/2)+
                                    (myCenter[2]-float(z))/float(size[2]/2) * (myCenter[2]-float(z))/float(size[2]/2));
                if (myDist > 1.0f) {
                    myPixels[x+y*size[1]] = 0;
                } else {
                    myPixels[x+y*size[1]] = Unsigned8((1.f - myDist) * 255.0f);
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
CTScan::applyMarchingCubes(const asl::Box3i & theVoxelBox, int theDownSampleRate, bool theCloseAtClippingBoxFlag,
                             bool theCreateNormalsFlag, ScenePtr theScene,
                             SegmentationPolicy & theSegmentizer,
                             unsigned int theNumVertices, unsigned int theNumTriangles)
{
    if (theNumVertices == 0 || theNumTriangles == 0) {
        // estimation not done yet - force it
        countMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theCloseAtClippingBoxFlag,
                theSegmentizer, theNumVertices, theNumTriangles);
    }

    SceneBuilderPtr mySceneBuilder = theScene->getSceneBuilder();
    std::string myMaterialId = setupMaterial( mySceneBuilder, theCreateNormalsFlag);
    ExportShapePolicy<VoxelT, SegmentationPolicy >
                myVertexStore(mySceneBuilder, "polygonal_shape", myMaterialId,
                theNumVertices, theNumTriangles, theCreateNormalsFlag);

    // here we go ...
    MarchingCubes<VoxelT, ExportShapePolicy<VoxelT, SegmentationPolicy >, SegmentationPolicy >
           myMarcher(theDownSampleRate, theCloseAtClippingBoxFlag, myVertexStore, theSegmentizer, this);

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
                           bool theCloseAtClippingBoxFlag, SegmentationPolicy & theSegmentizer,
                           unsigned int & theVertexCount, unsigned int & theTriangleCount)
{
    // here we go ...
    CountPolygonPolicy<VoxelT, SegmentationPolicy > myPolygonCounter;

    MarchingCubes<VoxelT,
                  CountPolygonPolicy<VoxelT, SegmentationPolicy >,
                  SegmentationPolicy >
           myMarcher(theDownSampleRate, theCloseAtClippingBoxFlag, myPolygonCounter, theSegmentizer, this);

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
                       int theDownSampleRate,
                       bool theCloseAtClippingBoxFlag)
{
    unsigned int myVertexCount;
    unsigned int myTriangleCount;
    bool isDone;
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char Voxel;
                GlobalThresholdSegmentationPolicy<Voxel> mySegmentizer((Voxel)theThresholdMin, (Voxel)theThresholdMax);
                isDone = countMarchingCubes<Voxel>(theVoxelBox, theDownSampleRate, theCloseAtClippingBoxFlag,
                        mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAY16:
            {
                typedef unsigned short Voxel;
                GlobalThresholdSegmentationPolicy<Voxel> mySegmentizer((Voxel)theThresholdMin, (Voxel)theThresholdMax);
                isDone = countMarchingCubes<Voxel>(theVoxelBox, theDownSampleRate, theCloseAtClippingBoxFlag,
                        mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAYS16:
            {
                typedef short Voxel;
                GlobalThresholdSegmentationPolicy<Voxel> mySegmentizer((Voxel)theThresholdMin, (Voxel)theThresholdMax);
                isDone = countMarchingCubes<Voxel>(theVoxelBox, theDownSampleRate, theCloseAtClippingBoxFlag,
                        mySegmentizer, myVertexCount, myTriangleCount);
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
                                          dom::NodePtr theThresholdPalette, int theDownSampleRate,
                                          bool theCloseAtClippingBoxFlag)
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
                PerVoxelThresholdSegmentationPolicy<Voxel> mySegmentizer(theThresholdPalette, theVolumeNode,
                        theDownSampleRate);
                isDone = countMarchingCubes<Voxel>(myBoundingBox, theDownSampleRate, theCloseAtClippingBoxFlag,
                        mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAY16:
            {
                typedef unsigned short Voxel;
                PerVoxelThresholdSegmentationPolicy<Voxel> mySegmentizer(theThresholdPalette, theVolumeNode,
                        theDownSampleRate);
                isDone = countMarchingCubes<Voxel>(myBoundingBox, theDownSampleRate, theCloseAtClippingBoxFlag,
                        mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAYS16:
            {
                typedef short Voxel;
                PerVoxelThresholdSegmentationPolicy<Voxel> mySegmentizer(theThresholdPalette, theVolumeNode,
                        theDownSampleRate);
                isDone = countMarchingCubes<Voxel>(myBoundingBox, theDownSampleRate, theCloseAtClippingBoxFlag,
                        mySegmentizer, myVertexCount, myTriangleCount);
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
                   int theDownSampleRate, bool theCloseAtClippingBoxFlag, bool theCreateNormalsFlag,
                   PackageManagerPtr thePackageManager, unsigned int theNumVertices, unsigned int theNumTriangles)
{
    asl::Time myStartTime;

    ScenePtr myScene = y60::Scene::createStubs(thePackageManager);

    bool isDone;
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                GlobalThresholdSegmentationPolicy<VoxelT> mySegmentizer( (VoxelT)theThresholdMin, (VoxelT)theThresholdMax);
                isDone = applyMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theCloseAtClippingBoxFlag, theCreateNormalsFlag,
                        myScene, mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                GlobalThresholdSegmentationPolicy<VoxelT> mySegmentizer( (VoxelT)theThresholdMin, (VoxelT)theThresholdMax);
                isDone = applyMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theCloseAtClippingBoxFlag, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                GlobalThresholdSegmentationPolicy<VoxelT> mySegmentizer( (VoxelT)theThresholdMin, (VoxelT)theThresholdMax);
                isDone = applyMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theCloseAtClippingBoxFlag, theCreateNormalsFlag, myScene,
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
                   int theDownSampleRate, bool theCloseAtClippingBoxFlag, bool theCreateNormalsFlag,
                   PackageManagerPtr thePackageManager,
                   unsigned int theNumVertices, unsigned int theNumTriangles)
{
    asl::Time myStartTime;
    asl::Box3i myBoundingBox = theVoxelBox;
    prepareBox(myBoundingBox);

    ScenePtr myScene = y60::Scene::createStubs(thePackageManager);
    bool isDone;
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                PerVoxelThresholdSegmentationPolicy<VoxelT> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                isDone = applyMarchingCubes<VoxelT>(myBoundingBox, theDownSampleRate, theCloseAtClippingBoxFlag,
                                    theCreateNormalsFlag, myScene,
                                    mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                PerVoxelThresholdSegmentationPolicy<VoxelT> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                isDone = applyMarchingCubes<VoxelT>(myBoundingBox, theDownSampleRate, theCloseAtClippingBoxFlag,
                        theCreateNormalsFlag, myScene, mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                PerVoxelThresholdSegmentationPolicy<VoxelT> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                isDone = applyMarchingCubes<VoxelT>(myBoundingBox, theDownSampleRate, theCloseAtClippingBoxFlag,
                        theCreateNormalsFlag, myScene, mySegmentizer, theNumVertices, theNumTriangles);
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
CTScan::getReconstructionDimensions(CTScan::Orientation theOrientation) const {
    if (_myState != COMPLETE) {
        throw CTScanException("cannot reconstruct image when loading not complete!", PLUS_FILE_LINE);
    }
    Vector3i mySize = getVoxelDimensions();
    Vector3i myResult;

    switch (theOrientation) {
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
            throw CTScanException("Illegal Orientation", PLUS_FILE_LINE);
            break;
    }
    return myResult;
}

void
CTScan::getReconstructionBounds(const Quaterniond & theOrientation, asl::Box3d & theBounds, asl::Vector3i & theSize) const {
    if (_myState != COMPLETE) {
        throw CTScanException("cannot reconstruct image when loading not complete!", PLUS_FILE_LINE);
    }
    Matrix4d myScreenToVoxelProjection(theOrientation);
    Vector3i myVoxelSize = getVoxelDimensions();
    Matrix4d myVoxelToScreenProjection = myScreenToVoxelProjection;
    myVoxelToScreenProjection.invert();
    theBounds = Box3d(Vector3d(0,0,0), Vector3d(double(myVoxelSize[0]), double(myVoxelSize[1]),
        double(myVoxelSize[2]))) * myVoxelToScreenProjection;
    Vector3d myFloatSize = theBounds.getSize();
    for (int i = 0; i < 3; ++i) {
        if (!almostEqual(myFloatSize[i], floor(myFloatSize[i]))) {
            myFloatSize[i] = ceil(myFloatSize[i]);
        }
        theSize[i] = int(myFloatSize[i]);
    }
}

Vector3i
CTScan::getReconstructionDimensions(const Quaternionf & theOrientation) const {
    Box3d myBounds;
    Vector3i mySize;
    getReconstructionBounds(Quaterniond(double(theOrientation[0]),
                                        double(theOrientation[1]),
                                        double(theOrientation[2]),
                                        double(theOrientation[3])),
                            myBounds, mySize);
    return mySize;
}
float
CTScan::getSliceThickness(const asl::Quaternionf & theOrientation) const {
    Matrix4f myMatrix(theOrientation);
    myMatrix.scale(getVoxelAspect());
    return magnitude(product(Vector4f(0,0,1,0), myMatrix));
 }

template <class VoxelT>
VoxelT
CTScan::fastValueAt(const asl::Vector3d & thePosition) {
    Vector3i myDiscretePos = Vector3i(int(round(thePosition[0])),
        int(round(thePosition[1])),
        int(round(thePosition[2])));

    if (isInside(myDiscretePos[0], myDiscretePos[1], myDiscretePos[2])) {
        dom::ResizeableRasterPtr & mySlice = _mySlices[myDiscretePos[2]];
        int myLineStride = getBytesRequired(mySlice->width(), _myEncoding);
        const VoxelT * mySource = reinterpret_cast<const VoxelT*>(mySlice->pixels().begin()+myLineStride*myDiscretePos[1]);
        return mySource[myDiscretePos[0]];
    } else {
        return NumericTraits<VoxelT>::min();
    }
}

template <class VoxelT>
VoxelT
CTScan::interpolatedValueAt(const asl::Vector3d & thePosition) {
    Vector3i myFloorPos(static_cast<int>(floor(thePosition[0])),
        static_cast<int>(floor(thePosition[1])),
        static_cast<int>(floor(thePosition[2])));
    asl::Vector3i myCeilPos = myFloorPos + Vector3i(1,1,1);
    float myValueA, myValueB;
    VoxelT myFloorValue, myCeilValue;
    float myFloorResult, myCeilResult;
    asl::Vector3i mySize = getVoxelDimensions();
    const VoxelT * mySource;

    int myLineStride = getBytesRequired(mySize[0], _myEncoding);
    unsigned myBpp = getBytesRequired(1, _myEncoding);
    if (myFloorPos[2] >= 0 && myFloorPos[2] < _mySlices.size()) {
        myFloorValue = NumericTraits<VoxelT>::min();
        myCeilValue = NumericTraits<VoxelT>::min();
        mySource = reinterpret_cast<const VoxelT*>(_mySlices[int(myFloorPos[2])]->pixels().begin() + myLineStride * myFloorPos[1]);
        if (isInside(myFloorPos[0], myFloorPos[1], myFloorPos[2])) {
            myFloorValue = mySource[myFloorPos[0]];
        }
        if (isInside(myCeilPos[0], myFloorPos[1], myFloorPos[2])) {
            myCeilValue = mySource[myCeilPos[0]];
        }
        myValueA = linearInterpolate(myFloorValue, myCeilValue,
            myFloorPos[0], myCeilPos[0], float(thePosition[0]));

        myFloorValue = NumericTraits<VoxelT>::min();
        myCeilValue = NumericTraits<VoxelT>::min();
        mySource = reinterpret_cast<const VoxelT*>(_mySlices[int(myFloorPos[2])]->pixels().begin() + myLineStride * myCeilPos[1]);
        if (isInside(myFloorPos[0], myCeilPos[1], myFloorPos[2])) {
            myFloorValue = mySource[myFloorPos[0]];
        }
        if (isInside(myCeilPos[0], myCeilPos[1], myFloorPos[2])) {
            myCeilValue = mySource[myCeilPos[0]];
        }
        myValueB = linearInterpolate(myFloorValue, myCeilValue,
            myFloorPos[0], myCeilPos[0], float(thePosition[0]));

        myFloorResult = linearInterpolate(myValueA, myValueB, myFloorPos[1], myCeilPos[1], float(thePosition[1]));
    } else {
        myFloorResult = NumericTraits<VoxelT>::min();
    }
    if (myCeilPos[2] >= 0 && myCeilPos[2] < _mySlices.size()) {
        myFloorValue = NumericTraits<VoxelT>::min();
        myCeilValue = NumericTraits<VoxelT>::min();
        mySource = reinterpret_cast<const VoxelT*>(_mySlices[int(myCeilPos[2])]->pixels().begin() + myLineStride*myFloorPos[1]);
        if (isInside(myFloorPos[0], myFloorPos[1], myCeilPos[2])) {
            myFloorValue = mySource[myFloorPos[0]];
        }
        if (isInside(myCeilPos[0], myFloorPos[1], myCeilPos[2])) {
            myCeilValue = mySource[myCeilPos[0]];
        }
        myValueA = linearInterpolate(myFloorValue, myCeilValue,
            int(myFloorPos[0]), int(myCeilPos[0]), float(thePosition[0]));

        myFloorValue = NumericTraits<VoxelT>::min();
        myCeilValue = NumericTraits<VoxelT>::min();
        mySource = reinterpret_cast<const VoxelT*>(_mySlices[int(myCeilPos[2])]->pixels().begin() + myLineStride*myCeilPos[1]);
        if (isInside(myFloorPos[0], myCeilPos[1], myCeilPos[2])) {
            myFloorValue = mySource[myFloorPos[0]];
        }
        if (isInside(myCeilPos[0], myCeilPos[1], myCeilPos[2])) {
            myCeilValue = mySource[myCeilPos[0]];
        }
        myValueB = linearInterpolate(myFloorValue, myCeilValue,
            int(myFloorPos[0]), int(myCeilPos[0]), float(thePosition[0]));
        myCeilResult = linearInterpolate(myValueA, myValueB, myFloorPos[1], myCeilPos[1], float(thePosition[1]));
    } else {
        myCeilResult = NumericTraits<VoxelT>::min();
    }
    return VoxelT(linearInterpolate(myFloorResult, myCeilResult,
        myFloorPos[2], myCeilPos[2], float(thePosition[2])));
}

void
CTScan::reconstructToImage(const asl::Quaternionf & theOrientation, const asl::Vector3f & theSlicePosition,
                           dom::NodePtr & theImageNode, bool theTrilliniarInterpolate) {
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                reconstructToImageImpl<VoxelT>(theOrientation, theSlicePosition, theImageNode, theTrilliniarInterpolate);
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                reconstructToImageImpl<VoxelT>(theOrientation, theSlicePosition, theImageNode, theTrilliniarInterpolate);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                reconstructToImageImpl<VoxelT>(theOrientation, theSlicePosition, theImageNode, theTrilliniarInterpolate);
            }
            break;
        default:
            throw CTScanException("Unhandled voxel type", PLUS_FILE_LINE);
    }
}

void
CTScan::reconstructToImage(CTScan::Orientation theOrientation, int theSliceIndex,
        dom::NodePtr & theImageNode) {
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                reconstructToImageImpl<VoxelT>(theOrientation, theSliceIndex, theImageNode);
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                reconstructToImageImpl<VoxelT>(theOrientation, theSliceIndex, theImageNode);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                reconstructToImageImpl<VoxelT>(theOrientation, theSliceIndex, theImageNode);
            }
            break;
        default:
            throw CTScanException("Unhandled voxel type", PLUS_FILE_LINE);
    }
}


template <class VoxelT>
void
CTScan::reconstructToImageImpl(CTScan::Orientation theOrientation, int theSliceIndex,
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

    switch (theOrientation) {
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
                    unsigned int myTargetLine = myLineStride * y;
                    memcpy(myTarget->begin()+myTargetLine,
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
                    int yTarget = myHeight-ySrc-1; // Flip Y
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
                int mySliceCount = getSliceCount();
                unsigned mySourceLineOffset = myBpp * theSliceIndex;
                int myTargetLineStride = getBytesRequired(myPoTWidth, _myEncoding);
                int mySourceLineStride = getBytesRequired(mySize[0], _myEncoding);
                for (int x=0; x < myWidth; ++x) {
                    const unsigned char * mySource = _mySlices[mySliceCount-x-1]->pixels().begin();
                    for (int y=0; y < myHeight; ++y) {
                        memcpy(myTarget->begin()+myTargetLineStride*y+myBpp*x,
                            mySource+mySourceLineStride*y+mySourceLineOffset, myBpp);
                    }
                }
                myPixelData = myTarget;
                break;
            }
        default:
            throw CTScanException("Illegal Orientation", PLUS_FILE_LINE);
    }
    // set the image data
    y60::ImagePtr myFacade = theImageNode->dom::Node::getFacade<y60::Image>();
    myFacade->createRaster(myPoTWidth, myPoTHeight, 1, _myEncoding, *myPixelData);
    // set the matrix to make up for the padded image
    asl::Matrix4f myScale;
    myScale.makeIdentity();
    myScale.scale(Vector3f(float(myWidth)/myPoTWidth, float(myHeight)/myPoTHeight, 1.0f));
    myFacade->y60::Image::set<y60::ImageMatrixTag>(myScale);
}

template <class VoxelT>
void
CTScan::reconstructToImageImpl(const Quaternionf & theOrientation, const asl::Vector3f & theSlicePosition,
        dom::NodePtr & theImageNode, bool theTrilliniarInterpolate)
{
    try {
        if (_myState != COMPLETE) {
            throw CTScanException("cannot reconstruct image when loading not complete!", PLUS_FILE_LINE);
        }

        int myWidth;
        int myHeight;
        int myPoTWidth;
        int myPoTHeight;
        Ptr<ReadableBlock> myPixelData;

        Box3d myBounds;
        Vector3i myBoundsSize;

        getReconstructionBounds(Quaterniond(double(theOrientation[0]),
                                        double(theOrientation[1]),
                                        double(theOrientation[2]),
                                        double(theOrientation[3])),
                            myBounds, myBoundsSize);

        asl::Quaterniond myOrientation((double)theOrientation[0],
                                       (double)theOrientation[1],
                                       (double)theOrientation[2],
                                       (double)theOrientation[3]);

        myWidth = myBoundsSize[0];
        myHeight = myBoundsSize[1];

        myPoTWidth = nextPowerOfTwo(myWidth);
        myPoTHeight = nextPowerOfTwo(myHeight);
        Matrix4d myScreenToVoxelProjection(myOrientation);

        // Fill with white
        Ptr<Block> myTarget(new Block(getBytesRequired(myPoTWidth* myPoTHeight, _myEncoding)));
        int myTargetLineStride = getBytesRequired(myPoTWidth, _myEncoding);
        int myTargetPixelStride = getBytesRequired(1, _myEncoding);

        double myCenterSlicePosition = myBounds.getCenter()[2];
        Vector3d mySlicePosition((double)theSlicePosition[0], (double)theSlicePosition[1], (double)theSlicePosition[2]);

        Vector3d myStartPos = product(Point3d(myBounds[Box3d::MIN][0], myBounds[Box3d::MIN][1], myCenterSlicePosition)+mySlicePosition,
                                      myScreenToVoxelProjection);
        Vector3d mySourceDeltaU = (product(Point3d(myBounds[Box3d::MAX][0], myBounds[Box3d::MIN][1], myCenterSlicePosition)+mySlicePosition,
            myScreenToVoxelProjection) - myStartPos) / myWidth;

        Vector3d mySourceDeltaV = (product(Point3d(myBounds[Box3d::MIN][0], myBounds[Box3d::MAX][1], myCenterSlicePosition)+mySlicePosition,
            myScreenToVoxelProjection) - myStartPos) / myHeight;

        AC_TRACE << "Width: " << myWidth << ", Height: " << myHeight << ", myStartPos: " << myStartPos
            << " DeltaU: " << mySourceDeltaU << " DeltaV " << mySourceDeltaV;
        typedef Line<double> Lined;
        Vector3i myVoxelSize = getVoxelDimensions();
        Box3d myVoxelBox(Point3d(0.0f, 0.0f, 0.0f),
            Point3d(double(myVoxelSize[0]), double(myVoxelSize[1]), double(myVoxelSize[2])));
        for (int v = 0; v < myHeight; ++v) {
            Vector3d myLinePos = myStartPos + (double(v) * mySourceDeltaV);
            Lined myScanLine(myLinePos, mySourceDeltaU);
            double myMinValue, myMaxValue;
            if (intersection(myVoxelBox, myScanLine, myMinValue, myMaxValue)) {
                if (myMinValue > myMaxValue) {
                    double myHelper = myMinValue;
                    myMinValue = myMaxValue;
                    myMaxValue = myHelper;
                    AC_WARNING << "Wrongly oriented intersection.";
                }
                int myStart = int(ceil(myMinValue));
                int myEnd = int(floor(myMaxValue));
                VoxelT * myAddress = reinterpret_cast<VoxelT*>(myTarget->begin()+(myTargetLineStride*v)+(myStart * myTargetPixelStride));
                for (int u = myStart; u < myEnd; ++u) {
                    Vector3d mySourcePos = myLinePos + (double(u) * mySourceDeltaU);
                    if (theTrilliniarInterpolate) {
                        *myAddress = interpolatedValueAt<VoxelT>(mySourcePos);
                    } else {
                        *myAddress = fastValueAt<VoxelT>(mySourcePos);
                    }
                    myAddress ++;
                }
            }
        }
        myPixelData = myTarget;
        y60::ImagePtr myFacade = theImageNode->dom::Node::getFacade<y60::Image>();
        myFacade->createRaster(myPoTWidth, myPoTHeight, 1, _myEncoding, *myPixelData);
        // set the matrix to make up for the padded image
        asl::Matrix4f myScale;
        myScale.makeIdentity();
        myScale.scale(Vector3f(float(myWidth)/myPoTWidth, float(myHeight)/myPoTHeight, 1.0f));
        myFacade->y60::Image::set<y60::ImageMatrixTag>(myScale);
    } catch (...) {
        AC_ERROR << "Exception in ReconstructToImage.";
    }
}


std::string
CTScan::setupMaterial(SceneBuilderPtr theSceneBuilder, bool theCreateNormalsFlag) {
    // instead of using a modelling function we roll our own material here,
    // beacuse we don't have a Scene object ...
    std::string myMaterialName = "mSTL"; // XXX [DS] this name should not be changed
                                         // XXX [TS] There is code in Polygonization that
                                         //          depends on this name
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

    myMaterialBuilder.addFeature("vertexparams", VectorOfRankedFeature(1, RankedFeature(10,"color")));

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
        the3DTexture.append(static_cast<void*>(asl::begin_ptr(myEmptySlice)), theBytesPerSlice);
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
    /*
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
    */
    for (int z = 0; z < myTextureSize[2]; ++z) {
        int mySlice = int(floor(float(z*mySliceCount)/myTextureSize[2]));
        AC_TRACE << " adding slice " << mySlice;
        CTScan::appendTo3DTexture(mySlice, my3DTexture, myTextureSize[0], myTextureSize[1]);
    }
    /*
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
    */
    y60::ImagePtr myImage = theImageNode->getFacade<y60::Image>();
    myImage->createRaster(myTextureSize[0], myTextureSize[1], myTextureSize[2], _myEncoding, my3DTexture);
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
        std::vector<Signed32> & theProfile, std::vector<asl::Point3i> & thePointsSampled)
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
CTScan::computeProfile(const asl::Point3i & thePosition, std::vector<Signed32> & theProfile) {
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
        std::vector<Signed32> & theProfile, std::vector<Point3i> & thePointsSampled)
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
    /*
    PLPNGEncoder myPNGEncoder;
    string myImageFile = string("__3DTex_")+as_string(the3dTexture.size())+".png";
    myPNGEncoder.MakeFileFromBmp(Path(myImageFile, UTF8).toLocale().c_str(), &myScaledBitmap);
*/


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
    myImage->getFacade<Image>()->createRaster(theWidth, theHeight, 1, y60::RGBA, myBlock);
    return myImage;
}

void
CTScan::resizeVoxelVolume(dom::NodePtr theVoxelVolume, const asl::Box3f & theDirtyBox) {
    const Box3f & myOldBox = theVoxelVolume->getAttributeValue<Box3f>("boundingbox");

    Box3f myNewBox = myOldBox;
    Box3f myDirtyBox(theDirtyBox);
    myNewBox.extendBy( myDirtyBox );

    if (myNewBox == myOldBox) {
        return;
    }

    const Vector3f & mySize = myNewBox.getSize();

    int myTargetWidth  = int( round(mySize[0]) );
    int myTargetHeight = int( round(mySize[1]) );
    int myNewRasterCount  = int( round(mySize[2]) );

    int myNewBoxBegin = int( round( myNewBox[Box3f::MIN][2] ));
    int myNewBoxEnd   = int( round( myNewBox[Box3f::MAX][2] ));

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
            myOldRaster = myRasters->childNode(0);
            ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>(
                    myNewRaster->childNode(0)->nodeValueWrapperPtr());
            ValuePtr mySourceRaster = myOldRaster->childNode(0)->nodeValueWrapperPtr();
            myTargetRaster->pasteRaster(myXOrigin, myYOrigin, * mySourceRaster);
            myRasters->removeChild(myOldRaster);
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
            asl::Vector4f myBrushPixel = myBrushRaster->getPixel(x, y);
            if (int(myBrushPixel[0]) == 255) {
                myCanvasRaster->setPixel(myXOrigin + x, myYOrigin + y, theColor);
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

    unsigned myCanvasWidth = myCanvas->width();
    unsigned myCanvasLineStride = getBytesRequired(myCanvasWidth, y60::RGBA);
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
                        unsigned myIndex = (y - int(myMeasurementBox[Box3f::MIN][1])) * myTargetLineStride +
                                            x - int(myMeasurementBox[Box3f::MIN][0]);
                        unsigned char * myTargetPixel = & myTargetPixels[myIndex];
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
                    unsigned myCanvasLineOffset = (myCanvasWidth-z-1) * myBytesPerPixel;

                    for (unsigned y = myYStart; y < myYEnd; ++y) {

                        const unsigned char * myRGBPixel = & mySourcePixels[y * myCanvasLineStride + myCanvasLineOffset];
                        myPixel = Vector3i(myRGBPixel[0], myRGBPixel[1], myRGBPixel[2]);

                        // XXX: argh! map.find() in inner loop... any ideas? [DS]
                        ColorMap::iterator myIt = myColorMap.find(myPixel);
                        if (myIt == myColorMap.end()) {
                            throw asl::Exception(string("Unknown color in canvas image: ") + as_string(myPixel),
                                                 PLUS_FILE_LINE);
                        }
                        unsigned myIndex = (y - int(myMeasurementBox[Box3f::MIN][1])) * myTargetLineStride +
                                            myAffectedX;
                        unsigned char * myTargetPixel = & myTargetPixels[myIndex];
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
CTScan::copyVoxelVolumeToCanvas(dom::NodePtr theMeasurement,
                                unsigned theGlobalThresholdIndex, dom::NodePtr theCanvas,
                                dom::NodePtr theReconstructedImage, unsigned theSliceIndex,
                                Orientation theOrientation, dom::NodePtr thePaletteNode)
{
    y60::PixelEncoding myEncoding;
    myEncoding = theReconstructedImage->getFacade<Image>()->getRasterEncoding();
    switch (myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                copyVoxelVolumeToCanvasImpl<VoxelT>(theMeasurement, theGlobalThresholdIndex, theCanvas, theReconstructedImage,
                                                    theSliceIndex, theOrientation, thePaletteNode);
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                copyVoxelVolumeToCanvasImpl<VoxelT>(theMeasurement, theGlobalThresholdIndex, theCanvas, theReconstructedImage,
                                                    theSliceIndex, theOrientation, thePaletteNode);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                copyVoxelVolumeToCanvasImpl<VoxelT>(theMeasurement, theGlobalThresholdIndex, theCanvas, theReconstructedImage,
                                                    theSliceIndex, theOrientation, thePaletteNode);
            }
            break;
        default:
            throw CTScanException("Unhandled voxel type", PLUS_FILE_LINE);
    }
}

template <class VoxelT>
unsigned char
CTScan::getSegmentationAlpha(unsigned theIndex, VoxelT theVoxel, const PaletteTable<VoxelT> & thePalette,
                             unsigned char theAlpha)
{
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
CTScan::copyVoxelVolumeToCanvasImpl(dom::NodePtr theMeasurement,
                                    unsigned theGlobalThresholdIndex, dom::NodePtr theCanvas,
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

    y60::PixelEncoding myEncoding = theReconstructedImage->dom::Node::getFacade<y60::Image>()->getRasterEncoding();
    unsigned myRIHeight = myCanvasHeight; // use non power of two size from dom
    unsigned myRIStride = getBytesRequired(myReconstructedRaster->width(), myEncoding);
    unsigned myRIBPP    = getBytesRequired(1, myEncoding);
    const unsigned char * myRIPixels = myReconstructedRaster->pixels().begin();


    unsigned myTargetLineStride = getBytesRequired(myCanvasWidth, y60::RGBA);
    unsigned myBytesPerPixel = getBytesRequired(1, y60::RGBA);
    unsigned char * myTargetPixels = myTargetRaster->pixels().begin();
    unsigned myPixelIndex;

    unsigned myXStart = unsigned(myMin[0]);
    unsigned myXEnd = unsigned(myMax[0]);
    unsigned myYStart = unsigned(myMin[1]);
    unsigned myYEnd = unsigned(myMax[1]);
    unsigned myZStart = unsigned( myMin[2]);
    unsigned myZEnd = unsigned( myMax[2]);
    switch ( theOrientation) {
        case CTScan::IDENTITY: // front
            {
                unsigned char * mySourcePixels = 0;
                unsigned mySourceLineStride  = 0;
                if (theSliceIndex >= myZStart && theSliceIndex < myZEnd) {
                    dom::NodePtr myRasterNode = theMeasurement->childNode(0)->childNode(int( theSliceIndex - myMin[2]));
                    ResizeableRasterPtr mySourceRaster =
                        dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());
                    mySourceLineStride  = getBytesRequired(mySourceRaster->width(), y60::GRAY);
                    mySourcePixels = mySourceRaster->pixels().begin();
                }

                Vector3i myColor;
                VoxelT myVoxel;
                for (unsigned y = 0; y < myCanvasHeight; ++y) {
                    for (unsigned x = 0; x < myCanvasWidth; ++x) {
                        myIndex = 0;
                        if ( y >= myYStart && y < myYEnd &&
                             x >= myXStart && x < myXEnd &&
                             mySourcePixels)
                        {
                            myIndex = mySourcePixels[(y - myYStart) * mySourceLineStride + (x - myXStart)];
                        }
                        if (myIndex == 0) {
                            myIndex = theGlobalThresholdIndex;
                        }
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
            {
                unsigned mySourceY = unsigned( theSliceIndex - myMin[1]);
                Vector3i myColor;
                VoxelT myVoxel;
                for (unsigned z = 0; z < myCanvasHeight; ++z) {
                    unsigned char * mySourcePixels = 0;
                    unsigned mySourceLineStride = 0;
                    if (z >= myZStart && z < myZEnd) { // inside the bounding box?
                        unsigned myCurrentSliceIndex = z - myZStart;
                        dom::NodePtr myRasterNode = theMeasurement->childNode(0)->childNode(myCurrentSliceIndex);
                        ResizeableRasterPtr mySourceRaster =
                            dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());

                        mySourceLineStride  = getBytesRequired(mySourceRaster->width(), y60::GRAY);
                        mySourcePixels = mySourceRaster->pixels().begin();
                    }

                    for (unsigned x = 0; x < myCanvasWidth; ++x) {
                        myIndex = 0;
                        if ( mySourcePixels &&
                             theSliceIndex >= myYStart && theSliceIndex < myYEnd &&
                             x >= myXStart && x < myXEnd) // inside the bounding box?
                        {
                            myIndex = mySourcePixels[mySourceY * mySourceLineStride + (x - myXStart)];
                        }
                        if (myIndex == 0) {
                            myIndex = theGlobalThresholdIndex;
                        }
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
            {
                unsigned mySourceX = unsigned( theSliceIndex - myMin[0]);
                Vector3i myColor;
                VoxelT myVoxel;
                for (unsigned z = 0; z < myCanvasWidth; ++z) {
                    unsigned char * mySourcePixels = 0;
                    unsigned mySourceLineStride = 0;
                    if (z >= myZStart && z < myZEnd) {
                        unsigned myCurrentSliceIndex = z - myZStart;
                        dom::NodePtr myRasterNode = theMeasurement->childNode(0)->childNode(myCurrentSliceIndex);
                        ResizeableRasterPtr mySourceRaster =
                            dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());

                        mySourceLineStride  = getBytesRequired(mySourceRaster->width(), y60::GRAY);
                        mySourcePixels = mySourceRaster->pixels().begin();
                    }
                    unsigned myTargetLineOffset = (myCanvasWidth-z-1) * myBytesPerPixel;
                    unsigned myVoxelLineOffset = (myCanvasWidth-z-1) * myRIBPP;

                    for (unsigned y = 0; y < myCanvasHeight; ++y) {
                        myIndex = 0;
                        if ( theSliceIndex >= myXStart && theSliceIndex < myXEnd &&
                             y >= myYStart && y < myYEnd &&
                             mySourcePixels) // inside the bounding box?
                        {
                            myIndex = mySourcePixels[(y - myYStart) * mySourceLineStride + mySourceX];
                        }
                        if (myIndex == 0) {
                            myIndex = theGlobalThresholdIndex;
                        }
                        myColor = myPalette[myIndex].color;
                        myVoxel = *reinterpret_cast<const VoxelT*>(&(myRIPixels[y * myRIStride + myVoxelLineOffset]));
                        myPixelIndex = y * myTargetLineStride + myTargetLineOffset;
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

void
CTScan::renderTransferFunction(dom::NodePtr theTransferFunction, dom::NodePtr theTargetImage) {
    if (theTransferFunction && theTargetImage) {
        ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>(
                theTargetImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());

        unsigned myWidth = myTargetRaster->width();
        unsigned myHeight = myTargetRaster->height();
        unsigned myPixelStride  = getBytesRequired(1, y60::RGBA);
        unsigned myLineStride  = getBytesRequired(myWidth, y60::RGBA);
        unsigned myColorCount = theTransferFunction->childNodesLength();
        const Vector2f & myValueRange = theTransferFunction->getAttributeValue<Vector2f>("value_range");
        Ptr<Block> myTarget(new Block(getBytesRequired(myWidth * myHeight, y60::RGBA)));
        unsigned char * myPixels = myTarget->begin();
        for (unsigned i = 0; i < myWidth; ++i) {
            Vector4f myColor(0.0, 0.0, 0.0, 0.0);
            float myAlphaSum(0.0);
            for (unsigned j = 0; j < myColorCount; ++j) {
                dom::NodePtr myColorNode = theTransferFunction->childNode(j);
                float myCenter = (float(myColorNode->getAttributeValue<int>("value")) - myValueRange[0]) /
                    (myValueRange[1] - myValueRange[0]) * myWidth;
                float myBellWidth = float(myColorNode->getAttributeValue<int>("width") / (myValueRange[1] - myValueRange[0])) *
                        myWidth;
                float myAlpha = powf(asl::EULER, 1 - sqr( 6 * ( i - myCenter ) / myBellWidth )) / asl::EULER;
                //myPixels[3] += (unsigned char)( myAlpha );
                myColor += myColorNode->getAttributeValue<Vector4f>("color") * myAlpha;
                myAlphaSum += myAlpha;
            }

            myColor /= myAlphaSum;
            myPixels[0] = (unsigned char)(myColor[0] * 255);
            myPixels[1] = (unsigned char)(myColor[1] * 255);
            myPixels[2] = (unsigned char)(myColor[2] * 255);
            myPixels[3] = (unsigned char)(myColor[3] * myAlphaSum * 255);
            myPixels += myPixelStride;
        }

        for (unsigned i = 1; i < myTargetRaster->height(); ++i) {
            //AC_WARNING << "line " << i;
            memcpy(myTarget->begin() + i * myLineStride, myTarget->begin(), myLineStride);
        }

        y60::ImagePtr myFacade = theTargetImage->dom::Node::getFacade<y60::Image>();
        Ptr<ReadableBlock> myPixelData = myTarget;
        myFacade->createRaster(myWidth, myHeight, 1, y60::RGBA, *myPixelData);
    }
}

} // end of namespace

