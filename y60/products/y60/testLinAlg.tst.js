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


use("UnitTest.js");
use("Timer.js");

var myRenderer = new Renderer();

function FixedVectorUnitTest() {
    this.Constructor(this, "FixedVectorUnitTest");
};

FixedVectorUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function almostEqual(a, b) {
        return (Math.abs(a - b) < 0.00001);
    }

    obj.run = function() {
        obj.myVec = new Vector2f(2,3);
        ENSURE('almostEqual(new Vector2f(1,-2), new Vector2f(1,-2))');
        ENSURE('myVec[0]==2');
        ENSURE('myVec[0]==2');
        ENSURE('myVec[1]==3');
        ENSURE('myVec.length==2');
        ENSURE('myVec.x == 2');
        ENSURE('myVec.y == 3');

        obj.myOtherVec = new Vector2f(0,0);
        obj.myOtherVec.value = (obj.myVec);
        ENSURE('myOtherVec[0]==2');
        ENSURE('myOtherVec[1]==3');

        ENSURE('almostEqual(new Vector2f(1,-2), new Vector2f(1,-2))');
        ENSURE('!almostEqual(new Vector2f(1,2), new Vector2f(1,-2))');

        obj.myVec.sub(obj.myVec);
        ENSURE('almostEqual(obj.myVec, new Vector2f(0,0))');
        obj.myVec.add(new Vector2f(1,2));
        ENSURE('almostEqual(obj.myVec, new Vector2f(1,2))');
        obj.myVec.add(new Vector2f(3,4));
        ENSURE('almostEqual(obj.myVec, new Vector2f(4,6))');
        obj.myVec.div(new Vector2f(2,2));
        ENSURE('almostEqual(obj.myVec, new Vector2f(2,3))');
        obj.myVec.mult(new Vector2f(3,3));
        ENSURE('almostEqual(obj.myVec, new Vector2f(6,9))');
        obj.myVec.mult(new Vector2f(3,4));
        ENSURE('almostEqual(obj.myVec, new Vector2f(18,36))');
        obj.myVec.div(new Vector2f(3,4));
        ENSURE('almostEqual(obj.myVec, new Vector2f(6,9))');
        obj.myVec.mult([-1,-1]);
        ENSURE('almostEqual(obj.myVec, new Vector2f(-6,-9))');
        obj.myVec.add(new Vector2f(12,12));
        ENSURE('almostEqual(obj.myVec, new Vector2f(6,3))');
        obj.myVec.sub(new Vector2f(2,2));
        ENSURE('almostEqual(obj.myVec, new Vector2f(4,1))');

        obj.myVector = new Vector2f(1,2);
        ENSURE('almostEqual(obj.myVector, new Vector2f(1,2))');
        ENSURE('obj.myVector.toString() == "[1,2]"');

        obj.myVector = new Vector3f(1,2,3);
        ENSURE('almostEqual(obj.myVector, new Vector3f(1,2,3))');
        ENSURE('obj.myVector.toString() == "[1,2,3]"');

        obj.myVector = new Vector4f(1,2,3,4);
        obj.myVector.x = 4;
        obj.myVector.y = 3;
        obj.myVector.z = 2;
        obj.myVector.w = 1;
        ENSURE('myVector.x == 4');
        ENSURE('myVector.y == 3');
        ENSURE('myVector.z == 2');
        ENSURE('myVector.w == 1');

        obj.myVector = new Vector4f(1,2,3,4);
        ENSURE('almostEqual(obj.myVector, new Vector4f(1,2,3,4))');
        ENSURE('obj.myVector.toString() == "[1,2,3,4]"');
        ENSURE('almostEqual(1.0000001, 1.0)');
        ENSURE('almostEqual(1.0, 1.0)');
        ENSURE('!almostEqual(1.0, 1.01)');

        // common Vector4f Tests
        ENSURE('almostEqual(new Vector4f(1,2,3,4),new Vector4f(1,2,3,4))');
        ENSURE('!almostEqual(new Vector4f(1,2,3,4),new Vector4f(1,2,3,4.01))');
        ENSURE('almostEqual(sum(new Vector4f(1,2,3,4),new Vector4f(4,3,2,1)),new Vector4f(5,5,5,5))');
        ENSURE('almostEqual(difference(new Vector4f(1,2,3,4),new Vector4f(4,3,2,1)),new Vector4f(-3,-1,1,3))');
        ENSURE('almostEqual(product(new Vector4f(1,2,3,4),new Vector4f(4,3,2,1)),new Vector4f(4,6,6,4))');
        ENSURE('almostEqual(quotient(new Vector4f(1,2,3,4),new Vector4f(1,2,3,4)),new Vector4f(1,1,1,1))');

        ENSURE('almostEqual(dot(new Vector4f(1,2,3,4),new Vector4f(5,6,7,8)), 1*5+2*6+3*7+4*8)');
        ENSURE('almostEqual(magnitude(new Vector4f(1,2,3,4)), Math.sqrt(1*1+2*2+3*3+4*4))');
        ENSURE('almostEqual(normalized(new Vector4f(0,0,0,9)),new Vector4f(0,0,0,1))');
        obj.myLen = Math.sqrt(1*1+2*2+3*3+4*4);
        ENSURE('almostEqual(normalized(new Vector4f(1,2,3,4)),new Vector4f(1/obj.myLen,2/obj.myLen,3/obj.myLen,4/obj.myLen))');

        // common new Vector3f Tests
        ENSURE('almostEqual(new Vector3f(1,2,3),new Vector3f(1,2,3))');
        ENSURE('!almostEqual(new Vector3f(1,2,3),new Vector3f(1,2,3.01))');
        ENSURE('almostEqual(sum(new Vector3f(1,2,3),new Vector3f(4,3,2)),new Vector3f(5,5,5))');
        ENSURE('almostEqual(difference(new Vector3f(1,2,3),new Vector3f(4,3,2)),new Vector3f(-3,-1,1))');
        ENSURE('almostEqual(product(new Vector3f(1,2,3),new Vector3f(4,3,2)),new Vector3f(4,6,6))');
        ENSURE('almostEqual(quotient(new Vector3f(1,2,3),new Vector3f(1,2,3)),new Vector3f(1,1,1))');

        ENSURE('almostEqual(dot(new Vector3f(1,2,3),new Vector3f(5,6,7)), 1*5+2*6+3*7)');
        ENSURE('almostEqual(magnitude(new Vector3f(1,2,3)), Math.sqrt(1*1+2*2+3*3))');
        ENSURE('almostEqual(normalized(new Vector3f(0,0,9)),new Vector3f(0,0,1))');
        obj.myLen = Math.sqrt(1*1+2*2+3*3);
        ENSURE('almostEqual(normalized(new Vector3f(1,2,3)),new Vector3f(1/obj.myLen,2/obj.myLen,3/obj.myLen))');

        // special Vector3f Tests
        // cross and normals test

        obj.myResult = cross(new Vector3f(0,0,1), new Vector3f(1,0,0));
        ENSURE('almostEqual(myResult , new Vector3f(0,1,0))', "Crossproduct of x- and z- axis.");

        obj.myResult1 = cross(new Vector3f(0,0,-1), new Vector3f(-1,0,0));
        ENSURE('almostEqual(myResult1 , new Vector3f(0,1,0))', "Crossproduct of  -x- and -z- axis.");

        obj.myResult2 = cross(new Vector3f(0,0,1), new Vector3f(1,0,1));
        ENSURE('almostEqual(myResult2 , new Vector3f(0,1,0))', "Crossproduct of (1,0,1) and z- axis.");

        obj.myResult3 = cross(new Vector3f(1,0,-1), new Vector3f(0,1,1));
        ENSURE('almostEqual(myResult3 , new Vector3f(1,-1,1))', "Crossproduct of (1,0,-1) and (0,1,1).");

        // common Vector2f Tests
        ENSURE('almostEqual(new Vector2f(1,2),new Vector2f(1,2))');
        ENSURE('!almostEqual(new Vector2f(1,2),new Vector2f(1,2.01))');
        ENSURE('almostEqual(sum(new Vector2f(1,2),new Vector2f(4,3)),new Vector2f(5,5))');
        ENSURE('almostEqual(difference(new Vector2f(1,2),new Vector2f(4,3)),new Vector2f(-3,-1))');
        ENSURE('almostEqual(product(new Vector2f(1,2),new Vector2f(4,3)),new Vector2f(4,6))');
        ENSURE('almostEqual(quotient(new Vector2f(1,2),new Vector2f(1,2)),new Vector2f(1,1))');

        ENSURE('almostEqual(dot(new Vector2f(1,2),new Vector2f(5,6)), 1*5+2*6)');
        ENSURE('almostEqual(magnitude(new Vector2f(1,2)), Math.sqrt(1*1+2*2))');
        ENSURE('almostEqual(normalized(new Vector2f(0,9)),new Vector2f(0,1))');
        obj.myLen = Math.sqrt(1*1+2*2);
        ENSURE('almostEqual(normalized(new Vector2f(1,2)),new Vector2f(1/obj.myLen,2/obj.myLen))');

        // String constructor
        obj.myVector = new Vector2f("[2,3]");
        ENSURE('almostEqual(obj.myVector, new Vector2f(2,3))');
        obj.myVector = new Vector3f("[2,3,4]");
        ENSURE('almostEqual(obj.myVector, new Vector3f(2,3,4))');
        obj.myVector = new Vector4f("[2,3,4,5]");
        ENSURE('almostEqual(obj.myVector, new Vector4f(2,3,4,5))');
    }
}

function Matrix4fUnitTest() {
    this.Constructor(this, "Matrix4fUnitTest");
};

Matrix4fUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {

        obj.myMatrix = new Matrix4f(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);

        ENSURE('almostEqual(new Vector4f(0,1,2,3),obj.myMatrix.getRow(0))');
        ENSURE('almostEqual(new Vector4f(4,5,6,7),obj.myMatrix.getRow(1))');
        ENSURE('almostEqual(new Vector4f(8,9,10,11),obj.myMatrix.getRow(2))');
        ENSURE('almostEqual(new Vector4f(12,13,14,15),obj.myMatrix.getRow(3))');

        ENSURE('almostEqual(new Vector4f(0,4,8,12),obj.myMatrix.getColumn(0))');
        ENSURE('almostEqual(new Vector4f(1,5,9,13),obj.myMatrix.getColumn(1))');
        ENSURE('almostEqual(new Vector4f(2,6,10,14),obj.myMatrix.getColumn(2))');
        ENSURE('almostEqual(new Vector4f(3,7,11,15),obj.myMatrix.getColumn(3))');

        ENSURE('almostEqual(new Vector4f(0,1,2,3),obj.myMatrix[0])');
        ENSURE('almostEqual(new Vector4f(4,5,6,7),obj.myMatrix[1])');
        ENSURE('almostEqual(new Vector4f(8,9,10,11),obj.myMatrix[2])');
        ENSURE('almostEqual(new Vector4f(12,13,14,15),obj.myMatrix[3])');

        for (var i = 0; i < 4; ++i) {
            DTITLE("Testing row "+String(i));
            obj.myExpectedResult = new Vector4f(i * 4, i * 4 + 1, i * 4 + 2, i * 4 + 3);
            DPRINT('obj.myExpectedResult.toString()');
            obj.i = i;
            DPRINT('obj.myMatrix[obj.i].toString()');
            ENSURE('obj.myMatrix[obj.i].toString() == obj.myExpectedResult.toString()');
            ENSURE('almostEqual(obj.myMatrix[obj.i], obj.myExpectedResult)');
        }
        DPRINT('new Matrix4f(obj.myMatrix.toString()).toString()');
        ENSURE('obj.myMatrix.toString() == new Matrix4f(obj.myMatrix.toString()).toString()');

        ENSURE('obj.myMatrix.type == Matrix4f.UNKNOWN', "Testing type");

        obj.myMatrix2 = new Matrix4f(obj.myMatrix);
        ENSURE('almostEqual(obj.myMatrix,obj.myMatrix2)');

        // makeIdentity
        DTITLE("Testing makeIdentity");
        obj.myMatrix.makeIdentity();
        DPRINT('obj.myMatrix.type');
        DPRINT('Matrix4f.IDENTITY');

        ENSURE('obj.myMatrix.type == Matrix4f.IDENTITY', "Test makeIdentity()");
        obj.myIdentity = new Matrix4f(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
        ENSURE('almostEqual(obj.myMatrix, myIdentity)', "Test makeIdentity()");

        DTITLE("Testing makeScaling");
        obj.myMatrix = new Matrix4f(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
        obj.myMatrix.makeScaling(new Vector3f(-1, 2, 3));
        DPRINT('obj.myMatrix.type');
        ENSURE('obj.myMatrix.type != Matrix4f.IDENTITY', "Test makeScaling()");
        ENSURE('obj.myMatrix.type == Matrix4f.SCALING', "Test makeScaling()");
        obj.myScaling = new Matrix4f(-1,0,0,0,0,2,0,0,0,0,3,0,0,0,0,1);
        ENSURE('almostEqual(obj.myMatrix, myScaling)', "Test makeScaling()");

        DTITLE("Testing makeTranslating");
        obj.myMatrix= new Matrix4f(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
        obj.myMatrix.makeTranslating(new Vector3f(3, 2, -1));
        DPRINT('obj.myMatrix.type');
        ENSURE('obj.myMatrix.type != Matrix4f.IDENTITY', "Test makeScaling()");
        ENSURE('obj.myMatrix.type == Matrix4f.TRANSLATING', "Test makeTranslating()");
        obj.myTranslating = new Matrix4f(1,0,0,0,0,1,0,0,0,0,1,0,3,2,-1,1);
        ENSURE('almostEqual(obj.myMatrix, obj.myTranslating)', "Test makeTranslating()");

        DTITLE("Testing rotate");
        obj.myIdentity = new Matrix4f();
        obj.myIdentity.makeIdentity();

        obj.myMatrix = new Matrix4f();
        obj.myMatrix.makeIdentity();

        obj.PI = Math.PI;

        with (obj) {
            myMatrix.rotateX(0);
            ENSURE('almostEqual(myMatrix, myIdentity)');
            myMatrix.rotateY(0);
            ENSURE('almostEqual(myMatrix, myIdentity)');
            myMatrix.rotateZ(0);
            ENSURE('almostEqual(myMatrix, myIdentity)');

            myMatrix.makeIdentity();
            myMatrix.rotateX(2 * PI);
            ENSURE('almostEqual(myMatrix, myIdentity)');

            myMatrix.makeIdentity();
            myMatrix.rotateY(2 * PI);
            ENSURE('almostEqual(myMatrix, myIdentity)');

            myMatrix.makeIdentity();
            myMatrix.rotateZ(2 * PI);
            ENSURE('almostEqual(myMatrix, myIdentity)');

            myMatrix.makeIdentity();
            myMatrix.rotateX(PI);
            myMatrix.rotateX(PI);
            ENSURE('almostEqual(myMatrix, myIdentity)');

            myMatrix.makeIdentity();
            myMatrix.rotateY(PI);
            myMatrix.rotateY(PI);
            ENSURE('almostEqual(myMatrix, myIdentity)');

            myMatrix.makeIdentity();
            myMatrix.rotateZ(PI);
            myMatrix.rotateZ(PI);
            ENSURE('almostEqual(myMatrix, myIdentity)');

            myMatrix.makeIdentity();
            myMatrix.rotateZ(PI);
            myMatrix.rotateX(PI);
            myMatrix.rotateY(PI);
            ENSURE('almostEqual(myMatrix, myIdentity)');

            obj.myExpectedResult = new Matrix4f();
            myExpectedResult.makeIdentity();

            myMatrix.makeXRotating(0);
            ENSURE('almostEqual(myMatrix, myExpectedResult)', "Testing makeXRotating");
            myMatrix.makeXRotating(2.0 * PI);
            ENSURE('myMatrix.type == Matrix4f.X_ROTATING');
            ENSURE('almostEqual(myMatrix, myExpectedResult)');
            myMatrix.makeXRotating(PI/4);
            myExpectedResult.rotateX(PI/4);
            ENSURE('almostEqual(myMatrix, myExpectedResult)');
            myExpectedResult.makeIdentity();

            myMatrix.makeYRotating(0);
            ENSURE('almostEqual(myMatrix, myExpectedResult)', "Testing makeYRotating");
            myMatrix.makeYRotating(2.0 * PI);
            ENSURE('myMatrix.type == Matrix4f.Y_ROTATING');
            ENSURE('almostEqual(myMatrix, myExpectedResult)');
            myMatrix.makeYRotating(PI/4);
            myExpectedResult.rotateY(PI/4);
            ENSURE('almostEqual(myMatrix, myExpectedResult)');
            myExpectedResult.makeIdentity();

            myMatrix.makeZRotating(0);
            ENSURE('almostEqual(myMatrix, myExpectedResult)', "Testing makeZRotating");
            myMatrix.makeZRotating(2.0 * PI);
            ENSURE('myMatrix.type == Matrix4f.Z_ROTATING');
            ENSURE('almostEqual(myMatrix, myExpectedResult)');
            myMatrix.makeZRotating(PI/4);
            myExpectedResult.rotateZ(PI/4);
            ENSURE('almostEqual(myMatrix, myExpectedResult)');
            myExpectedResult.makeIdentity();

            myMatrix.makeRotating(new Vector3f(1, 0, 0), 0.123);
            ENSURE('myMatrix.type ==Matrix4f.ROTATING');
            myExpectedResult.makeXRotating(0.123);
            ENSURE('almostEqual(myMatrix, myExpectedResult)', "Testing makeRotating");
            myMatrix.makeRotating(new Vector3f(1.0, 2.0, 3.0), 2 * PI);
            myExpectedResult.makeRotating(new Vector3f(1.0, 2.0, 3.0), 0);
            ENSURE('almostEqual(myMatrix, myExpectedResult)');

            // Test Vector * Matrix
            myMatrix = new Matrix4f(0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15);
            obj.myVector = new Vector4f(0,1,2,3);
            ENSURE('almostEqual(product(obj.myVector,obj.myMatrix), new Vector4f(56, 62, 68, 74))',
                    "Testing vector * matrix");

            myMatrix = new Matrix4f(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
            myVector = new Vector4f(-1,1,-1,1);
            ENSURE('almostEqual(product(myVector,myMatrix), new Vector4f(0,0,0,0))',
                    "Testing vector * matrix");

            // Point * Matrix
            obj.myPoint = new Point3f(0,1,2);
            myMatrix = new Matrix4f(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
            obj.myResult = product(new Vector4f(0, 1, 2, 1), obj.myMatrix);
            var f = 1.0 / dot(new Vector4f(0,1,2,1), myMatrix.getColumn(3));
            obj.myResult.mult(new Vector4f(f,f,f,f));
            obj.myProduct = product(myPoint,myMatrix);
            DPRINT('obj.myResult.toString()');
            DPRINT('obj.myProduct.toString()');
            ENSURE('almostEqual(obj.myProduct, new Point3f(myResult[0],myResult[1],myResult[2]))', "Testing point * matrix");
            myMatrix = new Matrix4f(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
            myPoint = new Point3f(-1,1,-1);
            myResult = product(new Vector4f(-1, 1, -1, 1), myMatrix);
            ENSURE(almostEqual(product(myPoint,myMatrix),
                        new Point3f(myResult[0],myResult[1],myResult[2])), "Testing point * matrix");

            // getTranslation
            myMatrix = new Matrix4f(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
            ENSURE(almostEqual(myMatrix.getTranslation(), new Vector3f(12, 13, 14)), "Testing getTranslation()");
        }

        {
            // Test setRow / setColumn
            var myRowMatrix = new Matrix4f();
            var myColMatrix = new Matrix4f();
            for (var j = 0; j < 4; ++j) {
                var myVector = new Vector4f(j*1, j*2, j*3, j*4);
                myRowMatrix.setRow(j, myVector);
                ENSURE(almostEqual(myVector, myRowMatrix.getRow(j)), "Testing getRow()");
                myColMatrix.setColumn(j, myVector);
                ENSURE(almostEqual(myVector, myColMatrix.getColumn(j)), "Testing getColumn()");
            }
        }
    }
};
function FixedPointUnitTest() {
    this.Constructor(this, "FixedPointUnitTest");
};

FixedPointUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function almostEqual(a, b) {
        return (Math.abs(a - b) < 0.00001);
    }

    obj.run = function() {

        obj.myPt = new Point2f(2,3);
        ENSURE('almostEqual(new Point2f(1,-2), new Point2f(1,-2))');
        ENSURE('myPt[0]==2');
        ENSURE('myPt[0]==2');
        ENSURE('myPt[1]==3');
        ENSURE('myPt.length==2');
        ENSURE('myPt.x == 2');
        ENSURE('myPt.y == 3');

        obj.myOtherPt = new Point2f(0,0);
        obj.myOtherPt.value = obj.myPt;
        ENSURE('obj.myOtherPt[0]==2');
        ENSURE('obj.myOtherPt[1]==3');

        ENSURE('almostEqual(new Point2f(1,-2), new Point2f(1,-2))');
        ENSURE('!almostEqual(new Point2f(1,2), new Point2f(1,-2))');

        obj.myPt.sub(obj.myPt);
        ENSURE('almostEqual(obj.myPt, new Point2f(0,0))');
        obj.myPt.add(new Vector2f(1,2));
        ENSURE('almostEqual(obj.myPt, new Point2f(1,2))');
        obj.myPt.add(new Vector2f(3,4));
        ENSURE('almostEqual(obj.myPt, new Point2f(4,6))');
        obj.myPt.div(new Vector2f(2,2));
        ENSURE('almostEqual(obj.myPt, new Point2f(2,3))');
        obj.myPt.mult(new Vector2f(3,3));
        ENSURE('almostEqual(obj.myPt, new Point2f(6,9))');
        obj.myPt.mult(new Vector2f(3,4));
        ENSURE('almostEqual(obj.myPt, new Point2f(18,36))');
        obj.myPt.div(new Vector2f(3,4));
        ENSURE('almostEqual(obj.myPt, new Point2f(6,9))');
        obj.myPt.mult([-1,-1]);
        ENSURE('almostEqual(obj.myPt, new Point2f(-6,-9))');
        obj.myPt.add(new Vector2f(12,12));
        ENSURE('almostEqual(obj.myPt, new Point2f(6,3))');
        obj.myPt.sub(new Vector2f(2,2));
        ENSURE('almostEqual(obj.myPt, new Point2f(4,1))');

        obj.myPoint = new Point2f(1,2);
        ENSURE('almostEqual(obj.myPoint, new Point2f(1,2))');
        ENSURE('obj.myPoint.toString() == "[1,2]"');

        obj.myPoint = new Point3f(1,2,3);
        ENSURE('almostEqual(obj.myPoint, new Point3f(1,2,3))');
        ENSURE('obj.myPoint.toString() == "[1,2,3]"');

        obj.myPoint = new Point4f(1,2,3,4);
        obj.myPoint.x = 4;
        obj.myPoint.y = 3;
        obj.myPoint.z = 2;
        obj.myPoint.w = 1;
        ENSURE('myPoint.x == 4');
        ENSURE('myPoint.y == 3');
        ENSURE('myPoint.z == 2');
        ENSURE('myPoint.w == 1');

        obj.myPoint = new Point4f(1,2,3,4);
        ENSURE('almostEqual(obj.myPoint, new Point4f(1,2,3,4))');
        ENSURE('obj.myPoint.toString() == "[1,2,3,4]"');
        ENSURE('almostEqual(1.0000001, 1.0)');
        ENSURE('almostEqual(1.0, 1.0)');
        ENSURE('!almostEqual(1.0, 1.01)');

        // common Point4f Tests
        ENSURE('almostEqual(new Point4f(1,2,3,4),new Point4f(1,2,3,4))');
        ENSURE('!almostEqual(new Point4f(1,2,3,4),new Point4f(1,2,3,4.01))');

        // common Point3f Tests
        ENSURE('almostEqual(new Point3f(1,2,3),new Point3f(1,2,3))');
        ENSURE('!almostEqual(new Point3f(1,2,3),new Point3f(1,2,3.01))');

        // special Point3f Tests
        ENSURE('almostEqual(new Point2f(1,2),new Point2f(1,2))');
        ENSURE('!almostEqual(new Point2f(1,2),new Point2f(1,2.01))');

        ENSURE('almostEqual(sum(new Point2f(1,2),new Vector2f(4,3)),new Point2f(5,5))');
        ENSURE('almostEqual(difference(new Point2f(1,2),new Vector2f(4,3)),new Point2f(-3,-1))');
        ENSURE('almostEqual(difference(new Point2f(1,2),new Point2f(4,3)),new Vector2f(-3,-1))');
        ENSURE('almostEqual(product(new Point2f(1,2),new Vector2f(4,3)),new Point2f(4,6))');
        ENSURE('almostEqual(quotient(new Point2f(1,2),new Vector2f(1,2)),new Point2f(1,1))');
    };
};

function BoxUnitTest() {
    this.Constructor(this, "BoxUnitTest");
};

BoxUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function almostEqual(a, b) {
        return (Math.abs(a - b) < 0.00001);
    }

    obj.run = function() {
        testPoint2();
        testPoint3();
    }
    function testPoint2() {
        obj.p0 = new Point2f(0.55, 0.6);
        obj.p1 = new Point2f(1.1, 1.2);
        obj.p2 = new Point2f(2.3, 2.5);
        obj.p3 = new Point2f(-1, -1.1);
        obj.v1 = new Vector2f(2.2, 3.3);
        SUCCESS("initialized");
        // Constructor tests
        {
            obj.b1 = new Box2f();
            ENSURE('obj.b1.isEmpty');

            obj.b1 = new Box2f(obj.p0, obj.p1);
            ENSURE('almostEqual(obj.b1.min, obj.p0)');
            ENSURE('almostEqual(obj.b1.max, obj.p1)');
            obj.b1 = new Box2f(obj.p1, obj.p0);
            ENSURE('almostEqual(obj.b1.min, obj.p0)');
            ENSURE('almostEqual(obj.b1.max, obj.p1)');
        }

        /*
        // Test get/set min / max / center
        {
            obj.b1 = new Box2f(obj.p0, obj.p1);
            obj.myCenter = quotient(sum(obj.p0,obj.p1),new Vector2f(2,2));
            ENSURE('almostEqual(obj.b1.min, obj.p0)');
            ENSURE('almostEqual(obj.b1.max, obj.p1)');
            ENSURE('almostEqual(obj.b1.minVector, obj.p0)');
            ENSURE('almostEqual(obj.b1.maxVector, obj.p1)');
            ENSURE('almostEqual(obj.b1.center, obj.myCenter)');

            obj.b1.center = obj.p3;
            ENSURE('almostEqual(obj.b1.center,obj.p3)');
            obj.b1.sizeVector = obj.v1;
            ENSURE('almostEqual(obj.b1.sizeVector,obj.v1)');
        }
        */
        // test extendBy
        {
            obj.b1 = new Box2f();
            obj.b1.extendBy(obj.p0);
            ENSURE('almostEqual(obj.b1.min, obj.p0)');
            ENSURE('almostEqual(obj.b1.max,obj. p0)');
            obj.b1.extendBy(obj.p1);
            ENSURE('almostEqual(obj.b1.min, obj.p0)');
            ENSURE('almostEqual(obj.b1.max, obj.p1)');
            obj.b1.extendBy(obj.p2);
            obj.b1.extendBy(obj.p3);
            ENSURE('almostEqual(obj.b1.min, obj.p3)');
            ENSURE('almostEqual(obj.b1.max, obj.p2)');
            obj.b1.extendBy(obj.b1);
            obj.b1.makeEmpty();
            obj.b2 = new Box2f(obj.p1, obj.p2);

            obj.b1.extendBy(obj.b2);
            ENSURE('almostEqual(obj.b1.min, obj.b2.min)');
            ENSURE('almostEqual(obj.b1.max, obj.b2.max)');
            obj.b1.makeEmpty();
            obj.b1.extendBy(obj.p1);
            obj.b1.extendBy(obj.b1);
            ENSURE('obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('!obj.b1.hasSize');
            obj.b1.extendBy(obj.b2);
            ENSURE('!obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('obj.b1.hasSize');
        }
        /*

        // test contains
        {
            obj.b1 = new Box2f(obj.p0, obj.p2);
            ENSURE(obj.b1.contains(obj.p1));
            obj.b1.makeEmpty();
            ENSURE(!obj.b1.contains(obj.p1));
        }

        // Test isEmpty, hasPosition, hasSize, hasArea
        {
            obj.p1=new Point2f(1, 1);
            obj.p2=new Point2f(1, 2);
            obj.p3=new Point2f(2, 2);

            obj.b1.makeEmpty();
            ENSURE('obj.b1.isEmpty');
            ENSURE('!obj.b1.hasPosition');
            ENSURE('!obj.b1.hasSize');
            ENSURE('!obj.b1.hasArea');
            obj.b1.extendBy(obj.p1);
            ENSURE('obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('!obj.b1.hasSize');
            ENSURE('!obj.b1.hasArea');
            obj.b1.extendBy(obj.p1);
            ENSURE('obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('!obj.b1.hasSize');
            ENSURE('!obj.b1.hasArea');
            obj.b1.extendBy(obj.p2);
            ENSURE('obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('obj.b1.hasSize');
            ENSURE('!obj.b1.hasArea');
            obj.b1.extendBy(obj.p3);
            ENSURE('!obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('obj.b1.hasSize');
            ENSURE('obj.b1.hasArea');
        }
        */
    } // function testPoint2

    function testPoint3() {
        obj.p0=new Point3f(0.55, 0.6, 0.7);
        obj.p1=new Point3f(1.1, 1.2, 1.4);
        obj.p2=new Point3f(2.3, 2.5, 2.9);
        obj.p3=new Point3f(-1, -1.1, -1.2);
        obj.v1=new Vector3f(2.2, 3.3, 5.5);

        // Constructor tests
        {
            obj.b1=new Box3f(obj.p0, obj.p1);
            ENSURE('almostEqual(obj.b1.min, obj.p0)');
            ENSURE('almostEqual(obj.b1.max, obj.p1)');

            obj.b1=new Box3f(obj.b1.min, obj.b1.max);
            ENSURE('almostEqual(obj.b1.min, obj.b1.min)');
            ENSURE('almostEqual(obj.b1.max, obj.b1.max)');

            obj.b3=new Box3f(new Point3f(0, 1, 2), new Point3f(3, 4, 5));
            ENSURE('almostEqual(b3.min, new Point3f(0, 1, 2))');
            ENSURE('almostEqual(b3.max, new Point3f(3, 4, 5))');
        }

        // Test get/set min / max / center / size
        {
            obj.b1=new Box3f(obj.p0, obj.p1);
            obj.myCenter = quotient(sum(obj.p0,obj.p1),new Vector3f(2,2,2));
            ENSURE('almostEqual(obj.b1.min, obj.p0)');
            ENSURE('almostEqual(obj.b1.max, obj.p1)');
            ENSURE('almostEqual(obj.b1.center, obj.myCenter)');

            obj.b1.center = obj.p3;
            ENSURE('almostEqual(obj.b1.center,obj.p3)');
            obj.b1.sizeVector = obj.v1;
            ENSURE('almostEqual(obj.b1.sizeVector,obj.v1)');
        }

        // test extendBy
        {
            obj.b1.makeEmpty();
            obj.b1.extendBy(obj.p0);
            ENSURE('almostEqual(obj.b1.min, obj.p0)');
            ENSURE('almostEqual(obj.b1.max, obj.p0)');
            obj.b1.extendBy(obj.p1);
            ENSURE('almostEqual(obj.b1.min, obj.p0)');
            ENSURE('almostEqual(obj.b1.max, obj.p1)');
            obj.b1.extendBy(obj.p2);
            obj.b1.extendBy(obj.p3);
            ENSURE('almostEqual(obj.b1.min, obj.p3)');
            ENSURE('almostEqual(obj.b1.max, obj.p2)');
            obj.b1.extendBy(obj.b1);
            ENSURE('almostEqual(obj.b1.min, obj.b1.min)');
            ENSURE('almostEqual(obj.b1.max, obj.b1.max)');
            obj.b1.makeEmpty();
            obj.b2=new Box3f(obj.p1, obj.p2);
            obj.b1.extendBy(obj.b2);
            ENSURE('almostEqual(obj.b1.min, obj.b2.min)');
            ENSURE('almostEqual(obj.b1.max, obj.b2.max)');

            obj.b1.makeEmpty();
            obj.b1.extendBy(obj.p1);
            obj.b1.extendBy(obj.b1);
            ENSURE('obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('!obj.b1.hasSize');
            ENSURE('!obj.b1.hasArea');
            ENSURE('!obj.b1.hasVolume');
            obj.b1.extendBy(obj.b2);
            ENSURE('!obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('obj.b1.hasSize');
            ENSURE('obj.b1.hasArea');
            ENSURE('obj.b1.hasVolume');
        }

        // Test contains
        {
            obj.b1=new Box3f(obj.p0, obj.p2);
            ENSURE('obj.b1.contains(obj.p1)');
            obj.b1.makeEmpty();
            ENSURE('!obj.b1.contains(obj.p1)');
        }

        // Test matrix multiply
        {
            obj.b1=new Box3f(obj.p0, obj.p2);
            obj.myMatrix=new Matrix4f() ;
            obj.myMatrix.makeIdentity();
            obj.b2 = new Box3f(obj.b1.min, obj.b1.max);
            obj.b1 = product(obj.b1, obj.myMatrix);
            ENSURE('almostEqual(obj.b2.min, obj.b1.min)');
            ENSURE('almostEqual(obj.b2.max, obj.b1.max)');
            obj.myMatrix.makeScaling(new Vector3f(2,2,2));
            obj.b2 = new Box3f(obj.b1.min, obj.b1.max);
            obj.b2.min.mult(new Vector3f(2,2,2)); // TODO: calling a function on a writeable attribute does nothing, and no error occurs
            obj.b2.max.mult(new Vector3f(2,2,2));
            obj.b1 = product(obj.b1, obj.myMatrix);
            print(obj.b1);
            print(obj.b2.min);
            print(obj.b2.max);
            //ENSURE('almostEqual(obj.b2.min, obj.b1.min)');
            //ENSURE('almostEqual(obj.b2.max, obj.b1.max)');
        }

        // Test isEmpty, hasPosition, hasSize, hasArea, hasVolume
        {
            obj.p1=new Point3f(1, 1, 1);
            obj.p2=new Point3f(1, 1, 2);
            obj.p3=new Point3f(1, 2, 2);
            obj.p4=new Point3f(2, 2, 2);

            obj.b1.makeEmpty();
            ENSURE('obj.b1.isEmpty');
            ENSURE('!obj.b1.hasPosition');
            ENSURE('!obj.b1.hasSize');
            ENSURE('!obj.b1.hasArea');
            ENSURE('!obj.b1.hasVolume');
            obj.b1.extendBy(obj.p1);
            ENSURE('obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('!obj.b1.hasSize');
            ENSURE('!obj.b1.hasArea');
            ENSURE('!obj.b1.hasVolume');
            obj.b1.extendBy(obj.p1);
            ENSURE('obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('!obj.b1.hasSize');
            ENSURE('!obj.b1.hasArea');
            ENSURE('!obj.b1.hasVolume');
            obj.b1.extendBy(obj.p2);
            ENSURE('obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('obj.b1.hasSize');
            ENSURE('!obj.b1.hasArea');
            ENSURE('!obj.b1.hasVolume');
            obj.b1.extendBy(obj.p3);
            ENSURE('obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('obj.b1.hasSize');
            ENSURE('obj.b1.hasArea');
            ENSURE('!obj.b1.hasVolume');
            obj.b1.extendBy(obj.p4);
            ENSURE('!obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('obj.b1.hasSize');
            ENSURE('obj.b1.hasArea');
            ENSURE('obj.b1.hasVolume');
        }
    } //function
};



function EmptyObject() {
}

EmptyObject.prototype.nop = function() {
}


function testPerformance() {
    var n = 1000000;
    var a = 0;
    var myTimer = new Timer("testPerformance");
    var myBaseLine = myTimer.add("Baseline",n);
    for (var i = 0; i < n; ++i) {
        a = a + 1;
    }
    myTimer.stop(myBaseLine);

    n = n/10;

    var v = new Vector3f(1,1,1);
    var o = new Vector3f(1,1,1);

    var myAddTimer = myTimer.add("method add",n);
    for (i = 0; i < n; ++i) {
        v.add(o);
    }
    myTimer.stop(myAddTimer);

    var M = new Matrix4f(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
    var M2 = new Matrix4f(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

    var myMultTimer = myTimer.add("function matrix multiplication",n);
    for (i = 0; i < n; ++i) {
        M.postMultiply(M2);
    }
    myTimer.stop(myMultTimer);

    var myIdentTimer = myTimer.add("function matrix make identity",n);
    for (i = 0; i < n; ++i) {
        M.makeIdentity();
    }
    myTimer.stop(myIdentTimer);

    var myObject = new EmptyObject();

    var myNopTimer = myTimer.add("function nop",n);
    for (i = 0; i < n; ++i) {
        myObject.nop();
    }
    myTimer.stop(myNopTimer);

    n = n / 10;

    var myAddFuncTimer = myTimer.add("function add",n);
    v = new Vector3f(1,1,1);
    for (i = 0; i < n; ++i) {
        v = sum(v,o);
    }
    myTimer.stop(myAddFuncTimer);
    myTimer.print();
}

function main() {
    var myTestName = "testFixedVector.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new FixedVectorUnitTest());
    mySuite.addTest(new FixedPointUnitTest());
    mySuite.addTest(new Matrix4fUnitTest());
    mySuite.addTest(new BoxUnitTest());
    mySuite.run();

    print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    exit(1);
};

testPerformance();


