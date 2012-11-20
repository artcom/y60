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
//   $RCSfile: LookAtCameraMover.js,v $
//   $Author: jens $
//   $Revision: 1.8 $
//   $Date: 2005/04/26 16:29:55 $
//
//=============================================================================

function LookAtCameraMover(theCamera, theLookAtPosition) {
    this.Constructor(this, theCamera, theLookAtPosition);
}

LookAtCameraMover.prototype.Constructor = function(obj, theCamera, theNodeToFollow) {

    const CAMERA_FRONT_DIRECTION = new Vector3f(0,0,1);;
    const CAMERA_UP_DIRECTION    = new Vector3f(0,1,0);
    const CAMERA_RIGHT_DIRECTION = cross(CAMERA_FRONT_DIRECTION, CAMERA_UP_DIRECTION);

    obj.setup = function() {
        obj.reset();
    }
    obj.reset = function() {
    }

    obj.setNodeToFollow = function(theNode) {
        _myNodeToFollow = theNode;
    }

    obj.getNodeToFollow = function() {
        return _myNodeToFollow;
    }

    obj.setEyePositionOffset = function(theOffset) {
        _myEyePositionOffset = theOffset;
    }

    obj.getEyePositionOffset = function() {
        return _myEyePositionOffset;
    }

    obj.setLookAtPositionOffset = function(theOffset) {
        _myLookAtOffset = theOffset;
    }

    obj.getLookAtPositionOffset = function() {
        return _myLookAtOffset;
    }

    obj.setSpringStrength = function(theSpringStrength) {
        _mySpringStrength = theSpringStrength;
        _mySpringCoef     = calcSpringCoef(theSpringStrength);
    }

    obj.getSpringStrength = function() {
        return _mySpringStrength;
    }

    obj.setOrientation = function(theOrientation) {
        _myOrientation = theOrientation;
    }

    obj.getCamera = function() {
        return _myCamera;
    }

    obj.getLookAtPosition = function() {
        return _myLookAtPosition;
    }

    obj.update = function(theTime, theDeltaT) {
        if (_myLastTime == null) {
            _myLastTime = theTime;
        }

        if (!_myNodeToFollow) {
            Logger.warning("No node to follow specified.");
            return;
        }

        var myDeltaTime = theDeltaT ? theDeltaT : (theTime - _myLastTime);

        _myLastTime = theTime;

        var myRotationMatrix = null;
        if (_myOrientation) {
            myRotationMatrix = new Matrix4f(_myOrientation);
        } else {
            myRotationMatrix = new Matrix4f(_myNodeToFollow.orientation);
        }

        var myNodePosition   = _myNodeToFollow.globalmatrix.getRow(3).xyz;
        var myEyePosition    = sum(myNodePosition, product(_myEyePositionOffset, myRotationMatrix));
        var myLookAtPosition = sum(myNodePosition, product(_myLookAtOffset, myRotationMatrix));

        // old code
        if ( isFinite(_mySpringStrength) ) {
            var myMotionFactor = _mySpringStrength * myDeltaTime;
            if (myMotionFactor > 1) {
                myMotionFactor = 1;
            }
            var myDistance = difference(myEyePosition, _myCamera.position);
            _myCamera.position = sum(_myCamera.position, product(myDistance, myMotionFactor));

            myDistance = difference(myLookAtPosition, _myLookAtPosition);
            _myLookAtPosition = sum(_myLookAtPosition, product(myDistance, myMotionFactor));
        } else {
            _myCamera.position = myEyePosition;
            _myLookAtPosition = myLookAtPosition;
        }

        // new code
        //if (isFinite(_mySpringStrength)) {
        //    _mySpringCoef = Math.atan(_mySpringStrength * myDeltaTime * PI_2) / PI_2;
        //} else {
        //    _mySpringCoef = 1.0;
        //}
        //_myCamera.position = sum(product(_myCamera.position, (1 - _mySpringCoef)), product(myEyePosition, _mySpringCoef));
        //_myLookAtPosition  = sum(product(_myLookAtPosition, (1 - _mySpringCoef)), product(myLookAtPosition, _mySpringCoef));

        // calc orientation
        var myViewVector = normalized(difference(_myLookAtPosition, _myCamera.position));
        _myCamera.orientation = getOrientationFromDirection(myViewVector, CAMERA_UP_DIRECTION);
    }

    function calcSpringCoef(theSpringStrength) {
        return Math.atan(theSpringStrength * 0.1) / PI_2;
    }

    var _myCamera       = theCamera;
    var _myNodeToFollow = theNodeToFollow;

    var _myLookAtPosition    = new Vector3f(0,0,0);
    var _myLookAtOffset      = new Vector3f(0,0,0);
    var _myEyePositionOffset = new Vector3f(10,2,0);
    var _myOrientation       = null;

    var _mySpringStrength = 1;
    var _mySpringCoef = calcSpringCoef(_mySpringStrength);
    var _myLastTime = null;

    obj.setup();
}
