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
//
//   $RCSfile: ShapeExporter.cpp,v $
//   $Author: pavel $
//   $Revision: 1.65 $
//   $Date: 2005/04/24 00:41:20 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#include "ShapeExporter.h"
#include "ExportExceptions.h"
#include "MayaHelpers.h"
#include "JointWeight.h"

#include <asl/math/linearAlgebra.h>
#include <asl/base/Dashboard.h>

#include <y60/scene/ShapeBuilder.h>
#include <y60/scene/ElementBuilder.h>

#include <maya/MGlobal.h>
#include <maya/MFnSet.h>
#include <maya/MDagPath.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MPlug.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MDagPathArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItGeometry.h>

#include <iostream>
#include <queue>
#include <set>

#define DB(x) // x
#define DB_Y(x)  x

using namespace std;
using namespace y60;
using namespace asl;

typedef std::map<std::string, y60::ElementBuilderPtr> ElementBuilderMap;

const unsigned int MAX_INFLUENCES = 8;
const asl::Vector4f DEFAULT_VERTEX_COLOR(1, 1, 1, 1);

ShapeExporter::ShapeExporter(MDagPath theDagPath) :
        _myInstanceNumber(0),
        _myDoubleSidedFlag(true),
        _myOppositeFlag(false)
{
    _myDagPath = new MDagPath(theDagPath);

    MStatus myStatus;
    _myMesh = new MFnMesh(*_myDagPath, & myStatus);

    MPlug myDoubleSidedPlug = MFnDependencyNode(_myMesh->object()).findPlug("doubleSided", & myStatus);
    if ((! myDoubleSidedPlug.isNull()) && myStatus == MStatus::kSuccess) {

        myDoubleSidedPlug.getValue(_myDoubleSidedFlag);
        if (myStatus != MStatus::kSuccess) {
            throw ExportException("Could not read attribute 'doubleSided'", PLUS_FILE_LINE);
        }
    }

    MPlug myOppositePlug = MFnDependencyNode(_myMesh->object()).findPlug("opposite", & myStatus);
    if ((! myOppositePlug.isNull()) && myStatus == MStatus::kSuccess) {

        myOppositePlug.getValue(_myOppositeFlag);
        if (myStatus != MStatus::kSuccess) {
            throw ExportException("Could not read attribute 'doubleSided'", PLUS_FILE_LINE);
        }
    }

    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not create MFnMesh", "ShapeExporter::ShapeExporter()");
    }

    //If the shape is instanced then we need to determine which instance this path refers to.
    if (_myDagPath->isInstanced()) {
        _myInstanceNumber = _myDagPath->instanceNumber();
    }
}

ShapeExporter::~ShapeExporter() {
    if (_myDagPath != NULL) {
        delete _myDagPath;
    }
    if (_myMesh != NULL) {
        delete _myMesh;
    }
}

void
ShapeExporter::extractGeometry() {
    MAKE_SCOPE_TIMER(ShapeExporter_extractGeometry);

    ATTR_DUMP(dumpAttributes(_myMesh->object()));
    if (MStatus::kFailure == _myMesh->getPoints(_myVertexArray, MSpace::kObject)) {
        throw ExportException("Could not get points", "ShapeExporter::extractGeometry()");
    }
    // (VS) Maya7.0 does not export vertex colors or face vertexcolors (?), if none present
    //      ignore return value
    if (MStatus::kFailure == _myMesh->getFaceVertexColors(_myColorArray) ) {
        // do not mind
    }

    if (MStatus::kFailure == _myMesh->getNormals(_myNormalArray, MSpace::kObject)) {
        throw ExportException("Could not get normals", "ShapeExporter::extractGeometry()");
    }

    //Have to make the path include the shape below it so that
    //we can determine if the underlying shape node is instanced.
    //By default, dag paths only include transform nodes.
    _myDagPath->extendToShape();

    //Get the connected sets and members - these will be used to determine texturing of different
    //faces
    if (!_myMesh->getConnectedSetsAndMembers(_myInstanceNumber, _myPolygonSets,
                                             _myPolygonComponents, true))
    {
        throw ExportException("Could not get connected sets and members", "ShapeExporter::extractGeometry()");
    }

}

void
ShapeExporter::appendToScene(y60::SceneBuilder & theSceneBuilder,
                             MaterialExporter & theMaterialExporter,
                             std::map<string, string>& thePathToIdMap)
{
    MAKE_SCOPE_TIMER(ShapeExporter_appendToScene);
    MDagPath firstPath;
    MDagPath::getAPathTo(_myMesh->object(), firstPath);

    MStatus status;
    string myPathName = firstPath.fullPathName(&status).asChar();

    // if we already have this mesh we are done ...
    if (thePathToIdMap.find(myPathName) != thePathToIdMap.end()) {
        return;
    }

    extractGeometry();

    DB("ShapeExporter::appendToScene(" << _myMesh->name().asChar() << ")");
    y60::ShapeBuilder myShapeBuilder(std::string(_myMesh->name().asChar()));
    {
        MAKE_SCOPE_TIMER(ShapeExporter_insertShapeIdInMap);
        string myId = theSceneBuilder.appendShape(myShapeBuilder);
        DB(AC_TRACE << "ShapeExporter::appendToScene: path='"<<myPathName<<"', id from theSceneBuilder.appendShape: "<< myId<<endl;)
        thePathToIdMap.insert(std::make_pair(myPathName, myId));
    }

    {
        MAKE_SCOPE_TIMER(ShapeExporter_outputMeat);
        outputPositions(myShapeBuilder);

        outputNormals(myShapeBuilder);
        outputColors(myShapeBuilder);
        outputUVs(myShapeBuilder);
        bool hasSkinAndBones = outputJointWeights(myShapeBuilder);
        if (!outputPrimitives(myShapeBuilder, theSceneBuilder, theMaterialExporter, hasSkinAndBones)) {
            // shape possibly incomplete
            displayError(string("ShapeExporter::appendToScene(): shape incomplete: mesh = ") + _myMesh->name().asChar() + ", path = " + myPathName);
        }
    }
    if (_myDoubleSidedFlag) {
        myShapeBuilder.setFrontFacing(true);
        myShapeBuilder.setBackFacing(true);
    } else if (_myOppositeFlag) {
        myShapeBuilder.setFrontFacing(false);
        myShapeBuilder.setBackFacing(true);
    } else {
        myShapeBuilder.setFrontFacing(true);
        myShapeBuilder.setBackFacing(false);
    }
}

void
ShapeExporter::getUVSetNamesFromTextures(const VectorOfString & theTextureNames,
                                         VectorOfString & theUVSetNameList)
{
    for (unsigned i = 0; i < theTextureNames.size(); ++i) {
        const std::string & myTextureName = theTextureNames[i];
        std::map<std::string, std::string>::iterator it = _myTextureUVSetMap.find(myTextureName);
        if (it != _myTextureUVSetMap.end()) {
            std::string myUVSetName = it->second;
            theUVSetNameList.push_back(myUVSetName);
        } else {
            throw ExportException(string("Could not find uv set for texture: ") + myTextureName +
                " in mesh: " + string(_myMesh->name().asChar()), PLUS_FILE_LINE);
        }
    }
}

void
ShapeExporter::checkVertexCountPerFace(unsigned theVertexCount, MStatus & theStatus) {
    if (theVertexCount < 3) {
        throw ExportException(string("Face: '") + string(_myMesh->name().asChar()) + "' contains less than 3 vertices",
                "ShapeExporter::checkVertexCountPerFace()");
    }

    if (theVertexCount > 4) {
        throw ExportException(string("Face: '") + string(_myMesh->name().asChar()) +
                "' contains too many vertices, you probably forgot to triangluate. VertexCount = " +
                asl::as_string(theVertexCount),
                "ShapeExporter::checkVertexCountPerFace()");
    }

    if (theStatus == MStatus::kFailure) {
        throw ExportException(string("Could not get vertex count of shape: ") + string(_myMesh->name().asChar()),
                "ShapeExporter::checkVertexCountPerFace()");
    }
}

bool
ShapeExporter::outputPrimitives(y60::ShapeBuilder & theShapeBuilder,
                                y60::SceneBuilder & theSceneBuilder,
                                MaterialExporter & theMaterialExporter,
                                bool theHasSkinAndBonesFlag)
{
    MAKE_SCOPE_TIMER(ShapeExporter_outputPrimitives);
    unsigned int myFaceCount = _myMesh->numPolygons();

    MStatus myStatus;

   //  Storage for set objects (shader objects)
    MObjectArray myShadingGroups;

    // Storage for indices matching faces to shaders. For each face,
    // this array contains the index into the shaders array for the shader assigned to the face.
    MIntArray    myIndices;

    myStatus = _myMesh->getConnectedShaders(_myInstanceNumber, myShadingGroups, myIndices);
    if (myStatus == MS::kFailure) {
        displayError(string("Could not get connected shaders for mesh ") + _myMesh->name().asChar());
        //return false;
    }
    MIntArray myPositionIndexArray;
    MIntArray myNormalIndexArray;

    ElementBuilderMap myElementMap;
    ElementBuilderPtr myElementBuilder;

    SpecialFeatures mySpecialFeatures;
    if (theHasSkinAndBonesFlag) {
        mySpecialFeatures.push_back(MaterialFeature("physics", "skin"));
    }

    try {
        DB_Y(cerr << "ShapeExporter::outputPrimitives: mesh = "<< _myMesh->name().asChar()<<", myFaceCount = "<<myFaceCount<<endl);
        unsigned myUVSetWarningCount = 0;
        for (unsigned i = 0; i < myFaceCount; i++) {
            unsigned myVertexCountPerFace = _myMesh->polygonVertexCount(i, & myStatus);
            checkVertexCountPerFace(myVertexCountPerFace, myStatus);

            // Get the material for the current face
            MObject myMaterialNode = theMaterialExporter.getMaterial(myIndices, myShadingGroups, i);

            // Get all texture objects connected to the current material
            string myMaterialName = MFnDependencyNode(myMaterialNode).name().asChar();

            // Export the material, if it does not exist, yet
            string myMaterialId = theMaterialExporter.createFaceMaterial(_myMesh, myMaterialNode, theSceneBuilder, mySpecialFeatures);

            VectorOfString myUVSetNames;
            if (theMaterialExporter.hasUVMappedTextures(myMaterialName)) {
                MAKE_SCOPE_TIMER(outputPrimitives_getTextureNames);
                const VectorOfString & myMaterialTextureNames = theMaterialExporter.getTextureNames(myMaterialName);
                // Get a list of uv set indices that corresponds to the the current material's textures
                getUVSetNamesFromTextures(myMaterialTextureNames, myUVSetNames);
            }

            // All primitives from the same type (myVertexCountPerFace) and same material (myMaterialId) are sorted into one bin
            string myElementMapKey = myMaterialId + asl::as_string(myVertexCountPerFace);

            ElementBuilderMap::iterator myElementIt = myElementMap.find(myElementMapKey);
            if (myElementIt == myElementMap.end()) {
                MAKE_SCOPE_TIMER(outputPrimitives_ElementBuilder);

                string myPrimitiveTye = PrimitiveTypeStrings[TRIANGLES];
                if (myVertexCountPerFace == 4) {
                    myPrimitiveTye = PrimitiveTypeStrings[QUADS];
                }

                // Create new primitives builder for this material and primitive type
                myElementMap[myElementMapKey] = y60::ElementBuilderPtr(
                    new y60::ElementBuilder(myPrimitiveTye, myMaterialId));
                myElementBuilder = myElementMap.find(myElementMapKey)->second;

                myElementBuilder->createIndex(POSITION_ROLE, POSITIONS, myFaceCount * 4);
                myElementBuilder->createIndex(NORMAL_ROLE, NORMALS, myFaceCount * 4);

                if (!_myColorIndices.empty()) {
                    myElementBuilder->createIndex(COLOR_ROLE, COLORS, myFaceCount * 4);
                }

                for (unsigned j = 0; j < myUVSetNames.size(); ++j) {
                    myElementBuilder->createIndex(myUVSetNames[j], getTextureRole(j), myFaceCount * 4);
                }

                if (theHasSkinAndBonesFlag) {
                    myElementBuilder->createIndex(JOINT_WEIGHTS_ROLE_0, JOINT_WEIGHTS_0, myFaceCount * 4);
                    myElementBuilder->createIndex(JOINT_INDICES_ROLE_0, JOINT_INDICES_0, myFaceCount * 4);
                    myElementBuilder->createIndex(JOINT_WEIGHTS_ROLE_1, JOINT_WEIGHTS_1, myFaceCount * 4);
                    myElementBuilder->createIndex(JOINT_INDICES_ROLE_1, JOINT_INDICES_1, myFaceCount * 4);
                }
            } else {
                myElementBuilder = myElementIt->second;
            }

            myStatus = _myMesh->getPolygonVertices (i, myPositionIndexArray);
            if (myStatus == MStatus::kFailure) {
                throw ExportException(string("Could not get polygon vertices of shape: ") + string(_myMesh->name().asChar()),
                    "ShapeExporter::outputPrimitives()");
            }

            myStatus = _myMesh->getFaceNormalIds (i, myNormalIndexArray);
            if (myStatus == MStatus::kFailure) {
                throw ExportException(string("Could not get face normal id of shape: ") + string(_myMesh->name().asChar()),
                    "ShapeExporter::outputPrimitives()");
            }

            for (unsigned j = 0; j < myVertexCountPerFace; j++) {
                MAKE_SCOPE_TIMER(outputPrimitives_for_index_count);

                if (!_myColorIndices.empty()) {
                    int myColorIndex;
                    myStatus = _myMesh->getFaceVertexColorIndex(i, j, myColorIndex);
                    myElementBuilder->appendIndex(COLORS, _myColorIndices[myColorIndex]);
                }

                myElementBuilder->appendIndex(POSITIONS, myPositionIndexArray[j]);
                myElementBuilder->appendIndex(NORMALS, myNormalIndexArray[j]);


                // Export Skin And Bones Indices
                if (theHasSkinAndBonesFlag) {
                    myElementBuilder->appendIndex(JOINT_WEIGHTS_0, myPositionIndexArray[j]);
                    myElementBuilder->appendIndex(JOINT_INDICES_0, myPositionIndexArray[j]);
                    myElementBuilder->appendIndex(JOINT_WEIGHTS_1, myPositionIndexArray[j]);
                    myElementBuilder->appendIndex(JOINT_INDICES_1, myPositionIndexArray[j]);
                }

                // Export all uv sets references
                for (unsigned k = 0; k < myUVSetNames.size(); ++k) {
                    MString myUVSetName = myUVSetNames[k].c_str();

                    int myUVIndex = -1;
                    myStatus = _myMesh->getPolygonUVid(i, j, myUVIndex, & myUVSetName);

                    if (myStatus == MStatus::kFailure || myUVIndex == -1) {
                        if (!myUVSetWarningCount) {
                            displayWarning(string("Could not get uv coordiantes of shape: ") +
                                std::string(_myMesh->name().asChar()) +
                                " for uvset: " + std::string(myUVSetName.asChar()) +
                                " of material: " + myMaterialName);
                        }
                        ++myUVSetWarningCount;

                        // We need to provide an uv coordinate, otherwise the renderer will crash
                        if (theShapeBuilder.getVertexDataSize<Vector2f>(myUVSetName.asChar()) == 0) {
                            theShapeBuilder.appendVertexData(myUVSetName.asChar(), asl::Vector2f(0.f, 0.f));
                        }
                        myUVIndex = 0;
                    }

                    myElementBuilder->appendIndex(getTextureRole(k), myUVIndex);
                }
            }
        }
        if (myUVSetWarningCount > 1) {
            displayWarning(string("'Could not get uv coordiantes...' repeated for ") + as_string(myUVSetWarningCount) + " times ");
        }

        ElementBuilderMap::iterator myIt = myElementMap.begin();
        for (; myIt != myElementMap.end(); ++myIt) {
            theShapeBuilder.appendElements(*(myIt->second));
        }
    } catch (asl::Exception & ex) {
        displayError(ex.what());
        return false;
    }
    return true;
}

void
ShapeExporter::outputPositions(y60::ShapeBuilder & theShape) {
    MAKE_SCOPE_TIMER(ShapeExporter_outputPositions);
    unsigned myVertexCount = _myVertexArray.length();

    theShape.createVertexDataBin<Vector3f>(POSITION_ROLE, myVertexCount);

    if (myVertexCount == 0) {
        throw ExportException("No vertices found", "ShapeExporter::outputPositions()");
    }

    for (unsigned i = 0; i < myVertexCount; ++i) {
        convertToMeter(_myVertexArray[i]);
        theShape.appendVertexData(POSITION_ROLE,
                                  asl::Vector3f(float(_myVertexArray[i].x),
                                                float(_myVertexArray[i].y),
                                                float(_myVertexArray[i].z)));
    }
}

void
ShapeExporter::outputNormals(y60::ShapeBuilder & theShape) {
    MAKE_SCOPE_TIMER(ShapeExporter_outputNormals);
    unsigned myNormalCount = _myNormalArray.length();
    if (myNormalCount == 0) {
        throw ExportException("Normal count is zero", "ShapeExporter::outputNormals()");
    }
    theShape.createVertexDataBin<Vector3f>(NORMAL_ROLE, myNormalCount);
    for (unsigned i = 0; i < myNormalCount; ++i) {
        theShape.appendVertexData(NORMAL_ROLE, asl::Vector3f(_myNormalArray[i].x,
                                          _myNormalArray[i].y,
                                          _myNormalArray[i].z));
    }
}

void
ShapeExporter::outputColors(y60::ShapeBuilder & theShape) {
    MAKE_SCOPE_TIMER(ShapeExporter_outputColors);
    unsigned myColorCount = _myColorArray.length();
    // (VS) Maya7.0 does not export vertex colors or face vertexcolors (?), if none present
    //      y60 must live without them
    /* if (myColorCount == 0) {
        throw ExportException("Color count is zero", "ShapeExporter::outputColors()");
    } */

    theShape.createVertexDataBin<Vector4f>(COLOR_ROLE, myColorCount);

    // The first color value is the default color, all color indices are reset to this value
    theShape.appendVertexData(COLOR_ROLE, DEFAULT_VERTEX_COLOR);
    _myColorIndices.resize(myColorCount, 0);

    bool myColorsExistFlag = false;
    for (unsigned i = 0; i < myColorCount; ++i) {
        if (isValidColor(_myColorArray[i])) {
            _myColorIndices[i] = theShape.appendVertexData(COLOR_ROLE,
                asl::Vector4f(_myColorArray[i].r,
                              _myColorArray[i].g,
                              _myColorArray[i].b,
                              _myColorArray[i].a));
            myColorsExistFlag = true;
        }
    }

    if (!myColorsExistFlag) {
        _myColorIndices.resize(0);
    }
}

void
ShapeExporter::outputUVs(y60::ShapeBuilder & theShape) {
    MAKE_SCOPE_TIMER(ShapeExporter_outputUVs);

    // Get uv sets from maya
    MStringArray myUVSetNames;
    if (_myMesh->getUVSetNames(myUVSetNames) == MStatus::kFailure)  {
        throw ExportException("Could not get uv set names", "ShapeExporter::outputUVs()");
    }

    unsigned int myUVSetCount = myUVSetNames.length();
    for (unsigned i = 0; i < myUVSetCount; ++i) {
        // Retrive the uv set name
        MString myUVSetName = myUVSetNames[i];

        // Retrieve the uv values
        MFloatArray uArray;
        MFloatArray vArray;
        if (_myMesh->getUVs(uArray, vArray, & myUVSetName) == MStatus::kFailure) {
            throw ExportException("Could not get uv coords", "ShapeExporter::outputUVs()");
        }

        // Add uv values
        unsigned myUVCount = uArray.length();
        theShape.createVertexDataBin<Vector2f>(myUVSetName.asChar(), myUVCount);

        for (unsigned j = 0; j < myUVCount; ++j) {
            // invert v to match native OpenGL
            theShape.appendVertexData(myUVSetName.asChar(), asl::Vector2f(float(uArray[j]), (1.0f - float(vArray[j]))));
        }

        // Get associated textures
        MObjectArray myTextures;
        _myMesh->getAssociatedUVSetTextures(myUVSetName, myTextures);

        for (unsigned j = 0; j < myTextures.length(); ++j) {
            std::string myTextureName = std::string(MFnDependencyNode(myTextures[j]).name().asChar());
            DB(AC_TRACE << "examining texture " << myTextureName << " for " << myUVSetName.asChar() << endl);
            if (myTextures[j].hasFn(MFn::kFileTexture) ) {
                DB(AC_TRACE << "registering texture " << myTextureName << " for " << myUVSetName.asChar() << endl);
                _myTextureUVSetMap.insert(std::make_pair(myTextureName, myUVSetName.asChar()));
            }
        }
    }
}

bool
ShapeExporter::outputJointWeights(y60::ShapeBuilder & theShape) {
    MStatus myStatus;
    MObject mySkinObject;

    if (!findSkinCluster(*_myMesh, mySkinObject)) {
        return false;
    }

    MFnSkinCluster mySkinCluster(mySkinObject);

    // Get max influences
    MObject myMaxInfluencesAttribute = mySkinCluster.attribute("maxInfluences", &myStatus);
    checkMStatus(myStatus, "Failed to get max number of influences", PLUS_FILE_LINE);

    MPlug myMaxInfluencePlug = mySkinCluster.findPlug(myMaxInfluencesAttribute, &myStatus);
    checkMStatus(myStatus, "Failed to get plug for max number of influences", PLUS_FILE_LINE);

    float myMaxInfluences = 0;
    myMaxInfluencePlug.getValue(myMaxInfluences);

    if (myMaxInfluences > MAX_INFLUENCES) {
        throw ExportException(string("Skin mesh '") + _myMesh->name().asChar() +
                "' is influenced by more than four joints. Your must set max influence to four.", PLUS_FILE_LINE);
    }


    MItGeometry myIter(_myMesh->dagPath());
    MDagPathArray myInfluences;

    DB(
        unsigned int myInfluenceCount = mySkinCluster.influenceObjects(myInfluences, &myStatus);
        cerr << "Mesh with SkinCluster: " << _myMesh->name().asChar()
            << ", Vertex-Count: " << myIter.count() << ", Influence-Count: " << myInfluenceCount << endl;

        for (unsigned int i = 0; i < myInfluenceCount; ++i) {
            cerr << "Influence: '" << myInfluences[i].partialPathName().asChar() << "'" << endl;
        }
    )

    theShape.createVertexDataBin<Vector4f>(JOINT_WEIGHTS_ROLE_0, myIter.count());
    theShape.createVertexDataBin<Vector4f>(JOINT_INDICES_ROLE_0, myIter.count());
    theShape.createVertexDataBin<Vector4f>(JOINT_WEIGHTS_ROLE_1, myIter.count());
    theShape.createVertexDataBin<Vector4f>(JOINT_INDICES_ROLE_1, myIter.count());

    for ( ; !myIter.isDone(); myIter.next() ) {
        MObject myVertex = myIter.component(&myStatus);
        checkMStatus(myStatus, "Failed to get component",PLUS_FILE_LINE);

        // Get the weights for this vertex (one per influence object)
        MFloatArray myWeights;
        unsigned myWeightCount;
        myStatus = mySkinCluster.getWeights(_myMesh->dagPath(),myVertex,myWeights,myWeightCount);
        checkMStatus(myStatus,"Error getting weights.",PLUS_FILE_LINE);
        if (myWeightCount == 0) {
            throw ExportException(string("Error: 0 influence objects in skin mesh '")+_myMesh->name().asChar()+
                    "'", PLUS_FILE_LINE);
        }

        // Output the weight data for this vertex
        // get the four most significant influences
        priority_queue<JointWeight> mySortedWeights;

        DB(AC_TRACE << "==== Vertex " << myIter.index() << endl;)
        for (unsigned int i = 0; i < myWeightCount ; ++i ) {
            mySortedWeights.push(JointWeight(myWeights[i], i));
        }

        asl::Vector4f currentWeights;
        asl::Vector4f currentIndices;

        for (unsigned int i = 0; i < 4; ++i) {
            if (mySortedWeights.empty()) {
                DB(AC_TRACE << "0.0, -1" ;)
                currentWeights[i] = 0.0;
                currentIndices[i] = 0.0;   // the weight is zero anyway, so the index doesn't matter
            } else {
                const JointWeight & myJointWeight = mySortedWeights.top();
                DB(AC_TRACE << myJointWeight.weight << ", " << myJointWeight.index;)
                currentWeights[i] = myJointWeight.weight;
                currentIndices[i] = float(myJointWeight.index);
                mySortedWeights.pop();
            }
            DB(AC_TRACE << "| ";)
        }

        theShape.appendVertexData(JOINT_WEIGHTS_ROLE_0, currentWeights);
        theShape.appendVertexData(JOINT_INDICES_ROLE_0, currentIndices);

        for (unsigned int i = 0; i < 4; ++i) {
            if (mySortedWeights.empty()) {
                DB(AC_TRACE << "0.0, -1" ;)
                currentWeights[i] = 0.0;
                currentIndices[i] = 0.0;   // the weight is zero anyway, so the index doesn't matter
            } else {
                const JointWeight & myJointWeight = mySortedWeights.top();
                DB(AC_TRACE << myJointWeight.weight << ", " << myJointWeight.index;)
                currentWeights[i] = myJointWeight.weight;
                currentIndices[i] = float(myJointWeight.index);
                mySortedWeights.pop();
            }
            DB(AC_TRACE << "| ";)
        }

        theShape.appendVertexData(JOINT_WEIGHTS_ROLE_1, currentWeights);
        theShape.appendVertexData(JOINT_INDICES_ROLE_1, currentIndices);

        if (!mySortedWeights.empty()) {
            float myNextWeight = mySortedWeights.top().weight;
            if (myNextWeight > 0.02) {
                throw ExportException(string("Vertex is influenced by more than 8 joints in skin mesh '")+_myMesh->name().asChar()+
                    "'", PLUS_FILE_LINE);
            } else if (myNextWeight > 0.005) {
                displayWarning(string("Vertex is influenced by more than 8 joints in skin mesh '") +
                    _myMesh->name().asChar() + "'. Weight is:" + as_string(myNextWeight));
            }
        }

        DB(AC_TRACE << endl;)
    }
    return true;
}


