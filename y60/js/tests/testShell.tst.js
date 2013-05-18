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

use("UnitTest.js");

function VectorUnitTest() {
    this.Constructor(this, "VectorUnitTest");
};

VectorUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
    /*
        obj.myNoisyObj = new Noisy();
        print(obj.myNoisyObj);
        var myA = obj.myNoisyObj.a;
        var myB = obj.myNoisyObj.b;
        obj.myNoisyObj.beep();
        obj.myTwice = obj.myNoisyObj.twice(1);
        ENSURE('obj.myTwice == 2');
        obj.mySum = obj.myNoisyObj.add(1,2);
        print('mySum='+obj.mySum);
        ENSURE('obj.mySum == 3');
        obj.myNoisyObj.myC = 23;
        ENSURE('obj.myNoisyObj.myC == 23');
        obj.myNoisyObj.c = 42;
        for (n in obj.myNoisyObj) {
            print(n);
            print(obj.myNoisyObj[n]);
        }
    */

        obj.myVector2f = new Vector2f();
        //obj.myVector2f.beep();
        obj.myVector2f = new Vector2f(1,2);
        DPRINT('obj.myVector2f');
        ENSURE('obj.myVector2f.length == 2');
        ENSURE('obj.myVector2f[0] == 1');
        ENSURE('obj.myVector2f[1] == 2');
        obj.myOtherVector2f = new Vector2f(3,4);
        SUCCESS("myOtherVector2f passed");
        obj.myVector2f.add(obj.myOtherVector2f);
        SUCCESS("add passed");
        ENSURE('obj.myVector2f[0] == 4');
        ENSURE('obj.myVector2f[1] == 6');

        obj.myVector2f.add([2,4]);
        ENSURE('obj.myVector2f[0] == 6');
        ENSURE('obj.myVector2f[1] == 10');

        obj.myVector2f[0] = -1;
        ENSURE('obj.myVector2f[0]== -1');
        DPRINT('obj.myVector2f[0]');

        obj.myVector2f = new Vector2f([2,3]);
        ENSURE('obj.myVector2f[0] == 2');
        ENSURE('obj.myVector2f[1] == 3');


        // Vector3
        obj.myVector3f = new Vector3f();
        //obj.myVector3f.beep();
        obj.myVector3f = new Vector3f(1,2,3);
        DPRINT('obj.myVector3f');
        ENSURE('obj.myVector3f.length == 3');
        ENSURE('obj.myVector3f[0] == 1');
        ENSURE('obj.myVector3f[1] == 2');
        ENSURE('obj.myVector3f[2] == 3');
        obj.myOtherVector3f = new Vector3f(3,4,5);
        SUCCESS("myOtherVector3f passed");
        obj.myVector3f.add(obj.myOtherVector3f);
        SUCCESS("add passed");
        ENSURE('obj.myVector3f[0] == 4');
        ENSURE('obj.myVector3f[1] == 6');
        ENSURE('obj.myVector3f[2] == 8');

        obj.myVector3f.add([2,4,6]);
        ENSURE('obj.myVector3f[0] == 6');
        ENSURE('obj.myVector3f[1] == 10');
        ENSURE('obj.myVector3f[2] == 14');

        obj.myVector3f[0] = -1;
        ENSURE('obj.myVector3f[0]== -1');
        DPRINT('obj.myVector3f[0]');

        ENSURE('!almostEqual("1","2")');
        ENSURE('almostEqual("1","1")');
        ENSURE('!almostEqual(1,2)');
        ENSURE('almostEqual(1,1)');
        //ENSURE_EXCEPTION('!almostEqual("bla","blub")');

        ENSURE('almostEqual(new Vector2f(2,3),new Vector2f(2,3))');
        ENSURE('!almostEqual(new Vector2f(2,3),new Vector2f(1,2))');

        ENSURE('almostEqual(new Vector3f(2,3,5),new Vector3f(2,3,5))');
        ENSURE('!almostEqual(new Vector3f(2,3,5),new Vector3f(2,3,4))');

        ENSURE('almostEqual(new Vector4f(2,3,4,5),new Vector4f(2,3,4,5))');
        ENSURE('!almostEqual(new Vector4f(2,3,4,5),new Vector4f(1,2,3,4))');

        ENSURE('almostEqual(new Vector2f(2,3),[2,3])');
        ENSURE('!almostEqual(new Vector2f(2,3),[2,4])');

        ENSURE('almostEqual(new Vector3f(2,3,5),[2,3,5])');
        ENSURE('!almostEqual(new Vector3f(2,3,5),[2,4,5])');

        ENSURE('almostEqual(new Vector4f(2,3,5,6),[2,3,5,6])');
        ENSURE('!almostEqual(new Vector4f(2,3,5,6),[2,4,5,6])');

        ENSURE('almostEqual([2,3,5,6],[2,3,5,6])');
        ENSURE('!almostEqual([2,3,5,7],[2,3,5,6])');

        // swizzle read tests
        obj.myVector = new Vector4f(0,1,2,3);
        ENSURE('almostEqual(obj.myVector,[0,1,2,3])');
        ENSURE('almostEqual(obj.myVector.xyzw,[0,1,2,3])');
        ENSURE('almostEqual(obj.myVector.wzyx,[3,2,1,0])');
        ENSURE('almostEqual(obj.myVector.x,0)');
        ENSURE('almostEqual(obj.myVector.y,1)');
        ENSURE('almostEqual(obj.myVector.z,2)');
        ENSURE('almostEqual(obj.myVector.w,3)');
        ENSURE('almostEqual(obj.myVector.xyz,[0,1,2])');
        ENSURE('almostEqual(obj.myVector.xy,[0,1])');
        ENSURE('almostEqual(obj.myVector.xyxy,[0,1,0,1])');
        ENSURE('almostEqual(obj.myVector.wwww,[3,3,3,3])');
        ENSURE('almostEqual(obj.myVector.wwww,new Vector4f(3,3,3,3))');
        ENSURE('almostEqual(obj.myVector.xw,new Vector2f(0,3))');
        ENSURE('almostEqual(obj.myVector.xy01,new Vector4f(0,1,0,1))');
        ENSURE('almostEqual(obj.myVector.xyz1,new Vector4f(0,1,2,1))');
        ENSURE('almostEqual(obj.myVector.xyz0,new Vector4f(0,1,2,0))');
        ENSURE('almostEqual(obj.myVector.xy00,new Vector4f(0,1,0,0))');

        // value magic
        obj.myVector = new Vector4f(0,1,2,3);
        ENSURE('almostEqual(obj.myVector,[0,1,2,3])');
        ENSURE('almostEqual(obj.myVector.value,[0,1,2,3])');
        obj.ref = obj.myVector;
        ENSURE('almostEqual(obj.ref,[0,1,2,3])');
        ENSURE('almostEqual(obj.ref.value,[0,1,2,3])');
        obj.ref.value = new Vector4f(2,3,4,5);
        ENSURE('almostEqual(obj.ref,[2,3,4,5])');
        ENSURE('almostEqual(obj.ref.value,[2,3,4,5])');
        ENSURE('almostEqual(obj.myVector,[2,3,4,5])');
        // TODO: array assigment
        //obj.ref.value = [1,3,4,5];
        //ENSURE('almostEqual(obj.myVector,[1,3,4,5])');

        // dot product
        ENSURE('almostEqual(dot(new Vector2f(1,2),new Vector2f(5,6)), 1*5+2*6)');
        ENSURE('almostEqual(dot(new Vector3f(1,2,3),new Vector3f(5,6,7)), 1*5+2*6+3*7)');
        ENSURE('almostEqual(dot(new Vector4f(1,2,3,4),new Vector4f(5,6,7,8)), 1*5+2*6+3*7+4*8)');

        // swizzle write tests
        obj.myVector = new Vector4f(2,0,0,0);

        SUCCESS('Constructed');
        obj.myVector.x = 1;
        ENSURE('almostEqual(obj.myVector,new Vector4f(1,0,0,0))');
        obj.myVector.xy = [2,3];
        ENSURE('almostEqual(obj.myVector,new Vector4f(2,3,0,0))');

        obj.myVector.w = 4;
        ENSURE('almostEqual(obj.myVector,new Vector4f(2,3,0,4))');
        obj.myVector.xyz = [0,1,2];
        ENSURE('almostEqual(obj.myVector,new Vector4f(0,1,2,4))');
        obj.myVector.yzw = [4,5,6];
        ENSURE('almostEqual(obj.myVector,new Vector4f(0,4,5,6))');

        ENSURE('almostEqual(sum(new Vector3f(1,2,3),new Vector3f(4,3,2)),new Vector3f(5,5,5))');
        ENSURE('almostEqual(difference(new Vector3f(1,2,3),new Vector3f(4,3,2)),new Vector3f(-3,-1,1))');
        ENSURE('almostEqual(product(new Vector3f(1,2,3),new Vector3f(4,3,2)),new Vector3f(4,6,6))');
        ENSURE('almostEqual(product(new Vector3f(1,2,3),2),new Vector3f(2,4,6))');
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

        obj.myResult4 = normal(new Vector3f(0,0,10), new Vector3f(0.3,0,0));
        ENSURE('almostEqual(myResult4 , new Vector3f(0,1,0))', "normal of scaled x- and z- axis.");

        obj.myResult5 = normal(new Vector3f(0,0,-3), new Vector3f(-1100,0,0));
        ENSURE('almostEqual(myResult5 , new Vector3f(0,1,0))', "normal of  scaled -x- and -z- axis.");

        obj.myResult6 = normal(new Vector3f(0,0,10), new Vector3f(100,0,1));
        ENSURE('almostEqual(myResult6 , new Vector3f(0,1,0))', "normal of (100,0,1) and scaled z- axis.");

        obj.myResult7 = normal(new Vector3f(1,0,-1), new Vector3f(0,1,1));
        ENSURE('almostEqual(myResult7, new Vector3f(0.57735,-0.57735,0.57735))', "normal of (1,0,-1) and (0,1,1).");

        obj.myVector = new Vector3d(2,3,4);
        obj.myVector.add([1,2,3]);
        ENSURE('almostEqual(myVector,[3,5,7])');
        obj.myVector.sub([1,2,3]);
        ENSURE('almostEqual(myVector,[2,3,4])');
        obj.myVector.mult([2,2,2]);
        ENSURE('almostEqual(myVector,[4,6,8])');
        obj.myVector.div([2,2,2]);
        ENSURE('almostEqual(myVector,[2,3,4])');

        obj.myVector = new Vector3d(2,3,4);
        obj.myVector = [1,2,3];
        ENSURE('almostEqual(myVector,[1,2,3])');
    }
};

function PointUnitTest() {
    this.Constructor(this, "PointUnitTest");
};

PointUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {

        obj.myPoint2f = new Point2f();
        ENSURE('obj.myPoint2f.length == 2');
        obj.myPoint2f[0] = 0;
        ENSURE('obj.myPoint2f[0] == 0');
        obj.myPoint2f[1] = 1;
        ENSURE('obj.myPoint2f[1] == 1');

        obj.myPoint2f = new Point2f(1,2);
        DPRINT('obj.myPoint2f');
        ENSURE('obj.myPoint2f.length == 2');
        ENSURE('obj.myPoint2f[0] == 1');
        ENSURE('obj.myPoint2f[1] == 2');
        obj.myOtherPoint2f = new Point2f(3,4);
        SUCCESS("myOtherPoint2f passed");
        obj.myPoint2f.add(obj.myOtherPoint2f);
        SUCCESS("add passed");
        ENSURE('obj.myPoint2f[0] == 4');
        ENSURE('obj.myPoint2f[1] == 6');

        obj.myPoint2f.add([2,4]);
        ENSURE('obj.myPoint2f[0] == 6');
        ENSURE('obj.myPoint2f[1] == 10');

        obj.myPoint2f[0] = -1;
        ENSURE('obj.myPoint2f[0]== -1');
        DPRINT('obj.myPoint2f[0]');

        obj.myPoint2f = new Point2f([2,3]);
        ENSURE('obj.myPoint2f[0] == 2');
        ENSURE('obj.myPoint2f[1] == 3');


        // Point3
        obj.myPoint3f = new Point3f();
        //obj.myPoint3f.beep();
        obj.myPoint3f = new Point3f(1,2,3);
        DPRINT('obj.myPoint3f');
        ENSURE('obj.myPoint3f.length == 3');
        ENSURE('obj.myPoint3f[0] == 1');
        ENSURE('obj.myPoint3f[1] == 2');
        ENSURE('obj.myPoint3f[2] == 3');
        obj.myOtherPoint3f = new Point3f(3,4,5);
        SUCCESS("myOtherPoint3f passed");
        obj.myPoint3f.add(obj.myOtherPoint3f);
        SUCCESS("add passed");
        ENSURE('obj.myPoint3f[0] == 4');
        ENSURE('obj.myPoint3f[1] == 6');
        ENSURE('obj.myPoint3f[2] == 8');

        obj.myPoint3f.add([2,4,6]);
        ENSURE('obj.myPoint3f[0] == 6');
        ENSURE('obj.myPoint3f[1] == 10');
        ENSURE('obj.myPoint3f[2] == 14');

        obj.myPoint3f[0] = -1;
        ENSURE('obj.myPoint3f[0]== -1');
        DPRINT('obj.myPoint3f[0]');

        ENSURE('!almostEqual("1","2")');
        ENSURE('almostEqual("1","1")');
        ENSURE('!almostEqual(1,2)');
        ENSURE('almostEqual(1,1)');
        //ENSURE_EXCEPTION('!almostEqual("bla","blub")');

        ENSURE('almostEqual(new Point2f(2,3),new Point2f(2,3))');
        ENSURE('!almostEqual(new Point2f(2,3),new Point2f(1,2))');

        ENSURE('almostEqual(new Point3f(2,3,5),new Point3f(2,3,5))');
        ENSURE('!almostEqual(new Point3f(2,3,5),new Point3f(2,3,4))');

        ENSURE('almostEqual(new Point4f(2,3,4,5),new Point4f(2,3,4,5))');
        ENSURE('!almostEqual(new Point4f(2,3,4,5),new Point4f(1,2,3,4))');

        ENSURE('almostEqual(new Point2f(2,3),[2,3])');
        ENSURE('!almostEqual(new Point2f(2,3),[2,4])');

        ENSURE('almostEqual(new Point3f(2,3,5),[2,3,5])');
        ENSURE('!almostEqual(new Point3f(2,3,5),[2,4,5])');

        ENSURE('almostEqual(new Point4f(2,3,5,6),[2,3,5,6])');
        ENSURE('!almostEqual(new Point4f(2,3,5,6),[2,4,5,6])');

        ENSURE('almostEqual([2,3,5,6],[2,3,5,6])');
        ENSURE('!almostEqual([2,3,5,7],[2,3,5,6])');

        // swizzle read tests
        obj.myPoint = new Point4f(0,1,2,3);
        ENSURE('almostEqual(obj.myPoint,[0,1,2,3])');
        ENSURE('almostEqual(obj.myPoint.xyzw,[0,1,2,3])');
        ENSURE('almostEqual(obj.myPoint.wzyx,[3,2,1,0])');
        ENSURE('almostEqual(obj.myPoint.x,0)');
        ENSURE('almostEqual(obj.myPoint.y,1)');
        ENSURE('almostEqual(obj.myPoint.z,2)');
        ENSURE('almostEqual(obj.myPoint.w,3)');
        ENSURE('almostEqual(obj.myPoint.xyz,[0,1,2])');
        ENSURE('almostEqual(obj.myPoint.xy,[0,1])');
        ENSURE('almostEqual(obj.myPoint.xyxy,[0,1,0,1])');
        ENSURE('almostEqual(obj.myPoint.wwww,[3,3,3,3])');
        ENSURE('almostEqual(obj.myPoint.wwww,new Point4f(3,3,3,3))');
        ENSURE('almostEqual(obj.myPoint.xw,new Point2f(0,3))');
        ENSURE('almostEqual(obj.myPoint.xy01,new Point4f(0,1,0,1))');
        ENSURE('almostEqual(obj.myPoint.xyz1,new Point4f(0,1,2,1))');
        ENSURE('almostEqual(obj.myPoint.xyz0,new Point4f(0,1,2,0))');
        ENSURE('almostEqual(obj.myPoint.xy00,new Point4f(0,1,0,0))');

        obj.myPoint = new Point3d(2,3,4);
        obj.myPoint.add([1,2,3]);
        ENSURE('almostEqual(myPoint,[3,5,7])');
        obj.myPoint.sub([1,2,3]);
        ENSURE('almostEqual(myPoint,[2,3,4])');
        obj.myPoint.mult([2,2,2]);
        ENSURE('almostEqual(myPoint,[4,6,8])');
        obj.myPoint.div([2,2,2]);
        ENSURE('almostEqual(myPoint,[2,3,4])');
    }
}

function Matrix4fUnitTest() {
    this.Constructor(this, "Matrix4fUnitTest");
};

Matrix4fUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {

            // assign
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
            ENSURE('obj.myMatrix.type == Matrix4f.UNKNOWN', "Testing type");

            obj.myMatrix2 = new Matrix4f();
            obj.myMatrix2 = obj.myMatrix;
            ENSURE('almostEqual(obj.myMatrix,obj.myMatrix2)');

            // makeIdentity
            DTITLE("Testing makeIdentity");
            obj.myMatrix= new Matrix4f(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
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
            obj.myScaling = new Matrix4f();
            obj.myScaling= new Matrix4f(-1,0,0,0,0,2,0,0,0,0,3,0,0,0,0,1);
            ENSURE('almostEqual(obj.myMatrix, myScaling)', "Test makeScaling()");

            DTITLE("Testing makeTranslating");
            obj.myMatrix= new Matrix4f(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
            obj.myMatrix.makeTranslating(new Vector3f(3, 2, -1));
            DPRINT('obj.myMatrix.type');
            ENSURE('obj.myMatrix.type != Matrix4f.IDENTITY', "Test makeScaling()");
            ENSURE('obj.myMatrix.type == Matrix4f.TRANSLATING', "Test makeTranslating()");
            obj.myTranslating = new Matrix4f();
            obj.myTranslating= new Matrix4f(1,0,0,0,0,1,0,0,0,0,1,0,3,2,-1,1);
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
                myMatrix.makeXRotating(1);
                ENSURE('myMatrix.type == Matrix4f.X_ROTATING');
                myMatrix.makeXRotating(2.0 * PI);
                ENSURE('almostEqual(myMatrix, myExpectedResult)');
                myMatrix.makeXRotating(PI/4);
                myExpectedResult.rotateX(PI/4);
                ENSURE('almostEqual(myMatrix, myExpectedResult)');
                myExpectedResult.makeIdentity();

                myMatrix.makeYRotating(0);
                ENSURE('almostEqual(myMatrix, myExpectedResult)', "Testing makeYRotating");
                myMatrix.makeYRotating(1);
                ENSURE('myMatrix.type == Matrix4f.Y_ROTATING');
                myMatrix.makeYRotating(2.0 * PI);
                ENSURE('almostEqual(myMatrix, myExpectedResult)');
                myMatrix.makeYRotating(PI/4);
                myExpectedResult.rotateY(PI/4);
                ENSURE('almostEqual(myMatrix, myExpectedResult)');
                myExpectedResult.makeIdentity();

                myMatrix.makeZRotating(0);
                ENSURE('almostEqual(myMatrix, myExpectedResult)', "Testing makeZRotating");
                myMatrix.makeZRotating(1);
                ENSURE('myMatrix.type == Matrix4f.Z_ROTATING');
                myMatrix.makeZRotating(2.0 * PI);
                ENSURE('almostEqual(myMatrix, myExpectedResult)');
                myMatrix.makeZRotating(PI/4);
                myExpectedResult.rotateZ(PI/4);
                ENSURE('almostEqual(myMatrix, myExpectedResult)');
                myExpectedResult.makeIdentity();

                myMatrix.makeRotating(new Vector3f(1, 0, 0), 0.123);
                ENSURE('myMatrix.type == Matrix4f.ROTATING');
                myExpectedResult.makeXRotating(0.123);
                ENSURE('almostEqual(myMatrix, myExpectedResult)', "Testing makeRotating");
                myMatrix.makeRotating(new Vector3f(1.0, 2.0, 3.0), 2 * PI);
                myExpectedResult.makeRotating(new Vector3f(1.0, 2.0, 3.0), 0);
                ENSURE('almostEqual(myMatrix, myExpectedResult)');

                // Test Vector * Matrix
                myMatrix= new Matrix4f(0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15);
                obj.myVector = new Vector4f(0,1,2,3);
                ENSURE('almostEqual(product(obj.myVector,obj.myMatrix), new Vector4f(56, 62, 68, 74))',
                        "Testing vector * matrix");

                myMatrix= new Matrix4f(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
                myVector = new Vector4f(-1,1,-1,1);
                ENSURE('almostEqual(product(myVector,myMatrix), new Vector4f(0,0,0,0))',
                        "Testing vector * matrix");

                // Point * Matrix
                obj.myPoint = new Point3f(0,1,2);
                myMatrix= new Matrix4f(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
                obj.myResult = product(new Vector4f(0, 1, 2, 1), obj.myMatrix);
                var f = 1.0 / dot(new Vector4f(0,1,2,1), myMatrix.getColumn(3));
                obj.myResult.mult(new Vector4f(f,f,f,f));
                obj.myProduct = product(myPoint,myMatrix);
                DPRINT('obj.myResult.toString()');
                DPRINT('obj.myProduct.toString()');
                ENSURE('almostEqual(obj.myProduct, new Point3f(myResult[0],myResult[1],myResult[2]))', "Testing point * matrix");
                myMatrix= new Matrix4f(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
                myPoint = new Point3f(-1,1,-1);
                myResult = product(new Vector4f(-1, 1, -1, 1), myMatrix);
                ENSURE(almostEqual(product(myPoint,myMatrix),
                            new Point3f(myResult[0],myResult[1],myResult[2])), "Testing point * matrix");

                // getTranslation
                myMatrix= new Matrix4f(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
                ENSURE(almostEqual(myMatrix.getTranslation(), new Vector3f(12, 13, 14)), "Testing getTranslation()");
            }

            {
                // Test setRow / setColumn
                var myRowMatrix = new Matrix4f();
                var myColMatrix = new Matrix4f();
                for (i = 0; i < 4; ++i) {
                    var myVector = new Vector4f(i*1, i*2, i*3, i*4);
                    myRowMatrix.setRow(i, myVector);
                    ENSURE(almostEqual(myVector, myRowMatrix.getRow(i)), "Testing getRow()");
                    myColMatrix.setColumn(i, myVector);
                    ENSURE(almostEqual(myVector, myColMatrix.getColumn(i)), "Testing getColumn()");
                }
            }
    }
};


function RendererUnitTest() {
    this.Constructor(this, "NodeUnitTest");
};

RendererUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myRenderer = new Renderer();
    }
}

function TriangleUnitTest() {
    this.Constructor(this, "TriangleUnitTest");
};

TriangleUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myP0 = new Point3f(0,0,0);
        obj.myP1 = new Point3f(1,0,0);
        obj.myP2 = new Point3f(0,1,0);

        obj.myTriangle0 = new Triangle();
        obj.myTriangle = new Triangle(obj.myP0,obj.myP1,obj.myP2);
        DPRINT('obj.myTriangle');
        ENSURE('obj.myTriangle.length == 3');
        ENSURE('almostEqual(obj.myTriangle[0], obj.myP0)');
        ENSURE('almostEqual(obj.myTriangle[1], obj.myP1)');
        ENSURE('almostEqual(obj.myTriangle[2], obj.myP2)');

        DPRINT('obj.myTriangle.normal');
        ENSURE('almostEqual(obj.myTriangle.normal, new Vector3f(0,0,1))');
        DPRINT('obj.myTriangle.plane');
        ENSURE('almostEqual(obj.myTriangle.plane.normal, new Vector3f(0,0,1))');
        ENSURE('almostEqual(obj.myTriangle.plane.offset, 0)');


        obj.myTriangle1 = new Triangle(obj.myTriangle);
        ENSURE('almostEqual(obj.myTriangle[0], obj.myTriangle1[0])');
        ENSURE('almostEqual(obj.myTriangle[1], obj.myTriangle1[1])');
        ENSURE('almostEqual(obj.myTriangle[2], obj.myTriangle1[2])');
    }
}

function PlaneUnitTest() {
    this.Constructor(this, "PlaneUnitTest");
};

PlaneUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myNormal = new Vector3f(0,0,1);

        obj.myPlane0 = new Planef();
        obj.myPlane = new Planef(obj.myNormal, 0);
        DPRINT('obj.myPlane');
        ENSURE('almostEqual(obj.myPlane.normal, obj.myNormal)');
        ENSURE('almostEqual(obj.myPlane.offset, 0)');

        ENSURE('almostEqual(obj.myPlane.normal, new Vector3f(0,0,1))');
        obj.myPlane1 = new Planef(obj.myNormal, new Point3f(0,0,2));
        DPRINT('obj.myPlane1');
        ENSURE('almostEqual(obj.myPlane1.normal, obj.myNormal)');
        ENSURE('almostEqual(obj.myPlane1.offset, -2)');

        obj.myPlane2 = new Planef(obj.myPlane1);
        ENSURE('almostEqual(obj.myPlane1.offset, obj.myPlane2.offset)');
        ENSURE('almostEqual(obj.myPlane1.normal, obj.myPlane2.normal)');
    }
}
function LineUnitTest() {
    this.Constructor(this, "LineUnitTest");
};

LineUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myOrigin = new Point3f(1,1,1);
        obj.myOtherPoint = new Point3f(1,1,2);
        obj.myDirection = new Vector3f(0,0,1);

        obj.myLine0 = new Line();
        obj.myLine = new Line(obj.myOrigin, obj.myDirection);
        DPRINT('obj.myLine');
        ENSURE('almostEqual(obj.myLine.direction, obj.myDirection)');
        ENSURE('almostEqual(obj.myLine.origin, obj.myOrigin)');

        obj.myLine1 = new Line(obj.myOrigin, obj.myOtherPoint);
        DPRINT('obj.myLine1');
        ENSURE('almostEqual(obj.myLine1.origin, obj.myOrigin)');
        ENSURE('almostEqual(obj.myLine1.direction, obj.myDirection)');

        obj.myLine2 = new Line(obj.myLine1);
        ENSURE('almostEqual(obj.myLine1.origin, obj.myLine2.origin)');
        ENSURE('almostEqual(obj.myLine1.direction, obj.myLine2.direction)');

        obj.myLine3 = new Line([1,1,1],[0,0,1]);
        ENSURE('almostEqual(obj.myLine3.origin, [1,1,1])');
        ENSURE('almostEqual(obj.myLine3.direction, [0,0,1])');

        obj.myLine3.origin = [1,2,3];
        ENSURE('almostEqual(obj.myLine3.origin, [1,2,3])');
        obj.myLine3.direction = [3,2,1];
        ENSURE('almostEqual(obj.myLine3.direction, [3,2,1])');
    }
}

function RayUnitTest() {
    this.Constructor(this, "RayUnitTest");
};

RayUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myOrigin = new Point3f(1,1,1);
        obj.myOtherPoint = new Point3f(1,1,2);
        obj.myDirection = new Vector3f(0,0,1);

        obj.myRay0 = new Ray();
        obj.myRay = new Ray(obj.myOrigin, obj.myDirection);
        DPRINT('obj.myRay');
        ENSURE('almostEqual(obj.myRay.direction, obj.myDirection)');
        ENSURE('almostEqual(obj.myRay.origin, obj.myOrigin)');

        obj.myRay1 = new Ray(obj.myOrigin, obj.myOtherPoint);
        DPRINT('obj.myRay1');
        ENSURE('almostEqual(obj.myRay1.origin, obj.myOrigin)');
        ENSURE('almostEqual(obj.myRay1.direction, obj.myDirection)');

        obj.myRay2 = new Ray(obj.myRay1);
        ENSURE('almostEqual(obj.myRay1.origin, obj.myRay2.origin)');
        ENSURE('almostEqual(obj.myRay1.direction, obj.myRay2.direction)');

        obj.myRay3 = new Ray([1,1,1],[0,0,1]);
        ENSURE('almostEqual(obj.myRay3.origin, [1,1,1])');
        ENSURE('almostEqual(obj.myRay3.direction, [0,0,1])');

        obj.myRay3.origin = [1,2,3];
        ENSURE('almostEqual(obj.myRay3.origin, [1,2,3])');
        obj.myRay3.direction = [3,2,1];
        ENSURE('almostEqual(obj.myRay3.direction, [3,2,1])');

    }
}


function LineSegmentUnitTest() {
    this.Constructor(this, "LineSegmentUnitTest");
};

LineSegmentUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myOrigin = new Point3f(1,1,1);
        obj.myOtherPoint = new Point3f(1,1,2);
        obj.myEnd = new Point3f(0,0,1);

        obj.myLineSegment0 = new LineSegment();
        obj.myLineSegment = new LineSegment(obj.myOrigin, obj.myEnd);
        DPRINT('obj.myLineSegment');
        ENSURE('almostEqual(obj.myLineSegment.end, obj.myEnd)');
        ENSURE('almostEqual(obj.myLineSegment.origin, obj.myOrigin)');

        obj.myLineSegment1 = new LineSegment(obj.myOrigin, obj.myOtherPoint);
        DPRINT('obj.myLineSegment1');
        ENSURE('almostEqual(obj.myLineSegment1.origin, obj.myOrigin)');
        ENSURE('almostEqual(obj.myLineSegment1.end, obj.myOtherPoint)');

        obj.myLineSegment2 = new LineSegment(obj.myLineSegment1);
        ENSURE('almostEqual(obj.myLineSegment1.origin, obj.myLineSegment2.origin)');
        ENSURE('almostEqual(obj.myLineSegment1.end, obj.myLineSegment2.end)');

        obj.myLineSegment3 = new LineSegment([1,1,1],[0,0,1]);
        ENSURE('almostEqual(obj.myLineSegment3.origin, [1,1,1])');
        ENSURE('almostEqual(obj.myLineSegment3.end, [0,0,1])');

        obj.myLineSegment3.origin = [1,2,3];
        ENSURE('almostEqual(obj.myLineSegment3.origin, [1,2,3])');
        obj.myLineSegment3.end = [3,2,1];
        ENSURE('almostEqual(obj.myLineSegment3.end, [3,2,1])');

    }
}

function IntersectionUnitTest() {
    this.Constructor(this, "IntersectionUnitTest");
};

IntersectionUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
          // test line <-> plane intersection
        obj.myLine = new Line(new Point3f(0,0,0), new Vector3f(1,0,0));
        obj.myPlane2 = new Planef(new Vector3f(0,1,0), 1.0);

        obj.myIntersection = intersection(obj.myLine, obj.myPlane2);
        ENSURE('obj.myIntersection == undefined', "Line parallel to plane should not intersect.");

        obj.myLine2 = new Line(new Point3f(0,0,0), new Vector3f(0,1,0) );
        DPRINT('obj.myLine2');
        obj.myPlane7 = new Planef(new Vector3f(0,1,0), -1.0);
        DPRINT('obj.myPlane7');
        obj.myIntersection = intersection(obj.myLine2, obj.myPlane7);
        ENSURE('obj.myIntersection != undefined', "Line <-> Plane should intersect.");
        DPRINT ('obj.myIntersection');
        ENSURE('almostEqual(obj.myIntersection, new Point3f(0, 1, 0))');

        obj.myLine3 = new Line(new Point3f(1,0,0), new Vector3f(0,1,0) );
        obj.myPlane8 = new Planef(new Vector3f(0,1,0), 0.0);
        obj.myIntersection = intersection(obj.myLine3, obj.myPlane8);
        ENSURE('obj.myIntersection != undefined', "Line <-> Plane should intersect.");
        DPRINT ('obj.myIntersection');
        ENSURE('almostEqual(obj.myIntersection, new Point3f(1, 0, 0))');

        obj.myIntersection2 = intersection(obj.myPlane8, obj.myLine3);
        ENSURE('almostEqual(obj.myIntersection2, obj.myIntersection)', "commutative test")


        // Test sphere <-> line intersection
        var mySphere = new Sphere([1,1,1], 10);
        var mySphereLine   = new Line(new Point3f(1,0,1), new Vector3f(0,1,0));
        obj.myIntersection = intersection(mySphere, mySphereLine);
        ENSURE('almostEqual(obj.myIntersection[0], new Point3f(1, 11, 1))', "1st intersection line sphere");
        ENSURE('almostEqual(obj.myIntersection[1], new Point3f(1, -9, 1))', "2nd intersection line sphere");

        mySphereLine = new Line(new Point3f(1,20,1), new Vector3f(1,0,0));
        obj.myIntersection2 = intersection(mySphere, mySphereLine);
        ENSURE('!obj.myIntersection2', "NonIntersection Line <-> Sphere");

        // test ray <-> plane intersection
        obj.myRay = new Ray(new Point3f(0,0,0), new Vector3f(1,0,0));
        obj.myPlane2 = new Planef(new Vector3d(0,1,0), 1.0);

        obj.myIntersection = intersection(obj.myRay, obj.myPlane2);
        ENSURE('obj.myIntersection == undefined', "Ray parallel to plane should not intersect.");

        obj.myRay2 = new Ray(new Point3f(0,0,0), new Vector3f(0,1,0) );
        DPRINT('obj.myRay2');
        obj.myPlane7 = new Planef(new Vector3f(0,1,0), -1.0);
        DPRINT('obj.myPlane7');
        obj.myIntersection = intersection(obj.myRay2, obj.myPlane7);
        ENSURE('obj.myIntersection != undefined', "Ray <-> Plane should intersect.");
        DPRINT ('obj.myIntersection');
        ENSURE('almostEqual(obj.myIntersection, new Point3f(0, 1, 0))');

        obj.myRay3 = new Ray(new Point3f(1,0,0), new Vector3f(0,1,0) );
        obj.myPlane8 = new Planef(new Vector3f(0,1,0), 0.0);
        obj.myIntersection = intersection(obj.myRay3, obj.myPlane8);
        ENSURE('obj.myIntersection != undefined', "Ray <-> Plane should intersect.");
        DPRINT ('obj.myIntersection');
        ENSURE('almostEqual(obj.myIntersection, new Point3f(1, 0, 0))');

        obj.myIntersection2 = intersection(obj.myPlane8, obj.myRay3);
        ENSURE('almostEqual(obj.myIntersection2, obj.myIntersection)', "commutative test")

        // test LineSegment <-> plane intersection
        obj.myLineSegment = new LineSegment(new Point3f(0,0,0), new Vector3f(1,0,0));
        obj.myPlane2 = new Planef(new Vector3d(0,1,0), 1.0);

        obj.myIntersection = intersection(obj.myLineSegment, obj.myPlane2);
        ENSURE('obj.myIntersection == undefined', "LineSegment parallel to plane should not intersect.");

        obj.myLineSegment2 = new LineSegment(new Point3f(0,0,0), new Vector3f(0,1,0) );
        DPRINT('obj.myLineSegment2');
        obj.myPlane7 = new Planef(new Vector3f(0,1,0), -1.0);
        DPRINT('obj.myPlane7');
        obj.myIntersection = intersection(obj.myLineSegment2, obj.myPlane7);
        ENSURE('obj.myIntersection != undefined', "LineSegment <-> Plane should intersect.");
        DPRINT ('obj.myIntersection');
        ENSURE('almostEqual(obj.myIntersection, new Point3f(0, 1, 0))');

        obj.myLineSegment3 = new LineSegment(new Point3f(1,0,0), new Vector3f(0,1,0) );
        obj.myPlane8 = new Planef(new Vector3f(0,1,0), 0.0);
        obj.myIntersection = intersection(obj.myLineSegment3, obj.myPlane8);
        ENSURE('obj.myIntersection != undefined', "LineSegment <-> Plane should intersect.");
        DPRINT ('obj.myIntersection');
        ENSURE('almostEqual(obj.myIntersection, new Point3f(1, 0, 0))');

        obj.myIntersection2 = intersection(obj.myPlane8, obj.myLineSegment3);
        ENSURE('almostEqual(obj.myIntersection2, obj.myIntersection)', "commutative test")

       // test line <-> triangle intersection
        obj.myP0 = new Point3f(-1,1,0);
        obj.myP1 = new Point3f(1,1,1);
        obj.myP2 = new Point3f(1,1,-1);
        obj.myTriangle = new Triangle(obj.myP0,obj.myP1,obj.myP2);
        DPRINT('obj.myTriangle.plane.normal');
        DPRINT('obj.myTriangle.plane.offset');
        obj.myLine = new Line(new Point3f(0.5,0,0), new Vector3f(0,1,0) );
        DPRINT('obj.myLine');

        obj.myIntersection = intersection(obj.myTriangle, obj.myLine);
        DPRINT('obj.myIntersection');
        ENSURE('obj.myIntersection != undefined', "Line <-> Triangle should intersect.");
        DPRINT ('obj.myIntersection');
        ENSURE('almostEqual(obj.myIntersection, new Point3f(0.5, 1, 0))');

        obj.myLine = new Line(new Point3f(1.1,0,0), new Vector3f(0,1,0) );
        obj.myIntersection = intersection(obj.myTriangle, obj.myLine);
        ENSURE('obj.myIntersection == undefined');
    }
}

function BoxUnitTest() {
    this.Constructor(this, "BoxUnitTest");
};

BoxUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
      //  testPoint2();
        testPoint3();
    }
    /*
    function testPoint2() {
        ENSURE('linalg != undefined');
        obj.ensureWith(linalg);
        with (linalg) {

            obj.p0 = Point2f(0.55, 0.6);
            obj.p1 = Point2f(1.1, 1.2);
            obj.p2 = Point2f(2.3, 2.5);
            obj.p3 = Point2f(-1, -1.1);
            obj.v1 = Vector2f(2.2, 3.3);
            SUCCESS("initialized");
        // Constructor tests
            {
                obj.b1 = Box2f(obj.p0, obj.p1);
                ENSURE('almostEqualPoint(obj.b1.min, obj.p0)');
                ENSURE('almostEqualPoint(obj.b1.max, obj.p1)');
            }
            // Test get/set min / max / center
            {
                obj.b1 = Box2f(obj.p0, obj.p1);
                obj.myCenter = asFloatPoint(divfv(addfv(asFloatVector(obj.p0),asFloatVector(obj.p1)),Vector2f(2,2)));
                ENSURE('almostEqualPoint(obj.b1.min, obj.p0)');
                ENSURE('almostEqualPoint(obj.b1.max, obj.p1)');
                ENSURE('almostEqualVector(obj.b1.minVector, asFloatVector(obj.p0))');
                ENSURE('almostEqualVector(obj.b1.maxVector, asFloatVector(obj.p1))');
                ENSURE('almostEqualPoint(obj.b1.center, obj.myCenter)');

                obj.b1.center = obj.p3;
                ENSURE('almostEqualPoint(obj.b1.center,obj.p3)');
                obj.b1.sizeVector = obj.v1;
                ENSURE('almostEqualVector(obj.b1.sizeVector,obj.v1)');
            }
            // test extendBy
            {
                obj.b1.makeEmpty();
                obj.b1.extendByPoint(obj.p0);
                ENSURE('almostEqualPoint(obj.b1.min, obj.p0)');
                ENSURE('almostEqualPoint(obj.b1.max,obj. p0)');
                obj.b1.extendByPoint(obj.p1);
                ENSURE('almostEqualPoint(obj.b1.min, obj.p0)');
                ENSURE('almostEqualPoint(obj.b1.max, obj.p1)');
                obj.b1.extendByPoint(obj.p2);
                obj.b1.extendByPoint(obj.p3);
                ENSURE('almostEqualPoint(obj.b1.min, obj.p3)');
                ENSURE('almostEqualPoint(obj.b1.max, obj.p2)');
                obj.b1.extendBy(obj.b1);
                obj.b1.makeEmpty();
                obj.b2 = Box2f(obj.p1, obj.p2);

                obj.b1.extendBy(obj.b2);
                ENSURE('almostEqualPoint(obj.b1.min, obj.b2.min)');
                ENSURE('almostEqualPoint(obj.b1.max, obj.b2.max)');
                obj.b1.makeEmpty();
                obj.b1.extendByPoint(obj.p1);
                obj.b1.extendBy(obj.b1);
                ENSURE('obj.b1.isEmpty');
                ENSURE('obj.b1.hasPosition');
                ENSURE('!obj.b1.hasSize');
                obj.b1.extendBy(obj.b2);
                ENSURE('!obj.b1.isEmpty');
                ENSURE('obj.b1.hasPosition');
                ENSURE('obj.b1.hasSize');
            }

            // test contains
            {
                obj.b1 = Box2f(obj.p0, obj.p2);
                ENSURE(obj.b1.containsPoint(obj.p1));
                obj.b1.makeEmpty();
                ENSURE(!obj.b1.containsPoint(obj.p1));
            }

            // Test isEmpty, hasPosition, hasSize, hasArea
            {
                obj.p1=Point2f(1, 1);
                obj.p2=Point2f(1, 2);
                obj.p3=Point2f(2, 2);

                obj.b1.makeEmpty();
                ENSURE('obj.b1.isEmpty');
                ENSURE('!obj.b1.hasPosition');
                ENSURE('!obj.b1.hasSize');
                ENSURE('!obj.b1.hasArea');
                obj.b1.extendByPoint(obj.p1);
                ENSURE('obj.b1.isEmpty');
                ENSURE('obj.b1.hasPosition');
                ENSURE('!obj.b1.hasSize');
                ENSURE('!obj.b1.hasArea');
                obj.b1.extendByPoint(obj.p1);
                ENSURE('obj.b1.isEmpty');
                ENSURE('obj.b1.hasPosition');
                ENSURE('!obj.b1.hasSize');
                ENSURE('!obj.b1.hasArea');
                obj.b1.extendByPoint(obj.p2);
                ENSURE('obj.b1.isEmpty');
                ENSURE('obj.b1.hasPosition');
                ENSURE('obj.b1.hasSize');
                ENSURE('!obj.b1.hasArea');
                obj.b1.extendByPoint(obj.p3);
                ENSURE('!obj.b1.isEmpty');
                ENSURE('obj.b1.hasPosition');
                ENSURE('obj.b1.hasSize');
                ENSURE('obj.b1.hasArea');
            }
        } // with
    } // function testPoint2
*/

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
            //ENSURE('almostEqual(obj.b1.minVector, obj.p0)');
            //ENSURE('almostEqual(obj.b1.maxVector, obj.p1)');
            ENSURE('almostEqual(obj.b1.center, obj.myCenter)');

            obj.b1.center = obj.p3;
            ENSURE('almostEqual(obj.b1.center,obj.p3)');
            //obj.b1.sizeVector = obj.v1;
            //ENSURE('almostEqual(obj.b1.sizeVector,obj.v1)');
        }

        // test extend
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

        // Test containsPoint
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
function SphereUnitTest() {
    this.Constructor(this, "SphereUnitTest");
};

SphereUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        testPoint3();
    }

    function testPoint3() {

        obj.p0=new Point3f(0.55, 0.6, 0.7);
        obj.p1=new Point3f(1.1, 1.2, 1.4);
        obj.p2=new Point3f(2.3, 2.5, 2.9);
        obj.p3=new Point3f(-1, -1.1, -1.2);
        obj.v1=new Vector3f(2.2, 3.3, 5.5);

        // Constructor tests
        {
            obj.b1=new Sphere(obj.p0, 2);
            ENSURE('almostEqual(obj.b1.center, obj.p0)');
            ENSURE('almostEqual(obj.b1.radius,2)');

            obj.b3=new Sphere(new Point3f(0, 1, 2), new Point3f(1, 1, 2));
            DPRINT('obj.b3');
            ENSURE('almostEqual(b3.center, new Point3f(0.5, 1, 2))');
            ENSURE('almostEqual(b3.radius, 0.5)');
        }

        // test extend & contains
        {

            obj.p0=new Point3f(0.0, 0.0, 0.0);
            obj.p1=new Point3f(2, 0, 0);
            obj.p2=new Point3f(-1, 0, 0);
            obj.p3=new Point3f(0, 4, 0);

            obj.b1.makeEmpty();
            obj.b1.extendBy(obj.p0);
            ENSURE('almostEqual(obj.b1.center, obj.p0)');
            ENSURE('obj.b1.isEmpty');
            DPRINT('obj.b1');
            obj.b1.extendBy(obj.p1);
            SUCCESS('obj.b1.extendBy(obj.p1)');
            DPRINT('obj.p1');
            DPRINT('obj.b1');
            ENSURE('obj.b1.contains(obj.p0)');
            ENSURE('obj.b1.contains(obj.p1)');
            ENSURE('!obj.b1.contains(obj.p2)');
            ENSURE('!obj.b1.contains(obj.p3)');

            obj.b1.extendBy(obj.p2);
            SUCCESS('obj.b1.extendBy(obj.p2)');
            DPRINT('obj.p0');
            DPRINT('obj.p1');
            DPRINT('obj.p2');
            DPRINT('obj.p3');
            DPRINT('obj.b1');
            ENSURE('obj.b1.contains(obj.p0)');
            ENSURE('obj.b1.contains(obj.p1)');
            ENSURE('obj.b1.contains(obj.p2)');
            ENSURE('!obj.b1.contains(obj.p3)');

            obj.b1.extendBy(obj.p3);
            SUCCESS('obj.b1.extendBy(obj.p3)');
            DPRINT('obj.p0');
            DPRINT('obj.p1');
            DPRINT('obj.p2');
            DPRINT('obj.p3');
            DPRINT('obj.b1');
            ENSURE('obj.b1.contains(obj.p0)');
            ENSURE('obj.b1.contains(obj.p1)');
            ENSURE('obj.b1.contains(obj.p2)');
            ENSURE('obj.b1.contains(obj.p3)');
            DPRINT('magnitude(difference(obj.p3, obj.b1.center))');

            obj.big = new Sphere([1,1,1],20)
            DPRINT('obj.b1');
            DPRINT('obj.big');
            obj.b1.extendBy(obj.big);
            SUCCESS('obj.b1.extendBy(obj.p3)');
            DPRINT('obj.b1');
            ENSURE('obj.b1.contains(obj.big)');
            ENSURE('almostEqual(obj.b1.center, obj.big.center)');
            ENSURE('almostEqual(obj.b1.radius, obj.big.radius)');

            // make sure it does not grow further
            obj.b1.extendBy(obj.big);
            ENSURE('obj.b1.contains(obj.big)');
            ENSURE('obj.b1.intersects(obj.big)');
            ENSURE('obj.b1.touches(obj.big)');
            ENSURE('!obj.b1.envelopes(obj.big)');
            ENSURE('obj.b1.envelopes(new Sphere([0,0,0],1))');
            ENSURE('almostEqual(obj.b1.center, obj.big.center)');
            ENSURE('almostEqual(obj.b1.radius, obj.big.radius)');

            obj.bigger = new Sphere([20,1,1],35)
            obj.b1.extendBy(obj.bigger);
            ENSURE('obj.b1.contains(obj.big)');
            ENSURE('obj.b1.contains(obj.bigger)');
            DPRINT('obj.b1');

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

            obj.b1.extendBy(obj.p2);
            ENSURE('!obj.b1.isEmpty');
            ENSURE('obj.b1.hasPosition');
            ENSURE('obj.b1.hasSize');
            ENSURE('obj.b1.hasArea');
            ENSURE('obj.b1.hasVolume');

            obj.b1 = new Sphere([0,0,0],1);
            DPRINT('obj.b1.size');
            DPRINT('obj.b1.area');
            DPRINT('obj.b1.volume');
        }

    } //function
};


function HexToStringTest() {
    this.Constructor(this, "HexToStringTest");
};

HexToStringTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        ENSURE('asHexString(0)   == "00"');
        ENSURE('asHexString(255) == "FF"');
        ENSURE('asHexString(99)  == "63"');

        ENSURE('fromHexString("00")  == 0');
        ENSURE('fromHexString("FF")  == 255');
        ENSURE('fromHexString("63")  == 99');

        obj.allCharsOk = true;
        for (var i = 0; i < 255; ++i) {
            obj.allCharsOk = obj.allCharsOk && (fromHexString(asHexString(i)) == i);
        }

        ENSURE('obj.allCharsOk');
    }
};

function ExceptionTest() {
    this.Constructor(this, "ExceptionTest");
};

ExceptionTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myExceptionCaught = false;
        try {
            print("### The next error is intentional:");
            plug("this will not work");
        } catch (ex) {
            obj.myExceptionCaught = true;
        }

        ENSURE("obj.myExceptionCaught");
    }
};

function EmptyObject() {
}

EmptyObject.prototype.nop = function() {
}


try {
    var myTestName = "testShell.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new PlaneUnitTest());
    mySuite.addTest(new TriangleUnitTest());
    mySuite.addTest(new IntersectionUnitTest());
    mySuite.addTest(new BoxUnitTest());
    mySuite.addTest(new LineUnitTest());
    mySuite.addTest(new RayUnitTest());
    mySuite.addTest(new LineSegmentUnitTest());
    mySuite.addTest(new SphereUnitTest());
    mySuite.addTest(new RendererUnitTest());
    mySuite.addTest(new Matrix4fUnitTest());
    mySuite.addTest(new PointUnitTest());
    mySuite.addTest(new VectorUnitTest());
    mySuite.addTest(new HexToStringTest());
    mySuite.addTest(new ExceptionTest());

    mySuite.run();

    print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}
