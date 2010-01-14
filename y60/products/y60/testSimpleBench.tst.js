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

print("Pavel's trivial javascript benchmark V.0.1");

function my_add(a, b) {
  return a + b;
};

function fullMatMult(a, b) {
  c = [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]
  c[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0];
  c[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1];
  c[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2];
  c[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3];

  c[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0];
  c[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1];
  c[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2];
  c[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3];

  c[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0];
  c[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1];
  c[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2];
  c[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3];

  c[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0];
  c[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1];
  c[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2];
  c[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3];
  return c;
}
function fullMatMult2(a, b) {
    c = [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]];
    for (var i = 0; i < 4; ++i) {
        for (var j = 0; j < 4; ++j) {
            c[j][i] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j] + a[i][3] * b[3][j];
        }
    }
    return c;
}

function matVecMult(v, m) {
    var result = [1,0,0,0]
    for (var i = 0; i < 4; ++i) {
        result[0] = v[0] * m[0][i] + v[1] * m[1][i] + v[2] * m[2][i] + v[3] * m[3][i];
    }
    return result;
}
//###################################
//# i = i + 1

var i = 0
var n = 5000000

var start = millisec()

for (i = 0; i < n; i+=1) {}

//while (i < n) i+=1;

var stop = millisec()
var duration = stop - start

print(n.toString() + " x {i+=1} took "+ duration +"msec. ("+ n/duration/1e3 +" mio. per sec)");

//###################################
//# my_add(i,1)
i = 0
n = 5000000

start = millisec()

while (i < n) i=my_add(i, 1)

stop = millisec()
duration = stop - start

print(n.toString() + " i = my_add(i,1) took "+ duration +"msec. ("+ n/duration/1e3 +" mio. per sec)");


//###################################
//# a = fullMatMult(b, c)
i = 0
n = 50000

var a = [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]
var b = [[2,0,0,0],[0,2,0,0],[0,0,2,0],[0,0,0,2]]

start = millisec()

for (i = 0; i < n; i+=1) {
    var c = fullMatMult2(a, b)
}
stop = millisec()
duration = stop - start

print(n.toString() + " j = fullMatMult took "+ duration +"msec. ("+ n/duration * 1000+" per sec)");

//###################################
//# a = product(vec,  mat)
var m = new Matrix4f(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)
var v = new Vector4f(2,0,0,0)

start = millisec()

for (i = 0; i < n; i+=1) {
    var c = product(v, m)
}
stop = millisec()
duration = stop - start

print(n.toString() + "  a = product(vec,  mat) took "+ duration +"msec. ("+ n/duration * 1000+" per sec)");

//###################################
//# a = product(vec,  mat)
var m = [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]
var v = [1,2,3,4]

start = millisec()

for (i = 0; i < n; i+=1) {
    var c = matVecMult(v, m)
}
stop = millisec()
duration = stop - start

print(n.toString() + "  a = matVecMult(vec,  mat) took "+ duration +"msec. ("+ n/duration * 1000+" per sec)");

