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
//   $RCSfile: SkinAndBones.cpp,v $
//   $Author: pavel $
//   $Revision: 1.8 $
//   $Date: 2005/04/24 00:41:21 $
//
//  Description: A simple material class.
//
//=============================================================================

#include "SkinAndBones.h"
#include "TransformHierarchyFacade.h"

#include <y60/DataTypes.h>
#include <y60/NodeNames.h>
#include <dom/Nodes.h>

#include <asl/Box.h>
#include <asl/Logger.h>

using namespace std;
using namespace asl;
using namespace dom;

namespace y60 {

    DEFINE_EXCEPTION(SkinAndBonesException, asl::Exception);
    void
    SkinAndBones::setup(NodePtr theSceneNode) {
		dom::NodePtr myProperties = getNode().childNode(PROPERTY_LIST_NAME);
        unsigned myChildCount = myProperties->childNodesLength();
        for (unsigned i = 0; i < myChildCount; ++i) {
            if (myProperties->childNode(i)->getAttributeString("name") == "BoneMatrix") {
	            _myBoneMatrixPropertyNode = myProperties->childNode(i)->childNode(0);
				break;
            }
        }
        if (!_myBoneMatrixPropertyNode) {
            throw SkinAndBonesException(std::string("Could not find BoneMatrix property in material: ") + get<IdTag>(),
                    PLUS_FILE_LINE);
        }

        // Find connected skin
        NodePtr myShapesNode = theSceneNode->childNode(SHAPE_LIST_NAME);
        NodePtr myWorldsNode = theSceneNode->childNode(WORLD_LIST_NAME);

        if (!myShapesNode || !myWorldsNode) {
            throw SkinAndBonesException("Could not find shapes or worlds node in scene", PLUS_FILE_LINE);
        }

        vector<NodePtr> myElements;
        myShapesNode->getNodesByAttribute(ELEMENTS_NODE_NAME, MATERIAL_REF_ATTRIB, get<IdTag>(), myElements);

        Node * myShape;
        string myShapeId;
        for (unsigned i = 0; i < myElements.size(); ++i) {
            Node * myOtherShape = myElements[i]->parentNode()->parentNode();
            if (i == 0) {
                myShape = myOtherShape;
                myShapeId = myShape->getAttributeString(ID_ATTRIB);
            }  else {
                const string & myOtherShapeId = myOtherShape->getAttributeString(ID_ATTRIB);
                if (myOtherShapeId != myShapeId) {
                    AC_WARNING << "Shape " << myOtherShapeId << " uses skin-and-bones material already used by shape " << myShapeId;
                }
            }
        }

        // Cache bounding box
        _myBoundingBoxNode = myShape->getFacade()->getNamedItem(BOUNDING_BOX_ATTRIB);

        vector<NodePtr> mySkeletons;
        myWorldsNode->getNodesByAttribute(BODY_NODE_NAME, BODY_SHAPE_ATTRIB, myShapeId, mySkeletons);

        if (mySkeletons.size() > 1) {
            throw SkinAndBonesException(string("More than one skeletons use the shape ") + myShapeId, PLUS_FILE_LINE);
        }
        
        // Find connected joints
        NodePtr mySkeletonAttribute = mySkeletons[0]->getAttribute(SKELETON_ATTRIB);

        if (!mySkeletonAttribute) {
            throw SkinAndBonesException(std::string("Skeleton node does not contain skeleton attribute:\n")
                 + asl::as_string(*mySkeletons[0]), PLUS_FILE_LINE);
        }

        // Cache global matrix pointer and inverted initial matrices of all connected joints
        const VectorOfString & myJointIds = mySkeletonAttribute->nodeValueRef<VectorOfString>();
        for (unsigned i = 0; i < myJointIds.size(); ++i) {
            NodePtr myJoint = myWorldsNode->getElementById(myJointIds[i]);

            if (!myJoint) {
                throw SkinAndBonesException(std::string("Skeleton node points to unknown joint: ") + myJointIds[i] + "\n"
                    +asl::as_string(*mySkeletons[0]), PLUS_FILE_LINE);
            }
            
            // Cache Joint Facade
            const JointFacadePtr myJointFacade = myJoint->getFacade<JointFacade>();
            _myJoints.push_back(myJointFacade);

            // get matrix for bind pose
            _myJointSpaceTransforms.push_back(myJointFacade->get<InverseGlobalMatrixTag>());
        }
        _myBoneMatrixPropertyNode->dom::Node::nodeValuePtrOpen<VectorOfVector4f>()->resize(_myJoints.size() * 3);
        _myBoneMatrixPropertyNode->dom::Node::nodeValuePtrClose<VectorOfVector4f>();
    }

    void
    SkinAndBones::update(TextureManager & theTextureManager, const dom::NodePtr theImages) {
        MaterialBase::update(theTextureManager, theImages);

		VectorOfVector4f * myBoneMatrixProperty = _myBoneMatrixPropertyNode->dom::Node::nodeValuePtrOpen<VectorOfVector4f>();
        if (!myBoneMatrixProperty) {
            throw SkinAndBonesException("SkinAndBones shader update has been called before setup", PLUS_FILE_LINE);
        }

        Box3f * myBoundingBox = _myBoundingBoxNode->nodeValuePtrOpen<Box3f>();
        myBoundingBox->makeEmpty();

        for (unsigned i = 0; i < _myJoints.size(); ++i) {
            Matrix4f myMatrix(_myJointSpaceTransforms[i]);
            myMatrix.postMultiply(_myJoints[i]->get<GlobalMatrixTag>());

            myBoneMatrixProperty->at(i * 3 + 0) = myMatrix.getColumn(0);
            myBoneMatrixProperty->at(i * 3 + 1) = myMatrix.getColumn(1);
            myBoneMatrixProperty->at(i * 3 + 2) = myMatrix.getColumn(2);

            myBoundingBox->extendBy(asPoint(_myJoints[i]->get<GlobalMatrixTag>().getTranslation()));
        }
        _myBoneMatrixPropertyNode->dom::Node::nodeValuePtrClose<VectorOfVector4f>();
        _myBoundingBoxNode->nodeValuePtrClose<Box3f>();        

   }
}
