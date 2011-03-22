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
#include <y60/scene/World.h>

using namespace asl;
using namespace dom;
using namespace std;
using namespace y60;

namespace y60 {
    dom::NodePtr
    Picking::pickBody (const dom::Node & theViewportNode, const unsigned int theScreenPixelX, const unsigned int theScreenPixelY) const {
        asl::LineSegment<float> myLineSegment;
        dom::NodePtr myWorldNode;
        getLineSegmentAndWorldNode(theViewportNode, theScreenPixelX, theScreenPixelY, myLineSegment, myWorldNode);
        return findNearestIntersectionInformation(myWorldNode, myLineSegment)._myBody;
    }

    y60::IntersectionInfo
    Picking::getPickedBodyInformation(const dom::Node & theViewportNode, const unsigned int theScreenPixelX, const unsigned int theScreenPixelY) const  {
        asl::LineSegment<float> myLineSegment;
        dom::NodePtr myWorldNode;
        getLineSegmentAndWorldNode(theViewportNode, theScreenPixelX, theScreenPixelY, myLineSegment, myWorldNode);
        return findNearestIntersectionInformation(myWorldNode, myLineSegment);
    }

    y60::IntersectionInfoVector 
    Picking::getPickedBodiesInformation(const dom::Node & theViewportNode, const unsigned int theScreenPixelX, const unsigned int theScreenPixelY) const {
        asl::LineSegment<float> myLineSegment;
        dom::NodePtr myWorldNode;
        getLineSegmentAndWorldNode(theViewportNode, theScreenPixelX, theScreenPixelY, myLineSegment, myWorldNode);
        return findAllIntersectionsInformation(myWorldNode, myLineSegment);
    }


    dom::NodePtr
    Picking::pickBodyBySweepingSphereFromBodies(const dom::Node & theViewportNode, const unsigned int theScreenPixelX, const unsigned int theScreenPixelY, const float theSphereRadius) const {
        dom::NodePtr myClosestBody;
        dom::NodePtr myCameraNode = theViewportNode.getElementById( theViewportNode.getAttributeString(CAMERA_ATTRIB) );
        CameraPtr myCamera = myCameraNode->getFacade<Camera>();
        dom::NodePtr myWorldNode = myCamera->getWorld(); 
        y60::CollisionInfoVector myCollisions = pickCollisionsBySweepingSphereFromBodies(myCamera, 
                                                                    theViewportNode,
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

    void
    Picking::getLineSegmentAndWorldNode(const dom::Node & theViewportNode, 
                                        const unsigned int theScreenPixelX, 
                                        const unsigned int theScreenPixelY, 
                                        asl::LineSegment<float> & theLineSegment, 
                                        dom::NodePtr & theWoldNode) const {
        asl::Point3f myNearPlanePos;
        asl::Point3f myFarPlanePos;
        dom::NodePtr myCameraNode = theViewportNode.getElementById( theViewportNode.getAttributeString(CAMERA_ATTRIB) );
        CameraPtr myCamera = myCameraNode->getFacade<Camera>();
        getNearAndFarPlanePos(myCamera, theViewportNode, theScreenPixelX, theScreenPixelY, myNearPlanePos, myFarPlanePos); 

        theLineSegment = asl::LineSegment<float>(myNearPlanePos, myFarPlanePos);
        theWoldNode = myCamera->getWorld(); 
    }

    y60::IntersectionInfoVector
    Picking::findAllIntersectionsInformation(const dom::NodePtr theRootNode, const asl::LineSegment<float> & theLineSegment) const {
        y60::IntersectionInfoVector myIntersections;
        y60::Scene::intersectBodies(theRootNode, theLineSegment, myIntersections, true);
        return myIntersections;
    }

    y60::IntersectionInfo
    Picking::findNearestIntersectionInformation(const dom::NodePtr theRootNode, const asl::LineSegment<float> & theLineSegment) const {
        y60::IntersectionInfoVector myIntersections = findAllIntersectionsInformation(theRootNode, theLineSegment);
        float myMinDistance = std::numeric_limits<float>::max();
        IntersectionInfo myPickedBodyInformation;
        for (y60::IntersectionInfoVector::const_iterator it = myIntersections.begin(); it != myIntersections.end(); ++it) {
            const Primitive::IntersectionList & myIntersectionList = *(it->_myPrimitiveIntersections);
            for (Primitive::IntersectionList::const_iterator it2 = myIntersectionList.begin(); it2 != myIntersectionList.end(); ++it2) {
                asl::Point3f myPosition = product( it2->_myPosition, it->_myTransformation);
                float myDistance = asl::distance(theLineSegment.origin, myPosition);
                if (myDistance < myMinDistance) {
                    myMinDistance = myDistance;
                    myPickedBodyInformation = *it;
                }
            }
        }
        return myPickedBodyInformation;
    }

    y60::CollisionInfoVector
    Picking::pickCollisionsBySweepingSphereFromBodies(const CameraPtr theCamera, 
                    const dom::Node & theViewportNode, const unsigned int theScreenPixelX, 
                    const unsigned int theScreenPixelY, const float theSphereRadius, 
                    const dom::NodePtr theRootNode) const 
    {
        asl::Point3f myNearPlanePos;
        asl::Point3f myFarPlanePos;
        getNearAndFarPlanePos(theCamera, theViewportNode, theScreenPixelX, theScreenPixelY, myNearPlanePos, myFarPlanePos); 

        asl::Sphere<float> mySphere = asl::Sphere<float>(myNearPlanePos, theSphereRadius);
        asl::Vector3f myMotion = asl::difference(myFarPlanePos, myNearPlanePos);
        y60::CollisionInfoVector myCollisionInfoVector;
        y60::Scene::collideWithBodies(theRootNode, mySphere, myMotion, myCollisionInfoVector, true);
        return myCollisionInfoVector;
    }

    void
    Picking::getNearAndFarPlanePos(const CameraPtr theCamera, const dom::Node & theViewportNode, const unsigned int theScreenPixelX, const unsigned int theScreenPixelY, asl::Point3f& theNearClipPos, asl::Point3f& theFarClipPos ) const {
        ViewportPtr myViewport = theViewportNode.getFacade<Viewport>();
        int myViewportLeft = myViewport->get<ViewportLeftTag>();
        int myViewportTop = myViewport->get<ViewportTopTag>();
        int myViewportPixelX = theScreenPixelX - myViewportLeft;
        int myViewportPixelY = theScreenPixelY - myViewportTop;
        AC_TRACE<<"Pixelcoordinates in viewport: [" <<
                     myViewportPixelX << ", " <<
                     myViewportPixelY << "]";
        int myViewportWidth = myViewport->get<ViewportWidthTag>();
        int myViewportHeight = myViewport->get<ViewportHeightTag>();
        float myClipPosX = 2 * (float)myViewportPixelX/myViewportWidth  - 1;
        float myClipPosY = - (2 * (float)myViewportPixelY/myViewportHeight - 1);
        theNearClipPos = asl::Point3f(myClipPosX, myClipPosY, -1);
        theFarClipPos = asl::Point3f(myClipPosX, myClipPosY, 1);
        transformClipToWorld(theNearClipPos, theFarClipPos, theCamera);
    }

    void
    Picking::transformClipToWorld(asl::Point3f & theNearClipPos, asl::Point3f & theFarClipPos, const CameraPtr theCamera) const {
        asl::Matrix4f myProjectionMatrix;
        theCamera->get<FrustumTag>().getProjectionMatrix(myProjectionMatrix);
        myProjectionMatrix.invert();
        myProjectionMatrix.postMultiply(theCamera->get<GlobalMatrixTag>());
        theNearClipPos = asl::product(theNearClipPos, myProjectionMatrix);
        theFarClipPos = asl::product(theFarClipPos, myProjectionMatrix);
    }
}

