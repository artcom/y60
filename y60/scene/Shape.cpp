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

// own header
#include "Shape.h"

#include "Scene.h"

#include <y60/base/VertexDataRoles.h>
#include <asl/base/string_functions.h>

using namespace dom;
using namespace std;
using namespace asl;

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
            AC_TRACE << "Shape::update: updating shape with name = " << getNode().getAttributeString(NAME_ATTRIB);
            NodePtr myVertexDataListNode = getNode().childNode(VERTEX_DATA_NAME);
            std::map<dom::UniqueId, std::set<y60::VertexDataRole> > myRolesPerElementToUpdate;
            bool myRebuildNeeded = false;
            if(myVertexDataListNode) {
                unsigned myVertexDataCount = myVertexDataListNode->childNodesLength();
                std::map<std::string, unsigned long long>::const_iterator i;
                for (unsigned j = 0; j < myVertexDataCount; ++j) {
                    i = _myVertexDataVersionMap.find(myVertexDataListNode->childNode(j)->getAttributeString(NAME_ATTRIB));
                    if(i != _myVertexDataVersionMap.end()) {
                        if(i->second < myVertexDataListNode->childNode(j)->nodeVersion()) {
                            AC_TRACE << "Shape::update: have to update all roles associated with vertex data: " << i->first;
                            for(std::multimap<std::string, std::string>::const_iterator myRoles = _myVertexDataToRolesMap.find(i->first);
                                myRoles != _myVertexDataToRolesMap.end() && myRoles != _myVertexDataToRolesMap.upper_bound(i->first); ++myRoles) {           
                                AC_TRACE << "Shape::update: role: " << myRoles->second;
                                for(std::multimap<std::string, dom::NodePtr>::const_iterator myCurElements = _myVertexDataNameToElementMap.find(i->first);
                                    myCurElements != _myVertexDataNameToElementMap.end() && myCurElements != _myVertexDataNameToElementMap.upper_bound(i->first); ++myCurElements) {

                                    NodePtr myElementsNode = myCurElements->second;
                                    
                                    if(myRolesPerElementToUpdate.find(myElementsNode->getUniqueId()) == myRolesPerElementToUpdate.end())
                                        myRolesPerElementToUpdate.insert(std::make_pair(myElementsNode->getUniqueId(), std::set<y60::VertexDataRole>()));                                    
                                    std::map<dom::UniqueId, std::set<y60::VertexDataRole> >::iterator myCurRoles = myRolesPerElementToUpdate.find(myElementsNode->getUniqueId());
                                                                        
                                    unsigned myIndicesCount = myElementsNode->childNodesLength(VERTEX_INDICES_NAME);
                                    for (unsigned k = 0; k < myIndicesCount; ++k) {
                                        NodePtr myIndicesNode = myElementsNode->childNode(VERTEX_INDICES_NAME, k);    
                                        if(myRoles->second == myIndicesNode->getAttributeString(VERTEX_DATA_ROLE_ATTRIB)) {
                                            myCurRoles->second.insert(VertexDataRole(getEnumFromString(myRoles->second, VertexDataRoleString)));
                                            break;
                                        }
                                    }
                                }                                
                            }
                        }
                    } else {
                        myRebuildNeeded = true;
                        myRolesPerElementToUpdate.clear();
                        AC_TRACE << "Shape::update: have to rebuild all; found new vertex data " << myVertexDataListNode->childNode(j)->getAttributeString(NAME_ATTRIB);
                        break;
                    }
                }
            }
            
            // if we already need a full rebuild here, we dont bother to check in a more finegrained fashion
            if(!myRebuildNeeded) {
                NodePtr myPrimitiveListNode = getNode().childNode(PRIMITIVE_LIST_NAME);
                if(myPrimitiveListNode) {
                    unsigned myPrimitiveCount = myPrimitiveListNode->childNodesLength(ELEMENTS_NODE_NAME);
                    for(unsigned l = 0; l < myPrimitiveCount; ++l) {
                        NodePtr myElementsNode = myPrimitiveListNode->childNode(ELEMENTS_NODE_NAME, l);
                        std::map<UniqueId, unsigned long long>::const_iterator i = _myElementToVersionMap.find(myElementsNode->getUniqueId());
                        if(i != _myElementToVersionMap.end()) {
                            if(i->second < myElementsNode->nodeVersion()) { 
                                 if(myRolesPerElementToUpdate.find(myElementsNode->getUniqueId()) == myRolesPerElementToUpdate.end())
                                        myRolesPerElementToUpdate.insert(std::make_pair(myElementsNode->getUniqueId(), std::set<y60::VertexDataRole>()));                                    
                                 std::map<dom::UniqueId, std::set<y60::VertexDataRole> >::iterator myCurRoles = myRolesPerElementToUpdate.find(myElementsNode->getUniqueId());
                                
                                // version of element is newer than the old one; thus something changed; we update all roles
                                unsigned myIndicesCount = myElementsNode->childNodesLength(VERTEX_INDICES_NAME);
                                for (unsigned k = 0; k < myIndicesCount; ++k) {
                                    NodePtr myIndicesNode = myElementsNode->childNode(VERTEX_INDICES_NAME, k);
                                    myCurRoles->second.insert(VertexDataRole(getEnumFromString(myIndicesNode->getAttributeString(VERTEX_DATA_ROLE_ATTRIB), VertexDataRoleString)));           
                                    AC_TRACE << "Shape::update: role: " << myIndicesNode->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
                                }
                            }
                        } else {
                            myRolesPerElementToUpdate.clear();
                            AC_TRACE << "Shape::update: have to rebuild all; found new element: " << *myElementsNode;
                            break;
                        }
                    }
                }                
            }            
            
            build(myRolesPerElementToUpdate);
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
    Shape::build(const std::map<dom::UniqueId, std::set<y60::VertexDataRole> >& theRolesPerElementToUpdate) {
        AC_TRACE << "Shape::build() id='"<<get<IdTag>()<<"'";
        NodePtr myPrimitiveListNode = getNode().childNode(PRIMITIVE_LIST_NAME);
        if(myPrimitiveListNode) {
            unsigned myPrimitiveCount = myPrimitiveListNode->childNodesLength(ELEMENTS_NODE_NAME);
            if(theRolesPerElementToUpdate.empty()) {
                _myPrimitives.resize(0);
            }
            std::map<dom::UniqueId, std::set<y60::VertexDataRole> >::const_iterator i;
            for (unsigned j = 0; j < myPrimitiveCount; ++j) {
                NodePtr myElementsNode = myPrimitiveListNode->childNode(ELEMENTS_NODE_NAME, j);
                PrimitivePtr myPrimitive = myElementsNode->getFacade<Primitive>();
                i = theRolesPerElementToUpdate.find(myElementsNode->getUniqueId());
                if(i != theRolesPerElementToUpdate.end()) { 
                    myPrimitive->updateVertexData(i->second);
                } else {
                    myPrimitive->updateVertexData();
                }
                if(theRolesPerElementToUpdate.empty()) {
                    _myPrimitives.push_back(myPrimitive);
                }
            }
            
            _myVertexDataVersionMap.clear();
            _myElementToVersionMap.clear();
            _myVertexDataToRolesMap.clear();      
            _myVertexDataNameToElementMap.clear();
            for(unsigned l = 0; l < myPrimitiveCount; ++l) {
                NodePtr myElementsNode = myPrimitiveListNode->childNode(ELEMENTS_NODE_NAME, l);
                unsigned myIndicesCount = myElementsNode->childNodesLength(VERTEX_INDICES_NAME);
                //AC_PRINT << " inserting " << myElementsNode->getUniqueId() << " = " << myElementsNode->nodeVersion();
                _myElementToVersionMap.insert(std::make_pair(myElementsNode->getUniqueId(), myElementsNode->nodeVersion()));
                for (unsigned k = 0; k < myIndicesCount; ++k) {
                    NodePtr myIndicesNode = myElementsNode->childNode(VERTEX_INDICES_NAME, k);
                    NodePtr myVertexDataNode = getVertexDataNode(myIndicesNode->getAttributeString(VERTEX_DATA_ATTRIB));
                    //AC_PRINT << " inserting " << myVertexDataNode->getAttributeString(NAME_ATTRIB) << " version = " << myVertexDataNode->nodeVersion();
                    _myVertexDataVersionMap.insert(std::make_pair(myVertexDataNode->getAttributeString(NAME_ATTRIB), myVertexDataNode->nodeVersion()));
                    _myVertexDataNameToElementMap.insert(std::make_pair(myVertexDataNode->getAttributeString(NAME_ATTRIB), myElementsNode));
                    //AC_PRINT << " inserting " << myVertexDataNode->getAttributeString(NAME_ATTRIB) << " role = " << myIndicesNode->getAttributeString(VERTEX_DATA_ROLE_ATTRIB);
                    _myVertexDataToRolesMap.insert(std::make_pair(myVertexDataNode->getAttributeString(NAME_ATTRIB), myIndicesNode->getAttributeString(VERTEX_DATA_ROLE_ATTRIB)));
                }
            }
        }
        _myLastRenderVersion = getNode().nodeVersion();
    }
     
     void
     Shape::updatePrimitives(const std::set<y60::VertexDataRole>& theRolesToUpdate) {
         
         NodePtr myVertexDataListNode = getNode().childNode(VERTEX_DATA_NAME);
       if(myVertexDataListNode) {
           unsigned myVertexDataCount = myVertexDataListNode->childNodesLength();
            for (unsigned j = 0; j < myVertexDataCount; ++j) {
                _myVertexDataVersionMap.insert(std::make_pair(std::string(myVertexDataListNode->childNode(j)->getAttributeString(NAME_ATTRIB)), myVertexDataListNode->childNode(j)->nodeVersion()));  
            }
       }      
         
        NodePtr myPrimitiveListNode = getNode().childNode(PRIMITIVE_LIST_NAME);
        if (myPrimitiveListNode) {
            unsigned myPrimitiveCount = myPrimitiveListNode->childNodesLength(ELEMENTS_NODE_NAME);
            for (unsigned j = 0; j < myPrimitiveCount; ++j) {
                NodePtr myElementsNode = myPrimitiveListNode->childNode(ELEMENTS_NODE_NAME, j);
                PrimitivePtr myPrimitive = myElementsNode->getFacade<Primitive>();
                myPrimitive->updateVertexData(theRolesToUpdate);
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
        return NodePtr(0); // XXX, needed for testSceneLeakage test but cannot be compiled with boost smartptr (vs)
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
        update();
        bool myResult = false;
        for (PrimitiveVector::size_type i = 0; i < _myPrimitives.size();++i) {
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
        update();
        const asl::SweptSphereContact<float> * myCurrentContact = &theCurrentContact;
        bool myResult = false;
        for (PrimitiveVector::size_type i = 0; i < _myPrimitives.size();++i) {
            if (_myPrimitives[i]->collide(theSphere, theMotion, theTransformation, *myCurrentContact, theNewContactInfo)) {
                myCurrentContact = &(theNewContactInfo._myMinContact);
                myResult = true;
            }
        }
        return myResult;
    }
}

