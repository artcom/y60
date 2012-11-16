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
//
//   $RCSfile: Trackball.js,v $
//   $Author: david $
//   $Revision: 1.1 $
//   $Date: 2005/04/04 14:40:15 $
//
//=============================================================================

/*jslint nomen:false*/
/*globals Quaternionf, Vector3f, cross, magnitude, difference, product,
          clamp*/

function Trackball() {
    this.Constructor(this);
}

Trackball.prototype.Constructor = function (obj) {
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var SPHERE_RADIUS   = 1.0;   // sphere for quat-based rotation (1=screen-width)
    var _curQuat        = new Quaternionf(0, 0, 0, 1);
    var _mySphereRadius = SPHERE_RADIUS;

    /////////////////////
    // Private Methods //
    /////////////////////
    
    function projectOnSphere(theVector) {
        var rsqr = _mySphereRadius * _mySphereRadius;
        var dsqr = theVector.x * theVector.x + theVector.y * theVector.y;
        var myProjectedVector = new Vector3f(theVector.x, theVector.y, 0);
        // if relatively "inside" sphere project to sphere else on hyperbolic sheet
        if (dsqr < (rsqr * 0.5)) {
            myProjectedVector.z = Math.sqrt(rsqr - dsqr);
        } else {
            myProjectedVector.z = rsqr / (2 * Math.sqrt(dsqr));
        }
        return myProjectedVector;
    }

    function getRotationFromMove(theVector0, theVector1) {
        // calculate axis of rotation and correct it to avoid "near zero length" rot axis
        var myRotAxis = cross(theVector1, theVector0);
        if (magnitude(myRotAxis) < 0.000000001) {
            myRotAxis = new Vector3f(1, 0, 0);
        }
        // find the amount of rotation
        var d = difference(theVector1, theVector0);
        var t = magnitude(d) / (2.0 * _mySphereRadius);
        t = clamp(t, -1.0, 1.0);
        var phi = 2.0 * Math.asin(t);
        return new Quaternionf(myRotAxis, phi);
    }

    ////////////////////
    // Public Methods //
    ////////////////////

    obj.rotate = function (thePrevMousePos, theCurMousePos) {
        var prevVector = projectOnSphere(thePrevMousePos);
        var curVector = projectOnSphere(theCurMousePos);
        var moveRotation = getRotationFromMove(prevVector, curVector);

        _curQuat = product(moveRotation, _curQuat);
    };

    obj.setSphereRadius = function (theRadius) {
        _mySphereRadius = theRadius;
    };
    
    obj.getSphereRadius = function (theRadius) {
        return _mySphereRadius;
    };

    obj.getQuaternion = function () {
        return new Quaternionf(_curQuat);
    };
    
    obj.setQuaternion = function (theNewValue) {
        _curQuat = theNewValue;
    };
};