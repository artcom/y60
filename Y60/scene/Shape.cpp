//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Shape.h"
#include "Scene.h"

using namespace dom;
using namespace std;

namespace y60 {
    
    Scene &
    Shape::getScene() {
        Node * myRoot = getNode().getRootElement();
        return *myRoot->getFacade<Scene>(); 
    }
    const Scene &
    Shape::getScene() const {
        const Node * myRoot = getNode().getRootElement();
        return *myRoot->getFacade<Scene>(); 
    }
    void
    Shape::update() {
        if (getNode().nodeVersion() > getLastRenderVersion()) {
            build();
        }
        if (getNode().nodeVersion() < getLastRenderVersion()) {
            reverseUpdate();
        }
    }
    void
    Shape::registerDependenciesRegistrators() {
        AC_TRACE << "Shape::registerDependenciesRegistrators()";
        BoundingBoxTag::Plug::setReconnectFunction(&Shape::registerDependenciesForBoundingBox);
    }

    void
    Shape::registerDependenciesForBoundingBox() {        
        AC_TRACE << "Shape::registerDependenciesForBoundingBox()";
        if (getNode()) {
            NodePtr myDataNode = getVertexDataNode(POSITION_ROLE);
            ValuePtr myPositions = myDataNode->childNode(0)->nodeValueWrapperPtr();
            BoundingBoxTag::Plug::dependsOn(myPositions);
            ShapePtr mySelf = dynamic_cast_Ptr<Shape>(getSelf());
            BoundingBoxTag::Plug::getValuePtr()->setCalculatorFunction(mySelf, &Shape::calculateBoundingBox);
        AC_TRACE << "Shape::registerDependenciesForBoundingBox() done";
        }
    }
#if 0
    void
    Shape::build() {
        Node & myShapeNode = getNode();
        AC_TRACE << "Shape::build() id='" << get<IdTag>() << "', name='"<<get<NameTag>()<<"', node version="<<myShapeNode.nodeVersion()
                 <<", last renderversion="<<_myLastRenderVersion;

        clear();
        unsigned long myShapeVertexCount = 0;
        ResourceManager * myResourceManager = getScene().getTextureManager()->getResourceManager();

        // Iterate over all indices and find the corresponding data
        NodePtr myPrimitiveListNode = myShapeNode.childNode(PRIMITIVE_LIST_NAME);
        if (myPrimitiveListNode) {
            unsigned myPrimitiveCount = myPrimitiveListNode->childNodesLength(ELEMENTS_NODE_NAME);
            for (unsigned j = 0; j < myPrimitiveCount; ++j) {
                NodePtr myElementsNode = myPrimitiveListNode->childNode(ELEMENTS_NODE_NAME, j);
                std::string myMaterialId = myElementsNode->getAttributeString(MATERIAL_REF_ATTRIB);
                PrimitiveType myPrimitiveType = Primitive::getTypeFromNode(myElementsNode);
                MaterialBaseFacadePtr myMaterial = myElementsNode->getElementById(myMaterialId)->getFacade<MaterialBase>();//getMaterial(myMaterialId);
                if (!myMaterial) {
                    throw ShapeException(std::string("Could not find material with id: ") +
                            myMaterialId, PLUS_FILE_LINE);
                }
                // Make sure elements do not get bigger than 64k
                const unsigned myMaximumElementSize = 1024 * 64;
                unsigned myBegin = 0;
                unsigned myEnd   = 0;
                unsigned myMaxIndexSize = findMaxIndexSize(myElementsNode);
                unsigned myVerticesPerPrimitive = getVerticesPerPrimitive(myPrimitiveType);
                do {
                    if (myVerticesPerPrimitive == 0) {
                        myEnd = myMaxIndexSize;
                    } else {
                        myEnd += myMaximumElementSize - (myMaximumElementSize % myVerticesPerPrimitive);
                        if (myEnd > myMaxIndexSize) {
                            myEnd = myMaxIndexSize;
                        }
                    }

                    Primitive & myPrimitive = createPrimitive(myPrimitiveType, myMaterial, myBegin);

                    // collect renderstyles for this element
                    myPrimitive.getRenderStyles() = myElementsNode->getAttributeValue<RenderStyles>(RENDER_STYLE_ATTRIB,
                            RenderStyles(0));
                    unsigned myIndicesCount = myElementsNode->childNodesLength(VERTEX_INDICES_NAME);
                    for (unsigned k = 0; k < myIndicesCount; ++k) {
                        NodePtr myIndicesNode = myElementsNode->childNode(VERTEX_INDICES_NAME, k);
                        const string & myName = myIndicesNode->getAttributeString(VERTEX_DATA_ATTRIB);
                        NodePtr myDataNode = getVertexDataNode(myName);

                        myPrimitive.load(myResourceManager, myIndicesNode, myDataNode, myBegin, myEnd);
                    }

                    myShapeVertexCount += myPrimitive.size();
                    myBegin = myEnd;
                } while (myEnd < myMaxIndexSize);
            }
        } else {
            throw ShapeException(std::string("Node with name '") +
                    PRIMITIVE_LIST_NAME + "' not found for shape: " +
                    get<IdTag>(), PLUS_FILE_LINE);
        }
        // Set vertex count
        setVertexCount(myShapeVertexCount);
        AC_TRACE << "shape: " << get<IdTag>() << " has " << _myPrimitives.size() << " materials";

        calculateBoundingBox();
        setLastRenderVersion(myShapeNode.nodeVersion());
    }

    unsigned
    Shape::findMaxIndexSize(NodePtr theElementsNode) {
        size_t myLargestSize = 0;
        unsigned myIndicesCount = theElementsNode->childNodesLength(VERTEX_INDICES_NAME);
        for (unsigned i = 0; i < myIndicesCount; ++i) {
            NodePtr myIndicesNode = theElementsNode->childNode(VERTEX_INDICES_NAME, i);
            const VectorOfUnsignedInt & myIndices = myIndicesNode->
                childNode(0)->nodeValueRef<VectorOfUnsignedInt>();

            myLargestSize = asl::maximum(myLargestSize, myIndices.size());
        }
        return myLargestSize;
    }
#else
    void
    Shape::build() {
        AC_TRACE << "Shape::build() id='"<<get<IdTag>()<<"'";
       NodePtr myPrimitiveListNode = getNode().childNode(PRIMITIVE_LIST_NAME);
        if (myPrimitiveListNode) {
            _myPrimitives.resize(0);
            unsigned myPrimitiveCount = myPrimitiveListNode->childNodesLength(ELEMENTS_NODE_NAME);
            for (unsigned j = 0; j < myPrimitiveCount; ++j) {
                NodePtr myElementsNode = myPrimitiveListNode->childNode(ELEMENTS_NODE_NAME, j);
                PrimitivePtr myPrimitive = myElementsNode->getFacade<Primitive>();
                myPrimitive->updateVertexData();
                _myPrimitives.push_back(myPrimitive);
            }
        }
        _myLastRenderVersion = getNode().nodeVersion();
     }
#endif
   void
   Shape::calculateBoundingBox() {
        AC_TRACE << "Shape::calculateBoundingBox()";
        NodePtr myDataNode = getVertexDataNode(POSITION_ROLE);
        const VectorOfVector3f & myPositions =
            myDataNode->childNode(0)->nodeValueRef<VectorOfVector3f>();

        asl::Box3f myBoundingBox;
        myBoundingBox.makeEmpty();
        for (unsigned i = 0; i < myPositions.size(); ++i) {
            myBoundingBox.extendBy(asPoint(myPositions[i]));
        }
        set<BoundingBoxTag>(myBoundingBox);
        AC_TRACE << "Shape::calculateBoundingBox() = "<<myBoundingBox;
    }

    NodePtr
    Shape::getVertexDataNode(const std::string & theDataName) {
        NodePtr myVertexDataListNode = getNode().childNode(VERTEX_DATA_NAME);
        if (myVertexDataListNode) {
            for (unsigned i = 0; i < myVertexDataListNode->childNodesLength(); ++i) {
                NodePtr myVertexDataNode = myVertexDataListNode->childNode(i);
                if (myVertexDataNode->nodeType() == Node::ELEMENT_NODE &&
                    myVertexDataNode->getAttributeString(NAME_ATTRIB) == theDataName)
                {
                    return myVertexDataNode;
                }
            }
        }
        throw ShapeException(std::string("Could not find vertex data '")+theDataName+"' in shape '"+get<IdTag>()+"'", "Scene::getListData()");
        return NodePtr(0);
    }

#if 0 
    void
    Shape::reverseUpdate() {
        Node & myShapeNode = getNode();
        AC_TRACE << "Shape::reverseUpdate() id='" << get<IdTag>() << "', name='"<<get<NameTag>()<<"', node version="<<myShapeNode.nodeVersion()
                 <<", last renderversion="<<_myLastRenderVersion;

        unsigned long myShapeVertexCount = 0;
        const std::string & myShapeId = myShapeNode.getAttributeString("id");

        // Iterate over all indices and find the corresponding data
        NodePtr myPrimitiveListNode = myShapeNode.childNode(PRIMITIVE_LIST_NAME);
        if (myPrimitiveListNode) {
            unsigned myPrimitiveCount = myPrimitiveListNode->childNodesLength(ELEMENTS_NODE_NAME);
            if (myPrimitiveCount != _myPrimitives.size()) {
                return;
            }
            for (unsigned j = 0; j < myPrimitiveCount; ++j) {
                NodePtr myElementsNode = myPrimitiveListNode->childNode(ELEMENTS_NODE_NAME, j);
                Primitive & myPrimitive = *_myPrimitives[j];

                unsigned myIndicesCount = myElementsNode->childNodesLength(VERTEX_INDICES_NAME);
                for (unsigned k = 0; k < myIndicesCount; ++k) {
                    NodePtr myIndicesNode = myElementsNode->childNode(VERTEX_INDICES_NAME, k);
                    const string & myName = myIndicesNode->getAttributeString(VERTEX_DATA_ATTRIB);
                    NodePtr myDataNode = getVertexDataNode(myName);
                    const VectorOfUnsignedInt & myIndices = myIndicesNode->
                        childNode(0)->nodeValueRef<VectorOfUnsignedInt>();
                    //TODO: find mechanism for selective range update with dirty flags/regions
                    unsigned myBegin = 0;
                    unsigned myEnd   =  myIndices.size();
                    myPrimitive.unload(myIndicesNode, myDataNode, myBegin, myEnd);
                }
            }
        } else {
            throw ShapeException(std::string("Node with name '") +
                PRIMITIVE_LIST_NAME + "' not found for shape: " +
                myShapeId, PLUS_FILE_LINE);
        }
        calculateBoundingBox();
        getNode().bumpVersion();
        setLastRenderVersion(getNode().nodeVersion()+1);
    }
#else
    void
    Shape::reverseUpdate() {
       dom::NodePtr myPrimitiveListNode = getNode().childNode(PRIMITIVE_LIST_NAME);
        if (myPrimitiveListNode) {
            unsigned myPrimitiveCount = myPrimitiveListNode->childNodesLength(ELEMENTS_NODE_NAME);
            for (unsigned j = 0; j < myPrimitiveCount; ++j) {
                dom::NodePtr myElementsNode = myPrimitiveListNode->childNode(ELEMENTS_NODE_NAME, j);
                PrimitivePtr myPrimitive = myElementsNode->getFacade<Primitive>();
                myPrimitive->reverseUpdateVertexData();
            }
        }
        _myLastRenderVersion = getNode().nodeVersion();
     }
#endif

    bool Shape::collide(const asl::Sphere<float> & theSphere,
        const asl::Vector3f & theMotion,
        const asl::Matrix4f & theTransformation,
        Primitive::SphereContactsList & theSphereContacts)
    {
        MAKE_SCOPE_TIMER(Shape_collide);
        bool myResult = false;
        for (int i = 0; i < _myPrimitives.size();++i) {
            if (_myPrimitives[i]->collide(theSphere, theMotion, theTransformation, theSphereContacts)) {
                myResult = true;
            }
        }
        return myResult;
    }

    bool Shape::collide(const asl::Sphere<float> & theSphere,
        const asl::Vector3f & theMotion,
        const asl::Matrix4f & theTransformation,
        const asl::SweptSphereContact<float> & theCurrentContact,
        Primitive::SphereContacts & theNewContactInfo)
    {
        MAKE_SCOPE_TIMER(Shape_collide_first);
        const asl::SweptSphereContact<float> * myCurrentContact = &theCurrentContact;
        bool myResult = false;
        for (int i = 0; i < _myPrimitives.size();++i) {
            if (_myPrimitives[i]->collide(theSphere, theMotion, theTransformation, *myCurrentContact, theNewContactInfo)) {
                myCurrentContact = &(theNewContactInfo._myMinContact);
                myResult = true;
            }
        }
        return myResult;
    }
}

