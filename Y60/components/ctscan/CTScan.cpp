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

template <class VoxelT>
void
CTScan::applyMarchingCubes(const asl::Box3i & theVoxelBox, 
                             double theThresholdMin, double theThresholdMax, int theDownSampleRate,
                             bool theCreateNormalsFlag, ScenePtr theScene, 
                             unsigned int theNumVertices, unsigned int theNumTriangles,
                             const SegmentationBitmap * theSegmentationBitmap)
{
    SceneBuilderPtr mySceneBuilder = theScene->getSceneBuilder();
    std::string myMaterialId = setupMaterial( mySceneBuilder, theCreateNormalsFlag);
    
    // here we go ...
    MarchingCubes<VoxelT> myMarcher(theDownSampleRate, mySceneBuilder, this);

    myMarcher.setBox(theVoxelBox, theSegmentationBitmap);
    myMarcher.setThreshold(asl::Vector2<VoxelT>(VoxelT(theThresholdMin), VoxelT(theThresholdMax)));
    //myMarcher.setThreshold(VoxelT(theThresholdMax));
    myMarcher.calcNormals(theCreateNormalsFlag);
    if (theNumVertices == 0 || theNumTriangles == 0) {
        myMarcher.estimate();
    } else {
        myMarcher.reserveBuffers(theNumVertices, theNumTriangles);
    }
    dom::NodePtr myShapeNode = myMarcher.apply("polygonal_shape", myMaterialId);

    setupScene(theScene, myShapeNode->getAttributeString("id"));
    asl::Box3f myBox;
    for (int i = 0; i < 6; ++i) {
        myBox[i/3][i%3] = float(theVoxelBox[i/3][i%3]) * _myVoxelSize[i%3];        
    }
    resetCamera(theScene, myBox);
}

template <class VoxelT>
void
CTScan::countMarchingCubes(const asl::Box3i & theVoxelBox,
                             double theThresholdMin, double theThresholdMax, int theDownSampleRate,
                             unsigned int & theVertexCount, unsigned int & theTriangleCount,
                             const SegmentationBitmap * theSegmentationBitmap)
{
    // here we go ...
    MarchingCubes<VoxelT> myMarcher(theDownSampleRate, SceneBuilderPtr(0), this);

    myMarcher.setBox(theVoxelBox, theSegmentationBitmap);
    myMarcher.setThreshold(asl::Vector2<VoxelT>(VoxelT(theThresholdMin), VoxelT(theThresholdMax)));
    //myMarcher.setThreshold(VoxelT(theThresholdMax));
    myMarcher.calcNormals(false);
    myMarcher.estimate(&theVertexCount, &theTriangleCount);
}
/**
 * @return a Vector of which x is VertexCount and y is TriangleCount
 */
asl::Vector2i
CTScan::countTriangles(const asl::Box3i & theVoxelBox, 
                       double theThresholdMin, 
                       double theThresholdMax, 
                       int theDownSampleRate, 
                       const SegmentationBitmap * theSegmentationBitmap)
{
    unsigned int myVertexCount;
    unsigned int myTriangleCount;

    switch (_myEncoding) {
        case y60::GRAY:
            countMarchingCubes<unsigned char>(theVoxelBox, theThresholdMin, 
                theThresholdMax, theDownSampleRate, myVertexCount, 
                myTriangleCount, theSegmentationBitmap);
            break;
        case y60::GRAY16:
            countMarchingCubes<unsigned short>(theVoxelBox, theThresholdMin, theThresholdMax, 
                theDownSampleRate, myVertexCount, myTriangleCount, theSegmentationBitmap);
            break;
        case y60::GRAYS16:
            countMarchingCubes<short>(theVoxelBox, theThresholdMin, theThresholdMax, 
                theDownSampleRate, myVertexCount, myTriangleCount, theSegmentationBitmap);
            break;
        default:
            throw CTScanException("Unhandled voxel type in CTScan::countTriangles()", PLUS_FILE_LINE);
    }
    return asl::Vector2i(myVertexCount, myTriangleCount);
}
ScenePtr
CTScan::polygonize(const asl::Box3i & theVoxelBox, double theThresholdMin, double theThresholdMax, 
                   int theDownSampleRate, bool theCreateNormalsFlag, PackageManagerPtr thePackageManager, 
                   unsigned int theNumVertices, unsigned int theNumTriangles, 
                   const SegmentationBitmap * theSegmentationBitmap)
{
    asl::Time myStartTime;

    ScenePtr myScene(new Scene);
    myScene->createStubs(thePackageManager);    
    switch (_myEncoding) {
        case y60::GRAY:
            applyMarchingCubes<unsigned char>(theVoxelBox, theThresholdMin, theThresholdMax, 
                theDownSampleRate, theCreateNormalsFlag, myScene, theNumVertices, 
                theNumTriangles, theSegmentationBitmap);
            break;
        case y60::GRAY16:
            applyMarchingCubes<unsigned short>(theVoxelBox, theThresholdMin, theThresholdMax, 
                theDownSampleRate, theCreateNormalsFlag, myScene, theNumVertices, theNumTriangles, 
                theSegmentationBitmap);
            break;
        case y60::GRAYS16:
            applyMarchingCubes<short>(theVoxelBox, theThresholdMin, theThresholdMax, 
                theDownSampleRate, theCreateNormalsFlag, myScene, theNumVertices, 
                theNumTriangles, theSegmentationBitmap);
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
                for (int y=0; y < myHeight; ++y) {
                    const unsigned char * mySource = _mySlices[y]->pixels().begin();
                    memcpy(myTarget->begin()+myLineStride*y,
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
                    for (int y=0; y < myHeight; ++y) {
                        memcpy(myTarget->begin()+myTargetLineStride*y+myBpp*x, 
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
/*
    VectorOfRankedFeature myVertexparamsFeature;
    RankedFeature myVertexParam;
    myVertexParam._myFeature.push_back(getStringFromEnum(i, LightingModelString));
    myVertexParam._myRanking = 10.f;

    myVertexParamsFeature.push_back(myVertexParam);
    */
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
    AC_DEBUG << "creating 3D texture, size=" << myTextureSize;
    
    asl::Block my3DTexture;
    int mySliceCount = getSliceCount();
    int myBytesPerSlice = getBytesRequired(myTextureSize[0]*myTextureSize[1], _myEncoding);

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
    for (unsigned z = 0; z < myTextureSize[2]-2; ++z) {
        int mySlice = int(floor(float(z*mySliceCount)/myTextureSize[2]));
        AC_TRACE << " adding slice " << mySlice; 
        CTScan::appendTo3DTexture(mySlice, my3DTexture, myTextureSize[0], myTextureSize[1]);
    }
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

    y60::ImagePtr myImage = theImageNode->getFacade<y60::Image>();
    myImage->set(myTextureSize[0], myTextureSize[1], myTextureSize[2], _myEncoding, my3DTexture);
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


}

