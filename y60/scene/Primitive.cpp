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
//   $RCSfile: Primitive.cpp,v $
//   $Author: pavel $
//   $Revision: 1.61 $
//   $Date: 2005/04/24 00:41:21 $
//
//  Description:
//
//=============================================================================

// own header
#include "Primitive.h"

#include "Shape.h"
#include "Scene.h"

#include <y60/base/NodeNames.h>
#include <y60/base/VertexDataRoles.h>
#include <y60/base/NodeValueNames.h>
#include <asl/base/string_functions.h>
#include <asl/math/Triangle.h>
#include <asl/math/linearAlgebra.h>
#include <asl/math/intersection.h>
#include <asl/base/Dashboard.h>
#include "ResourceManager.h"

using namespace asl;
using namespace std;

#define DB(x) //x

namespace y60 {

    DEFINE_EXCEPTION(PrimitiveException, asl::Exception);
    DEFINE_EXCEPTION(UnknownPrimitiveType, PrimitiveException);
#if 0
    Primitive::Primitive(PrimitiveType theType, MaterialBasePtr theMaterial,
                         const std::string & theShapeId, unsigned int theDomIndex)
        : _myType(theType),
          _myShapeId(theShapeId),
          _myMaterial(theMaterial),
          _myVertexData(MAX_VERTEX_DATA_ROLE, VertexDataBasePtr(0)),
          _myDomIndex(theDomIndex)
    {}
#endif
    Primitive::Primitive(dom::Node & theNode) :
                 dom::Facade(theNode),
                MaterialIdTag::Plug(theNode),
                PrimitiveTypeTag::Plug(theNode),
                RenderStylesTag::Plug(theNode),
                _myVertexData(MAX_VERTEX_DATA_ROLE, VertexDataBasePtr())
    { }

    Primitive::~Primitive() {
        AC_TRACE << "~Primitive()" << this << endl;
    }

    Shape & Primitive::getShape() {
        return *getNode().parentNode()->parentNode()->getFacade<Shape>();
    }
    const Shape & Primitive::getShape() const {
        return *getNode().parentNode()->parentNode()->getFacade<Shape>();
    }

    void
    Primitive::load(ResourceManager* theResourceManager, dom::NodePtr theIndicesNode, dom::NodePtr theDataNode)
    {
        const VectorOfUnsignedInt & myIndices = theIndicesNode->
            childNode(0)->nodeValueRef<VectorOfUnsignedInt>();

        if (myIndices.size() > 1024*64) {
            AC_DEBUG << "Primitive larger than 64k vertices, may cause problems on some graphics cards";
        }


        const dom::NodePtr myDataNode = theDataNode->childNode(0);
        if (!myDataNode) {
            throw PrimitiveException(std::string("Vertex data ") + myDataNode->getAttributeString(NAME_ATTRIB) +
                           " does not contain data.", PLUS_FILE_LINE);
        }

        const string & myRoleString = theIndicesNode->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
        VertexDataRole myRole = VertexDataRole(getEnumFromString(myRoleString, VertexDataRoleString));

        TypeId myTypeId;
        myTypeId.fromString(theDataNode->nodeName());

        VertexBufferUsage myUsage = getDefaultVertexBufferUsage();
        if (theDataNode->getAttribute(VERTEX_BUFFER_USAGE_ATTRIB)) {
            myUsage = theDataNode->getAttributeValue<VertexBufferUsage>(VERTEX_BUFFER_USAGE_ATTRIB);
        }
        AC_DEBUG << "Primitive::load " << theDataNode->parentNode()->parentNode()->getAttributeString(NAME_ATTRIB)
                 << ", usage:" << myUsage << " for role: " << myRoleString;

        VertexDataBasePtr myVertexData = createVertexDataBin(theResourceManager, myTypeId, myRole, myUsage);
        if (myVertexData) {
            DB(AC_TRACE << "uploading Vertex Data role=" << myRole);
            myVertexData->load(myIndices, myDataNode);
        }
    }
    void
    Primitive::unload(dom::NodePtr theIndicesNode, dom::NodePtr theDataNode)
    {
        dom::Node::WritableValue<VectorOfUnsignedInt> myIndicesLock(theIndicesNode->childNode(0));
        VectorOfUnsignedInt & myIndices = myIndicesLock.get();

        const dom::NodePtr myDataNode = theDataNode->childNode(0);
        if (!myDataNode) {
            throw PrimitiveException(std::string("Vertex data ") + myDataNode->getAttributeString(NAME_ATTRIB) +
                           " does not contain data.", PLUS_FILE_LINE);
        }

        const string & myRoleString = theIndicesNode->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
        VertexDataRole myRole = VertexDataRole(getEnumFromString(myRoleString, VertexDataRoleString));


        VertexDataBasePtr myDataBin = _myVertexData[myRole];
        if (myDataBin) {
            if (myDataBin->size() != myIndices.size()) {
                AC_ERROR << "Primitive::unload: gl vertex data and dom index array have different size, vertex data role "<<myRoleString<<
                    " has size "<<myDataBin->size()<<", index array has size "<< myIndices.size()<<", shape id ="<< getShape().Shape::get<IdTag>()<<endl;
                return;
            } else {
                myDataBin->unload(myIndices, myDataNode);
            }
        }

        //const VertexDataBase & myVertexData = getVertexData(myRole);
        //if (myVertexData.size() != myIndices.size()) {
        //    AC_ERROR << "Primitive::unload: gl vertex data and dom index array have different size, vertex data role "<<myRoleString<<
        //        " has size "<<myVertexData.size()<<", index array has size "<< myIndices.size()<<", shape id ="<<_myShapeId<<endl;
        //    return;
        //}
        //myVertexData.unload(myIndices, myDataNode, theBeginIndex, theEndIndex);
    }

    VertexDataBasePtr
    Primitive::createVertexDataBin(ResourceManager* theResourceManager, TypeId theBinType, VertexDataRole theRole,
                                   const VertexBufferUsage & theUsage)
   {
        switch (theBinType) {
            case VECTOR_OF_VECTOR2F:
                if (theResourceManager) {
                    _myVertexData[theRole] = theResourceManager->getVertexDataFactory2f().create(theUsage);
                } else {
                    // XXX Workaround for non-existent ResourceManager
                    _myVertexData[theRole] = asl::Ptr<VertexData2f>();
                }
                break;
            case VECTOR_OF_VECTOR3F:
                if (theResourceManager) {
                    _myVertexData[theRole] = theResourceManager->getVertexDataFactory3f().create(theUsage);
                } else {
                    // XXX Workaround for non-existent ResourceManager
                    _myVertexData[theRole] = asl::Ptr<VertexData3f>();
                }
                break;
            case VECTOR_OF_VECTOR4F:
                if (theResourceManager) {
                    _myVertexData[theRole] = theResourceManager->getVertexDataFactory4f().create(theUsage);
                } else {
                    // XXX Workaround for non-existent ResourceManager
                    _myVertexData[theRole] = asl::Ptr<VertexData4f>();
                }
                break;
            case BOOL:
            case INT:
            case VECTOR2I:
            case VECTOR3I:
            case VECTOR4I:
            case VECTOR_OF_VECTOR2I:
            case FLOAT:
            case STRING:
                throw NotYetImplemented(string("Can not create uvset with type ") +
                                     getStringFromEnum(theBinType, TypeIdStrings), PLUS_FILE_LINE);
                break;
            default:
                throw PrimitiveException(std::string("Can not create vertex data bin. Type '") +
                    getStringFromEnum(theBinType, TypeIdStrings) + "' is unknown", PLUS_FILE_LINE);
        }
        return _myVertexData[theRole];
    }

    unsigned
    Primitive::size() const {
        VertexDataBasePtr myDataBin = _myVertexData[POSITIONS];
        if (myDataBin) {
            return myDataBin->size();
        } else {
            return 0;
        }
    }

    bool
    Primitive::hasVertexData(VertexDataRole theRole) const {
        return _myVertexData.size() > static_cast<std::vector<VertexDataBasePtr>::size_type>(theRole && _myVertexData[theRole]);
    }

    const VertexDataBasePtr
    Primitive::getVertexDataPtr(VertexDataRole theRole) const {
        VertexDataBasePtr myDataBin = _myVertexData[theRole];
        return myDataBin;
    }

    void
    Primitive::updateVertexData(const std::set<y60::VertexDataRole>& theRolesToUpdate) {
        Shape & myShape = getShape();
        dom::Node & myElementsNode = getNode();
        ResourceManager * myResourceManager = myShape.getScene().getTextureManager()->getResourceManager();
        unsigned myIndicesCount = myElementsNode.childNodesLength(VERTEX_INDICES_NAME);
        for (unsigned k = 0; k < myIndicesCount; ++k) {
            dom::NodePtr myIndicesNode = myElementsNode.childNode(VERTEX_INDICES_NAME, k);
            const string & myName = myIndicesNode->getAttributeString(VERTEX_DATA_ATTRIB);
            const string & myRoleString = myIndicesNode->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
            VertexDataRole myRole = VertexDataRole(getEnumFromString(myRoleString, VertexDataRoleString));
            if(theRolesToUpdate.empty() || theRolesToUpdate.find(myRole) != theRolesToUpdate.end()) {
                dom::NodePtr myDataNode = myShape.getVertexDataNode(myName);
                load(myResourceManager, myIndicesNode, myDataNode);
            }
        }
    }

    void Primitive::reverseUpdateVertexData() {
        dom::Node & myElementsNode = getNode();
        Shape & myShape = getShape();
        unsigned myIndicesCount = myElementsNode.childNodesLength(VERTEX_INDICES_NAME);
        for (unsigned k = 0; k < myIndicesCount; ++k) {
            dom::NodePtr myIndicesNode = myElementsNode.childNode(VERTEX_INDICES_NAME, k);
            const string & myName = myIndicesNode->getAttributeString(VERTEX_DATA_ATTRIB);
            dom::NodePtr myDataNode = myShape.getVertexDataNode(myName);
            //const VectorOfUnsignedInt & myIndices = myIndicesNode->
            //    childNode(0)->nodeValueRef<VectorOfUnsignedInt>();
            //TODO: find mechanism for selective range update with dirty flags/regions
            unload(myIndicesNode, myDataNode);
        }
   }

    VertexDataBasePtr
    Primitive::getVertexDataPtr(VertexDataRole theRole) {
        // AGPMemoryFlushSingleton::get().flushGLAGPMemory(); moved to gl/util/VertexArray.lock()
        VertexDataBasePtr myDataBin = _myVertexData[theRole];
        return myDataBin;
    }

    const VertexDataBase &
    Primitive::getVertexData(VertexDataRole theRole) const {
        VertexDataBasePtr myDataBin = _myVertexData[theRole];

        if (!myDataBin) {
            throw PrimitiveException(string("Vertexdata bin for: '") +
                getStringFromEnum(theRole, VertexDataRoleString) +
                "' not found in shape " + getShape().get<IdTag>() + ", primitive type: " +
                getStringFromEnum(get<PrimitiveTypeTag>(), PrimitiveTypeStrings) + ", material: " +
                getMaterial().get<IdTag>(), PLUS_FILE_LINE);
        }

        return *myDataBin;
    }
    VertexDataBase &
    Primitive::getVertexData(VertexDataRole theRole) {
        // AGPMemoryFlushSingleton::get().flushGLAGPMemory(); moved to gl/util/VertexArray.lock()
        VertexDataBasePtr myDataBin = _myVertexData[theRole];

        if (!myDataBin) {
            throw PrimitiveException(string("Vertexdata bin for: '") +
                getStringFromEnum(theRole, VertexDataRoleString) +
                "' not found in shape " + getShape().get<IdTag>() + ", primitive type: " +
                getStringFromEnum(get<PrimitiveTypeTag>(), PrimitiveTypeStrings) + ", material: " +
                getMaterial().get<IdTag>(), PLUS_FILE_LINE);
        }

        return *myDataBin;
    }
#if 0
    PrimitiveType
    Primitive::getTypeFromNode(dom::NodePtr thePrimitiveNode) {
        std::string myPrimitiveTypeString = (*thePrimitiveNode)[PRIMITIVE_TYPE_ATTRIB].nodeValue();

        if (myPrimitiveTypeString == PRIMITIVE_TYPE_POINTS) {
            return POINTS;
        }
        if (myPrimitiveTypeString == PRIMITIVE_TYPE_LINES) {
            return LINES;
        }
        if (myPrimitiveTypeString == PRIMITIVE_TYPE_LINE_STRIP) {
            return LINE_STRIP;
        }
        if (myPrimitiveTypeString == PRIMITIVE_TYPE_LINE_LOOP) {
            return LINE_LOOP;
        }
        if (myPrimitiveTypeString == PRIMITIVE_TYPE_TRIANGLES) {
            return TRIANGLES;
        }
        if (myPrimitiveTypeString == PRIMITIVE_TYPE_TRIANGLE_STRIP) {
            return TRIANGLE_STRIP;
        }
        if (myPrimitiveTypeString == PRIMITIVE_TYPE_TRIANGLE_FAN) {
            return TRIANGLE_FAN;
        }
        if (myPrimitiveTypeString == PRIMITIVE_TYPE_QUADS) {
            return QUADS;
        }
        if (myPrimitiveTypeString == PRIMITIVE_TYPE_QUAD_STRIP) {
            return QUAD_STRIP;
        }
        if (myPrimitiveTypeString == PRIMITIVE_TYPE_POLYGON) {
            return POLYGON;
        }
        throw UnknownPrimitiveType(myPrimitiveTypeString, PLUS_FILE_LINE);
    }
#endif
    template <>
    void
    Primitive::createTangents(GfxVectorOfVector3f & theBin) {

#ifdef OLD
        const VertexData3f::VertexDataVector & myPositions =
            getVertexData(POSITIONS).getVertexDataCast<asl::Vector3f>();
        const VertexData3f::VertexDataVector & myNormals =
            getVertexData(NORMALS).getVertexDataCast<asl::Vector3f>();
#else
        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = getConstLockingPositionsAccessor();
        const VertexData3f & myPositions = myPositionAccessor->get();

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > & myNormalsAccessor = getConstLockingNormalsAccessor();
        const VertexData3f & myNormals = myNormalsAccessor->get();
#endif
        // get two sides of the triangle
        asl::Vector3f mySide0 = myPositions[0] - myPositions[1];
        asl::Vector3f mySide1 = myPositions[2] - myPositions[1];

        // look for the first (hopefully only) bumpmap
        MaterialBase & myMaterial = getMaterial();
        unsigned myTextureCount = myMaterial.getTextureUnitCount();
        unsigned myBumpmapIndex;
        for (myBumpmapIndex=0; myBumpmapIndex < myTextureCount; ++myBumpmapIndex) {
            if (myMaterial.getTextureUsage(myBumpmapIndex) == BUMP) {
                break;
            }
        }

        // TODO: Export bumpmap usage explicitly
        VertexDataRole myRole = VertexDataRole(TEXCOORD0 + myBumpmapIndex);

        if (myBumpmapIndex == myTextureCount) {
            throw MaterialFault( std::string("missing Bumpmap in Material ")+myMaterial.get<NameTag>(),
                                  PLUS_FILE_LINE );
        }

        if (_myVertexData[myRole]->getType() != VECTOR2F) {
            throw MaterialFault( std::string("UV Set is not a Vector2f! in Material ")+myMaterial.get<NameTag>(),
                                  PLUS_FILE_LINE );
        }

#ifdef OLD
        VertexData2f::VertexDataVector & myUVSet
            = _myVertexData[myRole]->VertexDataBase::getVertexDataCast<asl::Vector2f>();
#else
        asl::Ptr<VertexDataAccessor<Vector2f> > myUVSetAccessor =
            _myVertexData[myRole]->VertexDataBase::getVertexDataAccessor<asl::Vector2f>();
        VertexData2f & myUVSet = myUVSetAccessor->get();
#endif
        //               [bumpmap][vertex0][x] - [bumpmap][vertex1][x]
        float deltaS0 = myUVSet[0][0] - myUVSet[1][0];
        //               [bumpmap][vertex2][x] - [bumpmap][vertex1][x]
        float deltaS1 = myUVSet[2][0] - myUVSet[1][0];

        asl::Vector3f myTDirection = normalized(deltaS1 * mySide0) - (deltaS0 * mySide1);
        //AC_TRACE <<"tangent: ("<<myTDirection[0] <<" , "<<myTDirection[1] <<" , "<<myTDirection[2] <<")" <<std::endl;

        asl::Vector3f myTangent0 = orthonormal(myNormals[0], myTDirection);
        asl::Vector3f myTangent1 = orthonormal(myNormals[1], myTDirection);
        asl::Vector3f myTangent2 = orthonormal(myNormals[2], myTDirection);

        theBin.push_back(myTangent0);
        theBin.push_back(myTangent1);
        theBin.push_back(myTangent2);
    }

    template <class DETECTOR>
    bool
    Primitive::scanElements(DETECTOR & theDetector,
                             const VertexData3f & thePositions,
                             const VertexData3f * theNormals)
    {
#ifdef OLD
        const VertexData3f::VertexDataVector & myPositions = *_myPositions;
        // DS: Attention: the following reference may point to an illeagal address
        const VertexData3f::VertexDataVector & myNormals   = *_myNormals;
#endif
        bool myHasNormals(false);
        if (theNormals && theNormals->size() == thePositions.size()) {
            myHasNormals = true;
        }

        bool myHit = false;

        unsigned int myStep = 1;
        PrimitiveType myType = get<PrimitiveTypeTag>();
        switch (myType) {
            case POINTS:
            case LINES:
                myStep = 2;
            case LINE_STRIP:
            case LINE_LOOP:
                for (unsigned i = 0; (i + 1) < thePositions.size(); i += myStep) {
                    myHit |= theDetector(this, i, asl::asLineSegment(asl::asPoint(thePositions[i])));
                }
                if (myType == LINE_LOOP) {
                    asl::LineSegment<float> edge(asl::asPoint(thePositions[thePositions.size()-1]),
                                        asl::asPoint(thePositions[0]));
                    myHit |= theDetector(this, thePositions.size(), edge);
                }
                break;
            case TRIANGLE_FAN:
                {AC_TRACE << "Scanning " << thePositions.size() << " positions.";}
                for (unsigned i = 1; i < (thePositions.size()-1); ++i) {
                    asl::Triangle<float> myTriangle(asl::asPoint(thePositions[0]),
                            asl::asPoint(thePositions[i]),
                            asl::asPoint(thePositions[i+1]));
                    if (myHasNormals) {
                        myHit |= theDetector(this, i, myTriangle, asl::asVector3((*theNormals)[i]));
                    } else {
                        // get normals from triangle plane
                        myHit |= theDetector(this, i, myTriangle);
                    }
                }
                break;
            case TRIANGLES:
                myStep = 3;
            case TRIANGLE_STRIP:
                {AC_TRACE << "Scanning " << thePositions.size() << " positions.";}
                for (unsigned i = 0; (i + 2) < thePositions.size(); i += myStep) {
                    if (myHasNormals) {
                        myHit |= theDetector(this, i, asl::asTriangle(asl::asPoint(thePositions[i])),
                                                  asl::asVector3((*theNormals)[i]));
                    } else {
                        // get normals from triangle plane
                        myHit |= theDetector(this, i, asl::asTriangle(asl::asPoint(thePositions[i])));
                    }
                }
                break;
            case QUADS:
                myStep = 4;
            case QUAD_STRIP:
                for (unsigned i = 0; (i + 3) < thePositions.size(); i += myStep) {
                    bool myQuadHit = false;
                    if (myHasNormals) {
                        // supply normals for interpolation
                        myQuadHit = theDetector(this, i, asl::asTriangle(asl::asPoint(thePositions[i])),
                                                  asl::asVector3((*theNormals)[i]));
                    } else {
                        // get normals from plane
                        myQuadHit = theDetector(this, i, asl::asTriangle(asl::asPoint(thePositions[i])));
                    }
                    if (!myQuadHit) {
                        // TODO: make faster routine for quad intersection
                        asl::Triangle<float> myOtherTriangle;
                        asl::Vector3<asl::Vector3<float> > myOtherNormals;
                        if (myType == QUADS) {
                            myOtherTriangle = asl::Triangle<float>(
                                asl::asPoint(thePositions[i+2]),
                                asl::asPoint(thePositions[i+3]),
                                asl::asPoint(thePositions[i+0]));
                            if (myHasNormals) {
                                myOtherNormals = asl::Vector3<asl::Vector3<float> >((*theNormals)[i+2], (*theNormals)[i+3], (*theNormals)[i+0]);
                            }
                        } else {
                            myOtherTriangle = asl::Triangle<float>(
                                asl::asPoint(thePositions[i+1]),
                                asl::asPoint(thePositions[i+3]),
                                asl::asPoint(thePositions[i+2]));
                            if (myHasNormals) {
                                myOtherNormals = asl::Vector3<asl::Vector3<float> >((*theNormals)[i+1], (*theNormals)[i+3], (*theNormals)[i+2]);
                            }
                        }
                        if (myHasNormals) {
                            // supply normals for interpolation
                            myQuadHit = theDetector(this, i, myOtherTriangle, myOtherNormals);
                        } else {
                            // get normals from plane
                            myQuadHit = theDetector(this, i, myOtherTriangle);
                        }
                    }
                    myHit |= myQuadHit;
                }
                break;
            case POLYGON:
                return false;
            default:
                throw UnknownPrimitiveType(asl::as_string(myType), PLUS_FILE_LINE);
        }

        return myHit;
    }
    struct BoundingBoxTreeBuilder {
        BoundingBoxTree & _myTree;

        BoundingBoxTreeBuilder(BoundingBoxTree & theTree) :
                _myTree(theTree)
        { }

        bool operator()(Primitive * thePrimitive, int theElementIndex,
                        const asl::LineSegment<float> & theLineSegment) {
            Box3<float> myLineSegmentBounds;
            myLineSegmentBounds.makeEmpty();
            myLineSegmentBounds.extendBy(theLineSegment.origin);
            myLineSegmentBounds.extendBy(theLineSegment.end);
            _myTree.insertBox(myLineSegmentBounds, theElementIndex);
            return false;
        }

        bool operator()(Primitive * thePrimitive, int theElementIndex, const asl::Triangle<float> & theTriangle) {
            Box3<float> myTriangleBounds;
            myTriangleBounds.makeEmpty();
            myTriangleBounds.extendBy(theTriangle[0]);
            myTriangleBounds.extendBy(theTriangle[1]);
            myTriangleBounds.extendBy(theTriangle[2]);
            _myTree.insertBox(myTriangleBounds, theElementIndex);
            return false;
        }

        bool operator()(Primitive * thePrimitive, int theElementIndex, const asl::Triangle<float> & theTriangle,
                const asl::Vector3<asl::Vector3f> & theNormals)
        {
            return operator()(thePrimitive, theElementIndex, theTriangle);
        }

    };

    void Primitive::updateBoundingBoxTree() {
        _myBoundingBoxTree = BoundingBoxTreePtr(new BoundingBoxTree);
        BoundingBoxTreeBuilder myBuilder(*_myBoundingBoxTree);

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = getConstLockingPositionsAccessor();
        const VertexData3f & myPositions = myPositionAccessor->get();

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > & myNormalsAccessor = getConstLockingNormalsAccessor();
        const VertexData3f * myNormals = myNormalsAccessor ? &myNormalsAccessor->get() : 0;

        scanElements(myBuilder, myPositions, myNormals);
    };

    template <class DETECTOR>
    bool
    Primitive::scanHierarchy(DETECTOR & theDetector, BoundingBoxTree & theTree,
                             const VertexData3f & thePositions,
                             const VertexData3f * theNormals)
    {
        if (theDetector.discards(theTree.myBox)) {
            return false;
        }
        if (!theTree.isLeaf()) {
            bool myHit = false;
            for (std::vector<BoundingBoxTreePtr>::size_type i = 0; i < theTree.myChildren.size(); ++i) {
                myHit |= scanHierarchy(theDetector, *theTree.myChildren[i], thePositions, theNormals);
            }
            return myHit;
        }
#ifdef OLD
        // check leaf element
        const VertexData3f::VertexDataVector & thePositions = *_thePositions;
        const VertexData3f::VertexDataVector & theNormals = *_theNormals;
#endif
        bool myHit = false;
        bool myHasNormals = (theNormals->size() == thePositions.size());

        const int i = theTree.myMinIndex;
        PrimitiveType myType = get<PrimitiveTypeTag>();
        switch (myType) {
            case POINTS:
            case LINES:
            case LINE_STRIP:
            case LINE_LOOP:
                //dk TODO Copy/Paste from scanElements ?
                return false;
            case TRIANGLE_FAN:
                {AC_TRACE << "scanHierarchy " << thePositions.size() << " positions.";}
                {
                    asl::Triangle<float> myTriangle(asl::asPoint(thePositions[0]),
                            asl::asPoint(thePositions[i]),
                            asl::asPoint(thePositions[i+1]));
                    if (myHasNormals) {
                        myHit |= theDetector(this, i, myTriangle, asl::asVector3((*theNormals)[i]));
                    } else {
                        // get normals from triangle plane
                        myHit |= theDetector(this, i, myTriangle);
                    }
                }
                break;
            case TRIANGLES:
            case TRIANGLE_STRIP:
                if (myHasNormals) {
                    myHit |= theDetector(this, i, asl::asTriangle(asl::asPoint(thePositions[i])),
                                            asl::asVector3((*theNormals)[i]));
                } else {
                    // get normals from triangle plane
                    myHit |= theDetector(this, i, asl::asTriangle(asl::asPoint(thePositions[i])));
                }
                break;
            case QUADS:
            case QUAD_STRIP:
                {
                    bool myQuadHit = false;
                    if (myHasNormals) {
                        // supply normals for interpolation
                        myQuadHit = theDetector(this, i, asl::asTriangle(asl::asPoint(thePositions[i])),
                                                asl::asVector3((*theNormals)[i]));
                    } else {
                        // get normals from plane
                        myQuadHit = theDetector(this, i, asl::asTriangle(asl::asPoint(thePositions[i])));
                    }

                    if (!myQuadHit) {
                        // TODO: make faster routine for quad intersection
                        asl::Triangle<float> myOtherTriangle(
                            asl::asPoint(thePositions[i+2]),
                            asl::asPoint(thePositions[i+3]),
                            asl::asPoint(thePositions[i+0])
                        );
                        if (myHasNormals) {
                            asl::Vector3<asl::Vector3<float> > myOtherNormals((*theNormals)[i+2], (*theNormals)[i+3], (*theNormals)[i+0]);
                            // supply normals for interpolation
                            myQuadHit = theDetector(this, i, myOtherTriangle, myOtherNormals);
                        } else {
                            // get normals from plane
                            myQuadHit = theDetector(this, i, myOtherTriangle);
                        }
                    }
                    myHit |= myQuadHit;
                }
                break;
            case POLYGON:
                return false;
            default:
                throw UnknownPrimitiveType(asl::as_string(myType), PLUS_FILE_LINE);
        }

        return myHit;
    }
    template <class PROBE>
    struct IntersectTriangleDetector {
        const PROBE & _myStick;
        Primitive::IntersectionList & _myList;

        IntersectTriangleDetector(const PROBE & theStick, Primitive::IntersectionList & theList) :
            _myStick(theStick), _myList(theList)
        {}

        bool discards(const asl::Box3f & theBoundingBox) {
            return !intersection(theBoundingBox, _myStick);
        }
        bool operator()(Primitive * thePrimitive, int theElementIndex,
                        const asl::LineSegment<float> & theLineSegment) {
            //dk TODO
            asl::Point3f myIntersection;
            asl::Vector3f myNormal(0,0,0);
            if (asl::intersection(theLineSegment, _myStick, myIntersection)) {
                addIntersectionInfo(thePrimitive, theElementIndex, myIntersection, myNormal);
                return true;
            } else {
                return false;
            }
        }
        bool operator()(Primitive * thePrimitive, int theElementIndex, const asl::Triangle<float> & theTriangle) {
            asl::Point3f myIntersection;
            asl::Vector3f myNormal;
            if (asl::intersection(theTriangle, _myStick, myIntersection, myNormal)) {
                addIntersectionInfo(thePrimitive, theElementIndex, myIntersection, myNormal);
                return true;
            } else {
                return false;
            }
        }
        bool operator()(Primitive * thePrimitive, int theElementIndex, const asl::Triangle<float> & theTriangle,
                const asl::Vector3<asl::Vector3f> & theNormals) {
            asl::Point3f myIntersection;
            asl::Vector3f myNormal;
            if (asl::intersection(theTriangle, theNormals, _myStick, myIntersection, myNormal)) {
                addIntersectionInfo(thePrimitive, theElementIndex, myIntersection, myNormal);
                return true;
            } else {
                return false;
            }
        }
        void addIntersectionInfo(Primitive * thePrimitive, unsigned theStartVertex,
                const asl::Point3f & theIntersection, const asl::Vector3f & theNormal)
        {
            _myList.push_back(Primitive::Intersection());
            Primitive::Intersection & myInfo = _myList.back();

            myInfo._myElement._myPrimitive    = thePrimitive;
            myInfo._myElement._myStartVertex  = theStartVertex;// + thePrimitive->getDomIndex();
            myInfo._myElement._myType         = thePrimitive->get<PrimitiveTypeTag>();
            myInfo._myElement._myVertexCount  = getVerticesPerPrimitive(myInfo._myElement._myType);

            myInfo._myPosition = theIntersection;
            myInfo._myNormal   = theNormal;
        }
    };

    bool
    Primitive::intersect(const asl::LineSegment<float> & theStick,  IntersectionList & theIntersectionInfo) {

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = getConstLockingPositionsAccessor();
        const VertexData3f & myPositions = myPositionAccessor->get();

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = getConstLockingNormalsAccessor();
        const VertexData3f * myNormals = myNormalsAccessor ? &myNormalsAccessor->get() : 0;

        IntersectTriangleDetector<asl::LineSegment<float> > myDetector(theStick, theIntersectionInfo);
        if (_myBoundingBoxTree) {
            return scanHierarchy(myDetector, *_myBoundingBoxTree, myPositions, myNormals);
        } else {
            return scanElements(myDetector, myPositions, myNormals);
        }
    }
    bool
    Primitive::intersect(const asl::Ray<float> & theStick,  IntersectionList & theIntersectionInfo) {

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = getConstLockingPositionsAccessor();
        const VertexData3f & myPositions = myPositionAccessor->get();

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = getConstLockingNormalsAccessor();
        const VertexData3f * myNormals = myNormalsAccessor ? &myNormalsAccessor->get() : 0;

        IntersectTriangleDetector<asl::Ray<float> > myDetector(theStick, theIntersectionInfo);
        if (_myBoundingBoxTree) {
            return scanHierarchy(myDetector, *_myBoundingBoxTree, myPositions, myNormals);
        } else {
            return scanElements(myDetector, myPositions, myNormals);
        }
    }

    bool
    Primitive::intersect(const asl::Line<float> & theStick,  IntersectionList & theIntersectionInfo) {

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = getConstLockingPositionsAccessor();
        const VertexData3f & myPositions = myPositionAccessor->get();

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = getConstLockingNormalsAccessor();
        const VertexData3f * myNormals = myNormalsAccessor ? &myNormalsAccessor->get() : 0;

        IntersectTriangleDetector<asl::Line<float> > myDetector(theStick, theIntersectionInfo);
       if (_myBoundingBoxTree) {
            return scanHierarchy(myDetector, *_myBoundingBoxTree, myPositions, myNormals);
        } else {
            return scanElements(myDetector, myPositions, myNormals);
        }
    }

    struct SweptSphereAllContactsDetector {
        const asl::Sphere<float> & _mySphere;
        const asl::Vector3f & _myMotion;
        Primitive::SphereContactsList & _myList;
        const asl::Matrix4f & _mySphereSpaceTransform;

        SweptSphereAllContactsDetector(const asl::Sphere<float> & theSphere,
                                       const asl::Vector3f & theMotion,
                                       const asl::Matrix4f & theSphereSpaceTransform,
                                        Primitive::SphereContactsList & theList)
            : _mySphere(theSphere),
            _myMotion(theMotion),
            _myList(theList),
            _mySphereSpaceTransform(theSphereSpaceTransform)
        { }

        bool operator()(Primitive * thePrimitive, int theElementIndex,
                        const asl::LineSegment<float> & theEdge) {

            asl::SweptSphereContact<float> myFirstContact;
            asl::SweptSphereContact<float> myLastContact;

            asl::LineSegment<float> mySphereSpaceEdge;
            mySphereSpaceEdge.origin = theEdge.origin * _mySphereSpaceTransform;
            mySphereSpaceEdge.end = theEdge.end * _mySphereSpaceTransform;
            float x0, f0, x1, f1;
            //TODO check contact w/ vertices first ?
            int myContactCount = asl::contacts<float>(_mySphere, _myMotion, mySphereSpaceEdge,
                                                    x0, f0, x1, f1);

            //TODO fill myFirstContact,myLastContact from x0,f0,x1,f1

            if (myContactCount) {
                addContactInfo(thePrimitive, theElementIndex, myContactCount, myFirstContact, myLastContact);
                return true;
            } else {
                return false;
            }
        }

        bool operator()(Primitive * thePrimitive, int theElementIndex, const asl::Triangle<float> & theTriangle) {
            asl::SweptSphereContact<float> myFirstContact;
            asl::SweptSphereContact<float> myLastContact;

            asl::Triangle<float> mySphereSpaceTriangle;
            for (int i = 0; i < 3; ++i) {
                mySphereSpaceTriangle[i] = theTriangle[i] * _mySphereSpaceTransform;
            }
            int myContactCount = asl::contacts<float>(_mySphere, _myMotion, mySphereSpaceTriangle,
                    0, myFirstContact, myLastContact);

            if (myContactCount) {
                addContactInfo(thePrimitive, theElementIndex, myContactCount, myFirstContact, myLastContact);
                return true;
            } else {
                return false;
            }
        }

        bool operator()(Primitive * thePrimitive, int theElementIndex, const asl::Triangle<float> & theTriangle,
                const asl::Vector3<asl::Vector3f> & theNormals) {
            asl::SweptSphereContact<float> myFirstContact;
            asl::SweptSphereContact<float> myLastContact;

            asl::Triangle<float> mySphereSpaceTriangle;
            for (int i = 0; i < 3; ++i) {
                mySphereSpaceTriangle[i] = theTriangle[i] * _mySphereSpaceTransform;
            }
            asl::Vector3<asl::Vector3f> mySphereSpaceNormals;
            for (int i = 0; i < 3; ++i) {
                mySphereSpaceNormals[i] = transformedNormal(theNormals[i],_mySphereSpaceTransform);
            }

            int myContactCount = asl::contacts<float>(_mySphere, _myMotion, mySphereSpaceTriangle,
                    &mySphereSpaceNormals, myFirstContact, myLastContact);

            if (myContactCount) {
                addContactInfo(thePrimitive, theElementIndex, myContactCount, myFirstContact, myLastContact);
                return true;
            } else {
                return false;
            }
        }

        void addContactInfo(Primitive * thePrimitive, unsigned theStartVertex,
                int theContactCount, const asl::SweptSphereContact<float> & theFirstContact,
                const asl::SweptSphereContact<float> & theLastContact)
        {
            _myList.push_back(Primitive::SphereContacts());
            Primitive::SphereContacts & myInfo = _myList.back();

            myInfo._myElement._myPrimitive    = thePrimitive;
            myInfo._myElement._myStartVertex  = theStartVertex;
            myInfo._myElement._myType         = thePrimitive->get<PrimitiveTypeTag>();
            myInfo._myElement._myVertexCount  = getVerticesPerPrimitive(myInfo._myElement._myType);

            myInfo._myNumberOfContacts = theContactCount;
            myInfo._myMinContact= theFirstContact;
            myInfo._myMaxContact= theLastContact;
        }
    };

    bool
    Primitive::collide(const asl::Sphere<float> & theSphere, const asl::Vector3<float> & theMotion,
        const asl::Matrix4f & theSphereSpaceTransform, SphereContactsList & theCollisionInfo)
    {
        //updateNormalPositionColorPointers();
        SweptSphereAllContactsDetector myDetector(theSphere, theMotion, theSphereSpaceTransform, theCollisionInfo);

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = getConstLockingPositionsAccessor();
        const VertexData3f & myPositions = myPositionAccessor->get();

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = getConstLockingNormalsAccessor();
        const VertexData3f * myNormals = myNormalsAccessor ? &myNormalsAccessor->get() : 0;

        return scanElements(myDetector, myPositions, myNormals);
    }

    struct SweptSphereFirstContactDetector {
        const asl::Sphere<float> & _mySphere;
        const asl::Vector3f & _myMotion;
        const asl::Matrix4f & _mySphereSpaceTransform;
        asl::SweptSphereContact<float> _myCurrentContact;
        Primitive::SphereContacts & _myNewContactInfo;

        SweptSphereFirstContactDetector(const asl::Sphere<float> & theSphere, const asl::Vector3f & theMotion,
                           const asl::Matrix4f & theSphereSpaceTransform, const asl::SweptSphereContact<float> & theCurrentContact,
                           Primitive::SphereContacts & theNewContactInfo) :
                _mySphere(theSphere), _myMotion(theMotion), _mySphereSpaceTransform(theSphereSpaceTransform),
                _myCurrentContact(theCurrentContact), _myNewContactInfo(theNewContactInfo)
        { }

        bool operator()(Primitive * thePrimitive, int theElementIndex,
                        const asl::LineSegment<float> & theEdge) {

            asl::SweptSphereContact<float> nextContact;

            asl::LineSegment<float> mySphereSpaceEdge;
            mySphereSpaceEdge.origin = theEdge.origin * _mySphereSpaceTransform;
            mySphereSpaceEdge.end = theEdge.end * _mySphereSpaceTransform;


            float x0, f0, x1, f1;
            int myContactCount = asl::contacts<float>(_mySphere, _myMotion, mySphereSpaceEdge,
                                                    x0, f0, x1, f1);

            //TODO fill nextContact from x0,f0,x1,f1

            if (myContactCount) {
                addContactInfo(thePrimitive, theElementIndex, nextContact);
                _myCurrentContact = nextContact;
                return true;
            } else {
                return false;
            }
        }

        bool operator()(Primitive * thePrimitive, int theElementIndex, const asl::Triangle<float> & theTriangle) {
            asl::SweptSphereContact<float> nextContact;

            MAKE_SCOPE_TIMER(Primitive_Collide_First);

            START_TIMER(Primitive_Collide_First_Transform);
            asl::Triangle<float> mySphereSpaceTriangle;
            for (int i = 0; i < 3; ++i) {
                mySphereSpaceTriangle[i] = theTriangle[i] * _mySphereSpaceTransform;
            }

            STOP_TIMER(Primitive_Collide_First_Transform);
            if (asl::firstContact<float>(_mySphere, _myMotion, mySphereSpaceTriangle,
                    0, _myCurrentContact, nextContact)) {
                addContactInfo(thePrimitive, theElementIndex, nextContact);
                _myCurrentContact = nextContact;
                return true;
            }
            return false;
        }

        bool operator()(Primitive * thePrimitive, int theElementIndex, const asl::Triangle<float> & theTriangle,
                const asl::Vector3<asl::Vector3f> & theNormals) {
            asl::SweptSphereContact<float> nextContact;

            MAKE_SCOPE_TIMER(Primitive_Collide_First);

            START_TIMER(Transform_Position_Normals);
            asl::Triangle<float> mySphereSpaceTriangle;
            asl::Vector3<asl::Vector3f> mySphereSpaceNormals;
            for (int i = 0; i < 3; ++i) {
                mySphereSpaceTriangle[i] = theTriangle[i] * _mySphereSpaceTransform;
                mySphereSpaceNormals[i] = transformedNormal(theNormals[i],_mySphereSpaceTransform);
            }
            STOP_TIMER(Transform_Position_Normals);

            if (asl::firstContact<float>(_mySphere, _myMotion, mySphereSpaceTriangle,
                    &mySphereSpaceNormals, _myCurrentContact, nextContact)) {
                addContactInfo(thePrimitive, theElementIndex, nextContact);
                _myCurrentContact = nextContact;
                return true;
            }
            return false;
        }

        void addContactInfo(Primitive * thePrimitive, unsigned theStartVertex,
                const asl::SweptSphereContact<float> & theContact)
        {
            _myNewContactInfo._myElement._myPrimitive    = thePrimitive;
            _myNewContactInfo._myElement._myStartVertex  = theStartVertex;
            _myNewContactInfo._myElement._myType         = thePrimitive->get<PrimitiveTypeTag>();
            _myNewContactInfo._myElement._myVertexCount  = getVerticesPerPrimitive(_myNewContactInfo._myElement._myType);

            _myNewContactInfo._myNumberOfContacts = 1;
            _myNewContactInfo._myMinContact= theContact;
            _myNewContactInfo._myMaxContact= theContact;
        }
    };

    bool
    Primitive::collide(const asl::Sphere<float> & theSphere, const asl::Vector3<float> & theMotion,
            const asl::Matrix4f & theSphereSpaceTransform, const asl::SweptSphereContact<float> & theCurrentContact,
                Primitive::SphereContacts & theNewContactInfo) {

        SweptSphereFirstContactDetector myDetector(theSphere, theMotion, theSphereSpaceTransform, theCurrentContact, theNewContactInfo);

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = getConstLockingPositionsAccessor();
        const VertexData3f & myPositions = myPositionAccessor->get();

        const asl::Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = getConstLockingNormalsAccessor();
        const VertexData3f * myNormals = myNormalsAccessor ? &myNormalsAccessor->get() : 0;

        return scanElements(myDetector, myPositions, myNormals);
    }
#ifdef OLD
    void
    Primitive::updateNormalPositionColorPointers() const {
        _myPositions = const_cast<Primitive*>(this)->getPositions();
        _myNormals = const_cast<Primitive*>(this)->getNormals();
        _myColors = const_cast<Primitive*>(this)->getColors();
    }
#endif
#ifdef OLD
    VertexData3f::VertexDataVector *
    Primitive::getPositions() {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(POSITIONS));
        if (myVertices) {
            return &myVertices->y60::VertexDataBase::getVertexDataCast<asl::Vector3f>();
        }
        return 0;
    }
    VertexData3f::VertexDataVector *
    Primitive::getNormals() {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(NORMALS));
        if (myVertices) {
            return &myVertices->y60::VertexDataBase::getVertexDataCast<asl::Vector3f>();
        }
        return 0;
    }
    VertexData4f::VertexDataVector *
    Primitive::getColors() {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(COLORS));
        if (myVertices) {
            return &myVertices->y60::VertexDataBase::getVertexDataCast<asl::Vector4f>();
        }
        return 0;
    }

    VertexData1f::VertexDataVector *
    Primitive::getTexCoords1f(unsigned theSlot) {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(TEXCOORD0+theSlot));
        if (myVertices) {
            return &myVertices->y60::VertexDataBase::getVertexDataCast<float>();
        }
        return 0;
    }

    VertexData2f::VertexDataVector *
    Primitive::getTexCoords2f(unsigned theSlot) {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(TEXCOORD0+theSlot));
        if (myVertices) {
            return &myVertices->y60::VertexDataBase::getVertexDataCast<asl::Vector2f>();
        }
        return 0;
    }

    VertexData3f::VertexDataVector *
    Primitive::getTexCoords3f(unsigned theSlot) {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(TEXCOORD0+theSlot));
        if (myVertices) {
            return &myVertices->y60::VertexDataBase::getVertexDataCast<asl::Vector3f>();
        }
        return 0;
    }
#else
    asl::Ptr<VertexDataAccessor<Vector3f> >
    Primitive::getLockingPositionsAccessor(bool forWriting, bool  forReading) {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(POSITIONS));
        if (myVertices) {
            return myVertices->getVertexDataAccessor<asl::Vector3f>(forWriting, forReading);
        }
        return asl::Ptr<VertexDataAccessor<Vector3f> >();
    }

    asl::Ptr<ConstVertexDataAccessor<Vector3f> >
    Primitive::getConstLockingPositionsAccessor() const {
        const y60::VertexDataBasePtr myVertices = getVertexDataPtr(VertexDataRole(POSITIONS));
        if (myVertices) {
            const VertexDataBase& myVerticesRef = *myVertices;
            return myVerticesRef.getVertexDataAccessor<asl::Vector3f>(); // read-only
        }
        return asl::Ptr<ConstVertexDataAccessor<Vector3f> >();
    }

    asl::Ptr<VertexDataAccessor<Vector3f> >
    Primitive::getLockingNormalsAccessor(bool forWriting, bool  forReading) {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(NORMALS));
        if (myVertices) {
            return myVertices->getVertexDataAccessor<asl::Vector3f>(forWriting, forReading);
        }
        return asl::Ptr<VertexDataAccessor<Vector3f> >();
    }

    asl::Ptr<ConstVertexDataAccessor<Vector3f> >
    Primitive::getConstLockingNormalsAccessor() const {
        const y60::VertexDataBasePtr myVertices = getVertexDataPtr(VertexDataRole(NORMALS));
        if (myVertices) {
            const VertexDataBase& myVerticesRef = *myVertices;
            return myVerticesRef.getVertexDataAccessor<asl::Vector3f>(); // read-only
        }
        return asl::Ptr<ConstVertexDataAccessor<Vector3f> >();
    }

    asl::Ptr<VertexDataAccessor<Vector4f> >
    Primitive::getLockingColorsAccessor(bool forWriting, bool  forReading) {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(COLORS));
        if (myVertices) {
            return myVertices->getVertexDataAccessor<asl::Vector4f>(forWriting, forReading);
        }
        return asl::Ptr<VertexDataAccessor<Vector4f> >();
    }

    asl::Ptr<ConstVertexDataAccessor<Vector4f> >
    Primitive::getConstLockingColorsAccessor() const {
        const y60::VertexDataBasePtr myVertices = getVertexDataPtr(VertexDataRole(COLORS));
        if (myVertices) {
            const VertexDataBase& myVerticesRef = *myVertices;
            return myVerticesRef.getVertexDataAccessor<asl::Vector4f>(); // read only
        }
        return asl::Ptr<ConstVertexDataAccessor<Vector4f> >();
    }

    asl::Ptr<VertexDataAccessor<float> >
    Primitive::getLockingTexCoord1fAccessor(unsigned theSlot, bool forWriting, bool  forReading) {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(TEXCOORD0+theSlot));
        if (myVertices) {
            return myVertices->getVertexDataAccessor<float>(forWriting, forReading);
        }
        return asl::Ptr<VertexDataAccessor<float> >();
    }

    asl::Ptr<VertexDataAccessor<Vector2f> >
    Primitive::getLockingTexCoord2fAccessor(unsigned theSlot, bool forWriting, bool  forReading) {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(TEXCOORD0+theSlot));
        if (myVertices) {
            return myVertices->getVertexDataAccessor<asl::Vector2f>(forWriting, forReading);
        }
        return asl::Ptr<VertexDataAccessor<Vector2f> >();
    }

    asl::Ptr<VertexDataAccessor<Vector3f> >
    Primitive::getLockingTexCoord3fAccessor(unsigned theSlot, bool forWriting, bool  forReading) {
        y60::VertexDataBasePtr myVertices = const_cast<Primitive*>(this)->getVertexDataPtr(VertexDataRole(TEXCOORD0+theSlot));
        if (myVertices) {
            return myVertices->getVertexDataAccessor<asl::Vector3f>(forWriting, forReading);
        }
        return asl::Ptr<VertexDataAccessor<Vector3f> >();
    }

    void
    Primitive::generateMesh(dom::NodePtr theIndicesNode) {

    }


#endif
}

