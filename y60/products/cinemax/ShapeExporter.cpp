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
//   $Author: valentin $
//   $Revision: 1.32 $
//   $Date: 2005/04/29 15:07:34 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#include "ShapeExporter.h"

#include <c4d.h>

#include <asl/math/GeometryUtils.h>
#include <asl/math/numeric_functions.h>

#include <y60/scene/ElementBuilder.h>
#include <y60/scene/SceneBuilder.h>
#include <y60/scene/ShapeBuilder.h>

#include "CinemaHelpers.h"

using namespace asl;
using namespace y60;
using namespace std;

ShapeExporter::ShapeExporter(SceneBuilder & theSceneBuilder, MaterialExporter & theMaterialExporter) :
    _mySceneBuilder(theSceneBuilder),
    _myMaterialExporter(theMaterialExporter)
{}

std::vector<std::string>
ShapeExporter::writeShape(BaseObject * theNode, BaseObject * thePolygonNode, bool theForceFrontBackFacing)
{
    std::vector<std::string> myShapeIdList;
    StatusSetText("Exporting shape: " + getTreeName(theNode));

    // collect SelectionTags
    std::vector<SelectionTag*> mySelectionList;
    SelectionTag * mySelectionTag = 0;
    for (LONG si = 0; (mySelectionTag = static_cast<SelectionTag*>(thePolygonNode->GetTag(Tpolygonselection, si))); ++si) {
        mySelectionList.push_back(mySelectionTag);
    }

    // create one selection that holds all the polygons
    PolygonObject * myPolygon = ToPoly(thePolygonNode);
    LONG myPolygonCount  = myPolygon->GetPolygonCount();

    SelectionTag * myBaseSelection = SelectionTag::Alloc(Tpolygonselection);
    myBaseSelection->SetName("_");
    myBaseSelection->GetBaseSelect()->SelectAll(0, myPolygonCount-1);

    // export all selections
    // iterate over selection, collect all textures that use this selection
    TextureList myMaterialList;
    UVTagList myUVTagList;
    for (std::vector<SelectionTag*>::size_type si = 0; si < mySelectionList.size(); ++si) {

        myMaterialList.clear();
        myUVTagList.clear();

        SelectionTag * mySelection = mySelectionList[si];

        // collect all textures that use this selection
        TextureTag * myTextureTag = 0;
        LONG myUVIndex = 0;
        for (LONG i = 0; (myTextureTag = static_cast<TextureTag*>(thePolygonNode->GetTag(Ttexture, i))); ++i) {
#if 1
            GeData myData;
            myTextureTag->GetParameter(DescID(TEXTURETAG_RESTRICTION), myData, 0);
            String myRestriction = myData.GetString();
#else
            // UH: this is supposed to work but doesn't
            String myRestriction = myTextureTag->GetDataInstance()->GetString(TEXTURETAG_RESTRICTION);
#endif

            if (mySelectionList[si]->GetName() == myRestriction) {
                UVWTag * myUvTag = static_cast<UVWTag*>(thePolygonNode->GetTag(Tuvw, myUVIndex));
                if (!myUvTag) {
                    myUvTag = GenerateUVW(theNode, theNode->GetMg(),myTextureTag, myTextureTag->GetMl(), NULL);
                }
                myUVTagList.push_back(myUvTag);
                myUVIndex++;
                myMaterialList.push_back(std::pair<TextureTag*,UVWTag*>(myTextureTag, myUvTag));
            }
        }

        // write selection
        std::string myShapeId = writeSelection(theNode, thePolygonNode,
                                                mySelection, myBaseSelection, myMaterialList, myUVTagList,
                                                theForceFrontBackFacing);
        if (myShapeId.empty() == false) {
            myShapeIdList.push_back(myShapeId);
        }
    }

    // export baseSelection
    // Collect all textures
    myMaterialList.clear();
    myUVTagList.clear();
    TextureTag * myTextureTag = 0;
    for (LONG i = 0; (myTextureTag = static_cast<TextureTag*>(thePolygonNode->GetTag(Ttexture, i))); ++i) {
        GeData myData;
        myTextureTag->GetParameter(DescID(TEXTURETAG_RESTRICTION), myData, 0);
        String myRestriction = myData.GetString();
        // do not take account for material, that are used in other selections
        if (myBaseSelection->GetName() == myRestriction ||  myRestriction == "") {
            UVWTag * myUvTag = static_cast<UVWTag*>(thePolygonNode->GetTag(Tuvw, i));
            if (!myUvTag) {
                myUvTag = GenerateUVW(theNode, theNode->GetMg(),myTextureTag, myTextureTag->GetMl(), NULL);
            }
            myUVTagList.push_back(myUvTag);
            myMaterialList.push_back(std::pair<TextureTag*,UVWTag*>(myTextureTag, myUvTag));
        }
    }

    // if no material is found, search above the node
    if (myMaterialList.size() == 0) {
        TextureTag * myParentTextureTag = 0;
        BaseObject * myParentNode = theNode->GetUp();
        while(myParentNode && !myParentTextureTag) {
            myParentTextureTag = static_cast<TextureTag*>(myParentNode->GetTag(Ttexture, 0));
            myParentNode = myParentNode->GetUp();
        }
        if (myParentTextureTag) {
            UVWTag * myUvTag = static_cast<UVWTag*>(thePolygonNode->GetTag(Tuvw, 0));
            if (!myUvTag) {
                myUvTag = GenerateUVW(theNode, theNode->GetMg(),myParentTextureTag, myParentTextureTag->GetMl(), NULL);
            }
            myUVTagList.push_back(myUvTag);
            myMaterialList.push_back(std::pair<TextureTag*,UVWTag*>(myParentTextureTag, myUvTag));
        }
    }

    if (myBaseSelection->GetBaseSelect()->GetCount() ) {
        std::string myShapeId = writeSelection(myPolygon, thePolygonNode,
                                        myBaseSelection, 0, myMaterialList, myUVTagList,
                                        theForceFrontBackFacing);

        SelectionTag::Free(myBaseSelection);
        if (myShapeId.empty() == false) {
            myShapeIdList.push_back(myShapeId);
        }
    }

    return myShapeIdList;
}


void
ShapeExporter::findExtrema(asl::Vector3f & theVector, const Vector & theVertex, bool theMinFlag) {
    if (theMinFlag) {
        theVector[0] = theVertex.x < theVector[0] ? theVertex.x : theVector[0];
        theVector[1] = theVertex.y < theVector[1] ? theVertex.y : theVector[1];
        theVector[2] = theVertex.z < theVector[2] ? theVertex.z : theVector[2];
    } else {
        theVector[0] = theVertex.x > theVector[0] ? theVertex.x : theVector[0];
        theVector[1] = theVertex.y > theVector[1] ? theVertex.y : theVector[1];
        theVector[2] = theVertex.z > theVector[2] ? theVertex.z : theVector[2];
    }
}


std::string ShapeExporter::writeSelection(BaseObject * theNode, BaseObject * thePolygonNode,
                                          SelectionTag * theSelection,
                                          SelectionTag * theBaseSelection,
                                          const TextureList & theMaterialList,
                                          const UVTagList & theUVTagList,
                                          bool theForceFrontBackFacing)
{
    LONG myMaterialCount       = theMaterialList.size();
    LONG myUVSetCount          = theUVTagList.size();
    std::string myName = getString(theNode->GetName() + "_" + theSelection->GetName());

    PolygonObject * myPolygon = ToPoly(thePolygonNode);
#if API_VERSION >= 11000
    const Vector  * myVertices      = myPolygon->GetPointR();
    const CPolygon * myPolygons      = myPolygon->GetPolygonR();
#elif API_VERSION >= 9800
    const Vector  * myVertices      = myPolygon->GetPointR();
    const Polygon * myPolygons      = myPolygon->GetPolygonR();
#else
    Vector  * myVertices      = myPolygon->GetPoint();
    Polygon * myPolygons      = myPolygon->GetPolygon();
#endif
    // ShapeBuilder
    y60::ShapeBuilder myShapeBuilder(myName);
    myShapeBuilder.createVertexDataBin<Vector3f>(POSITION_ROLE, 1);
    myShapeBuilder.createVertexDataBin<Vector3f>(NORMAL_ROLE, 1);

    // Material
     ExportedMaterialInfo myMaterialInfo = _myMaterialExporter.initiateExport(thePolygonNode, theMaterialList, _mySceneBuilder);
//    y60::MaterialBuilderPtr myMaterialBuilder = _myMaterialExporter.writeMaterial(thePolygonNode, theMaterialList, _mySceneBuilder);
//    ExportedMaterialInfo myMaterialInfo = _myMaterialExporter.writeMaterial(thePolygonNode, theMaterialList, _mySceneBuilder);
//    LONG myY60TextureCount = myMaterialInfo._myTextureCount;
    LONG myY60TextureCount = myMaterialInfo._myTextureCount;

    // ElementBuilder
    ElementBuilder myElementBuilder(y60::PRIMITIVE_TYPE_TRIANGLES, myMaterialInfo._myMaterialId);
    //ElementBuilder myElementBuilder(y60::PRIMITIVE_TYPE_TRIANGLES, myMaterialBuilder->getId());
    myElementBuilder.createIndex(POSITION_ROLE, POSITIONS, 1);
    myElementBuilder.createIndex(NORMAL_ROLE, NORMALS, 1);

    // Color
    ObjectColorProperties myColorProperties;
    myPolygon->GetColorProperties(&myColorProperties);
    int myColorIndex = -1;
    if (myColorProperties.usecolor != ID_BASEOBJECT_USECOLOR_OFF) {
        Vector myColor = myColorProperties.color;
        myShapeBuilder.createVertexDataBin<Vector4f>(COLOR_ROLE);
        myColorIndex = myShapeBuilder.appendVertexData(COLOR_ROLE, Vector4f(myColor.x, myColor.y, myColor.z, 1.0));
        myElementBuilder.createIndex(COLOR_ROLE, COLORS, 1);
    }

    // NormalBuilder
    asl::VertexNormalBuilder<float> myVertexNormalBuilder;
    BaseTag * myPhongTag = thePolygonNode->GetTag(Tphong);
    float mySmoothingAngle = asl::PI;
    if (myPhongTag) {
        setupNormalBuilder(myVertexNormalBuilder, theNode);

        BaseContainer * myPhongContainer = myPhongTag->GetDataInstance();
        if (myPhongContainer->GetBool(PHONGTAG_PHONG_ANGLELIMIT)) {
            mySmoothingAngle = myPhongContainer->GetReal(PHONGTAG_PHONG_ANGLE);
        }

        Bool myIgnoreSelectedEdges = myPhongContainer->GetBool(PHONGTAG_PHONG_USEEDGES);
        if (myIgnoreSelectedEdges) {
            // XXX collect edges to ignore
            BaseSelect * myIgnorePhongEdges = myPolygon->GetPhongBreak();
            long myEdges = myIgnorePhongEdges->GetSegments();
        }
    }

    map<string,bool> myCreatedUVBins;
    // create uvsets
    LONG myUVSetIndex = 0;
    for (; myUVSetIndex < myUVSetCount; ++myUVSetIndex) {
        if (myMaterialInfo._myTexureMapping.size() > static_cast<std::size_t>(myUVSetIndex)
         && myMaterialInfo._myTexureMapping[myUVSetIndex] == UV_MAP) {
            std::string myBinName = std::string("UVCoords_") + asl::as_string(myUVSetIndex);
            GePrint(String(myName.c_str()) + ": Creating bin=" + String(myBinName.c_str()));
            myShapeBuilder.createVertexDataBin<Vector2f>(myBinName, 1);
            myElementBuilder.createIndex(myBinName, getTextureRole(myUVSetIndex), 1);
            myCreatedUVBins[myBinName] = true;
            GePrint(String(myName.c_str()) + ": Creating index bin=" + String(myBinName.c_str()) + " myUVTagIndex=" + LongToString(myUVSetIndex));
        }
    }

    // Front/back facing
    bool myShapeIsFrontSide = false;
    bool myShapeIsBackSide = false;
    if (theForceFrontBackFacing == false) {
        for (LONG myMaterialIndex = 0; myMaterialIndex < myMaterialCount; ++myMaterialIndex) {

            // Get texture side
            GeData myData;
#if 1
            theMaterialList[myMaterialIndex].first->GetParameter(DescID(TEXTURETAG_SIDE), myData, 0);
#else
            const DescLevel myRenderFace(TEXTURETAG_SIDE);
            const DescID myRenderFaceId(myRenderFace);
            theMaterialList[myMaterialIndex].first->GetParameter(myRenderFaceId, myData, 0);
#endif
            switch (myData.GetLong()) {
                case TEXTURETAG_SIDE_FRONTANDBACK:
                    myShapeIsFrontSide = myShapeIsBackSide = true;
                    break;
                case TEXTURETAG_SIDE_FRONT:
                    myShapeIsFrontSide = true;
                    break;
                case TEXTURETAG_SIDE_BACK:
                    myShapeIsBackSide = true;
                    break;
            };
        }
    }
    if (!myShapeIsFrontSide && !myShapeIsBackSide) {
        myShapeIsFrontSide = true;
        myShapeIsBackSide = true;
    }
    myShapeBuilder.setFrontFacing(myShapeIsFrontSide);
    myShapeBuilder.setBackFacing(myShapeIsBackSide);


    // Create UV indices sets
    for (LONG myTextureIndex = 0; myTextureIndex < myY60TextureCount; ++myTextureIndex) {
        std::string myBinName = std::string("UVCoords_") + asl::as_string(myTextureIndex);
        if (myMaterialInfo._myTexureMapping.size() > static_cast<std::size_t>(myUVSetIndex)
         && myMaterialInfo._myTexureMapping[myTextureIndex] == UV_MAP &&
            myCreatedUVBins.find(myBinName) == myCreatedUVBins.end())
        {


            // do we have enough uvsets for our Y60 textures, if not,take the first uv set
            if (myUVSetCount < myY60TextureCount) {
                myBinName = "UVCoords_0";
            }
            GePrint(String(myName.c_str()) + ": Creating index bin=" + String(myBinName.c_str()) +
                                            " myTexIndex=" + LongToString(myTextureIndex));
            myElementBuilder.createIndex(myBinName, getTextureRole(myTextureIndex), 1);
            myCreatedUVBins[myBinName] = true;

        }
    }

    asl::Vector3f myMinCoord(asl::NumericTraits<float>::max(), asl::NumericTraits<float>::max(), asl::NumericTraits<float>::max());
    asl::Vector3f myMaxCoord(-asl::NumericTraits<float>::max(), -asl::NumericTraits<float>::max(), -asl::NumericTraits<float>::max());

    BaseSelect * myBaseSelect = theSelection->GetBaseSelect();
    LONG myPolyFrom, myPolyTo;
    LONG mySelectionIndex = 0;
    myBaseSelect->GetRange(mySelectionIndex, &myPolyFrom, &myPolyTo);
    //GePrint(String(myName.c_str()) + " from=" + LongToString(myPolyFrom) + " to=" + LongToString(myPolyTo));

    bool myVerticesFound = false;
    for (mySelectionIndex = 0; myBaseSelect->GetRange(mySelectionIndex, &myPolyFrom, &myPolyTo); ++mySelectionIndex) {
        for (LONG myPolyIndex = myPolyFrom; myPolyIndex <= myPolyTo; ++myPolyIndex) {
            myVerticesFound = true;
            if (theBaseSelection) {
                // do not export polygons in baseobejct that are used in selections
                theBaseSelection->GetBaseSelect()->Deselect(myPolyIndex);
            }
            LONG a = myPolygons[myPolyIndex].a;
            LONG b = myPolygons[myPolyIndex].b;
            LONG c = myPolygons[myPolyIndex].c;
            LONG d = myPolygons[myPolyIndex].d;

            Vector myVertexA = myVertices[a];
            Vector myVertexB = myVertices[b];
            Vector myVertexC = myVertices[c];

            // find the minimum of all vertices
            findExtrema(myMinCoord, myVertexA, true);
            findExtrema(myMinCoord, myVertexB, true);
            findExtrema(myMinCoord, myVertexC, true);

            findExtrema(myMaxCoord, myVertexA, false);
            findExtrema(myMaxCoord, myVertexB, false);
            findExtrema(myMaxCoord, myVertexC, false);

            // Vertex data
            int myVertexIndexA, myVertexIndexB, myVertexIndexC;
            myVertexIndexA = myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(myVertexA.x, myVertexA.y, (-1)*myVertexA.z));
            myVertexIndexB = myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(myVertexB.x, myVertexB.y, (-1)*myVertexB.z));
            myVertexIndexC = myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(myVertexC.x, myVertexC.y, (-1)*myVertexC.z));

            // Normals
            asl::Vector3f myFaceNormal = asl::generateFaceNormal(Vector3f(myVertexA.x, myVertexA.y, (-1)*myVertexA.z),
                                                                 Vector3f(myVertexB.x, myVertexB.y, (-1)*myVertexB.z),
                                                                 Vector3f(myVertexC.x, myVertexC.y, (-1)*myVertexC.z));

            int myNormalIndexA, myNormalIndexB, myNormalIndexC;
            if (myPhongTag) {
                myNormalIndexA = appendVertexNormal(myVertexNormalBuilder, myShapeBuilder,
                                                    myFaceNormal, a, mySmoothingAngle);
                myNormalIndexB = appendVertexNormal(myVertexNormalBuilder, myShapeBuilder,
                                                    myFaceNormal, b, mySmoothingAngle);
                myNormalIndexC = appendVertexNormal(myVertexNormalBuilder, myShapeBuilder,
                                                    myFaceNormal, c, mySmoothingAngle);
            } else {
                myNormalIndexA = myShapeBuilder.appendVertexData(NORMAL_ROLE, myFaceNormal);
                myNormalIndexB = myNormalIndexA;
                myNormalIndexC = myNormalIndexA;
            }

            // Export texture coordinates
            if (theUVTagList.size() == 0 && myY60TextureCount > 0) {
                throw ExportException(std::string("Sorry, shape: ") + string(getString(thePolygonNode->GetName())) +
                                     string(" does not have any uvsets, but textures: ") +
                        std::string(asl::as_string(myY60TextureCount)), PLUS_FILE_LINE);
            }
            vector<int> myUVIndexA;
            vector<int> myUVIndexB;
            vector<int> myUVIndexC;
            for (LONG myTexIndex = 0; myTexIndex < myY60TextureCount; ++myTexIndex) {
                if (myMaterialInfo._myTexureMapping[myTexIndex] == UV_MAP) {
                    int myUVSetIndex = myTexIndex;
                    std::string myBinName = std::string("UVCoords_") + asl::as_string(myTexIndex);
                    // do we have enough uvsets for our Y60 textures, if not,take the first uv set
                    if (myUVSetCount < myY60TextureCount) {
                        myBinName = "UVCoords_0";
                        myUVSetIndex = 0;
                    }

                    UVWTag * myUvTag = theUVTagList[myUVSetIndex];

                    if (myUvTag) {
                        UVWStruct myUVCoords = myUvTag->Get(myPolyIndex);
                        myUVIndexA.push_back(myShapeBuilder.appendVertexData(myBinName, asl::Vector2f(myUVCoords.a.x, myUVCoords.a.y)));
                        myUVIndexB.push_back(myShapeBuilder.appendVertexData(myBinName, asl::Vector2f(myUVCoords.b.x, myUVCoords.b.y)));
                        myUVIndexC.push_back(myShapeBuilder.appendVertexData(myBinName, asl::Vector2f(myUVCoords.c.x, myUVCoords.c.y)));
                    } else {
                        throw ExportException(std::string("Could not get uvset for texture: ") +
                            std::string(asl::as_string(myTexIndex)), PLUS_FILE_LINE);
                    }
                }
            }

             // Export vertices
            appendVertexIndices(myElementBuilder, myVertexIndexA, myColorIndex, myNormalIndexA, myUVIndexA, myY60TextureCount);
            appendVertexIndices(myElementBuilder, myVertexIndexC, myColorIndex, myNormalIndexC, myUVIndexC, myY60TextureCount);
            appendVertexIndices(myElementBuilder, myVertexIndexB, myColorIndex, myNormalIndexB, myUVIndexB, myY60TextureCount);

            // Second triangle in case of quads
            if (c != d) {
                Vector myVertexD = myVertices[d];
                int myVertexIndexD, myNormalIndexD = -1;
                vector<int> myUVIndexD;

                myVertexIndexD = myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(myVertexD.x, myVertexD.y, (-1)*myVertexD.z));

                findExtrema(myMinCoord, myVertexD, true);
                findExtrema(myMaxCoord, myVertexD, false);

                if (myPhongTag) {
                    myNormalIndexD = appendVertexNormal(myVertexNormalBuilder, myShapeBuilder,
                                                        myFaceNormal, d, mySmoothingAngle);
                } else {
                    myNormalIndexD = myNormalIndexA;
                }

                for (LONG myTexIndex = 0; myTexIndex < myY60TextureCount; ++myTexIndex) {
                    if (myMaterialInfo._myTexureMapping[myTexIndex] == UV_MAP) {
                        int myUVSetIndex = myTexIndex;
                        std::string myBinName = std::string("UVCoords_") + asl::as_string(myTexIndex);
                        // do we have enough uvsets for our Y60 textures, if not,take the first uv set
                        if (myUVSetCount < myY60TextureCount) {
                            myBinName = "UVCoords_0";
                            myUVSetIndex = 0;
                        }

                        UVWTag * myUvTag = theUVTagList[myUVSetIndex];
                        if (myUvTag) {
                            UVWStruct myUVCoords = myUvTag->Get(myPolyIndex);
                            myUVIndexD.push_back(myShapeBuilder.appendVertexData(myBinName, asl::Vector2f(myUVCoords.d.x, myUVCoords.d.y)));
                        } else {
                            throw ExportException(std::string("Could not get uvset for texture: ") +
                                                std::string(asl::as_string(myTexIndex)), PLUS_FILE_LINE);
                        }
                    }
                }

                appendVertexIndices(myElementBuilder, myVertexIndexA, myColorIndex, myNormalIndexA, myUVIndexA, myY60TextureCount);
                appendVertexIndices(myElementBuilder, myVertexIndexD, myColorIndex, myNormalIndexD, myUVIndexD, myY60TextureCount);
                appendVertexIndices(myElementBuilder, myVertexIndexC, myColorIndex, myNormalIndexC, myUVIndexC, myY60TextureCount);
            }
        }
    }
    if (!myVerticesFound) {
        return "";
    }
    _myMaterialExporter.writeMaterial(myMaterialInfo, thePolygonNode,
                                      theMaterialList, _mySceneBuilder,
                                      myMinCoord, myMaxCoord);

    myShapeBuilder.appendElements(myElementBuilder);
    StatusSetText(String("Append shape: ") + theNode->GetName());
    return _mySceneBuilder.appendShape(myShapeBuilder);
}

void
ShapeExporter::setupNormalBuilder(asl::VertexNormalBuilder<float> & theVertexNormalBuilder,
                                  BaseObject * theNode)
{
    PolygonObject * myPolygon = ToPoly(theNode);

    LONG      myVertexCount   = myPolygon->GetPointCount();
#if API_VERSION >= 11000
    const Vector  * myVertices      = myPolygon->GetPointR();
    const CPolygon * myPolygons      = myPolygon->GetPolygonR();
#elif API_VERSION >= 9800
    const Polygon * myPolygons      = myPolygon->GetPolygonR();
    const Vector  * myVertices      = myPolygon->GetPointR();
#else
    Polygon * myPolygons      = myPolygon->GetPolygon();
    Vector  * myVertices      = myPolygon->GetPoint();
#endif
    LONG      myPolygonCount  = myPolygon->GetPolygonCount();

    for (LONG i = 0; i < myVertexCount; ++i) {
        theVertexNormalBuilder.addVertex(Vector3f(myVertices[i].x, myVertices[i].y, (-1) * myVertices[i].z));
    }

    for (LONG i = 0; i < myPolygonCount; ++i) {
        theVertexNormalBuilder.addFace(myPolygons[i].a, myPolygons[i].b, myPolygons[i].c);
        if (myPolygons[i].c != myPolygons[i].d) {
            theVertexNormalBuilder.addFace(myPolygons[i].a, myPolygons[i].c, myPolygons[i].d);
        }
    }
}

unsigned
ShapeExporter::appendVertexNormal(VertexNormalBuilder<float> & theVertexNormalBuilder,
                                  ShapeBuilder & theShapeBuilder,
                                  const Vector3f & theFaceNormal,
                                  long theVertexIndex, float theSmoothingAngle)
{
    return theShapeBuilder.appendVertexData(NORMAL_ROLE,
        theVertexNormalBuilder.getVertexNormal(theFaceNormal, theVertexIndex, theSmoothingAngle));
}

void
ShapeExporter::appendVertexIndices(ElementBuilder & theElementBuilder,
                                   int thePositionIndex,
                                   int theColorIndex,
                                   int theNormalIndex,
                                   std::vector<int> theUVCoordIndex,
                                   int theRealTextureCount)
{
    theElementBuilder.appendIndex(POSITIONS, thePositionIndex);
    theElementBuilder.appendIndex(NORMALS, theNormalIndex);
    if (theColorIndex >= 0) {
        theElementBuilder.appendIndex(COLORS, theColorIndex);
    }

    for (LONG i = 0; i < theRealTextureCount; ++i) {
        if (theRealTextureCount == theUVCoordIndex.size()) {
            // we have UV sets for all textures
            theElementBuilder.appendIndex(getTextureRole(i), theUVCoordIndex[i]);
        } else {
            // share the first UV set that we have
            if (!theUVCoordIndex.empty()) {
                theElementBuilder.appendIndex(getTextureRole(i), theUVCoordIndex[0]);
            }
        }
    }
}
