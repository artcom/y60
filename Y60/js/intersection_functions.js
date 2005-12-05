 //=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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
    var myMaxDistance = undefined;
    var myMaxInfo = undefined;
    for (var i = 0; i < myIntersectionInfo.length; ++i) {
        for (var j = 0; j < myIntersectionInfo[i].intersections.length; ++j) {
            var myIntersection = myIntersectionInfo[i].intersections[j];
            var myDistance = distance(myReferencePoint, myIntersection.position);
            if (myMaxDistance == undefined || myDistance > myMaxDistance) {
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
    var myMinDistance = undefined;
    var myMinInfo = undefined;
    for (var i = 0; i < myIntersectionInfo.length; ++i) {
        for (var j = 0; j < myIntersectionInfo[i].intersections.length; ++j) {
            var myIntersection = myIntersectionInfo[i].intersections[j];
            var myDistance = distance(myReferencePoint, myIntersection.position);
            if (myMinDistance == undefined || myDistance < myMinDistance) {
                myMinDistance = myDistance;
                myMinInfo = myIntersectionInfo[i];
                myNearestIntersection = myIntersection;
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

// this function will dispatch according to the type of myLineSegment.
// it may be a LineSegment or a Ray
function nearestIntersection(theRootNode, myLineSegment) {
    var myIntersectionInfo = Scene.intersectBodies(theRootNode, myLineSegment);
    if (myIntersectionInfo) {
        return findNearestIntersection(myIntersectionInfo, myLineSegment.origin);
    }
    return undefined;
}
function farthestIntersection(theRootNode, myLineSegment) {
    var myIntersectionInfo = Scene.intersectBodies(theRootNode, myLineSegment);
    if (myIntersectionInfo) {
        return findFarthestIntersection(myIntersectionInfo, myLineSegment.origin);
    }    
    return undefined;
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
