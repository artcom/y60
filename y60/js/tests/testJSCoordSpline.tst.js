/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2010, ART+COM AG Berlin, Germany <www.artcom.de>
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
*/
/*global use, UnitTest, UnitTestSuite, Keyframe, Quaternionf, CoordSpline, */ 
/*global product, almostEqual, print, exit, ENSURE, difference, magnitude */

use("UnitTest.js");

function CoordSplineTest() {
    this.Constructor(this, "CoordSplineTest");
}

// Testing our spline implementation is an interesting exercise. We don't want to simply
// compare the results with pre-computed values - reimplementing the spline would surely break the tests.
//
// Much better is to test the 'contract': what does the CoordSpline promise to deliver?
//
// 1) It will hit all the keyframe positions at the keyframes' times
// 2) It will have the keyframe orientations at the keyframes' times
// 3) The positions are continuous (no large jumps in the positions over time)
// 4) The path is smooth (no large jumps in the first derivative of positions over time)
// 5) The rotations are continuous (no large jumps in the orientations over time)


CoordSplineTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    // set up our spline. We have three keyframes, with 1 time unit between them.

    obj.keyFrames = [];
    obj.keyFrames.push(new Keyframe([-1,0,0],  Quaternionf.createFromEuler([0,0,0]), 1, 0.5));
    obj.keyFrames.push(new Keyframe([ 0,1,0],  Quaternionf.createFromEuler([0,Math.PI/4,Math.PI/4]), 1, 0.5));
    obj.keyFrames.push(new Keyframe([ 1,0,0],  Quaternionf.createFromEuler([0,Math.PI/2,Math.PI/2]), 1, 0.5));
    obj.myPath = new CoordSpline(obj.keyFrames);


    //
    // a few utility functions
    //
    
    // scans an array looking for the largest value. 'toScalar' is used
    // on convert the array elements to a scalar value.
    function maxScalar(theArray, toScalar) {
        var max = toScalar(theArray[0]);
        var len = theArray.length;
        for (var i = 1; i < len; i++) { 
            if (toScalar(theArray[i]) > max) { 
                max = toScalar(theArray[i]);
            }
        }
        return max;
    }

    // given an array of samples, creates the 1st derivitive.
    // theFunc is used to calculate the deltas
    function derive(theArray, theFunc) {
        var derivedArray = [];
        for (var i = 1; i < theArray.length; ++i) {
            derivedArray.push(theFunc(theArray[i], theArray[i-1]));
        }
        return derivedArray;
    }

    // returns 'x' for a * x = b 
    function rotationalDifference(a,b) {
        var invertB = new Quaternionf(b);
        invertB.invert();
        return product(a, invertB); 
    }

    // returns the angle rotated as an absolute float (0..PI) 
    function quaternionMagnitude(a) {
        var angle = a.angle;
        if (angle > Math.PI) {
            return Math.abs(2*Math.PI-angle);
        } else {
            return angle;
        }
    }
 
    // returns true if two rotations are almost equivalent 
    obj.almostEqualRotation = function(a,b) {
        //XXX: almostEqual for doubles has a threshold of 0.00001 for equality, 
        //     on linux 32 bit, the Quaternion methods (probably the createFromEuler)
        //     produce inaccuracies that deliver values with differences of less than
        //     power of 3
        return (quaternionMagnitude(rotationalDifference(a,b)) <= 0.01);
    };

    //
    // now our tests
    //

   // 1) It will hit all the keyframe positions at the keyframes' times
    function testKeyframePositions() {
        ENSURE('almostEqual(obj.myPath.position(0), obj.keyFrames[0].position)');
        ENSURE('almostEqual(obj.myPath.position(1), obj.keyFrames[1].position)');
        ENSURE('almostEqual(obj.myPath.position(2), obj.keyFrames[2].position)'); 
    }

    // 2) It will have the keyframe orientations at the keyframes' times
    function testKeyframeOrientations() {
        ENSURE('obj.almostEqualRotation(obj.myPath.orientation(0), obj.keyFrames[0].orientation)');
        ENSURE('obj.almostEqualRotation(obj.myPath.orientation(1), obj.keyFrames[1].orientation)');
        ENSURE('obj.almostEqualRotation(obj.myPath.orientation(2), obj.keyFrames[2].orientation)');
    }

    // 3) The positions are continuous (no large jumps in the positions over time)
    function testPositionsContinuous() {
        var splinePositions = [];
        for (var i = 0; i < 2; i += 0.01) {
            splinePositions.push(obj.myPath.position(i));
        }
        var splineMovement = derive(splinePositions, difference);
        obj.maxMovement = maxScalar(splineMovement, magnitude);
        ENSURE('obj.maxMovement < 0.02');
    }

    // 4) The path is smooth (no large jumps in the first derivative of positions over time)
    function testMovementSmooth() {
        var splinePositions = [];
        for (var i = 0; i < 2; i += 0.01) {
            splinePositions.push(obj.myPath.position(i));
        }
        var splineMovement = derive(splinePositions, difference);
        var splineCurvature = derive(splineMovement, difference);
        obj.maxCurvature = maxScalar(splineCurvature, magnitude);
        ENSURE('obj.maxCurvature < 0.001');
    }

    // 5) The rotations are continuous (no large jumps in the orientations over time)
    function testRotationsContinuous() {
        obj.splineOrientations = [];
        for (var i = 0; i < 2; i += 0.01) {
            obj.splineOrientations.push(obj.myPath.orientation(i));
        }
        var splineRotation = derive(obj.splineOrientations, rotationalDifference);
        obj.maxRotation = maxScalar(splineRotation, quaternionMagnitude);
        // we have to rotate 180° in 200 steps, give some leeway and we'll say 2° per step
        ENSURE('degFromRad(obj.maxRotation) < 2.0');
    }


    obj.run = function() {
        testKeyframePositions();
        testKeyframeOrientations();
        testPositionsContinuous();
        testMovementSmooth();
        testRotationsContinuous();
    };
};

try {
    var myScriptPathSegments = __FILE__().split("/");
    var mySuiteName = myScriptPathSegments[myScriptPathSegments.length - 1];
    var mySuite = new UnitTestSuite(mySuiteName);

    mySuite.addTest(new CoordSplineTest()); 
    mySuite.run();

    print(">> Finished test suite '" + mySuiteName + "', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}

