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
    
    return _mySlices.size();
}

/*
void
CTScan::appendStencil(dom::NodePtr theImage) {
    dom::ResizeableRasterPtr myRaster = theImage->getFacade<Image>()->getRasterPtr();
    // XXX Check size
    if (myRaster) {
        _myStencils.push_back(myRaster);
    } else {
        throw CTScanException(std::string("No raster for image ") + theImage->getFacade<Image>()->get<NameTag>(),
            PLUS_FILE_LINE);
    }            
}

void
CTScan::allocateStencils() {
    _myStencils.clear();
    if (!_mySlices.empty()) {
        int myHeight = _mySlices[0]->width();
        int myWidth = _mySlices[0]->height();
        asl::Block myBlock(myWidth * myHeight, 1);
        _myStencils.reserve(_mySlices.size());
        for (int i = 0; i < _mySlices.size(); ++i) {
            dom::ValuePtr myValuePointer = createRasterValue(GRAY, myWidth, myHeight, myBlock);
            dom::ResizeableRasterPtr myRaster = dynamic_cast_Ptr<ResizeableRaster>(myValuePointer);
            if (myRaster) {
                _myStencils.push_back(myRaster);
            } else {
                throw CTScanException(std::string("Couldn't cast raster for slice ") + as_string(i) , PLUS_FILE_LINE);
            }            
        }
    }
}
*/
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
CTScan::getVoxelDimensions() {
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
void
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
    myMarcher.march();

    setupScene(theScene, myVertexStore.getShapeNode()->getAttributeString("id"));
    asl::Box3f myBox;
    for (int i = 0; i < 6; ++i) {
        myBox[i/3][i%3] = float(theVoxelBox[i/3][i%3]) * _myVoxelSize[i%3];        
    }
    resetCamera(theScene, myBox);
}

template <class VoxelT, class SegmentationPolicy>
void
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
    myMarcher.march();
    theVertexCount = myPolygonCounter.getVertexCount();
    theTriangleCount = myPolygonCounter.getTriangleCount();
}
/**
 * @return a Vector of which x is VertexCount and y is TriangleCount
 */
asl::Vector2i
CTScan::countTrianglesGlobal(const asl::Box3i & theVoxelBox, 
                       double theThresholdMin, 
                       double theThresholdMax, 
                       int theDownSampleRate)
{
    unsigned int myVertexCount;
    unsigned int myTriangleCount;

    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char Voxel;
                GlobalThresholdSegmentationPolicy<Voxel> mySegmentizer((Voxel)theThresholdMin, (Voxel)theThresholdMax);
                countMarchingCubes<Voxel>(theVoxelBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAY16:
            {
                typedef unsigned short Voxel;
                GlobalThresholdSegmentationPolicy<Voxel> mySegmentizer((Voxel)theThresholdMin, (Voxel)theThresholdMax);
                countMarchingCubes<Voxel>(theVoxelBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAYS16:
            {
                typedef short Voxel;
                GlobalThresholdSegmentationPolicy<Voxel> mySegmentizer((Voxel)theThresholdMin, (Voxel)theThresholdMax);
                countMarchingCubes<Voxel>(theVoxelBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        default:
            throw CTScanException("Unhandled voxel type in CTScan::countTriangles()", PLUS_FILE_LINE);
    }
    return asl::Vector2i(myVertexCount, myTriangleCount);
}

asl::Vector2i
CTScan::countTrianglesInVolumeMeasurement(dom::NodePtr theVolumeNode,
        dom::NodePtr theThresholdPalette, int theDownSampleRate)
{
    unsigned int myVertexCount;
    unsigned int myTriangleCount;

    Box3f myFloatBox = theVolumeNode->getAttributeValue<Box3f>("boundingbox");
    Box3i myBoundingBox;
    myBoundingBox[asl::Box3i::MIN][0] = int( myFloatBox[asl::Box3f::MIN][0] );
    myBoundingBox[asl::Box3i::MIN][1] = int( myFloatBox[asl::Box3f::MIN][1] );
    myBoundingBox[asl::Box3i::MIN][2] = int( myFloatBox[asl::Box3f::MIN][2] );
    myBoundingBox[asl::Box3i::MAX][0] = int( myFloatBox[asl::Box3f::MAX][0] );
    myBoundingBox[asl::Box3i::MAX][1] = int( myFloatBox[asl::Box3f::MAX][1] );
    myBoundingBox[asl::Box3i::MAX][2] = int( myFloatBox[asl::Box3f::MAX][2] );


    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char Voxel;
                PerVoxelThresholdSegmentationPolicy<Voxel> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                countMarchingCubes<Voxel>(myBoundingBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAY16:
            {
                typedef unsigned short Voxel;
                PerVoxelThresholdSegmentationPolicy<Voxel> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                countMarchingCubes<Voxel>(myBoundingBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        case y60::GRAYS16:
            {
                typedef short Voxel;
                PerVoxelThresholdSegmentationPolicy<Voxel> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                countMarchingCubes<Voxel>(myBoundingBox, theDownSampleRate, mySegmentizer, myVertexCount, myTriangleCount);
                break;
            }
        default:
            throw CTScanException("Unhandled voxel type in CTScan::countTriangles()", PLUS_FILE_LINE);
    }
    return asl::Vector2i(myVertexCount, myTriangleCount);
}
ScenePtr
CTScan::polygonizeGlobal(const asl::Box3i & theVoxelBox, double theThresholdMin, double theThresholdMax, 
                   int theDownSampleRate, bool theCreateNormalsFlag, PackageManagerPtr thePackageManager, 
                   unsigned int theNumVertices, unsigned int theNumTriangles)
{
    asl::Time myStartTime;

    ScenePtr myScene(new Scene);
    myScene->createStubs(thePackageManager);    
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                GlobalThresholdSegmentationPolicy<VoxelT> mySegmentizer( (VoxelT)theThresholdMin, (VoxelT)theThresholdMax);
                applyMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                GlobalThresholdSegmentationPolicy<VoxelT> mySegmentizer( (VoxelT)theThresholdMin, (VoxelT)theThresholdMax);
                applyMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                GlobalThresholdSegmentationPolicy<VoxelT> mySegmentizer( (VoxelT)theThresholdMin, (VoxelT)theThresholdMax);
                applyMarchingCubes<VoxelT>(theVoxelBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        default:
            throw CTScanException("Unhandled voxel type in CTScan::polygonize()", PLUS_FILE_LINE);
    }
    asl::Time myEndTime;
    AC_INFO << "===> Marching Cubes took " << myEndTime - myStartTime << " seconds.";
    return myScene;
}

ScenePtr
CTScan::polygonizeVolumeMeasurement(dom::NodePtr theVolumeNode, dom::NodePtr theThresholdPalette, 
                   int theDownSampleRate, bool theCreateNormalsFlag, PackageManagerPtr thePackageManager, 
                   unsigned int theNumVertices, unsigned int theNumTriangles)
{
    asl::Time myStartTime;

    Box3f myFloatBox = theVolumeNode->getAttributeValue<Box3f>("boundingbox");
    Box3i myBoundingBox;
    myBoundingBox[asl::Box3i::MIN][0] = int( myFloatBox[asl::Box3f::MIN][0] );
    myBoundingBox[asl::Box3i::MIN][1] = int( myFloatBox[asl::Box3f::MIN][1] );
    myBoundingBox[asl::Box3i::MIN][2] = int( myFloatBox[asl::Box3f::MIN][2] );
    myBoundingBox[asl::Box3i::MAX][0] = int( myFloatBox[asl::Box3f::MAX][0] );
    myBoundingBox[asl::Box3i::MAX][1] = int( myFloatBox[asl::Box3f::MAX][1] );
    myBoundingBox[asl::Box3i::MAX][2] = int( myFloatBox[asl::Box3f::MAX][2] );


    ScenePtr myScene(new Scene);
    myScene->createStubs(thePackageManager);    
    switch (_myEncoding) {
        case y60::GRAY:
            {
                typedef unsigned char VoxelT;
                PerVoxelThresholdSegmentationPolicy<VoxelT> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                applyMarchingCubes<VoxelT>(myBoundingBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAY16:
            {
                typedef unsigned short VoxelT;
                PerVoxelThresholdSegmentationPolicy<VoxelT> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                applyMarchingCubes<VoxelT>(myBoundingBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        case y60::GRAYS16:
            {
                typedef short VoxelT;
                PerVoxelThresholdSegmentationPolicy<VoxelT> mySegmentizer(theThresholdPalette, theVolumeNode, theDownSampleRate);
                applyMarchingCubes<VoxelT>(myBoundingBox, theDownSampleRate, theCreateNormalsFlag, myScene,
                            mySegmentizer, theNumVertices, theNumTriangles);
            }
            break;
        default:
            throw CTScanException("Unhandled voxel type in CTScan::polygonize()", PLUS_FILE_LINE);
    }
    asl::Time myEndTime;
    AC_INFO << "===> Marching Cubes took " << myEndTime - myStartTime << " seconds.";
    return myScene;
}

void 
CTScan::reconstructToImage(Orientation theOrientation, int theSliceIndex, 
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
    }
    // set the image data
    theImageNode->getFacade<Image>()->set(myPoTWidth, myPoTHeight, 1, _myEncoding, *myPixelData);
    // set the matrix to make up for the padded image
    asl::Matrix4f myScale;
    myScale.makeIdentity();
    myScale.scale(Vector3f(float(myWidth)/myPoTWidth, float(myHeight)/myPoTHeight, 1.0f)); 
    theImageNode->getFacade<y60::Image>()->set<y60::ImageMatrixTag>(myScale); 
}


std::string
CTScan::setupMaterial(SceneBuilderPtr theSceneBuilder, bool theCreateNormalsFlag) {
    // instead of using a modelling function we roll our own material here,
    // beacuse we don't have a Scene object ...
    std::string myMaterialName = "PolygonMaterial";
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
    
    myMaterialBuilder.addFeature("vertexparams", "[10[color]]");

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
CTScan::countVoxelValues(const asl::Box3i & theVOI, std::vector<unsigned> & theHistogram) {
    Vector2d myValueRange = getValueRange();
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
CTScan::computeHistogram(const Box3i & theVOI, std::vector<unsigned> & theHistogram) {
    switch (_myEncoding) {
        case y60::GRAY:
            countVoxelValues<unsigned char>(theVOI, theHistogram);
            break;
        case y60::GRAY16:
            countVoxelValues<unsigned short>(theVOI, theHistogram);
            break;
        case y60::GRAYS16:
            countVoxelValues<short>(theVOI, theHistogram);
            break;
        default:
            throw CTScanException("Unhandled voxel type in CTScan::computeHistogram()", PLUS_FILE_LINE);
    }
    
};

void
CTScan::computeProfile(const std::vector<asl::Vector3i> & thePoints, std::vector<unsigned> & theProfile) {
    if (thePoints.empty()) {
        return;
    }
    
    // for each segment, add the profile from [i, i+1[ (half-open interval)
    for (int i = 0; i+1 < thePoints.size(); ++i) {
        computeLineSegmentProfile(thePoints[i], thePoints[i+1], theProfile);
    }
    // now add the last point
    computeProfile(thePoints[thePoints.size()-1], theProfile);
}

void
CTScan::computeProfile(const asl::Vector3i & thePosition, std::vector<unsigned> & theProfile) {
    const unsigned char * myData = _mySlices[thePosition[2]]->pixels().begin();
    myData += thePosition[0]+thePosition[1]*getBytesRequired(_mySlices[thePosition[2]]->width(), _myEncoding);
    theProfile.push_back(*myData);
}

// computes the grey-scale profile of all voxels
// from [theStart, theEnd[  (without theEnd)
void
CTScan::computeLineSegmentProfile(const asl::Vector3i & theStart, const asl::Vector3i & theEnd, std::vector<unsigned> & theProfile) {
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

        asl::Vector3i curPoint(theStart + Vector3i(int(round(curDelta[0])), 
                                                   int(round(curDelta[1])), 
                                                   int(round(curDelta[2]))));
        computeProfile(curPoint, theProfile);
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

void 
CTScan::notifyProgress(double theProgress, const std::string & theMessage) {
    //cerr << "progress = " << theProgress << endl;
    _myProgressSignal.emit(theProgress, Glib::ustring(theMessage));
}

NodePtr
CTScan::createGrayImage(dom::NodePtr theParent, int theWidth, int theHeight, int theValue) {
    NodePtr myRasterNode = theParent->appendChild(NodePtr(new Element("rasterofgray")));
    myRasterNode->appendChild(NodePtr(new Text()));

    ResizeableRasterPtr myRaster
        = dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());

    Block myBlock(theWidth * theHeight, theValue);
    myRaster->assign(theWidth, theHeight, myBlock);
    AC_DEBUG << "created raster of " << myRaster->width() << "x" << myRaster->height();
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
CTScan::resizeVoxelVolume(dom::NodePtr theVoxelVolume, const asl::Box3f theDirtyBox) {
    const Box3f & myOldBox = theVoxelVolume->getAttributeValue<Box3f>("boundingbox");

    Box3f myNewBox = myOldBox;
    myNewBox.extendBy( theDirtyBox );

    if (myNewBox == myOldBox) {
        AC_INFO << "Raster box didn't change.";
        return;
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

    dom::NodePtr myRasters = theVoxelVolume->childNode("rasters");
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

    theVoxelVolume->getAttribute("boundingbox")->nodeValueAssign(myNewBox);
}


// comparator for Vector3i
struct Vector3iCmp {
    bool operator() (const Vector3i & a, const Vector3i & b) const {
        if ( a[0] < b[0] ||
            (a[0] == b[0] && a[1] < b[1]) ||
            (a[0] == b[0] && a[1] == b[1] && a[2] < b[2]))
        {
            return true;
        }
        return false;
    }
};

typedef std::map<Vector3i, unsigned char, Vector3iCmp> ColorMap;


void 
CTScan::copyCanvasToVoxelVolume(dom::NodePtr theMeasurement, dom::NodePtr theCanvasImage,
                                const asl::Box3f & theDirtyBox, Orientation theOrientation,
                                dom::NodePtr thePaletteNode)
{
    ColorMap myColorMap;
    Vector3i myColor;
    unsigned char myIndex;
    for (unsigned i = 0; i < thePaletteNode->childNodesLength(); ++i) {
        dom::NodePtr myItem = thePaletteNode->childNode(i);
        myColor = myItem->getAttributeValue<Vector3i>("color");
        myIndex = (unsigned char)(myItem->getAttributeValue<int>("index"));
        myColorMap[myColor] = myIndex;
        //cerr << "index = " << int(myIndex) << " color = " << myColor << endl;
    }

    ResizeableRasterPtr myCanvas = dynamic_cast_Ptr<ResizeableRaster>(
            theCanvasImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());

    Box3f myMeasurementBox = theMeasurement->getAttributeValue<Box3f>("boundingbox");

    switch (theOrientation) {
        case CTScan::IDENTITY:
            {
                Vector4f myFloatPixel;
                Vector3i myPixel;
                unsigned myAffectedSlice = unsigned( theDirtyBox[Box3f::MIN][2] - myMeasurementBox[Box3f::MIN][2]);
                unsigned myXStart = unsigned(theDirtyBox[Box3f::MIN][0]);
                unsigned myXEnd   = unsigned(theDirtyBox[Box3f::MAX][0]);
                unsigned myYStart = unsigned(theDirtyBox[Box3f::MIN][1]);
                unsigned myYEnd   = unsigned(theDirtyBox[Box3f::MAX][1]);
                AC_DEBUG << "affected slice = " << myAffectedSlice;
                dom::NodePtr myRasterNode = theMeasurement->childNode("rasters")->childNode(myAffectedSlice);
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
                            throw asl::Exception(string("Unknown color in canvas image: ") + as_string(myPixel),
                                                 PLUS_FILE_LINE);
                        }
                        float myValue = float( myIt->second);
                        myTargetRaster->setPixel( x - int(myMeasurementBox[Box3f::MIN][0]),
                                y - int(myMeasurementBox[Box3f::MIN][1]),
                                myValue, myValue, myValue, myValue);
                    }
                }
            }
            break;
        case CTScan::Y2Z: // top
            {
                Vector4f myFloatPixel;
                Vector3i myPixel;
                //cerr << "dirty box = " << theDirtyBox << endl;
                //cerr << "measurement box = " << myMeasurementBox << endl;
                unsigned myAffectedY = unsigned( theDirtyBox[Box3f::MIN][1] - myMeasurementBox[Box3f::MIN][1]);
                unsigned myXStart = unsigned(theDirtyBox[Box3f::MIN][0]);
                unsigned myXEnd   = unsigned(theDirtyBox[Box3f::MAX][0]);
                unsigned myZStart = unsigned(theDirtyBox[Box3f::MIN][2]);
                unsigned myZEnd   = unsigned(theDirtyBox[Box3f::MAX][2]);
                unsigned myCanvasHeight = theCanvasImage->getAttributeValue<unsigned>("height");
                AC_DEBUG << "affected y = " << myAffectedY;
                for (unsigned z = myZStart; z < myZEnd; ++z) {
                    unsigned myCurrentSlice = z - unsigned(myMeasurementBox[Box3f::MIN][2]);
                    //cerr << "current slice = " << myCurrentSlice << endl;
                    dom::NodePtr myRasterNode = theMeasurement->childNode("rasters")->childNode(myCurrentSlice);
                    if ( ! myRasterNode) {
                        throw asl::Exception("Failed to get affected raster.");
                    }
                    ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>(
                            myRasterNode->childNode(0)->nodeValueWrapperPtr());

                    for (unsigned x = myXStart; x < myXEnd; ++x) {
                        unsigned mySourceY = myCanvasHeight - z - 1;
                        //cerr << "getPixel(" << x << ", " << mySourceY << ")" << endl;
                        myFloatPixel = myCanvas->getPixel(x, mySourceY);
                        myPixel = Vector3i(int( myFloatPixel[0]), int( myFloatPixel[1]), int( myFloatPixel[2]));
                        // XXX: argh! map.find() in inner loop... any ideas? [DS]
                        ColorMap::iterator myIt = myColorMap.find(myPixel);
                        if (myIt == myColorMap.end()) {
                            throw asl::Exception(string("Unknown color in canvas image: ") + as_string(myPixel),
                                                 PLUS_FILE_LINE);
                        }
                        float myValue = float( myIt->second);
                        //cerr << "myValue = " << myValue << endl;
                        //cerr << "setPixel(" << x - myMeasurementBox[Box3f::MIN][0] << ", " 
                        //     << myAffectedY << ") on slice " << myCurrentSlice << endl;
                        myTargetRaster->setPixel( x - int(myMeasurementBox[Box3f::MIN][0]), myAffectedY,
                                myValue, myValue, myValue, myValue);
                    }
                }
            }
            break;
        case CTScan::X2Z:
            {
                Vector4f myFloatPixel;
                Vector3i myPixel;
                cerr << "dirty box = " << theDirtyBox << endl;
                cerr << "measurement box = " << myMeasurementBox << endl;
                unsigned myAffectedX = unsigned( theDirtyBox[Box3f::MIN][0] - myMeasurementBox[Box3f::MIN][0]);
                unsigned myYStart = unsigned(theDirtyBox[Box3f::MIN][1]);
                unsigned myYEnd   = unsigned(theDirtyBox[Box3f::MAX][1]);
                unsigned myZStart = unsigned(theDirtyBox[Box3f::MIN][2]);
                unsigned myZEnd   = unsigned(theDirtyBox[Box3f::MAX][2]);
                unsigned myCanvasWidth = theCanvasImage->getAttributeValue<unsigned>("width");
                AC_WARNING << "affected x = " << myAffectedX;
                for (unsigned z = myZStart; z < myZEnd; ++z) {
                    unsigned myCurrentSlice = z - unsigned(myMeasurementBox[Box3f::MIN][2]);
                    cerr << "current slice = " << myCurrentSlice << endl;
                    dom::NodePtr myRasterNode = theMeasurement->childNode("rasters")->childNode(myCurrentSlice);
                    if ( ! myRasterNode) {
                        throw asl::Exception("Failed to get affected raster.");
                    }
                    ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>(
                            myRasterNode->childNode(0)->nodeValueWrapperPtr());

                    for (unsigned y = myYStart; y < myYEnd; ++y) {
                        cerr << "getPixel(" << z << ", " << y << ")" << endl;
                        myFloatPixel = myCanvas->getPixel(z, y);
                        myPixel = Vector3i(int( myFloatPixel[0]), int( myFloatPixel[1]), int( myFloatPixel[2]));
                        // XXX: argh! map.find() in inner loop... any ideas? [DS]
                        ColorMap::iterator myIt = myColorMap.find(myPixel);
                        if (myIt == myColorMap.end()) {
                            throw asl::Exception(string("Unknown color in canvas image: ") + as_string(myPixel),
                                                 PLUS_FILE_LINE);
                        }
                        float myValue = float( myIt->second);
                        cerr << "myValue = " << myValue << endl;
                        cerr << "setPixel(" << myAffectedX << ", " 
                             << y - int(myMeasurementBox[Box3f::MIN][1]) << ") on slice " << myCurrentSlice << endl;
                        myTargetRaster->setPixel( myAffectedX, y - int(myMeasurementBox[Box3f::MIN][1]),
                                myValue, myValue, myValue, myValue);
                    }
                }
            }
            break;
        default:
            break;

    }
}

void 
CTScan::copyVoxelVolumeToCanvas(dom::NodePtr theMeasurement, dom::NodePtr theCanvas, unsigned theSliceIndex,
        Orientation theOrientation, dom::NodePtr thePaletteNode)
{

    std::vector<Vector3i> myPalette(255, Vector3i( -1, -1, -1));
    unsigned myIndex;
    Vector3i myColor;
    for (unsigned i = 0; i < thePaletteNode->childNodesLength(); ++i) {
        myIndex = thePaletteNode->childNode(i)->getAttributeValue<unsigned>("index");
        myColor = thePaletteNode->childNode(i)->getAttributeValue<Vector3i>("color");
        myPalette[myIndex] = myColor;
    }

    Box3f myBoundingBox = theMeasurement->getAttributeValue<Box3f>("boundingbox");

    ResizeableRasterPtr myTargetRaster = dynamic_cast_Ptr<ResizeableRaster>( 
            theCanvas->childNode(0)->childNode(0)->nodeValueWrapperPtr());
    unsigned myCanvasWidth = theCanvas->getAttributeValue<unsigned>("width");
    unsigned myCanvasHeight = theCanvas->getAttributeValue<unsigned>("height");

    switch ( theOrientation) {
        case CTScan::IDENTITY: // front
            //cerr << "index = " << theSliceIndex << " bbox = " << myBoundingBox[Box3f::MIN][2] << endl;
            if (theSliceIndex >= myBoundingBox[Box3f::MIN][2] && theSliceIndex < myBoundingBox[Box3f::MAX][2]) {
                dom::NodePtr myRasterNode = theMeasurement->childNode(0)->childNode(int( theSliceIndex - myBoundingBox[Box3f::MIN][2]));
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
                        myTargetRaster->setPixel(x, y, float(myColor[0]), float(myColor[1]), float(myColor[2]),
                                (myIndex == 0 ? 0.0 : 255.0)); // index zero is our erase color
                    }
                }
                //cerr << "done" << endl;
            }
            break;
        case CTScan::Y2Z: // top
            if (theSliceIndex >= myBoundingBox[Box3f::MIN][1] && theSliceIndex < myBoundingBox[Box3f::MAX][1]) {
                unsigned mySourceY = unsigned( theSliceIndex - myBoundingBox[Box3f::MIN][1]);
                unsigned myZStart = unsigned( myBoundingBox[Box3f::MIN][2]);
                unsigned myZEnd = unsigned( myBoundingBox[Box3f::MAX][2]);
                unsigned myXStart = unsigned(myBoundingBox[Box3f::MIN][0]);
                unsigned myXEnd = unsigned(myBoundingBox[Box3f::MAX][0]);
                Vector4f myFloatPixel;
                for (unsigned z = myZStart; z < myZEnd; ++z) {
                    unsigned myCurrentSliceIndex = z - myZStart;
                    dom::NodePtr myRasterNode = theMeasurement->childNode(0)->childNode(myCurrentSliceIndex);
                    ResizeableRasterPtr mySourceRaster =
                        dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());
                    for (unsigned x = myXStart; x < myXEnd; ++x) {
                        //cerr << "getPixel(" << x - myXStart << ", " << mySourceY << ")" << endl;
                        myFloatPixel = mySourceRaster->getPixel(x - myXStart, mySourceY);
                        myIndex = (unsigned char)(myFloatPixel[0]);
                        myColor = myPalette[myIndex];
                        //cerr << "setPixel x = " << x << " z = " << myCanvasHeight - z - 1 << " index = " << myIndex << endl;
                        myTargetRaster->setPixel(x, myCanvasHeight - z - 1, float(myColor[0]), float(myColor[1]), float(myColor[2]),
                                (myIndex == 0 ? 0.0 : 255.0)); // index zero is our erase color
                        
                    }
                }
            }
            break;
        case CTScan::X2Z: // side
            if (theSliceIndex >= myBoundingBox[Box3f::MIN][0] && theSliceIndex < myBoundingBox[Box3f::MAX][0]) {
                unsigned mySourceX = unsigned( theSliceIndex - myBoundingBox[Box3f::MIN][0]);
                unsigned myZStart = unsigned( myBoundingBox[Box3f::MIN][2]);
                unsigned myZEnd = unsigned( myBoundingBox[Box3f::MAX][2]);
                unsigned myYStart = unsigned(myBoundingBox[Box3f::MIN][1]);
                unsigned myYEnd = unsigned(myBoundingBox[Box3f::MAX][1]);
                Vector4f myFloatPixel;
                for (unsigned z = myZStart; z < myZEnd; ++z) {
                    unsigned myCurrentSliceIndex = z - myZStart;
                    dom::NodePtr myRasterNode = theMeasurement->childNode(0)->childNode(myCurrentSliceIndex);
                    ResizeableRasterPtr mySourceRaster =
                        dynamic_cast_Ptr<ResizeableRaster>(myRasterNode->childNode(0)->nodeValueWrapperPtr());
                    for (unsigned y = myYStart; y < myYEnd; ++y) {
                        // cerr << "getPixel(" << mySourceX << ", " << y - myYStart << ")" << endl;
                        myFloatPixel = mySourceRaster->getPixel(mySourceX, y - myYStart);
                        myIndex = (unsigned char)(myFloatPixel[0]);
                        myColor = myPalette[myIndex];
                        // cerr << "setPixel z = " << z << " y = " << y << endl;
                        myTargetRaster->setPixel(z, y, float(myColor[0]), float(myColor[1]), float(myColor[2]),
                                (myIndex == 0 ? 0.0 : 255.0)); // index zero is our erase color
                        
                    }
                }
            }
            break;
    }
}

}

