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
//   $RCSfile: intersection_functions.js,v $
//   $Author: jens $
//   $Revision: 1.13 $
//   $Date: 2005/04/26 15:27:29 $
//
//
//=============================================================================

use("Y60JSSL.js");
use("Exception.js");


function findFarthestIntersection(myIntersectionInfo, myReferencePoint) {
    var myFarthestIntersection;
    var myMaxDistance;
    var myMaxInfo;
    for (var i = 0; i < myIntersectionInfo.length; ++i) {
        for (var j = 0; j < myIntersectionInfo[i].intersections.length; ++j) {
            var myIntersection = myIntersectionInfo[i].intersections[j];
            var myDistance = distance(myReferencePoint, myIntersection.position);
            if (myMaxDistance === undefined || myDistance > myMaxDistance) {
                myMaxDistance = myDistance;
                myMaxInfo = myIntersectionInfo[i];
                myFarthestIntersection = myIntersection;
            }
        }
    }
    if (myFarthestIntersection) {
        myFarthestIntersection.distance = myMaxDistance;
        myFarthestIntersection.info = myMaxInfo;
    }
    return myFarthestIntersection;
}

function findNearestIntersection(myIntersectionInfo, myReferencePoint) {
    var myNearestIntersection = null;
    var myMinDistance;
    var myMinInfo;
    for (var i = 0; i < myIntersectionInfo.length; ++i) {
        for (var j = 0; j < myIntersectionInfo[i].intersections.length; ++j) {
            var myIntersection = myIntersectionInfo[i].intersections[j];
            var myDistance = distance(myReferencePoint, myIntersection.position);
            if (myMinDistance === undefined || myDistance < myMinDistance) {
                //if (myIntersectionInfo[i].body.visible) {
                    myMinDistance = myDistance;
                    myMinInfo = myIntersectionInfo[i];
                    myNearestIntersection = myIntersection;
                //} else {
                //    print("Sorry, not visible: " + myIntersectionInfo[i].body.name);
                //}
            }
        }
    }
    if (myNearestIntersection) {
        myNearestIntersection.distance = myMinDistance;
        myNearestIntersection.info = myMinInfo;
    }
    return myNearestIntersection;
}

function findNearestCollision(theCollisions, theReferencePoint) {
    var myNearestCollision;
    var myMinDistance = Number.MAX_VALUE;
    for (var i = 0; i < theCollisions.length; ++i) {
        var myCollision = theCollisions[i].body.globalmatrix.getRow(3);
        myCollision = new Vector3f(myCollision[0], myCollision[1], myCollision[2]);
        //print("collision " + i + " at: " + myCollision + " body: " + theCollisions[i].body.id);
        var myDistance = distance(theReferencePoint, myCollision);
        if (myDistance < myMinDistance) {
            myMinDistance = myDistance;
            myNearestCollision = theCollisions[i];
        }
    }
    if (myNearestCollision) {
        myNearestCollision.distance = myMinDistance;
    }
    return myNearestCollision;
}

function findIntersectionsByBox(theRootNode, myBox, theIntersectInVisibleShapes) {
    if (theIntersectInVisibleShapes === null || theIntersectInVisibleShapes === undefined) {
        theIntersectInVisibleShapes = true;
    }
    var myIntersectionInfo = Scene.intersectBodies(theRootNode, myBox);
    var myResult= [];
    if (myIntersectionInfo && myIntersectionInfo.length >0) {
        for (var i = 0; i < myIntersectionInfo.length; ++i) {
            myResult.push(myIntersectionInfo[i].body);
        }
    }
    return myResult;
}

function findIntersectionsByBoxCenter(theRootNode, myBox) {
    var myIntersectionInfo = Scene.intersectBodyCenters(theRootNode, myBox);
    var myResult= [];
    if (myIntersectionInfo && myIntersectionInfo.length >0) {
        for (var i = 0; i < myIntersectionInfo.length; ++i) {
            myResult.push(myIntersectionInfo[i].body);
        }
    }
    return myResult;
}

// this function will dispatch according to the type of myLineSegment.
// it may be a LineSegment or a Ray
function nearestIntersection(theRootNode, myLineSegment, theIntersectInVisibleShapes) {
    if (theIntersectInVisibleShapes === null || theIntersectInVisibleShapes === undefined) {
        theIntersectInVisibleShapes = true;
    }
    var myIntersectionInfo = Scene.intersectBodies(theRootNode, myLineSegment, theIntersectInVisibleShapes);
    if (myIntersectionInfo) {
        return findNearestIntersection(myIntersectionInfo, myLineSegment.origin);
    }
}
function farthestIntersection(theRootNode, myLineSegment, theIntersectInVisibleShapes) {
    if (theIntersectInVisibleShapes === null || theIntersectInVisibleShapes === undefined) {
        theIntersectInVisibleShapes = true;
    }
    var myIntersectionInfo = Scene.intersectBodies(theRootNode, myLineSegment, theIntersectInVisibleShapes);
    if (myIntersectionInfo) {
        return findFarthestIntersection(myIntersectionInfo, myLineSegment.origin);
    }
}

function drawIntersection(theIntersectionInfo, theIntersection) {
    var myMatrix = new Matrix4f();
    myMatrix.makeIdentity();

    // We have to turn of backface culling to show found triangles that do not face the camera
    var myCulling = renderer.backfaceCulling;
    renderer.backfaceCulling = false;
/*
    // Draw intersected triangles
    var myScale         = new Vector3f(1.01, 1.01, 1.01);
    var myPolygonOffset = new Vector3f(0.1,0.1,0.1);
    var myA = product(theIntersection.primitive[0], myScale);
        myA = sum(myA, myPolygonOffset);
    var myB = product(theIntersection.primitive[1], myScale);
        myB = sum(myB, myPolygonOffset);
    var myC = product(theIntersection.primitive[2], myScale);
        myC = sum(myC, myPolygonOffset);
    var myTriangle = new Triangle(myA, myB, myC);
    window.draw(myTriangle, [0.5,0,1,1], myMatrix);

    if (theIntersection.primitive.length == 4) {
        var myD = product(theIntersection.primitive[3], myScale);
        myD = sum(myD, myPolygonOffset);
        var myTriangle2nd = new Triangle(myA, myC, myD);
        window.draw(myTriangle2nd, [0.5,0,1,1], myMatrix);
    }
*/
    // Draw normal
    //
    var myNormalTo = sum(theIntersection.position, product(theIntersection.normal, [3,3,3]));

    window.draw(new LineSegment(theIntersection.position, myNormalTo),
                [0.5,1.0,0.5,1], myMatrix);

    var myPlaneNormalTo = sum(theIntersection.position, product(theIntersection.primitive.normal, [3,3,3]));
    window.draw(new LineSegment(theIntersection.position, myNormalTo),
                [1.0,1.0,0.5,1], myMatrix);

    // Reset backface culling to previous state
    renderer.backfaceCulling = myCulling;
}

function pickBody(theX, theY) {
    // TODO: This is not portrait orientation aware.
    var myViewport = window.scene.canvas.childNode("viewport");
    var myPosX = 2 * (theX-myViewport.left) / myViewport.width  - 1;
    var myPosY = - (2 * (theY-myViewport.top) / myViewport.height - 1);
    var myClipNearPos = new Point3f(myPosX, myPosY, -1);
    var myClipFarPos = new Point3f(myPosX, myPosY, +1);
    var myProjectionMatrix = window.camera.frustum.projectionmatrix;
    myProjectionMatrix.invert();
    myProjectionMatrix.postMultiply(window.camera.globalmatrix);

    var myWorldNearPos = product(myClipNearPos, myProjectionMatrix);
    var myWorldFarPos  = product(myClipFarPos, myProjectionMatrix);
    var myMouseRay     = new Ray(myWorldNearPos, myWorldFarPos);

    return nearestIntersection(window.scene.world, myMouseRay);
}
