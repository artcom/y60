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

// own header
#include "Picking.h"

#include <asl/base/Logger.h>
#include <asl/math/Line.h>
#include <asl/dom/Facade.h>
#include <asl/dom/AttributePlug.h>
#include <asl/xpath/xpath_api.h>
#include <y60/base/NodeNames.h>
#include <y60/base/CommonTags.h>
#include <y60/base/DataTypes.h>
#include <y60/scene/Scene.h>
#include <y60/scene/Camera.h>
#include <y60/scene/Viewport.h>
#include <y60/scene/Primitive.h>

using namespace asl;
using namespace dom;
using namespace std;
using namespace y60;

namespace jslib {
    dom::NodePtr
    Picking::pickBody (const dom::Node & theViewportNode, const unsigned int theScreenPixelX, const unsigned int theScreenPixelY) const {

        asl::Point3f myNearClipPos;
        asl::Point3f myFarClipPos;
        getNearAndFarClipPos(theViewportNode, theScreenPixelX, theScreenPixelY, myNearClipPos, myFarClipPos); 
        dom::NodePtr myCameraNode = theViewportNode.getElementById( theViewportNode.getAttributeString(CAMERA_ATTRIB) );
        CameraPtr myCamera = myCameraNode->getFacade<Camera>();
        asl::Matrix4f myProjectionMatrix;
        myCamera->get<FrustumTag>().getProjectionMatrix(myProjectionMatrix);
        myProjectionMatrix.invert();
        myProjectionMatrix.postMultiply(myCamera->get<GlobalMatrixTag>());
        asl::Point3f myNearPlanePos = asl::product(myNearClipPos, myProjectionMatrix);
        asl::Point3f myFarPlanePos = asl::product(myFarClipPos, myProjectionMatrix);

        asl::LineSegment<float> myLineSegment = asl::LineSegment<float>(myNearPlanePos, myFarPlanePos);
        dom::NodePtr myWorldNode = xpath::find(xpath::Path(std::string("/") + WORLD_LIST_NAME + std::string("/") 
                                    + WORLD_NODE_NAME + std::string("/") + CAMERA_NODE_NAME
                                    + std::string("[@id='") + myCamera->get<IdTag>() + std::string("']")), myCameraNode);
        return nearestIntersection(myWorldNode, myLineSegment);
    }

    dom::NodePtr
    Picking::pickBodyBySweepingSphereFromBodies(const dom::Node & theViewportNode, const unsigned int theScreenPixelX, const unsigned int theScreenPixelY, const float theSphereRadius) const {
        dom::NodePtr myClosestBody;
        dom::NodePtr myCameraNode = theViewportNode.getElementById( theViewportNode.getAttributeString(CAMERA_ATTRIB) );
        CameraPtr myCamera = myCameraNode->getFacade<Camera>();
        dom::NodePtr myWorldNode = xpath::find(xpath::Path(std::string("/") + WORLD_LIST_NAME + std::string("/") 
                                    + WORLD_NODE_NAME + std::string("/") + CAMERA_NODE_NAME
                                    + std::string("[@id='") + myCamera->get<IdTag>() + std::string("']")), myCameraNode);

        y60::CollisionInfoVector myCollisions = pickCollisionsBySweepingSphereFromBodies(theViewportNode,
                                                                    theScreenPixelX,
                                                                    theScreenPixelY,
                                                                    theSphereRadius,
                                                                    myWorldNode);
        if (!myCollisions.empty()) {
            asl::Vector3f myCameraPos =  myCamera->get<GlobalMatrixTag>().getTranslation();

            y60::CollisionInfoVector::const_iterator it = myCollisions.begin();
            myClosestBody = it->_myBody;
            float myClosestDistance = std::numeric_limits<float>::max();
            for (; it != myCollisions.end(); ++it) {
                const y60::Primitive::SphereContactsList & mySphereContactList = *(it->_myPrimitiveSphereContacts);
                for (y60::Primitive::SphereContactsList::const_iterator it2 = mySphereContactList.begin(); it2 != mySphereContactList.end(); ++it2) {
                    asl::Vector3f mySphereCenter = it2->_myMinContact.contactSphereCenter;
                    float myDistance = asl::distance(myCameraPos, mySphereCenter); 
                    if (myDistance < myClosestDistance) {
                        myClosestDistance = myDistance;
                        myClosestBody = it->_myBody;
                    }
                }
            }
        }
        return myClosestBody;
    }


    dom::NodePtr
    Picking::findNearestIntersection(const y60::IntersectionInfoVector & theIntersectionInfo, const asl::Point3f & theReferencePoint) const {
        float myMinDistance = std::numeric_limits<float>::max();
        dom::NodePtr myPickedBody;
        for (y60::IntersectionInfoVector::const_iterator it = theIntersectionInfo.begin(); it != theIntersectionInfo.end(); ++it) {
            const Primitive::IntersectionList & myIntersectionList = *(it->_myPrimitiveIntersections);
            for (Primitive::IntersectionList::const_iterator it2 = myIntersectionList.begin(); it2 != myIntersectionList.end(); ++it2) {
                float myDistance = asl::distance(theReferencePoint, it2->_myPosition);
                if (myDistance < myMinDistance) {
                    myMinDistance = myDistance;
                    myPickedBody = it->_myBody;
                }
            }
        }
        return myPickedBody;
    }

    dom::NodePtr
    Picking::nearestIntersection(const dom::NodePtr theRootNode, const asl::LineSegment<float> & theLineSegment) const {
        y60::IntersectionInfoVector myIntersections;
        ScenePtr myScene = xpath::find(xpath::Path(std::string("//") + SCENE_ROOT_NAME), theRootNode)->getFacade<Scene>();
        bool myIntersectedFlag = myScene->intersectBodies(theRootNode, theLineSegment, myIntersections, true);
        if (myIntersectedFlag) {
            return findNearestIntersection(myIntersections, theLineSegment.origin);
        }
        return dom::NodePtr();
    }

    y60::CollisionInfoVector
    Picking::pickCollisionsBySweepingSphereFromBodies(const dom::Node & theViewportNode, const unsigned int theScreenPixelX, const unsigned int theScreenPixelY, const float theSphereRadius, const dom::NodePtr theRootNode) const {
        asl::Point3f myNearPlanePos;
        asl::Point3f myFarPlanePos;
        getNearAndFarClipPos(theViewportNode, theScreenPixelX, theScreenPixelY, myNearPlanePos, myFarPlanePos); 

        asl::Sphere<float> mySphere = asl::Sphere<float>(myNearPlanePos, theSphereRadius);
        asl::Vector3f myMotion = asl::difference(myFarPlanePos, myNearPlanePos);
        ScenePtr myScene = xpath::find(xpath::Path(std::string("//") + SCENE_ROOT_NAME), theRootNode)->getFacade<Scene>();
        y60::CollisionInfoVector myCollisionInfoVector;
        myScene->collideWithBodies(theRootNode, mySphere, myMotion, myCollisionInfoVector, true);
        return myCollisionInfoVector;
    }

    void
    Picking::getNearAndFarClipPos(const dom::Node & theViewportNode, const unsigned int theScreenPixelX, const unsigned int theScreenPixelY, asl::Point3f& theNearClipPos, asl::Point3f& theFarClipPos ) const {
        int myViewportLeft = asl::as_int(theViewportNode.getAttribute(VIEWPORT_LEFT_ATTRIB)->nodeValue());
        int myViewportTop = asl::as_int(theViewportNode.getAttribute(VIEWPORT_TOP_ATTRIB)->nodeValue());
        int myViewportPixelX = theScreenPixelX - myViewportLeft;
        int myViewportPixelY = theScreenPixelY - myViewportTop;
        AC_TRACE<<"Pixelcoordinates in viewport: [" <<
                     myViewportPixelX << ", " <<
                     myViewportPixelY << "]";
        int myViewportWidth = asl::as_int(theViewportNode.getAttribute(VIEWPORT_WIDTH_ATTRIB)->nodeValue());
        int myViewportHeight = asl::as_int(theViewportNode.getAttribute(VIEWPORT_HEIGHT_ATTRIB)->nodeValue());
        float myClipPosX = 2 * (float)myViewportPixelX/myViewportWidth  - 1;
        float myClipPosY = - (2 * (float)myViewportPixelY/myViewportHeight - 1);
        theNearClipPos = asl::Point3f(myClipPosX, myClipPosY, -1);
        theFarClipPos = asl::Point3f(myClipPosX, myClipPosY, 1);
    }
}

