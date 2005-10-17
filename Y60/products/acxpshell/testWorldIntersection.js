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
//   $RCSfile: testWorldIntersection.js,v $
//   $Author: martin $
//   $Revision: 1.15 $
//   $Date: 2004/11/07 04:50:21 $
//
//
//=============================================================================

use("SceneViewer.js");

function drawUnitLines() {
	var myMatrix = new Matrix4f();
	myMatrix.makeIdentity();

	window.draw(new LineSegment([0, 0, 0], [1,0,0]), [1,0,0,1], myMatrix);
	window.draw(new LineSegment([0, 0, 0], [0,1,0]), [0,1,0,1], myMatrix);
	window.draw(new LineSegment([0, 0, 0], [0,0,1]), [0,0,1,1], myMatrix);
}

function getMouseRay(theX, theY) {

	//print(" renderer.width="+ renderer.width + ", renderer.height="+renderer.height);
	//print(" theX="+ theX + ", theY="+theY);

    var myXPos = 2 * theX / renderer.width  - 1;
    var myYPos = - (2 * theY / renderer.height - 1);
    var myScreenPos = new Point3f(myXPos, myYPos, -1);
	//print(" myScreenPos="+ myScreenPos );

    var myInverseViewProjectionMatrix = window.projectionmatrix;
    myInverseViewProjectionMatrix.invert();
    myInverseViewProjectionMatrix.postMultiply(window.camera.globalmatrix);

	var myWorldPos = product(myScreenPos, myInverseViewProjectionMatrix);

	var myCameraPos = new Point3f(window.camera.globalmatrix.getRow(3).xyz);
	var myMouseRay = new Ray(myCameraPos, myWorldPos);

	//var myViewVector = window.camera.globalmatrix.getColumn(2).xyz;
    //myViewVector.mult(new Vector3f(-1,-1,1));
	//var myTargetPlane = new Plane(myViewVector, -1);
	//var myTargetPlane = new Plane(myViewVector, sum(myCameraPos,myViewVector));

	//var myTargetPos = intersection(myMouseRay, myTargetPlane);
    //var myMatrix = new Matrix4f();
	//myMatrix.makeIdentity();

	// Draw line from world center to mouse position
	//var myStick = new LineSegment([0, 0, 0], myTargetPos);
	//window.draw(myStick, new Vector4f(1,1,0,1), myMatrix);

	var myStick = myMouseRay;
	//window.draw(new LineSegment(myCameraPos, myTargetPos), new Vector4f(1,1,0,1), myMatrix);
	//window.draw(new LineSegment([0,0,0], myTargetPos), new Vector4f(1,1,0,1), myMatrix);

	return myStick;
}

function drawIntersection(theIntersectionInfo, theIntersection) {
    var myMatrix = new Matrix4f();
    myMatrix.makeIdentity();

    // We have to turn of backface culling to show found triangles that do not face the camera
    var myCulling = window.backfaceCulling;
    window.backfaceCulling = false;

	// Draw intersected triangles
	var myScale = new Vector3f(1.01, 1.01, 1.01);
	var myA = product(theIntersection.primitive[0], myScale);
	var myB = product(theIntersection.primitive[1], myScale);
	var myC = product(theIntersection.primitive[2], myScale);
	var myTriangle = new Triangle(myA, myB, myC);
	window.draw(myTriangle, [0.5,0,1,1], myMatrix);
    if (theIntersection.primitive.length == 4) {
	    var myD = product(theIntersection.primitive[3], myScale);
	    myTriangle = new Triangle(myA, myC, myD);
    	window.draw(myTriangle, [0.5,0,1,1], myMatrix);
    }

	// Draw normal
	var myNormalTo = sum(theIntersection.position, product(theIntersection.normal, [2,2,2]));
	window.draw(new LineSegment(theIntersection.position, myNormalTo),
				[0.5,1.0,0.5,1], myMatrix);

    // Reset backface culling to previous state
    window.backfaceCulling = myCulling;
}

try {
    var ourShow = null;
    var SceneViewer;
	var _myMouseXPos = 0;
	var _myMouseYPos = 0;

    if (arguments.length == 0) {
		ourShow = new SceneViewer("unit_cube.x60", "shaderlibrary_lambert.xml");
	}
    if (arguments.length == 1) {
		ourShow = new SceneViewer(arguments[0], "shaderlibrary_lambert.xml");
	}
    if (arguments.length == 2) {
		ourShow = new SceneViewer(arguments[0], arguments[1]);
	}

    SceneViewer.onFrame = ourShow.onFrame;
    ourShow.onFrame = function (theTime) {
        SceneViewer.onFrame(theTime);
    }

	SceneViewer.onMouseMotion = ourShow.onMouseMotion;
    ourShow.onMouseMotion = function(theX, theY) {
		_myMouseXPos = theX;
		_myMouseYPos = theY;
		SceneViewer.onMouseMotion(theX, theY);
    }

    // Picking
    SceneViewer.onMouseButton = ourShow.onMouseButton;
    ourShow.onMouseButton = function(theButton, theState, theX, theY) {
        if (theState == 1) {
    		var myStick = getMouseRay(_myMouseXPos, _myMouseYPos);
    		var myIntersectionInfo = window.intersectWorld(myStick);

            if (myIntersectionInfo) {
                var myBodyIndex = 0;
                if (myIntersectionInfo.length > 1) {
                    var myCameraPos = window.camera.globalmatrix.getRow(3).xyz;
                    var myMinDistance = 100000000;
        			for (var i = 0; i < myIntersectionInfo.length; ++i) {
        			    var myPosition = myIntersectionInfo[i].transform.getRow(3).xyz;
        			    var myDistance = vectorLength(difference(myCameraPos, myPosition));

                        if (myDistance < myMinDistance) {
                            myMinDistance = myDistance;
                            myBodyIndex = i;
        			    }
        			}
                }
    			print("  -> You picked body: " + myIntersectionInfo[myBodyIndex].body.name);
    		}
    	}
    }

    ourShow.onPostRender = function() {

		// var myUndefined;
		// myUndefined.fail();
return;
        drawUnitLines();

		var myStick = getMouseRay(_myMouseXPos, _myMouseYPos);
		var myIntersectionInfo = window.intersectWorld(myStick);

		if (myIntersectionInfo) {
			//print("--------------------------------------");
			//print(myIntersectionInfo.length + " Intersection Infos:");
			for (var i = 0; i < myIntersectionInfo.length; ++i) {
			/*
				print("  Intersection Info " + i);
				print("     Body:           " + myIntersectionInfo[i].body.id);
				print("     Shape:          " + myIntersectionInfo[i].shape.id);
				print("     Transform:      " + myIntersectionInfo[i].transform);
				print("     Inv. Transform: " + myIntersectionInfo[i].inverse_transform);
				print("     Intersections:  " + myIntersectionInfo[i].intersections.length);
			*/
				for (var j = 0; j < myIntersectionInfo[i].intersections.length; ++j) {
					var myIntersection = myIntersectionInfo[i].intersections[j];

					print("       Intersection " + j);
					print("         Position:   " + myIntersection.position);
					print("         Normal:     " + myIntersection.normal);
					print("         Primitive:  " + myIntersection.primitive);

                drawIntersection(myIntersectionInfo[i], myIntersection);
				}
			}
		}
    }

    ourShow.setup();

    window.updateScene(Renderer.ALL);
    ourShow.go();
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}

