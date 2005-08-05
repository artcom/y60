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
//   $RCSfile: LookAtCameraMover.js,v $
//   $Author: jens $
//   $Revision: 1.8 $
//   $Date: 2005/04/26 16:29:55 $
//
//=============================================================================

function LookAtCameraMover(theCamera, theLookAtPosition) {
    this.Constructor(this, theCamera, theLookAtPosition);
}

LookAtCameraMover.prototype.Constructor = function(obj, theCamera, theLookAtPosition) {

    const CAMERA_FRONT_DIRECTION = new Vector3f(0,0,1);;
    const CAMERA_UP_DIRECTION    = new Vector3f(0,1,0);
    const CAMERA_RIGHT_DIRECTION = cross(CAMERA_FRONT_DIRECTION, CAMERA_UP_DIRECTION);
    
    var _myCamera = theCamera;
    var _myLookAtPosition = theLookAtPosition;
    var _myCameraDestinationPosition = new Vector3f(0,0,0);
    var _myDestinationSpringStrength = 1;
     
    var _myLastTime = 0;

    obj.setup = function() {      
        obj.reset();
    }
    obj.reset = function() {
    }

    obj.setDestinationPosition = function(theCameraDestinationPosition) {
        _myCameraDestinationPosition = theCameraDestinationPosition;
    }

    obj.setLookAtPosition = function(theLookAtPosition) {
        _myLookAtPosition = theLookAtPosition;
    }

    obj.setSpringStrength = function(theDestinationSpringStrength) {
        _myDestinationSpringStrength = theDestinationSpringStrength;
    }
        
    obj.getCamera = function() {
        return _myCamera;
    }
    
    obj.update = function(theTime) {
        var myDeltaTime = theTime - _myLastTime;
        if (!_myCamera || myDeltaTime == 0) {
            return;
        }
        _myLastTime = theTime;
        
        var myCameraMotionToDestination = difference(_myCameraDestinationPosition, _myCamera.position);
        var myCameraMotionFactor = _myDestinationSpringStrength * myDeltaTime;
        if (myCameraMotionFactor > 1) {
            myCameraMotionFactor = 1;
        }
        _myCamera.position = sum(_myCamera.position, product(myCameraMotionToDestination, myCameraMotionFactor));      

        // calc orientation
        var myViewVector = normalized(difference(_myLookAtPosition, _myCamera.position));
        _myCamera.orientation = getOrientationFromDirection(myViewVector, CAMERA_UP_DIRECTION);
    }
    
    obj.setup();
}
