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
    
    var _myCamera       = theCamera;
    var _myNodeToFollow = theNodeToFollow;

    var _myLookAtPosition    = new Vector3f(0,0,0);
    var _myLookAtOffset      = new Vector3f(0,0,0);
    var _myEyePositionOffset = new Vector3f(10,2,0);
    var _myOrientation       = null;

    var _mySpringStrength = 1;
    var _myLastTime = null;

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
    
    obj.update = function(theTime) {
        if (_myLastTime == null) {
            _myLastTime = theTime;
        }

        if (!_myNodeToFollow) {
            Logger.warning("No node to follow specified.");
            return;
        }

        var myDeltaTime = theTime - _myLastTime;
        _myLastTime = theTime;
        
        // var myMatrix = _myNodeToFollow.globalmatrix;

        var myRotationMatrix = null;
        if (_myOrientation) {
            myRotationMatrix = new Matrix4f(_myOrientation);
        } else {
            myRotationMatrix = new Matrix4f(_myNodeToFollow.orientation);
        }

        //var myNodePosition = _myNodeToFollow.boundingbox.center;
        var myNodePosition = _myNodeToFollow.globalmatrix.getRow(3).xyz;

        if ( isFinite(_mySpringStrength) ) {
            var myMotionFactor = _mySpringStrength * myDeltaTime;
            if (myMotionFactor > 1) {
                myMotionFactor = 1;
            }

            var myEyePosition    = sum(myNodePosition, product(_myEyePositionOffset, myRotationMatrix));
            var myDistance = difference(myEyePosition, _myCamera.position);
            _myCamera.position = sum(_myCamera.position, product(myDistance, myMotionFactor));      

            var myLookAtPosition = sum(myNodePosition, product(_myLookAtOffset, myRotationMatrix));
            myDistance = difference(myLookAtPosition, _myLookAtPosition);
            _myLookAtPosition = sum(_myLookAtPosition, product(myDistance, myMotionFactor)); 
        } else {
            _myCamera.position = sum(myNodePosition, product(_myEyePositionOffset, myRotationMatrix));
            _myLookAtPosition = sum(myNodePosition, product(_myLookAtOffset, myRotationMatrix));
        }

        // calc orientation
        var myViewVector = normalized(difference(_myLookAtPosition, _myCamera.position));
        _myCamera.orientation = getOrientationFromDirection(myViewVector, CAMERA_UP_DIRECTION);
    }
    
    obj.setup();
}
