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
//   $RCSfile: testSweptSphereWorld.js,v $
//   $Author: danielk $
//   $Revision: 1.6 $
//   $Date: 2005/01/20 17:22:26 $
//
//
//=============================================================================

use("SceneViewer.js");

var myIdentity = new Matrix4f();
myIdentity.makeIdentity();

function drawUnitLines() {
	window.draw(new LineSegment([0, 0, 0], [1,0,0]), [1,0,0,1], myIdentity);
	window.draw(new LineSegment([0, 0, 0], [0,1,0]), [0,1,0,1], myIdentity);
	window.draw(new LineSegment([0, 0, 0], [0,0,1]), [0,0,1,1], myIdentity);
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
    // var myMatrix = new Matrix4f();
	// myMatrix.makeIdentity();

	// Draw line from world center to mouse position
	//var myStick = new LineSegment([0, 0, 0], myTargetPos);
	//window.draw(myStick, new Vector4f(1,1,0,1), myMatrix);

	var myStick = myMouseRay;
	//window.draw(new LineSegment(myCameraPos, myTargetPos), new Vector4f(1,1,0,1), myMatrix);
	//window.draw(new LineSegment([0,0,0], myTargetPos), new Vector4f(1,1,0,1), myMatrix);

	return myStick;
}

function drawPrimitive(thePrimitive, theColor) {
    // We have to turn of backface culling to show found triangles that do not face the camera
    var myCulling = window.backfaceCulling;
    window.backfaceCulling = false;

	// Draw intersected triangles
	var myScale = new Vector3f(1.01, 1.01, 1.01);
	var myA = product(thePrimitive[0], myScale);
	var myB = product(thePrimitive[1], myScale);
	var myC = product(thePrimitive[2], myScale);
	var myTriangle = new Triangle(myA, myB, myC);
	
	window.draw(myTriangle, theColor, myIdentity);
    if (thePrimitive.length == 4) {
	    var myD = product(thePrimitive[3], myScale);
	    myTriangle = new Triangle(myA, myC, myD);
    	window.draw(myTriangle, theColor, myIdentity);
    }
/*
	// Draw normal
	var myNormalTo = sum(theIntersection.position, product(theIntersection.normal, [2,2,2]));
	window.draw(new LineSegment(theIntersection.position, myNormalTo),
				[0.5,1.0,0.5,1], myMatrix);
*/
    // Reset backface culling to previous state
    window.backfaceCulling = myCulling;
}

try {
    var ourShow = null;
    var SceneViewer;
	var _myMouseXPos = 0;
	var _myMouseYPos = 0;


    ourShow = new SceneViewer(arguments);

/*if (arguments.length == 0) {
		ourShow = new SceneViewer("unit_cube.x60", "shaderlibrary_lambert.xml");
	}
    if (arguments.length == 1) {
		ourShow = new SceneViewer(arguments[0], "shaderlibrary_lambert.xml");
	}
    if (arguments.length == 2) {
		ourShow = new SceneViewer(arguments[0], arguments[1]);
	}
*/

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
/*
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
*/
    ourShow.onPostRender = function() {
    
		// var myUndefined;
		// myUndefined.fail();
    
        drawUnitLines();

		var myStick = getMouseRay(_myMouseXPos, _myMouseYPos);
		var mySphere = new Sphere(sum(myStick.origin,myStick.direction), 0.1);
		var myMotion = product(myStick.direction,100);
		// print ("Sphere:" + mySphere);
		// print ("myMotion:" + myMotion);
    	// window.draw(new LineSegment(mySphere.center, [0,0,0]),[1,1,0,1], myIdentity);
    	// window.draw(new LineSegment(sum(mySphere.center,myMotion), [1,1,1]),[1,1,0,1], myIdentity);
    	window.draw(new LineSegment(mySphere.center, sum(mySphere.center,myMotion)),[1,1,0,1], myIdentity);
    		
    	var myCollisions;
		// myCollisions = window.collideWithWorld(mySphere, myMotion);

		if (myCollisions) {
			// print("--------------------------------------");
			// print(myCollisions.length + " Body Collisions:");
			
			for (var i = 0; i < myCollisions.length; ++i) {
			/*
				print("  Body  Info " + i);
				print("     Body:           " + myCollisions[i].body.id);
				print("     Shape:          " + myCollisions[i].shape.id);
				print("     Transform:      " + myCollisions[i].transform);
				print("     Inv. Transform: " + myCollisions[i].inverse_transform);
				print("     Intersections:  " + myCollisions[i].intersections.length);
			*/	
				for (var j = 0; j < myCollisions[i].collisions.length; ++j) {
					myCollision = myCollisions[i].collisions[j];
					
					// print("       myCollision " + j);
					// print("         Primitive:  " + myCollision.primitive);
			
                    drawPrimitive(myCollision.primitive, [0.3,0,0.7,1]);
				}
			}
			
		}
		var myFirstContact;
		myFirstContact = window.collideWithWorldOnce(mySphere, myMotion);

		if (myFirstContact) {
		    /*
				print("  Body  Info " + i);
				print("     Body:           " + myCollision.body.id);
				print("     Shape:          " + myCollision.shape.id);
				print("     Transform:      " + myCollision.transform);
				print("     Inv. Transform: " + myCollision.inverse_transform);
				print("     Intersections:  " + myCollision.intersections.length);
			*/	
			for (j = 0; j < myFirstContact.collisions.length; ++j) {
				myCollision = myFirstContact.collisions[j];
				
				// print("       myCollision " + j);
				// print("         Primitive:  " + myCollision.primitive);
		
                drawPrimitive(myCollision.primitive, [0.5,0,1,1]);
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

