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
        _myShapeNode = theScene.getShapesRoot()->appendChild(dom::Element(SHAPE_NODE_NAME));
        _myShapeNode->appendAttribute(NAME_ATTRIB, "Supershape");
        _myShapeNode->appendAttribute(ID_ATTRIB, IdTag::getDefault());
        _myShapeNode->appendChild(dom::Element(VERTEX_DATA_NAME));
        _myShapeNode->appendChild(dom::Element(PRIMITIVE_LIST_NAME));
    }

    dom::NodePtr
    SceneOptimizer::SuperShape::getVertexData(const std::string & theType, const std::string & theName) {
        if (_myVertexDataMap.find(theName) == _myVertexDataMap.end()) {
            dom::Element myNewVertexData(theType);
            myNewVertexData.appendAttribute(NAME_ATTRIB, theName);
            myNewVertexData.appendChild(dom::Text("[]"));
            _myVertexDataMap[theName] = _myShapeNode->childNode(VERTEX_DATA_NAME)->appendChild(myNewVertexData);
        }

        return _myVertexDataMap[theName];
    }

    SceneOptimizer::PrimitiveCachePtr
    SceneOptimizer::SuperShape::getPrimitive(const std::string & theType, const std::string & theMaterial, const RenderStyles & theRenderStyles) {
        std::string myKey = theType + "_" + theMaterial + "_" + as_string(theRenderStyles);

        PrimitiveCachePtr myPrimitiveCache(0);
        if (_myPrimitiveMap.find(myKey) == _myPrimitiveMap.end()) {
            dom::Element myPrimitive(ELEMENTS_NODE_NAME);
            myPrimitive.appendAttribute(PRIMITIVE_TYPE_ATTRIB, theType);
            myPrimitive.appendAttribute(MATERIAL_REF_ATTRIB, theMaterial);
            myPrimitive.appendAttribute(RENDER_STYLE_ATTRIB, theRenderStyles);
            dom::NodePtr myPrimitiveNode = _myShapeNode->childNode(PRIMITIVE_LIST_NAME)->appendChild(myPrimitive);
            myPrimitiveCache = PrimitiveCachePtr(new PrimitiveCache(myPrimitiveNode));
            if (theType == PRIMITIVE_TYPE_TRIANGLES) {
                _myPrimitiveMap[myKey] = myPrimitiveCache;
            } else if (theType == PRIMITIVE_TYPE_LINE_STRIP) {
                // nothing to do
            } else {
                throw asl::NotYetImplemented(std::string("The optimizer doesn't understand primitives of type '") + theType + "' yet",
                                             PLUS_FILE_LINE);
            }
        } else {
            myPrimitiveCache = PrimitiveCachePtr(_myPrimitiveMap[myKey]);
        }

        return myPrimitiveCache;
    }

    std::string
    SceneOptimizer::SuperShape::getShapeId() const {
        return _myShapeNode->getAttributeString(ID_ATTRIB);
    }

    dom::NodePtr
    SceneOptimizer::SuperShape::getShapeNode() {
        return _myShapeNode;
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
    SceneOptimizer::run() {
        run(_myScene.getWorldRoot());
    }

    void
    SceneOptimizer::run(const dom::NodePtr & theRootNode) {
        AC_INFO << "Running Scene optimizer";

        _myRootNode = theRootNode;

        if (! _myRootNode) {
             _myRootNode = _myScene.getWorldRoot();
        }

        // Check whether the given root node is of the transform hierarchy
        TransformHierarchyFacadePtr myFacade(0);
        try {
            myFacade =  _myRootNode->getFacade<TransformHierarchyFacade>();
        } catch (...) {
            throw;
        }
        _myStickyNodes.push_back(_myRootNode);

        // Remove invisible nodes
        AC_INFO << "  Removing invisible nodes...";
        removeInvisibleNodes(_myRootNode);

        // Set nodes with animations on it to sticky
        AC_INFO << "  Set animated nodes sticky...";
        pinAnimatedNodes(_myRootNode);

        // Pin body nodes that reference the same shape node (memory friendly)
        //XXX: commented out for now, maybe add a switch to be able to turn it off,
        //     but it's not that useful anyway
        //pinBodiesWithSameShapes(_myRootNode);

        // Optimize all sticky nodes until the list is empty
        while (!_myStickyNodes.empty()) {
            dom::NodePtr myRootNode = _myStickyNodes[0];
            runNode(myRootNode);
            _myStickyNodes.erase(_myStickyNodes.begin());
        }

        // Remove empty transforms and
        // merge transforms with single childs with their child
        AC_INFO << "  Cleanup scene...";
        cleanupScene(_myRootNode);

        // Remove unused shapes
        AC_INFO << "  Removing unused shapes...";
        removeUnusedShapes();
        AC_INFO << "  Optimizing done!";
    }

    void
    SceneOptimizer::runNode(dom::NodePtr & theRootNode) {
        // Reset the supershape
        _mySuperShape = SuperShapePtr(0);

        // Merge all bodies into one superbody
        AC_INFO << "  Merging bodies...";
        asl::Matrix4f myInitialMatrix = theRootNode->getFacade<TransformHierarchyFacade>()->get<InverseGlobalMatrixTag>();
        mergeBodies(theRootNode, myInitialMatrix);

        // Create a superbody and append the supershape
        // or reuse the rootnode, if it is a body
        if (_mySuperShape != 0) {
            AC_INFO << "  Append superbody/supershape...";
            if (theRootNode->nodeName() == BODY_NODE_NAME) {
                dom::NodePtr myShapeAttributeNode = theRootNode->getAttribute(BODY_SHAPE_ATTRIB);
                myShapeAttributeNode->nodeValue(_mySuperShape->getShapeId());
            } else {
                dom::NodePtr myBodyNode = theRootNode->appendChild(dom::Element(BODY_NODE_NAME));
                myBodyNode->appendAttribute(NAME_ATTRIB, "Optimized Body");
                myBodyNode->appendAttribute(ID_ATTRIB, IdTag::getDefault());
                myBodyNode->appendAttribute(BODY_SHAPE_ATTRIB, _mySuperShape->getShapeId());
            }
        }
    }

    void
    SceneOptimizer::transformToParent(dom::NodePtr & theNode) {
        dom::NodePtr myParentNode = theNode->parentNode()->self().lock();

        // Get the inverse localmatrix of the node
        TransformHierarchyFacadePtr myFacade       = theNode->getFacade<TransformHierarchyFacade>();
        TransformHierarchyFacadePtr myParentFacade = myParentNode->getFacade<TransformHierarchyFacade>();
        asl::Matrix4f myMatrix                     = asl::product(myFacade->get<LocalMatrixTag>(),
                                                                  myParentFacade->get<LocalMatrixTag>());
        asl::Vector3f myScale;
        asl::Vector3f myShear;
        asl::Quaternionf myOrientation;
        asl::Vector3f myPosition;
        myMatrix.decompose(myScale, myShear, myOrientation, myPosition);

        // Transform the node
        myFacade->set<ScaleTag>(myScale);
        myFacade->set<OrientationTag>(myOrientation);
        myFacade->set<PositionTag>(myPosition);
    }

    void
    SceneOptimizer::cleanupScene(dom::NodePtr & theNode) {
        unsigned myNumChildren = theNode->childNodesLength();
        for (signed i = myNumChildren - 1; i >= 0; --i) {
            dom::NodePtr myChild = theNode->childNode(i);

            // Search depth first
            cleanupScene(myChild);
        }

        dom::NodePtr myParentNode    = theNode->parentNode()->self().lock();
        TransformHierarchyFacadePtr myFacade = theNode->getFacade<TransformHierarchyFacade>();        
        std::string myParentNodeName = myParentNode->nodeName();
        std::string myNodeName       = theNode->nodeName();

        // Remove empty transforms, includes and worlds
        unsigned myCurNumChildren = theNode->childNodesLength();
        if (myCurNumChildren == 0 && !theNode->getAttributeValue<bool>(STICKY_ATTRIB) &&
            (myNodeName == TRANSFORM_NODE_NAME || myNodeName == INCLUDE_NODE_NAME || myNodeName == WORLD_NODE_NAME)) {
            myParentNode->removeChild(theNode);
            return;
        }

        // Make an included world node a transform node
        if (myNodeName == WORLD_NODE_NAME) {
            if (myParentNodeName == INCLUDE_NODE_NAME && !theNode->getAttributeValue<bool>(STICKY_ATTRIB)) {
                convertToTransformNode(theNode);
            }
        }

        // Make an include node a transform node
        // when it doesn't have a world node as child
        else if (myNodeName == INCLUDE_NODE_NAME) {
            if (!theNode->getAttributeValue<bool>(STICKY_ATTRIB)) {
                bool myConvertFlag = true;
                for (unsigned i = 0; i < myCurNumChildren; ++i) {
                    dom::NodePtr myChild = theNode->childNode(i);
                    if (myChild->nodeName() == WORLD_NODE_NAME) {
                        myConvertFlag = false;
                    }
                }
                if (myConvertFlag) {
                    convertToTransformNode(theNode);
                }
            }
        }

        // Merge nodes with their single child
        if (theNode->childNodesLength() == 1) {
            // Read the node name once more, since it could've been changed above
            std::string myParentNodeName = myParentNode->nodeName();
            std::string myNodeName       = theNode->nodeName();
            dom::NodePtr myChildNode     = theNode->childNode(0);
            std::string myChildNodeName  = myChildNode->nodeName();
            unsigned myNumGrandChildren  = myChildNode->childNodesLength();

            // Include or transform node => becomes child node
            if (myNodeName == INCLUDE_NODE_NAME || myNodeName == TRANSFORM_NODE_NAME) {
                if (!theNode->getAttributeValue<bool>(STICKY_ATTRIB)) {
                    transformToParent(myChildNode);

                    // Replace the parent node with the child node
                    myParentNode->replaceChild(myChildNode, theNode);
                }
            }

            // Light or camera or projector node => takeover children of transform nodes
            else if ((myNodeName == LIGHT_NODE_NAME || myNodeName == CAMERA_NODE_NAME || myNodeName == PROJECTOR_NODE_NAME) &&
                     myChildNodeName == TRANSFORM_NODE_NAME)
            {
                if (!myChildNode->getAttributeValue<bool>(STICKY_ATTRIB)) {
                    for (signed j = myNumGrandChildren - 1; j >= 0; --j) {
                        dom::NodePtr myGrandChild = myChildNode->childNode(j);
                        transformToParent(myGrandChild);

                        // Append the grandchild to the parent
                        theNode->appendChild(myGrandChild);
                    }

                    // Remove the child node
                    theNode->removeChild(myChildNode);
                }
            }
        }
    }

    // Specialization for positions and normals
    unsigned
    SceneOptimizer::transformVertexData(dom::NodePtr & theSrcVertexData, dom::NodePtr & theDstVertexData,
                                        bool theFlipFlag, const asl::Matrix4f & theMatrix, bool theNormalFlag) {
        const VectorOfVector3f & mySrc = theSrcVertexData->firstChild()->nodeValueRef<VectorOfVector3f>();
        VectorOfVector3f & myDst       = theDstVertexData->firstChild()->nodeValueRefOpen<VectorOfVector3f>();
        unsigned myOffset              = myDst.size();

        myDst.resize(mySrc.size() + myOffset);

        unsigned mySrcSize = mySrc.size();
        if (theNormalFlag) {
            float myScaleSign = theFlipFlag ? -1.f : 1.f;
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
    SceneOptimizer::mergeVertexData(const dom::NodePtr & theVertexData, bool theFlipFlag, const asl::Matrix4f & theMatrix,
                                    const RoleMap & theRoles, VertexDataMap & theVertexDataOffsets) {
        unsigned myNumVertexData = theVertexData->childNodesLength();
        for (unsigned i = 0; i < myNumVertexData; ++i) {
            dom::NodePtr mySrcVertexData = theVertexData->childNode(i);

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

                // Position
                if (myRole == POSITION_ROLE && myTypeId == VECTOR_OF_VECTOR3F) {
                    theVertexDataOffsets[myRole] = transformVertexData(mySrcVertexData, myDstVertexData, theFlipFlag, theMatrix);
                }

                // Normal
                else if (myRole == NORMAL_ROLE && myTypeId == VECTOR_OF_VECTOR3F) {
                    theVertexDataOffsets[myRole] = transformVertexData(mySrcVertexData, myDstVertexData, theFlipFlag, theMatrix, true);
                }

                // Color
                else if (myRole == COLOR_ROLE && myTypeId == VECTOR_OF_VECTOR4F) {
                    theVertexDataOffsets[myRole] = copyVertexData<asl::Vector4f>(mySrcVertexData, myDstVertexData);
                }

                // Texcoords
                else if ((myRole == TEXCOORD0_ROLE || myRole == TEXCOORD1_ROLE || myRole == TEXCOORD2_ROLE || myRole == TEXCOORD3_ROLE ||
                          myRole == TEXCOORD4_ROLE || myRole == TEXCOORD5_ROLE || myRole == TEXCOORD6_ROLE || myRole == TEXCOORD7_ROLE) &&
                         myTypeId == VECTOR_OF_VECTOR2F) {
                    theVertexDataOffsets[myRole] = copyVertexData<asl::Vector2f>(mySrcVertexData, myDstVertexData);
                }

                // Joint weights
                else if ((myRole == JOINT_WEIGHTS_ROLE_0 || myRole == JOINT_WEIGHTS_ROLE_1) && myTypeId == VECTOR_OF_VECTOR4F) {
                    theVertexDataOffsets[myRole] = copyVertexData<asl::Vector4f>(mySrcVertexData, myDstVertexData);
                }

                // Joint indices
                else if ((myRole == JOINT_INDICES_ROLE_0 || myRole == JOINT_INDICES_ROLE_1) && myTypeId == VECTOR_OF_VECTOR4F) {
                    theVertexDataOffsets[myRole] = copyVertexData<asl::Vector4i>(mySrcVertexData, myDstVertexData);
                }

                // Not yet implemented
                else {
                    throw asl::NotYetImplemented(std::string("Can not copy vertex data with role '") + myRole + "' and type '" +
                                                 getStringFromEnum(myTypeId, TypeIdStrings) + "'", PLUS_FILE_LINE);
                }
            }
        }
    }

    void
    SceneOptimizer::mergePrimitives(const dom::NodePtr & theElements, bool theFlipFlag,
                                    VertexDataMap & theVertexDataOffsets, const RenderStyles & theRenderStyles) {
        // Get element type
        std::string myNewPrimitiveType = theElements->getAttributeString(PRIMITIVE_TYPE_ATTRIB);
        if (myNewPrimitiveType == PRIMITIVE_TYPE_QUADS) {
            myNewPrimitiveType = PRIMITIVE_TYPE_TRIANGLES;
        }

        // Get element material ref
        std::string myMaterialRef = theElements->getAttributeString(MATERIAL_REF_ATTRIB);

        PrimitiveCachePtr myDstElements = _mySuperShape->getPrimitive(myNewPrimitiveType, myMaterialRef, theRenderStyles);
        unsigned myNumSrcElements       = theElements->childNodesLength();
        for (unsigned j = 0; j < myNumSrcElements; ++j) {
            dom::NodePtr mySrcIndex = theElements->childNode(j);
            std::string myName      = mySrcIndex->getAttributeString(VERTEX_DATA_ATTRIB);
            std::string myRole      = mySrcIndex->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
            dom::NodePtr myDstIndex = myDstElements->getIndex(myName, myRole);

            const VectorOfUnsignedInt & mySrc = mySrcIndex->firstChild()->nodeValueRef<VectorOfUnsignedInt>();
            VectorOfUnsignedInt & myDst       = myDstIndex->firstChild()->nodeValueRefOpen<VectorOfUnsignedInt>();
            unsigned myOffset                 = myDst.size();
            unsigned myVertexDataOffset       = theVertexDataOffsets[myRole];

            // Triangulate quads
            std::string myOldPrimitiveType = theElements->getAttributeString(PRIMITIVE_TYPE_ATTRIB);
            if (myOldPrimitiveType == PRIMITIVE_TYPE_QUADS) {
                myDst.resize(unsigned(1.5 * mySrc.size() + myOffset));
                unsigned mySrcSize = mySrc.size();
                for (unsigned k = 0; k < mySrcSize; k += 4) {
                    if (theFlipFlag) {
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
            } else if (myOldPrimitiveType == PRIMITIVE_TYPE_TRIANGLES || myOldPrimitiveType == PRIMITIVE_TYPE_LINE_STRIP) {
                unsigned myVerticesPerPrimitive = getVerticesPerPrimitive(Primitive::getTypeFromNode(theElements));
                myDst.resize(mySrc.size() + myOffset);
                unsigned mySrcSize = mySrc.size();
                if (theFlipFlag) {
                    for (unsigned k = 0; k < mySrcSize; ++k) {
                        //unsigned mySrcIndex = k + myVerticesPerPrimitive - 2 * (k % myVerticesPerPrimitive) - 1;
                        //myDst[myOffset + k] = mySrc[mySrcIndex] + myVertexDataOffset;
                        myDst[myOffset + k] = mySrc[mySrcSize - 1 - k] + myVertexDataOffset;
                    }
                } else {
                    for (unsigned k = 0; k < mySrcSize; ++k) {
                        myDst[myOffset + k] = mySrc[k] + myVertexDataOffset;
                    }
                }
            } else {
                throw asl::NotYetImplemented(std::string("The optimizer doesn't understand primitives of type '") + myOldPrimitiveType + "' yet",
                                             PLUS_FILE_LINE);
            }

            myDstIndex->firstChild()->nodeValueRefClose<VectorOfUnsignedInt>();
        }
    }

    bool
    SceneOptimizer::mergeBodies(dom::NodePtr & theNode, const asl::Matrix4f & theInitialMatrix) {
        // Search depth first
        unsigned myNumChildren = theNode->childNodesLength();
        for (signed i = myNumChildren - 1; i >= 0; --i) {
            dom::NodePtr myChild = theNode->childNode(i);
            if (myChild->getAttributeValue<bool>(STICKY_ATTRIB)) {
                _myStickyNodes.push_back(myChild);
            } else {
                if (mergeBodies(myChild, theInitialMatrix)) {
                    if (myChild->childNodesLength() == 0) {
                        // Remove the merged body if it has no children
                        theNode->removeChild(myChild);
                    } else {
                        // Otherwise change it to a transform node
                        convertToTransformNode(myChild);
                    }
                }
            }
        }

        // Merge the shape into the supershape
        if (theNode->nodeName() == BODY_NODE_NAME && theNode->getAttributeValue<bool>(VISIBLE_ATTRIB)) {
            BodyPtr myBody = theNode->getFacade<Body>();
            AC_INFO << "    Merge body: " + myBody->get<NameTag>();
            Shape & myShape = myBody->getShape();

            // Initialize the supershape if it isn't yet
            if (_mySuperShape == 0) {
                _mySuperShape = SuperShapePtr(new SuperShape(_myScene));
            }

            // Calculate the appropriate matrix
            asl::Matrix4f myMatrix = myBody->get<GlobalMatrixTag>();
            myMatrix.postMultiply(theInitialMatrix);

            // Calculate the flip flag for the normals
            const asl::Vector3f & myXVector = asl::asVector3(myMatrix[0][0]);
            const asl::Vector3f & myYVector = asl::asVector3(myMatrix[1][0]);
            const asl::Vector3f & myZVector = asl::asVector3(myMatrix[2][0]);
            bool myNormalFlipFlag           = dot(myXVector, cross(myYVector, myZVector)) < 0;

            // Get shape renderstyles
            RenderStyles myShapeRenderStyles = myShape.get<RenderStyleTag>();

            // Collect vertexdata used by primitives
            dom::NodePtr myVertexData   = myShape.getNode().childNode(VERTEX_DATA_NAME);
            dom::NodePtr myPrimitives   = myShape.getNode().childNode(PRIMITIVE_LIST_NAME);
            unsigned myNumElements      = myPrimitives->childNodesLength();
            for (unsigned i = 0; i < myNumElements; ++i) {
                dom::NodePtr myElements = myPrimitives->childNode(i);

                // Merge vertex data
                RoleMap myRoles;
                unsigned myNumIndices = myElements->childNodesLength();
                for (unsigned j = 0; j < myNumIndices; ++j) {
                    dom::NodePtr myIndices = myElements->childNode(j);
                    std::string myName     = myIndices->getAttributeString(VERTEX_DATA_ATTRIB);
                    std::string myRole     = myIndices->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
                    myRoles[myName].push_back(myRole);
                }
                VertexDataMap myVertexDataOffsets;
                mergeVertexData(myVertexData, myNormalFlipFlag, myMatrix, myRoles, myVertexDataOffsets);

                // Merge primitives
                RenderStyles myElementsRenderStyles;
                dom::NodePtr myRenderStyleAttr = myElements->getAttribute(RENDER_STYLE_ATTRIB);
                if (!myRenderStyleAttr) {
                    myElementsRenderStyles = myShapeRenderStyles;
                }
                bool myRewindFlag = myNormalFlipFlag;
                if (myElementsRenderStyles[BACK] && !myElementsRenderStyles[FRONT]) {
                    myElementsRenderStyles[BACK]  = false;
                    myElementsRenderStyles[FRONT] = true;
                    myRewindFlag                  = !myRewindFlag;
                }
                mergePrimitives(myElements, myRewindFlag, myVertexDataOffsets, myElementsRenderStyles);
            }

            return true;
        }

        return false;
    }

    void
    SceneOptimizer::removeInvisibleNodes(dom::NodePtr & theNode) {
        // Search depth first
        unsigned myNumChildren = theNode->childNodesLength();
        for (signed i = myNumChildren - 1; i >= 0; --i) {
            dom::NodePtr myChild = theNode->childNode(i);
            removeInvisibleNodes(myChild);
        }

        // Node visible?
        if (theNode->getAttributeValue<bool>(VISIBLE_ATTRIB)) {
            return;
        }

        // Node sticky?
        if(theNode->getAttributeValue<bool>(STICKY_ATTRIB)) {
            return;
        }

        // Node has sticky descendants?
        std::vector<dom::NodePtr> myStickyNodes;
        theNode->getNodesByAttribute("", STICKY_ATTRIB, "1", true, myStickyNodes);
        if (myStickyNodes.size() > 0) {
            return;
        }

        // Search (character) animations
        std::set<std::string> myNodeIds;
        collectIds(theNode, myNodeIds);
        dom::NodePtr mySceneNode      = _myScene.getSceneDom()->firstChild();
        dom::NodePtr myAnimationsNode = mySceneNode->childNode(ANIMATION_LIST_NAME);
        dom::NodePtr myCharactersNode = mySceneNode->childNode(CHARACTER_LIST_NAME);
        std::vector<dom::NodePtr> myAnimationNodes;
        for (std::set<std::string>::const_iterator it = myNodeIds.begin(); it != myNodeIds.end(); it++) {
            std::vector<dom::NodePtr> myAnimationNodes;
            myAnimationsNode->getNodesByAttribute(ANIMATION_NODE_NAME, ANIM_NODEREF_ATTRIB, *it, true, myAnimationNodes);
            myCharactersNode->getNodesByAttribute(ANIMATION_NODE_NAME, ANIM_NODEREF_ATTRIB, *it, true, myAnimationNodes);
        }

        // Remove the animations
        for (unsigned j = 0; j < myAnimationNodes.size(); ++j) {
            myAnimationNodes[j]->parentNode()->removeChild(myAnimationNodes[j]);
        }

        // Then remove the node itself
        theNode->parentNode()->removeChild(theNode);
        AC_INFO << "    Node removed: " << theNode->getAttributeString(NAME_ATTRIB);
    }

    void
    SceneOptimizer::pinAnimatedNodes(dom::NodePtr & theRootNode) {
        // Get all animation nodes
        dom::NodePtr mySceneNode = _myScene.getSceneDom()->firstChild();
        std::vector<dom::NodePtr> myAnimationNodes;
        mySceneNode->getNodesByTagName(ANIMATION_NODE_NAME, true, myAnimationNodes);

        // Collect animated nodes
        std::set<std::string> myNodeIds;
        for (unsigned i = 0; i < myAnimationNodes.size(); ++i) {
            myNodeIds.insert(myAnimationNodes[i]->getAttributeString(ANIM_NODEREF_ATTRIB));
        }

        // Set animated nodes and their parent to sticky
        dom::NodePtr myWorldNode = _myScene.getWorldRoot();
        for (std::set<std::string>::const_iterator it = myNodeIds.begin(); it != myNodeIds.end(); it++) {
            dom::NodePtr myNode   = myWorldNode->getElementById(*it, ID_ATTRIB);
            if (myNode) {
                TransformHierarchyFacadePtr myFacade = myNode->getFacade<TransformHierarchyFacade>();
                myFacade->set<StickyTag>(true);

                dom::NodePtr myParent                      = myNode->parentNode()->self().lock();
                TransformHierarchyFacadePtr myParentFacade = myParent->getFacade<TransformHierarchyFacade>();
                myParentFacade->set<StickyTag>(true);
            }
        }
    }

    void
    SceneOptimizer::pinBodiesWithSameShapes(dom::NodePtr & theRootNode) {
        // Get all body nodes
        std::vector<dom::NodePtr> myBodyNodes;
        theRootNode->getNodesByTagName(BODY_NODE_NAME, true, myBodyNodes);

        // Collect referenced shape nodes
        typedef std::pair<std::string, dom::NodePtr> ShapeMapPair;
        typedef std::multimap<std::string, dom::NodePtr> ShapeMap;
        ShapeMap myShapeMap;
        for (unsigned i = 0; i < myBodyNodes.size(); ++i) {
            myShapeMap.insert(ShapeMapPair(myBodyNodes[i]->getAttributeString(BODY_SHAPE_ATTRIB), myBodyNodes[i]));
        }

        // Set body nodes that reference the same shape to sticky
        typedef std::set<std::string> ShapeMapKeys;
        ShapeMapKeys myShapeMapKeys;
        for (ShapeMap::const_iterator it = myShapeMap.begin(); it != myShapeMap.end(); ++it) {
            myShapeMapKeys.insert((*it).first);
        }

        for (ShapeMapKeys::iterator it = myShapeMapKeys.begin(); it != myShapeMapKeys.end(); ++it) {
            if (myShapeMap.count(*it) > 1) {
                std::pair<ShapeMap::iterator, ShapeMap::iterator> myRange = myShapeMap.equal_range(*it);
                for (ShapeMap::iterator j = myRange.first; j != myRange.second; ++j) {
                    dom::NodePtr myNode = (*j).second;
                    TransformHierarchyFacadePtr myFacade = myNode->getFacade<TransformHierarchyFacade>();
                    myFacade->set<StickyTag>(true);
                }
            }
        }
    }

    void
    SceneOptimizer::collectIds(const dom::NodePtr & theNode, std::set<std::string> & theIds) {
        theIds.insert(theNode->getAttributeString(ID_ATTRIB));

        unsigned myNumChildren = theNode->childNodesLength();
        for (unsigned i = 0; i < myNumChildren; ++i) {
            collectIds(theNode->childNode(i), theIds);
        }
    }

    void
    SceneOptimizer::collectShapeIds(const dom::NodePtr & theNode, std::set<std::string> & theIds) {
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

    void
    SceneOptimizer::convertToTransformNode(dom::NodePtr & theNode) {
        // Create a new transform node
        y60::TransformBuilder myTransform(theNode->getAttributeString(NAME_ATTRIB));
        dom::NodePtr myTransformNode = myTransform.getNode();

        // Cycle through all transform node attributes
        unsigned myNumTransformAttributes = myTransformNode->attributesLength();
        for (unsigned i = 0; i < myNumTransformAttributes; ++i) {
            dom::NodePtr myTransformAttributeNode = myTransformNode->getAttribute(i);
            std::string myAttributeName           = myTransformAttributeNode->nodeName();
            dom::NodePtr myAttributeNode          = theNode->getAttribute(myAttributeName);
            if (myAttributeNode) {
                myTransformAttributeNode->nodeValue(myAttributeNode->nodeValue());
            } else {
                throw asl::Exception("A '" + theNode->nodeName() + "' node doesn't know about the attribute '" + myAttributeName + "'",
                                     PLUS_FILE_LINE);
            }
        }

        // Append the node's children to the transform
        unsigned myNumChildren = theNode->childNodesLength();
        for (signed i = myNumChildren - 1; i >= 0; --i) {
            dom::NodePtr myChild = theNode->childNode(i);
            myTransformNode->appendChild(myChild);
        } 

        // Remove the node and append transform
        theNode->parentNode()->replaceChild(myTransformNode, theNode);
        theNode = myTransformNode;
    }
}

