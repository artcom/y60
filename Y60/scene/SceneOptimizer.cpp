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
#include "TransformBuilder.h"
#include <asl/Logger.h>
#include <y60/NodeNames.h>

namespace y60 {

    SceneOptimizer::SceneOptimizer(Scene & theScene) :
        _myScene(theScene)
    {}

    SceneOptimizer::SuperShape::SuperShape(Scene & theScene) {
        RenderStyles myDefaultRenderStyles;
        myDefaultRenderStyles.set(FRONT);
        myDefaultRenderStyles.set(BACK);

        _myShape = theScene.getShapesRoot()->appendChild(dom::Element(SHAPE_NODE_NAME));
        _myShape->appendAttribute(NAME_ATTRIB, "Supershape");
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
    SceneOptimizer::SuperShape::getPrimitive(const std::string & theType, const std::string & theMaterial, const RenderStyles & theRenderStyles) {
        std::string myKey = theType + "_" + theMaterial + "_" + as_string(theRenderStyles);

        if (_myPrimitiveMap.find(myKey) == _myPrimitiveMap.end()) {
            dom::Element myNewElements(ELEMENTS_NODE_NAME);
            myNewElements.appendAttribute(PRIMITIVE_TYPE_ATTRIB, theType);
            myNewElements.appendAttribute(MATERIAL_REF_ATTRIB, theMaterial);
            myNewElements.appendAttribute(RENDER_STYLE_ATTRIB, theRenderStyles);
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
            dom::Element myNewIndex(VERTEX_INDICES_NAME);
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
        //AC_PRINT << "Running optimizer";
        AC_INFO << "Running Scene optimizer";
        if (!theRootNode) {
            theRootNode = _myScene.getWorldRoot();
        }
        _myStickyNodes.push_back(theRootNode);

        // check whether the given root node is of the transform hierarchy
        try {
            theRootNode->getFacade<TransformHierarchyFacade>();
        } catch (...) {
            throw;
        }

        // remove invisible nodes
        AC_INFO << "  Removing invisible nodes...";
        removeInvisibleNodes(theRootNode);

        // optimize all sticky nodes until the list is empty
        while (!_myStickyNodes.empty()) {
            dom::NodePtr myRootNode = _myStickyNodes[0];
            _myStickyNodes.erase(_myStickyNodes.begin());
            runNode(myRootNode);
        }

        // remove unused shapes
        AC_INFO << "  Removing unused shapes...";
        removeUnusedShapes();
        AC_INFO << "  optimizing done...";
    }

    void
    SceneOptimizer::runNode(dom::NodePtr theRootNode) {
        // reset the supershape
        _mySuperShape = SuperShapePtr(0);

        // merge all bodies into one superbody
        AC_INFO << "  Merging bodies...";
        asl::Matrix4f myInitialMatrix = theRootNode->getFacade<TransformHierarchyFacade>()->get<InverseGlobalMatrixTag>();
        mergeBodies(theRootNode, myInitialMatrix);

        // remove empty transforms and
        // merge transforms with single childs with their child
        AC_INFO << "  Cleanup Scene...";
        cleanupScene(theRootNode);

        // create a superbody and append the supershape
        if (_mySuperShape != 0) {
            dom::NodePtr mySuperBody;
            mySuperBody = theRootNode->appendChild(dom::Element(BODY_NODE_NAME));
            mySuperBody->appendAttribute(NAME_ATTRIB, "Optimized Body");
            mySuperBody->appendAttribute(BODY_SHAPE_ATTRIB, _mySuperShape->getShapeId());
        }
    }

    void
    SceneOptimizer::transformToParent(dom::NodePtr theParent, dom::NodePtr theChild) {
        //XXX: just curious why theChild->parentNode() doesn't work
        //if (theChild) {
        //    AC_PRINT << "theChild node exists: '" << theChild->nodeName() << "'";
        //} else {
        //    AC_PRINT << "theChild node doesn't exist";
        //}

        //if (theParent) {
        //    AC_PRINT << "theParent exists: '" << theParent->nodeName() << "'";
        //} else {
        //    AC_PRINT << "theParent doesn't exist";
        //}

        //if (theChild->parentNode()) {
        //    AC_PRINT << "parentNode exists: '" << theChild->parentNode()->nodeName() << "'";
        //} else {
        //    AC_PRINT << "parentNode doesn't exist";
        //}

        // get the inverse localmatrix of the parent
        TransformHierarchyFacadePtr myFacade = theParent->getFacade<TransformHierarchyFacade>();
        asl::Matrix4f myMatrix               = asl::inverse(myFacade->get<LocalMatrixTag>());
        asl::Vector3f myScale;
        asl::Vector3f myShear;
        asl::Quaternionf myOrientation;
        asl::Vector3f myPosition;
        myMatrix.decompose(myScale, myShear, myOrientation, myPosition);

        // transform the node
        theChild->getAttribute(SCALE_ATTRIB)->nodeValueAssign<asl::Vector3f>(myScale);
        theChild->getAttribute(ORIENTATION_ATTRIB)->nodeValueAssign<asl::Quaternionf>(myOrientation);
        theChild->getAttribute(POSITION_ATTRIB)->nodeValueAssign<asl::Vector3f>(myPosition);
    }

    void
    SceneOptimizer::cleanupScene(dom::NodePtr theNode) {
        unsigned myNumChildren = theNode->childNodesLength();
        for (signed i = myNumChildren - 1; i >= 0; --i) {
            dom::NodePtr myChild = theNode->childNode(i);

            // don't clean up when sticky
            if (myChild->getAttributeValue<bool>(STICKY_ATTRIB)) {
                return;
            }

            // search depth first
            cleanupScene(myChild);

            std::string myParentNodeName = theNode->nodeName();
            std::string myNodeName       = myChild->nodeName();

            //XXX: untested
            //// make an included world node a transform node
            //// and merge it with its include node
            //if (myNodeName == WORLD_NODE_NAME && myParentNodeName == INCLUDE_NODE_NAME) {
            //    // create a new transform node
            //    y60::TransformBuilder myTransform(myChild->getAttributeString(NAME_ATTRIB));
            //    dom::NodePtr myTransformNode = myTransform.getNode();

            //    // cycle through all transform node attributes
            //    unsigned myNumTransformAttributes = myTransformNode->attributesLength();
            //    for (unsigned j = 0; j < myNumTransformAttributes; ++j) {
            //        dom::NodePtr myTransformAttributeNode = myTransformNode->getAttribute(j);
            //        std::string myAttributeName           = myTransformAttributeNode->nodeName();
            //        dom::NodePtr myWorldAttributeNode     = myChild->getAttribute(myAttributeName);
            //        if (myWorldAttributeNode) {
            //            myTransformAttributeNode->nodeValue(myWorldAttributeNode->nodeValue());
            //        } else {
            //            throw asl::Exception("A world node doesn't know about the attribute '" + myAttributeName + "'",  PLUS_FILE_LINE);
            //        }
            //    }

            //    // remove world and append transform
            //    theNode->replaceChild(myTransformNode, myChild);
            //}

            // remove empty transforms, includes and worlds
            unsigned myNumGrandChildren = myChild->childNodesLength();
            if (myNumGrandChildren == 0 &&
                (myNodeName == TRANSFORM_NODE_NAME ||
                 myNodeName == INCLUDE_NODE_NAME ||
                 myNodeName == WORLD_NODE_NAME))
            {
                theNode->removeChild(myChild);
            }

            //XXX: untested
            //// merge nodes with their single child
            //if (myNumChildren == 1) {
            //    // include or transform node => becomes child node
            //    if (myParentNodeName == INCLUDE_NODE_NAME || myNodeName == TRANSFORM_NODE_NAME) {
            //        transformToParent(theNode, myChild);

            //        // replace the parent node with the child node
            //        theNode->parentNode()->replaceChild(myChild, theNode);
            //    }

            //    // light or camera or projector node => takeover children of transform nodes
            //    else if ((myParentNodeName == LIGHT_NODE_NAME || myParentNodeName == CAMERA_NODE_NAME || myParentNodeName == PROJECTOR_NODE_NAME) &&
            //             myNodeName == TRANSFORM_NODE_NAME) {
            //        unsigned myNumGrandChildren = myChild->childNodesLength();
            //        for (unsigned j = 0; j < myNumGrandChildren; ++j) {
            //            dom::NodePtr myGrandChild = myChild->childNode(j);
            //            transformToParent(myChild, myGrandChild);

            //            // append the grandchild to the parent
            //            theNode->appendChild(myGrandChild);
            //        }

            //        // remove the child node
            //        theNode->removeChild(myChild);
            //    }

            //    // world node with worlds parent or
            //    // body node without children or
            //    // light/camera/projector node with light/camera/projector child node
            //    // => do nothing
            //    else if (myParentNodeName == WORLD_NODE_NAME && theNode->parentNode()->nodeName() == WORLD_LIST_NAME ||
            //             myParentNodeName == BODY_NODE_NAME && myNumGrandChildren == 0 ||
            //             (myParentNodeName == LIGHT_NODE_NAME || myParentNodeName == CAMERA_NODE_NAME || myParentNodeName == PROJECTOR_NODE_NAME) &&
            //             (      myNodeName == LIGHT_NODE_NAME ||       myNodeName == CAMERA_NODE_NAME ||       myNodeName == PROJECTOR_NODE_NAME))
            //    {
            //        // allowed, but nothing to do
            //    }

            //    // unknown or unallowed configuration => exception
            //    else {
            //        throw asl::Exception("Unknown configuration: '" + myParentNodeName + "' node with child '" + myNodeName + "' node", PLUS_FILE_LINE);
            //    }
            //}
        }
    }

    // Specialization for positions and normals
    unsigned
    SceneOptimizer::transformVertexData(dom::NodePtr theSrcVertexData, dom::NodePtr theDstVertexData,
                                        bool theFlipFlag, asl::Matrix4f theMatrix, bool theNormalFlag) {
        const VectorOfVector3f & mySrc = theSrcVertexData->firstChild()->nodeValueRef<VectorOfVector3f>();
        VectorOfVector3f & myDst       = theDstVertexData->firstChild()->nodeValueRefOpen<VectorOfVector3f>();
        unsigned myOffset              = myDst.size();

        myDst.resize(mySrc.size() + myOffset);

        unsigned mySrcSize = mySrc.size();
        if (theNormalFlag) {
            float myScaleSign = theMatrix[0][0] * theMatrix[1][1] * theMatrix[2][2];
            myScaleSign = theFlipFlag ? -1.f : 1.f;
            for (unsigned j = 0; j < mySrcSize; ++j) {
                myDst[myOffset + j] = transformedNormal(mySrc[j], theMatrix) * myScaleSign;
            }
        } else {
            for (unsigned j = 0; j < mySrcSize; ++j) {
               myDst[myOffset + j] = product(asPoint(mySrc[j]), theMatrix);
            }
        }
        theDstVertexData->firstChild()->nodeValueRefClose<VectorOfVector3f>();
        return myOffset;
    }

    void
    SceneOptimizer::mergeVertexData(const Shape & theShape, bool theFlipFlag, const asl::Matrix4f & theMatrix,
                                    const RoleMap & theRoles, VertexDataMap & theVertexDataOffsets) {
        dom::NodePtr myVertexData = theShape.getNode().childNode(VERTEX_DATA_NAME);
        for (unsigned i = 0; i < myVertexData->childNodesLength(); ++i) {
            dom::NodePtr mySrcVertexData = myVertexData->childNode(i);

            // Skip unused vertex data
            RoleMap::const_iterator it = theRoles.find(mySrcVertexData->getAttributeString(NAME_ATTRIB));
            if (it == theRoles.end()) {
                continue;
            }

            std::vector<std::string> myRoles = it->second;
            unsigned myRolesSize = myRoles.size();
            for (unsigned j = 0; j < myRolesSize; j++) {
                const std::string & myRole = myRoles[j];
                dom::NodePtr myDstVertexData = _mySuperShape->getVertexData(mySrcVertexData->nodeName(), myRole);
                TypeId myTypeId;
                myTypeId.fromString(mySrcVertexData->nodeName());

                // position
                if (myRole == POSITION_ROLE && myTypeId == VECTOR_OF_VECTOR3F) {
                    theVertexDataOffsets[myRole] = transformVertexData(mySrcVertexData, myDstVertexData, theFlipFlag, theMatrix);
                }

                // normal
                else if (myRole == NORMAL_ROLE && myTypeId == VECTOR_OF_VECTOR3F) {
                    theVertexDataOffsets[myRole] = transformVertexData(mySrcVertexData, myDstVertexData, theFlipFlag, theMatrix, true);
                }

                // color
                else if (myRole == COLOR_ROLE && myTypeId == VECTOR_OF_VECTOR4F) {
                    theVertexDataOffsets[myRole] = copyVertexData<asl::Vector4f>(mySrcVertexData, myDstVertexData);
                }

                // texcoords
                else if ((myRole == TEXCOORD0_ROLE || myRole == TEXCOORD1_ROLE || myRole == TEXCOORD2_ROLE || myRole == TEXCOORD3_ROLE ||
                          myRole == TEXCOORD4_ROLE || myRole == TEXCOORD5_ROLE || myRole == TEXCOORD6_ROLE || myRole == TEXCOORD7_ROLE) &&
                         myTypeId == VECTOR_OF_VECTOR2F) {
                    theVertexDataOffsets[myRole] = copyVertexData<asl::Vector2f>(mySrcVertexData, myDstVertexData);
                }

                // joint weights
                else if ((myRole == JOINT_WEIGHTS_ROLE_0 || myRole == JOINT_WEIGHTS_ROLE_1) && myTypeId == VECTOR_OF_VECTOR4F) {
                    theVertexDataOffsets[myRole] = copyVertexData<asl::Vector4f>(mySrcVertexData, myDstVertexData);
                }

                // joint indices
                else if ((myRole == JOINT_INDICES_ROLE_0 || myRole == JOINT_INDICES_ROLE_1) && myTypeId == VECTOR_OF_VECTOR4F) {
                    theVertexDataOffsets[myRole] = copyVertexData<asl::Vector4i>(mySrcVertexData, myDstVertexData);
                }

                // not yet implemented
                else {
                    throw asl::NotYetImplemented(std::string("Can not copy vertex data with role '") + myRole + "' and type '" +
                                                 getStringFromEnum(myTypeId, TypeIdStrings) + "'", PLUS_FILE_LINE);
                }
            }
        }
    }

    void
    SceneOptimizer::mergePrimitives(const Shape & theShape, bool theFlipFlag, VertexDataMap & theVertexDataOffsets, const RenderStyles & theRenderStyles) {
        dom::NodePtr myPrimitives = theShape.getNode().childNode(PRIMITIVE_LIST_NAME);

        for (unsigned i = 0; i < myPrimitives->childNodesLength(); ++i) {
            dom::NodePtr mySrcElements = myPrimitives->childNode(i);

            // get element type
            std::string myElementType = mySrcElements->getAttributeString(PRIMITIVE_TYPE_ATTRIB);
            if (myElementType == PRIMITIVE_TYPE_QUADS) {
                myElementType = PRIMITIVE_TYPE_TRIANGLES;
            }

            // get element material ref
            std::string myMaterialRef = mySrcElements->getAttributeString(MATERIAL_REF_ATTRIB);

            // get element renderstyles
            dom::NodePtr myAttribute = mySrcElements->getAttribute(RENDER_STYLE_ATTRIB);
            if (!myAttribute) {
                myAttribute = mySrcElements->appendAttribute(RENDER_STYLE_ATTRIB, theRenderStyles);
            }
            RenderStyles & myRenderStyles = myAttribute->nodeValueRefOpen<RenderStyles>();
            bool myFlipFlag               = theFlipFlag;
            if (myRenderStyles[BACK] && myRenderStyles.count() == 1) {
                myRenderStyles[BACK]  = false;
                myRenderStyles[FRONT] = true;
                myFlipFlag            = !myFlipFlag;
            }
            myAttribute->nodeValueRefClose<RenderStyles>();

            PrimitiveCachePtr myDstElements = _mySuperShape->getPrimitive(myElementType, myMaterialRef, myRenderStyles);
            unsigned myNumSrcElements       = mySrcElements->childNodesLength();
            for (unsigned j = 0; j < myNumSrcElements; ++j) {
                dom::NodePtr mySrcIndex = mySrcElements->childNode(j);
                std::string myName      = mySrcIndex->getAttributeString(VERTEX_DATA_ATTRIB);
                std::string myRole      = mySrcIndex->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
                dom::NodePtr myDstIndex = myDstElements->getIndex(myName, myRole);

                const VectorOfUnsignedInt & mySrc = mySrcIndex->firstChild()->nodeValueRef<VectorOfUnsignedInt>();
                VectorOfUnsignedInt & myDst       = myDstIndex->firstChild()->nodeValueRefOpen<VectorOfUnsignedInt>();
                unsigned myOffset                 = myDst.size();
                unsigned myVertexDataOffset       = theVertexDataOffsets[myRole];

                // Triangulate quads
                if (mySrcElements->getAttributeString(PROPERTY_TYPE_ATTRIB) == PRIMITIVE_TYPE_QUADS) {
                    myDst.resize(unsigned(1.5 * mySrc.size() + myOffset));
                    unsigned mySrcSize = mySrc.size();
                    for (unsigned k = 0; k < mySrcSize; k += 4) {
                        if (myFlipFlag) {
                            myDst[myOffset++] = mySrc[k + 0] + myVertexDataOffset;
                            myDst[myOffset++] = mySrc[k + 3] + myVertexDataOffset;
                            myDst[myOffset++] = mySrc[k + 1] + myVertexDataOffset;
                            myDst[myOffset++] = mySrc[k + 1] + myVertexDataOffset;
                            myDst[myOffset++] = mySrc[k + 3] + myVertexDataOffset;
                            myDst[myOffset++] = mySrc[k + 2] + myVertexDataOffset;
                        } else {
                            myDst[myOffset++] = mySrc[k + 0] + myVertexDataOffset;
                            myDst[myOffset++] = mySrc[k + 1] + myVertexDataOffset;
                            myDst[myOffset++] = mySrc[k + 3] + myVertexDataOffset;
                            myDst[myOffset++] = mySrc[k + 1] + myVertexDataOffset;
                            myDst[myOffset++] = mySrc[k + 2] + myVertexDataOffset;
                            myDst[myOffset++] = mySrc[k + 3] + myVertexDataOffset;
                        }
                    }
                } else {
                    unsigned myVerticesPerPrimitive = getVerticesPerPrimitive(Primitive::getTypeFromNode(mySrcElements));
                    myDst.resize(mySrc.size() + myOffset);

                    unsigned mySrcSize = mySrc.size();
                    if (myFlipFlag) {
                        for (unsigned k = 0; k < mySrcSize; ++k) {
                            unsigned mySrcIndex = k + myVerticesPerPrimitive - 2 * (k % myVerticesPerPrimitive) - 1;
                            myDst[myOffset + k] = mySrc[mySrcIndex] + myVertexDataOffset;
                        }
                    } else {
                        for (unsigned k = 0; k < mySrcSize; ++k) {
                            myDst[myOffset + k] = mySrc[k] + myVertexDataOffset;
                        }
                    }
                }

                myDstIndex->firstChild()->nodeValueRefClose<VectorOfUnsignedInt>();
            }
        }
    }

    bool
    SceneOptimizer::mergeBodies(dom::NodePtr theNode, const asl::Matrix4f & theInitialMatrix) {
        // Search depth first, to avoid removing parents with unmerged children
        unsigned myNumChildren = theNode->childNodesLength();
        for (signed i = myNumChildren - 1; i >= 0; --i) {
            dom::NodePtr myChild = theNode->childNode(i);
            if (myChild->getAttributeValue<bool>(STICKY_ATTRIB)) {
                _myStickyNodes.push_back(myChild);
            } else if (mergeBodies(myChild, theInitialMatrix)) {
                theNode->removeChild(myChild);
            }
        }

        if (theNode->nodeName() == BODY_NODE_NAME) {
            BodyPtr myBody = theNode->getFacade<Body>();
            AC_INFO << "    Merge body: " + myBody->get<NameTag>();
            Shape & myShape = myBody->getShape();
            RoleMap myRoles;

            // Collect vertexdata used by primitives
            dom::NodePtr myPrimitives = myShape.getNode().childNode(PRIMITIVE_LIST_NAME);
            unsigned myNumElements = myPrimitives->childNodesLength();
            for (unsigned i = 0; i < myNumElements; ++i) {
                dom::NodePtr myElement = myPrimitives->childNode(i);
                unsigned myNumIndices  = myElement->childNodesLength();
                for (unsigned j = 0; j < myNumIndices; ++j) {
                    dom::NodePtr myIndices = myElement->childNode(j);
                    std::string myName     = myIndices->getAttributeString(VERTEX_DATA_ATTRIB);
                    std::string myRole     = myIndices->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
                    myRoles[myName].push_back(myRole);
                }
            }
            asl::Matrix4f myMatrix = myBody->get<GlobalMatrixTag>();
            myMatrix.postMultiply(theInitialMatrix);
            bool myFlipFlag = ((myMatrix[0][0] * myMatrix[1][1] * myMatrix[2][2]) < 0);
            RenderStyles myRenderStyles = myShape.get<RenderStyleTag>();

            if (_mySuperShape == 0) {
                _mySuperShape = SuperShapePtr(new SuperShape(_myScene));
            }

            VertexDataMap myVertexDataOffsets;
            mergeVertexData(myShape, myFlipFlag, myMatrix, myRoles, myVertexDataOffsets);
            mergePrimitives(myShape, myFlipFlag, myVertexDataOffsets, myRenderStyles);

            return true;
        }

        return false;
    }

    void
    SceneOptimizer::removeInvisibleNodes(dom::NodePtr theNode) {
        unsigned myNumChildren = theNode->childNodesLength();
        for (signed i = myNumChildren - 1; i >= 0; --i) {
            dom::NodePtr myChild = theNode->childNode(i);
            std::string myChildNodeName = myChild->nodeName();
            if (!myChild->getAttributeValue<bool>(VISIBLE_ATTRIB) && !myChild->getAttributeValue<bool>(STICKY_ATTRIB)) {
                theNode->removeChild(myChild);
            } else {
                removeInvisibleNodes(myChild);
            }
        }
    }

    void
    SceneOptimizer::collectShapeIds(dom::NodePtr theNode, std::set<std::string> & theIds) {
        if (theNode->nodeName() == BODY_NODE_NAME) {
            theIds.insert(theNode->getAttributeString(BODY_SHAPE_ATTRIB));
        }

        unsigned myNumChildren = theNode->childNodesLength();
        for (unsigned i = 0; i < myNumChildren; ++i) {
            collectShapeIds(theNode->childNode(i), theIds);
        }
    }

    void
    SceneOptimizer::removeUnusedShapes() {
        std::set<std::string> myShapeIds;
        collectShapeIds(_myScene.getWorldRoot(), myShapeIds);

        dom::NodePtr myShapes = _myScene.getShapesRoot();
        unsigned myNumShapes  = myShapes->childNodesLength();
        for (signed i = myNumShapes - 1; i >= 0; --i) {
            dom::NodePtr myShape = myShapes->childNode(i);
            if (myShapeIds.find(myShape->getAttributeString(ID_ATTRIB)) == myShapeIds.end()) {
                myShapes->removeChild(myShape);
            }
        }
    }
}
