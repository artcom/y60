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

//own header
#include "SkinAndBonesShader.h"


#include <y60/scene/TransformHierarchyFacade.h>
#include <y60/base/DataTypes.h>
#include <y60/base/NodeNames.h>
#include <asl/dom/Nodes.h>

#include <asl/math/Box.h>
#include <asl/base/Logger.h>

using namespace std;
using namespace asl;
using namespace dom;

namespace y60 {

    DEFINE_EXCEPTION(SkinAndBonesShaderException, asl::Exception);

    void
    SkinAndBonesShader::setup(MaterialBase & theMaterial) {
        dom::NodePtr myProperties = theMaterial.getNode().childNode(PROPERTY_LIST_NAME);
        unsigned myChildCount = myProperties->childNodesLength();
        for (unsigned i = 0; i < myChildCount; ++i) {
            if (myProperties->childNode(i)->getAttributeString("name") == "BoneMatrix") {
                _myBoneMatrixPropertyNode = myProperties->childNode(i)->childNode(0);
                break;
            }
        }
        if (!_myBoneMatrixPropertyNode) {
            throw SkinAndBonesShaderException(std::string("Could not find BoneMatrix property in material: ")
                + theMaterial.get<IdTag>(), PLUS_FILE_LINE);
        }

        // Find connected skin
        Node * mySceneNode = theMaterial.getNode().parentNode()->parentNode();
        NodePtr myShapesNode = mySceneNode->childNode(SHAPE_LIST_NAME);
        NodePtr myWorldsNode = mySceneNode->childNode(WORLD_LIST_NAME);

        if (!myShapesNode || !myWorldsNode) {
            throw SkinAndBonesShaderException("Could not find shapes or worlds node in scene", PLUS_FILE_LINE);
        }

        vector<NodePtr> myElements;
        myShapesNode->getNodesByAttribute(ELEMENTS_NODE_NAME, MATERIAL_REF_ATTRIB, theMaterial.get<IdTag>(), true, myElements);

        Node * myShape = NULL;
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
        myWorldsNode->getNodesByAttribute(BODY_NODE_NAME, BODY_SHAPE_ATTRIB, myShapeId, true, mySkeletons);

        if (mySkeletons.size() > 1) {
            throw SkinAndBonesShaderException(string("More than one skeletons use the shape ") + myShapeId, PLUS_FILE_LINE);
        }

        // Find connected joints
        NodePtr mySkeletonAttribute = mySkeletons[0]->getAttribute(SKELETON_ATTRIB);

        // Workaround for culling problem with skin and bones
        mySkeletons[0]->getAttribute(CULLABLE_ATTRIB)->nodeValue("0");

        if (!mySkeletonAttribute) {
            throw SkinAndBonesShaderException(std::string("Skeleton node does not contain skeleton attribute:\n")
                 + asl::as_string(*mySkeletons[0]), PLUS_FILE_LINE);
        }

        // Cache global matrix pointer and inverted initial matrices of all connected joints
        const VectorOfString & myJointIds = mySkeletonAttribute->nodeValueRef<VectorOfString>();
        _myJoints.clear();
        for (unsigned i = 0; i < myJointIds.size(); ++i) {
            NodePtr myJoint = myWorldsNode->getElementById(myJointIds[i]);

            if (!myJoint) {
                throw SkinAndBonesShaderException(std::string("Skeleton node points to unknown joint: ") + myJointIds[i] + "\n"
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
    SkinAndBonesShader::activate(MaterialBase & theMaterial, const Viewport & theViewport, const MaterialBase * theLastMaterial) {
        VectorOfVector4f * myBoneMatrixProperty = _myBoneMatrixPropertyNode->dom::Node::nodeValuePtrOpen<VectorOfVector4f>();
        if (!myBoneMatrixProperty) {
            throw SkinAndBonesShaderException("SkinAndBones shader update has been called before setup", PLUS_FILE_LINE);
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

        CgShader::activate(theMaterial, theViewport, 0);
   }
}
