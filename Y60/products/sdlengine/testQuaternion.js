/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testQuaternion.js,v $
//
//   $Revision: 1.6 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");
use("Y60JSSL.js");

function QuaternionUnitTest() {
    this.Constructor(this, "QuaternionUnitTest");
}

QuaternionUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.q = new Quaternionf();
        obj.q = new Quaternionf(2,4,6,8);
        ENSURE('obj.q[0] == 2'); 
        ENSURE('obj.q[1] == 4'); 
        ENSURE('obj.q[2] == 6'); 
        ENSURE('obj.q[3] == 8'); 
        obj.q2 = new Quaternionf(obj.q);
        ENSURE('almostEqual( obj.q2, obj.q)');
        ENSURE('almostEqual( obj.q2.imag, [2,4,6])');
        ENSURE('almostEqual( obj.q2.real, 8)');
       
        obj.q2.imag = [1,2,3];
        ENSURE('almostEqual( obj.q2.imag, [1,2,3])');
        obj.q2.real = 4;
        ENSURE('almostEqual( obj.q2, [1,2,3,4])');
        
        var myAxis = new Vector3f(1.0, 0.0, 0.0);
        obj.q2 = new Quaternionf(myAxis, 0);

        obj.myResult = new Quaternionf();

        obj.v = new Vector3f();
        obj.v = cross(obj.q.imag, obj.q2.imag);
        obj.v = sum( obj.v, product(obj.q.imag, obj.q2.real));
        obj.v = sum( obj.v, product(obj.q2.imag, obj.q.real)); 

        obj.myResult.imag = obj.v;
        obj.myResult.real = obj.q.real * obj.q2.real - 
                            dot( obj.q.imag, obj.q2.imag);

        ENSURE('almostEqual( obj.myResult, product( obj.q, obj.q2 ))');
        ENSURE('product(obj.q, obj.q2) instanceof Quaternionf');

        obj.myV = new Vector4f(-1.0, 0.0, 0.0, 1.0);
        obj.myQ = Quaternionf.createFromEuler( new Vector3f( 0.0, 0.0, Math.PI * 0.5));
        obj.myResult = product(obj.myV, obj.myQ);
        product(obj.myResult, 1.0);
        print(obj.myResult);
        ENSURE('almostEqual(obj.myResult, [0.0,-1.0,0.0,1.0])');

        obj.myResult = obj.q;
        obj.myResult.multiply(obj.q2);
        obj.myResult1 = product( obj.q, obj.q2 );
        ENSURE('almostEqual( obj.myResult, obj.myResult1)');


        // Test lerp (linear interpolation)
        obj.myQuat1 = Quaternionf.createFromEuler(new Vector3f(0,0,0));
        obj.myQuat2 = Quaternionf.createFromEuler(new Vector3f(Math.PI * 0.5,0,0));
        obj.myV1 = new Vector4f(0,0,1,1);
        obj.myResultQuat = Quaternionf.lerp(obj.myQuat1, obj.myQuat2, 1.0);
        obj.myResultVector = product(obj.myV1, obj.myResultQuat);
        ENSURE('almostEqual(obj.myResultVector, [0,-1,0,1])');

        // Test slerp (spherical linear interpolation)
        obj.myQuat3 = Quaternionf.createFromEuler(new Vector3f(0,0,0));
        obj.myQuat4 = Quaternionf.createFromEuler(new Vector3f(Math.PI * 0.5, 0, 0));
        obj.myV2 = new Vector4f(0,0,1,1);
        obj.myResultQuat1 = Quaternionf.slerp(obj.myQuat3, obj.myQuat4, 1.0);
        obj.myResultVector1 = product(obj.myV2, obj.myResultQuat1);
        ENSURE('almostEqual(obj.myResultVector1, [0,-1,0,1])');


        // Test matrix construction
        obj.myQuat5 = Quaternionf.createFromEuler( new Vector3f(0.0, 0.0, 0.0));
        obj.myMatrix = new Matrix4f( obj.myQuat5);
        obj.myExpectedMatrix = new Matrix4f;
        obj.myExpectedMatrix.makeIdentity();
        ENSURE('almostEqual(obj.myMatrix, obj.myExpectedMatrix)');

        // Test matrix construction seperately for each component,
        // because of the ambiguity of euler
        obj.myQuat6 = Quaternionf.createFromEuler( new Vector3f(1.0, 0.0, 0.0));
        obj.myMatrix = new Matrix4f( obj.myQuat6);
        obj.myExpectedMatrix = new Matrix4f;
        obj.myExpectedMatrix.makeXYZRotating(new Vector3f(1.0, 0.0, 0.0));
        ENSURE('almostEqual(obj.myMatrix, obj.myExpectedMatrix)');

        obj.myQuat6 = Quaternionf.createFromEuler( new Vector3f(0.0, 1.0, 0.0));
        obj.myMatrix = new Matrix4f( obj.myQuat6);
        obj.myExpectedMatrix = new Matrix4f;
        obj.myExpectedMatrix.makeXYZRotating(new Vector3f(0.0, 1.0, 0.0));
        ENSURE('almostEqual(obj.myMatrix, obj.myExpectedMatrix)');

        obj.myQuat6 = Quaternionf.createFromEuler( new Vector3f(0.0, 0.0, 1.0));
        obj.myMatrix = new Matrix4f( obj.myQuat6);
        obj.myExpectedMatrix = new Matrix4f;
        obj.myExpectedMatrix.makeXYZRotating(new Vector3f(0.0, 0.0, 1.0));
        ENSURE('almostEqual(obj.myMatrix, obj.myExpectedMatrix)');
                
        obj.testGetOrientationFromDirection([0,0,-1],  [0,0,-1]);
        obj.testGetOrientationFromDirection([0,0,-1],  [0,1,0]);
        obj.testGetOrientationFromDirection([1,1,0],  [0,0,1]);
        obj.testGetOrientationFromDirection([1,1,0],  [0,1,0]);        
        obj.testGetOrientationFromDirection([1,0,-1], [0,1,0]);
        obj.testGetOrientationFromDirection([0,1,-1], [1,0,0]);
        obj.testGetOrientationFromDirection([0,0,1],  [1,1,1]);
        obj.testGetOrientationFromDirection([1,1,1],  [1,0,0]);
        obj.testGetOrientationFromDirection([0,1,0],  [1,0,0]);
        obj.testGetOrientationFromDirection([1,0,0],  [0,1,0]);
        obj.testGetOrientationFromDirection([1,1,-1], [1,0,0]);
        obj.testGetOrientationFromDirection([0.1,0.2,0.3], [1,0,1]);
        obj.testGetOrientationFromDirection([-1.1,-0.5,1.2], [1.4,0.1,-1.5]);
        
    }
    
    obj.testGetOrientationFromDirection = function(theViewVector, theUpVector) {
        obj.myQuaternion = getOrientationFromDirection(normalized(theViewVector), normalized(theUpVector));
        obj.myViewVector = theViewVector;
        obj.myUpVector = theUpVector;
        
        obj.myResViewVector = product([0,0,-1], obj.myQuaternion);
        ENSURE("almostEqual(normalized(obj.myViewVector), obj.myResViewVector)");
        obj.myResUpVector = product([0,1,0], obj.myQuaternion);
        obj.myPlane = new Plane(cross(obj.myUpVector, obj.myResViewVector),[0,0,0]);
        ENSURE("almostEqual(obj.myResUpVector, projection(obj.myResUpVector, obj.myPlane))");
    }
}

function main() {
    var myTestName = "testQuaternion.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new QuaternionUnitTest());
    mySuite.run();

    print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    exit(1);
};


