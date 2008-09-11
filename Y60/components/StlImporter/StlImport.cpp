//============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================


#include "StlImport.h"
#include "JSStlCodec.h"

#include <y60/scene/SceneBuilder.h>
#include <y60/scene/MaterialBuilder.h>
#include <y60/scene/ShapeBuilder.h>
#include <y60/scene/ElementBuilder.h>
#include <y60/scene/TransformBuilder.h>
#include <y60/scene/WorldBuilder.h>
#include <y60/scene/CameraBuilder.h>
#include <y60/scene/BodyBuilder.h>

#include <y60/scene/Shape.h>

#include <y60/base/PropertyNames.h>
#include <y60/base/property_functions.h>

#include <y60/base/DataTypes.h>
#include <y60/scene/VertexData.h>
#include <y60/scene/Primitive.h>

#include <asl/math/linearAlgebra.h>
#include <asl/base/file_functions.h>
#include <asl/base/settings.h>
#include <asl/base/Stream.h>
#include <asl/base/Assure.h>

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <string.h>

#define DB(x) //x;

using namespace std;
using namespace asl;

extern "C"
EXPORT PlugInBase* y60StlImporter_instantiatePlugIn(DLHandle myDLHandle) {
	return new y60::StlImport(myDLHandle);
}

namespace y60 {


    /**********************************************************************
     *
     * Public
     *
     **********************************************************************/
    std::string
    StlImport::canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theSource) {
        AC_DEBUG << " StlImport::canDecode: theUrl = '" << theUrl << "', theSource=" << theSource;
        if (theSource) {
            asl::ReadableStream & theStream = theSource->getStream();
            if (theStream) {
                if (isBigEndian(theStream) || isLittleEndian(theStream)) {
                    return MIME_TYPE_STL;
                }
            }
            AC_DEBUG << "StlImport::canDecode: returning empty string";
            return "";
        }
        if (asl::toLowerCase(asl::getExtension(theUrl)) == "stl") {
            return MIME_TYPE_STL;
        }
        return "";
    }

    bool 
    StlImport::setProgressNotifier(IProgressNotifierPtr theNotifier) {
        _myProgressNotifier = theNotifier;
        return true;
    }
    
    bool
    StlImport::decodeScene(asl::Ptr<asl::ReadableStreamHandle> theStreamHandle, dom::DocumentPtr theScene) {
        asl::ReadableStream & theSource = theStreamHandle->getStream();
        _myStlShapes.clear();
        if (theSource.size() < StlCodec::HEADER_SIZE + StlCodec::NUM_FACET_SIZE) {
            throw ImportException(std::string("Unable to read stl header: "), PLUS_FILE_LINE);
        }

        string IS_ASCII("solid");
        string IS_ASCII_END("endsolid");
        string myAsciiHeader;
        theSource.readString(myAsciiHeader, IS_ASCII.size(), 0);
        bool binaryFormat = (myAsciiHeader != IS_ASCII);
        if (!binaryFormat) {
            AC_WARNING << "This file starts with 'solid' and therefore looks like an ASCII file. Still trying";
        }

        SceneBuilder sceneBuilder(theScene);

        // material
        dom::NodePtr material = setupMaterial("mSTL", sceneBuilder);
        const std::string& materialId = material->getAttribute("id")->nodeValue();

        // import
        bool ok = false;
        if (isBigEndian(theSource)) {
            asl::ReadableStreamAdapter<PowerPCByteOrder, AC_HOST_BYTE_ORDER> myAdapter(theSource);
            AC_SIZE_TYPE myPos = 0;
            while (myPos < myAdapter.size()) {
                DB(cerr << "STL binary big endian import file position " << myPos << endl);
                myPos += readFileBinary(myAdapter, myPos, sceneBuilder, materialId);
            }
            ok = myPos;
            AC_INFO << "Loaded STL binary big endian file";
        } else if (isLittleEndian(theSource)) {
            asl::ReadableStreamAdapter<X86ByteOrder, AC_HOST_BYTE_ORDER> myAdapter(theSource);
            AC_SIZE_TYPE myPos = 0;
            while (myPos < myAdapter.size()) {
                DB(cerr << "STL binary little endian file position " << myPos << endl);
                myPos += readFileBinary(myAdapter, myPos, sceneBuilder, materialId);
            }
            ok = myPos;
            AC_INFO << "Loaded STL binary little endian file";
        } else {
            throw ImportException(std::string("This file is not in STL binary format. Tried big- and little-endian."), 
                    PLUS_FILE_LINE);
        }
        if (!ok) {
            return false;
        }

        // create world
        WorldBuilder worldBuilder;
        sceneBuilder.appendWorld(worldBuilder);

        // camera
        CameraBuilder cameraBuilder("myCamera");
        worldBuilder.appendObject(cameraBuilder);
        dom::NodePtr myCameraNode = cameraBuilder.getNode();
        // Move camera out of object
        dom::NodePtr myAttr = myCameraNode->getAttribute("position");
        Vector3f & myPosition = myAttr->nodeValueRefOpen<asl::Vector3f>();
        myPosition = asl::Vector3f(0.0, 0.0, 100.0);
        myAttr->nodeValueRefClose<asl::Vector3f>();
        sceneBuilder.setInitialCamera(cameraBuilder.getNode()->getAttribute("id")->nodeValue());

        TransformBuilder transformBuilder("Main");
        worldBuilder.appendObject(transformBuilder);
        // body
        for (int i=0; i < _myStlShapes.size(); ++i) {
            BodyBuilder bodyBuilder(_myStlShapes[i]->getAttribute("id")->nodeValue(),
                                     _myStlShapes[i]->getAttribute("name")->nodeValue());
            transformBuilder.appendObject(bodyBuilder);
        }
        return true;
    }


    /**********************************************************************
     *
     * Private
     *
     **********************************************************************/

     template <class AC_BYTE_ORDER_LOCAL>
     unsigned int 
     StlImport::readFileBinary(const asl::ReadableArrangedStream<AC_BYTE_ORDER_LOCAL> & theData,
                       AC_SIZE_TYPE theOffset, SceneBuilder& sceneBuilder,
                       const std::string& materialId)
     {
         std::string myShapeName;
         theData.readString(myShapeName, StlCodec::HEADER_SIZE, theOffset);
         std::string::size_type myShapeNameEnd = myShapeName.find('0', 0);
         if (myShapeNameEnd == std::string::npos) {
             myShapeName = "No Name";
         } else {
             myShapeName.erase(myShapeNameEnd);
         }

         ElementBuilder elementBuilder(PrimitiveTypeStrings[TRIANGLES], materialId);
         ShapeBuilder shapeBuilder(std::string("s") + myShapeName, false);
         shapeBuilder.appendElements(elementBuilder);
         sceneBuilder.appendShape(shapeBuilder);

         // seek to size filed
         theOffset += StlCodec::HEADER_SIZE;
         AC_SIZE_TYPE expectFacets;
         theData.readUnsigned32(expectFacets, theOffset);
         theOffset += StlCodec::NUM_FACET_SIZE;

         AC_TRACE << "expecting " << expectFacets << " Facets.";
         if (expectFacets <= 0) {
             throw ImportException(std::string("malformed header in stl file!"), PLUS_FILE_LINE);
         }
         shapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE,
                 expectFacets*3);
         shapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(NORMAL_ROLE,
                 expectFacets*3);
         elementBuilder.createIndex(POSITION_ROLE, POSITIONS, expectFacets*3);
         elementBuilder.createIndex(NORMAL_ROLE, NORMALS, expectFacets*3);

         bool haveColor = false;

         // read facets
         unsigned int normalIndex, vertexIndex;
         for (unsigned int myFaceNumber = 0; myFaceNumber < expectFacets; ++myFaceNumber) {
             StlCodec::StlFacet myFacet;
             AC_SIZE_TYPE myOffset = theOffset;
             for (int i = 0; i < 3; ++i) {
                 myOffset = theData.readFloat32(myFacet.normal[i], myOffset);
             }
             for (int i = 0; i < 9; ++i) {
                 myOffset = theData.readFloat32(myFacet.vertex[i/3][i%3], myOffset);
             }
             myOffset = theData.readUnsigned16(myFacet.color, myOffset);
             theOffset += StlCodec::FACET_SIZE;
             if (theOffset != myOffset) {
                 AC_WARNING << "Strange Offset: myOffset=" << myOffset << " and theOffset=" << theOffset;
             }
             // vertices
             vertexIndex = shapeBuilder.appendVertexData(POSITION_ROLE, myFacet.vertex[0]);
             elementBuilder.appendIndex(POSITIONS, vertexIndex);

             vertexIndex = shapeBuilder.appendVertexData(POSITION_ROLE, myFacet.vertex[1]);
             elementBuilder.appendIndex(POSITIONS, vertexIndex);

             vertexIndex = shapeBuilder.appendVertexData(POSITION_ROLE, myFacet.vertex[2]);
             elementBuilder.appendIndex(POSITIONS, vertexIndex);

             // color extension
             if (myFacet.color & StlCodec::StlColorValid) {

                 if (haveColor == false) {
                     shapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector4f>(COLOR_ROLE,1);
                     elementBuilder.createIndex(COLOR_ROLE, COLORS,1);
                     haveColor = true;
                 }

                 float r = ((myFacet.color >> 10) & StlCodec::StlColorMask) / (float) StlCodec::StlColorMask;
                 float g = ((myFacet.color >> 5) & StlCodec::StlColorMask) / (float) StlCodec::StlColorMask;
                 float b = ((myFacet.color) & StlCodec::StlColorMask) / (float) StlCodec::StlColorMask;
                 asl::Vector4f color(r,g,b,1.0f);

                 unsigned int colorIndex = shapeBuilder.appendVertexData(COLOR_ROLE, color);
                 elementBuilder.appendIndex(COLORS, colorIndex);
                 elementBuilder.appendIndex(COLORS, colorIndex);
                 elementBuilder.appendIndex(COLORS, colorIndex);
             }

             // normal (per-vertex)
             if (_myGenerateNormal) {
                 normalIndex = shapeBuilder.appendVertexData(NORMAL_ROLE,
                         generateNormal(myFacet.vertex[0],myFacet.vertex[1], myFacet.vertex[2]));
             }
             else {
                 normalIndex = shapeBuilder.appendVertexData(NORMAL_ROLE, myFacet.normal);
             }
             elementBuilder.appendIndex(NORMALS, normalIndex);
             elementBuilder.appendIndex(NORMALS, normalIndex);
             elementBuilder.appendIndex(NORMALS, normalIndex);
             if (_myProgressNotifier && (myFaceNumber % 1000 == 0)) {
                 _myProgressNotifier->onProgress(float(myOffset)/float(theData.size()));
             }
         }
         _myStlShapes.push_back(shapeBuilder.getNode());
         unsigned int bytesRead = StlCodec::FACET_SIZE *expectFacets + StlCodec::HEADER_SIZE + StlCodec::NUM_FACET_SIZE;
         AC_DEBUG << expectFacets << " Facets read, " << bytesRead << " bytes read.";
         return bytesRead;
     }

    asl::Vector3f StlImport::generateNormal(const asl::Vector3f& v0,
                                            const asl::Vector3f& v1,
                                            const asl::Vector3f& v2)
    {
        asl::Vector3f d01 = difference(v1, v0);
        asl::Vector3f d02 = difference(v2, v0);
        return normalized(cross(d01, d02));
    }

    dom::NodePtr StlImport::setupMaterial(const std::string& theBaseName,
                                          SceneBuilder& sceneBuilder)
    {
        MaterialBuilder materialBuilder(theBaseName);

        asl::Vector4f myOpaqueWhite(1.0, 1.0, 1.0, 1.0);
        setPropertyValue<asl::Vector4f>(materialBuilder.getNode(),
              "vector4f", AMBIENT_PROPERTY, myOpaqueWhite);

        setPropertyValue<asl::Vector4f>(materialBuilder.getNode(),
              "vector4f", DIFFUSE_PROPERTY, myOpaqueWhite);

        VectorOfRankedFeature feature;
        createLightingFeature(feature, LAMBERT);
        materialBuilder.setType(feature);

        sceneBuilder.appendMaterial(materialBuilder);

        return materialBuilder.getNode();
    }

    bool StlImport::isLittleEndian(asl::ReadableStream & theSource) const {
        asl::ReadableStreamAdapter<X86ByteOrder, AC_HOST_BYTE_ORDER> myAdapter(theSource);
        AC_SIZE_TYPE myPos = 0;
        while (myPos + StlCodec::HEADER_SIZE + StlCodec::NUM_FACET_SIZE < myAdapter.size()) {
            // skip header
            myPos += StlCodec::HEADER_SIZE;
            // read number of facets
            AC_SIZE_TYPE expectFacets;
            myAdapter.readUnsigned32(expectFacets, myPos);
            myPos += StlCodec::NUM_FACET_SIZE;
            // skip the facet data
            myPos += StlCodec::FACET_SIZE *expectFacets;
        }
        if (myPos > 0 && myPos == myAdapter.size()) {
            return true;
        }
        return false;
    }

    bool StlImport::isBigEndian(asl::ReadableStream & theSource) const {
        asl::ReadableStreamAdapter<PowerPCByteOrder, AC_HOST_BYTE_ORDER> myAdapter(theSource);
        AC_SIZE_TYPE myPos = 0;
        while (myPos + StlCodec::HEADER_SIZE + StlCodec::NUM_FACET_SIZE < myAdapter.size()) {
            // skip header
            myPos += StlCodec::HEADER_SIZE;
            // read number of facets
            AC_SIZE_TYPE expectFacets;
            myAdapter.readUnsigned32(expectFacets, myPos);
            myPos += StlCodec::NUM_FACET_SIZE;
            // skip the facet data
            myPos += StlCodec::FACET_SIZE *expectFacets;
        }
        if (myPos > 0 && myPos == myAdapter.size()) {
            return true;
        }
        return false;
    }

    void 
    StlImport::initClasses(JSContext * theContext, JSObject *theGlobalObject) {
        jslib::JSStlCodec::initClass(theContext, theGlobalObject);
    }
}
