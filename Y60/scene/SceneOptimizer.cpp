//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "SceneOptimizer.h"
#include "TransformHierarchyFacade.h"
#include <asl/Logger.h>
#include <y60/NodeNames.h>

using namespace std;
using namespace asl;
using namespace dom;

namespace y60 {
    SceneOptimizer::SceneOptimizer(Scene & theScene) :
        _myScene(theScene)
    {}

    SceneOptimizer::SuperShape::SuperShape(Scene & theScene, const std::string & theRenderStyle) {
        _myShape = theScene.getShapesRoot()->appendChild(dom::Element(SHAPE_NODE_NAME));
        _myShape->appendAttribute(NAME_ATTRIB, "Supershape");
        _myShape->appendAttribute(RENDER_STYLE_ATTRIB, theRenderStyle);
        _myShape->appendAttribute(ID_ATTRIB, IdTag::getDefault());
        _myShape->appendChild(dom::Element(VERTEX_DATA_NAME));
        _myShape->appendChild(dom::Element(PRIMITIVE_LIST_NAME));
    }

    dom::NodePtr
    SceneOptimizer::SuperShape::getVertexData(const std::string & theType, const std::string & theName) {
        if (_myVertexDataMap.find(theName) == _myVertexDataMap.end()) {
            dom::Element myNewVertexData(theType);
            myNewVertexData.appendAttribute(NAME_ATTRIB, theName);
            myNewVertexData.appendChild(dom::Text("[]"));
            _myVertexDataMap[theName] = _myShape->childNode(VERTEX_DATA_NAME)->appendChild(myNewVertexData);
        }

        return _myVertexDataMap[theName];
    }

    SceneOptimizer::PrimitiveCachePtr
    SceneOptimizer::SuperShape::getPrimitive(const std::string & theType, const std::string & theMaterial) {
        string myKey = theType + "_" + theMaterial;

        if (_myPrimitiveMap.find(myKey) == _myPrimitiveMap.end()) {
            dom::Element myNewElements(ELEMENTS_NODE_NAME);
            myNewElements.appendAttribute(PRIMITIVE_TYPE_ATTRIB, theType);
            myNewElements.appendAttribute(MATERIAL_REF_ATTRIB, theMaterial);
            dom::NodePtr myPrimitive = _myShape->childNode(PRIMITIVE_LIST_NAME)->appendChild(myNewElements);
            _myPrimitiveMap[myKey] = PrimitiveCachePtr(new PrimitiveCache(myPrimitive));
        }

        return _myPrimitiveMap[myKey];
    }

    std::string
    SceneOptimizer::SuperShape::getShapeId() const {
        return _myShape->getAttributeString(ID_ATTRIB);
    }

    dom::NodePtr
    SceneOptimizer::PrimitiveCache::getIndex(const std::string & theName, const std::string & theRole) {
        NodeMap::iterator it = _myIndices.find(theRole);
        if (it == _myIndices.end()) {
            Element myNewIndex(VERTEX_INDICES_NAME);
            myNewIndex.appendAttribute(VERTEX_DATA_ATTRIB, theRole);
            myNewIndex.appendAttribute(VERTEX_DATA_ROLE_ATTRIB, theRole);
            myNewIndex.appendChild(dom::Text("[]"));
            _myIndices[theRole] = _myPrimitives->appendChild(myNewIndex);
            return _myIndices[theRole];
        } else {
            return it->second;
        }
    }

    void
    SceneOptimizer::run(dom::NodePtr theRootNode) {
        AC_INFO << "Running Scene optimizer";
        _myShapes.clear();
        if (!theRootNode) {
            theRootNode = _myScene.getWorldRoot();
        } else if (theRootNode->nodeName() != "transform") {
            throw asl::Exception("Only transform nodes are allowed as root node for scene optimizer", PLUS_FILE_LINE);
        }

        // First step: Remove invisible nodes
        AC_INFO << "  Removing invisible nodes...";
        removeInvisibleNodes(theRootNode);

        // Second step: Merge all bodies into one superbody
        AC_INFO << "  Merging bodies...";
        Matrix4f myInitialMatrix = theRootNode->getFacade<TransformHierarchyFacade>()->get<InverseGlobalMatrixTag>();
        mergeBodies(theRootNode, myInitialMatrix);

        // Third step: Remove empty transforms and
        // merge transforms with single childs with their child
        AC_INFO << "  Cleanup Scene...";
        cleanupScene(theRootNode);

        for (SuperShapeMap::iterator it = _myShapes.begin(); it != _myShapes.end(); ++it) {
            dom::NodePtr mySuperBody;
            mySuperBody = theRootNode->appendChild(dom::Element(BODY_NODE_NAME));
            mySuperBody->appendAttribute(NAME_ATTRIB, "Optimized Body");
            mySuperBody->appendAttribute(BODY_SHAPE_ATTRIB, it->second->getShapeId());
        }

        // Fourth step: Remove unused shapes
        AC_INFO << "  Removing unused shapes...";
        removeUnusedShapes();
    }

    void
    SceneOptimizer::cleanupScene(dom::NodePtr theNode) {
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            NodePtr myChild = theNode->childNode(i);

            // Search depth first
            cleanupScene(myChild);

            if (myChild->nodeName() == "transform" && myChild->childNodesLength() == 0) {
                myChild->parentNode()->removeChild(myChild);
                --i;

            }
        }
    }

    // Specialization for positions and normals
    unsigned
    SceneOptimizer::transformVertexData(dom::NodePtr theSrcVertexData, dom::NodePtr theDstVertexData, asl::Matrix4f theMatrix, bool theNormalFlag) {
        const VectorOfVector3f & mySrc = theSrcVertexData->firstChild()->nodeValueRef<VectorOfVector3f>();
        VectorOfVector3f & myDst = theDstVertexData->firstChild()->nodeValueRefOpen<VectorOfVector3f>();
        unsigned myOffset = myDst.size();

        myDst.resize(mySrc.size() + myOffset);

        if (theNormalFlag) {
            float myScaleSign = theMatrix[0][0] * theMatrix[1][1] * theMatrix[2][2];
            myScaleSign = (myScaleSign > 0) ? 1.0f : -1.0f;
            for (unsigned j = 0; j < mySrc.size(); ++j) {
                myDst[myOffset + j] = transformedNormal(mySrc[j], theMatrix) * myScaleSign;
            }
        } else {
            for (unsigned j = 0; j < mySrc.size(); ++j) {
               myDst[myOffset + j] = product(asPoint(mySrc[j]), theMatrix);
            }
        }
        theDstVertexData->firstChild()->nodeValueRefClose<VectorOfVector3f>();
        return myOffset;
    }

    void
    SceneOptimizer::mergeVertexData(const Shape & theShape, SuperShapePtr theSuperShape, const asl::Matrix4f & theMatrix, const RoleMap & theRoles, VertexDataMap & theVertexDataOffsets) {
        NodePtr myVertexData = theShape.getNode().childNode(VERTEX_DATA_NAME);
        for (unsigned i = 0; i < myVertexData->childNodesLength(); ++i) {
            NodePtr mySrcVertexData = myVertexData->childNode(i);
            RoleMap::const_iterator it = theRoles.find(mySrcVertexData->getAttributeString(NAME_ATTRIB));

            // Skip unused vertex data
            if (it == theRoles.end()) {
                continue;
            }

            const string & myRole = it->second;
            NodePtr myDstVertexData = theSuperShape->getVertexData(mySrcVertexData->nodeName(), myRole);
            TypeId myTypeId = TypeId(getEnumFromString(mySrcVertexData->nodeName(), TypeIdStrings));
            switch (myTypeId) {
                case VECTOR_OF_VECTOR2F:
                    theVertexDataOffsets[myRole] = copyVertexData<Vector2f>(mySrcVertexData, myDstVertexData);
                    break;
                case VECTOR_OF_VECTOR3F:
                    if (myRole == "position") {
                        theVertexDataOffsets[myRole] = transformVertexData(mySrcVertexData, myDstVertexData, theMatrix);
                    } else if (myRole == "normal") {
                        theVertexDataOffsets[myRole] = transformVertexData(mySrcVertexData, myDstVertexData, theMatrix, true);
                    } else {
                        theVertexDataOffsets[myRole] = copyVertexData<Vector3f>(mySrcVertexData, myDstVertexData);
                    }
                    break;
                case VECTOR_OF_VECTOR4F:
                    theVertexDataOffsets[myRole] = copyVertexData<Vector4f>(mySrcVertexData, myDstVertexData);
                    break;
                default:
                    throw NotYetImplemented(string("Can not copy vertex data with type ") +
                                         getStringFromEnum(myTypeId, TypeIdStrings), PLUS_FILE_LINE);
            }
        }
    }

    void
    SceneOptimizer::mergePrimitives(const Shape & theShape, SuperShapePtr theSuperShape, bool theFlipFlag, VertexDataMap & theVertexDataOffsets) {
        NodePtr myPrimitives = theShape.getNode().childNode(PRIMITIVE_LIST_NAME);

        for (unsigned i = 0; i < myPrimitives->childNodesLength(); ++i) {
            NodePtr mySrcElements = myPrimitives->childNode(i);
            string myElementType = mySrcElements->getAttributeString(PRIMITIVE_TYPE_ATTRIB);
            unsigned myVerticesPerPrimitive = getVerticesPerPrimitive(Primitive::getTypeFromNode(mySrcElements));

#ifdef TRIANGULATE_QUADS
            if (myElementType == "quads") {
                myElementType = "triangles";
            }
#endif

            PrimitiveCachePtr myDstElements = theSuperShape->getPrimitive(myElementType, mySrcElements->getAttributeString(MATERIAL_REF_ATTRIB));

            for (unsigned j = 0; j < mySrcElements->childNodesLength(); ++j) {
                NodePtr mySrcIndex = mySrcElements->childNode(j);
                string myName = mySrcIndex->getAttributeString(VERTEX_DATA_ATTRIB);
                string myRole = mySrcIndex->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
                NodePtr myDstIndex = myDstElements->getIndex(myName, myRole);

                const VectorOfUnsignedInt & mySrc = mySrcIndex->firstChild()->nodeValueRef<VectorOfUnsignedInt>();
                VectorOfUnsignedInt & myDst = myDstIndex->firstChild()->nodeValueRefOpen<VectorOfUnsignedInt>();
                unsigned myOffset = myDst.size();
                unsigned myVertexDataOffset = theVertexDataOffsets[myRole];

#ifdef TRIANGULATE_QUADS
                // Triangulate quads
                if (mySrcElements->getAttributeString(PROPERTY_TYPE_ATTRIB) == "quads") {
                    myDst.resize(unsigned(1.5 * mySrc.size() + myOffset));
                    for (unsigned k = 0; k < mySrc.size(); k += 4) {
                        myDst[myOffset++] = mySrc[k + 0] + myVertexDataOffset;
                        myDst[myOffset++] = mySrc[k + 1] + myVertexDataOffset;
                        myDst[myOffset++] = mySrc[k + 3] + myVertexDataOffset;
                        myDst[myOffset++] = mySrc[k + 1] + myVertexDataOffset;
                        myDst[myOffset++] = mySrc[k + 2] + myVertexDataOffset;
                        myDst[myOffset++] = mySrc[k + 3] + myVertexDataOffset;
                    }
                } else {
#endif
                    myDst.resize(mySrc.size() + myOffset);


                    if (theFlipFlag) {
                        for (unsigned k = 0; k < mySrc.size(); ++k) {
                            unsigned mySrcIndex = k + myVerticesPerPrimitive - 2 * (k % myVerticesPerPrimitive) - 1;
                            myDst[myOffset + k] = mySrc[mySrcIndex] + myVertexDataOffset;
                        }
                    } else {
                        for (unsigned k = 0; k < mySrc.size(); ++k) {
                            myDst[myOffset + k] = mySrc[k] + myVertexDataOffset;
                        }
                    }
#ifdef TRIANGULATE_QUADS
                }
#endif

                myDstIndex->firstChild()->nodeValueRefClose<VectorOfUnsignedInt>();
            }
        }
    }

    bool
    SceneOptimizer::mergeBodies(dom::NodePtr theNode, const Matrix4f & theInitialMatrix) {
        // Search depth first, to avoid removing parents with unmerged children
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            NodePtr myChild = theNode->childNode(i);
            if (mergeBodies(myChild, theInitialMatrix)) {
                myChild->parentNode()->removeChild(myChild);
                --i;
            }
        }

        if (theNode->nodeName() == BODY_NODE_NAME) {
            BodyPtr myBody = theNode->getFacade<Body>();
            AC_INFO << "    Merge body: " + myBody->get<NameTag>();
            Shape & myShape = myBody->getShape();
            RoleMap myRoles;

            // Collect vertexdata used by primitives
            NodePtr myPrimitives = myShape.getNode().childNode(PRIMITIVE_LIST_NAME);
            for (unsigned i = 0; i < myPrimitives->childNodesLength(); ++i) {
                for (unsigned j = 0; j < myPrimitives->childNode(i)->childNodesLength(); ++j) {
                    string myName = myPrimitives->childNode(i)->childNode(j)->getAttributeString(VERTEX_DATA_ATTRIB);
                    string myRole = myPrimitives->childNode(i)->childNode(j)->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
                    myRoles[myName] = myRole;
                }
            }
            Matrix4f myMatrix = theInitialMatrix;
            myMatrix.postMultiply(myBody->get<GlobalMatrixTag>());
            bool myFlipFlag = ((myMatrix[0][0] * myMatrix[1][1] * myMatrix[2][2]) < 0);
            std::string myRenderStyle = as_string(myShape.get<RenderStyleTag>());
            if (myRenderStyle == "[backfacing]") {
                myRenderStyle = "[frontfacing]";
                myFlipFlag = !myFlipFlag;
            }

            if (_myShapes.find(myRenderStyle) == _myShapes.end()) {
                _myShapes[myRenderStyle] = SuperShapePtr(new SuperShape(_myScene, myRenderStyle));
            }
            SuperShapePtr mySuperShape = _myShapes[myRenderStyle];

            VertexDataMap myVertexDataOffsets;
            mergeVertexData(myShape, mySuperShape, myMatrix, myRoles, myVertexDataOffsets);
            mergePrimitives(myShape, mySuperShape, myFlipFlag, myVertexDataOffsets);

            return true;
        }

        return false;
    }

    void
    SceneOptimizer::removeInvisibleNodes(dom::NodePtr theNode) {
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            NodePtr myChild = theNode->childNode(i);
            if (myChild->getAttributeValue<bool>(VISIBLE_ATTRIB) ||
                myChild->nodeName() == "light")
            {
                removeInvisibleNodes(myChild);
            } else {
                myChild->parentNode()->removeChild(myChild);
                --i;
            }
        }
    }

    void
    SceneOptimizer::collectShapeIds(dom::NodePtr theNode, std::set<std::string> & theIds) {
        if (theNode->nodeName() == "body") {
            theIds.insert(theNode->getAttributeString(BODY_SHAPE_ATTRIB));
        }

        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            collectShapeIds(theNode->childNode(i), theIds);
        }
    }

    void
    SceneOptimizer::removeUnusedShapes() {
        set<string> myShapeIds;
        collectShapeIds(_myScene.getWorldRoot(), myShapeIds);

        NodePtr myShapes = _myScene.getShapesRoot();
        for (unsigned i = 0; i < myShapes->childNodesLength(); ++i) {
            NodePtr myShape = myShapes->childNode(i);
            if (myShapeIds.find(myShape->getAttributeString(ID_ATTRIB)) == myShapeIds.end()) {
                myShapes->removeChild(myShape);
                --i;
            }
        }
    }
}
