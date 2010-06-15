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
//
//   $RCSfile: WalkMover.js,v $
//   $Author: valentin $
//   $Revision: 1.1 $
//   $Date: 2005/04/25 15:52:51 $
//
//=============================================================================


use("MoverBase.js");
use("picking_functions.js");
use("intersection_functions.js");

function WalkMover(theViewport) {
    this.Constructor(this, theViewport);
};

WalkMover.prototype.Constructor = function(self, theViewport) {
    MoverBase.prototype.Constructor(self, theViewport);
    self.Mover = [];

    //////////////////////////////////////////////////////////////////////

    const MODEL_FRONT_DIRECTION    = new Vector3f(0,0,1);
    const MODEL_UP_DIRECTION       = new Vector3f(0,1,0);
    const MODEL_RIGHT_DIRECTION    = cross(MODEL_FRONT_DIRECTION, MODEL_UP_DIRECTION);
    const INITIAL_WALK_SPEED       = 0.01; // percentage of world size per second
    const INITIAL_EYEHEIGHT        = 0.5;
    const ROTATE_SPEED             = 1.0;
    const GRAVITY                  = 9.81;
    const PERSON_MASS              = 100;
    const GRAVITY_DIRECTION        = product(MODEL_UP_DIRECTION,-1);
    const GRAVITY_ACCELERATION     = product(GRAVITY_DIRECTION, GRAVITY);
    const PERSON_WEIGHT_FORCE      = product(GRAVITY_ACCELERATION, PERSON_MASS);

    //////////////////////////////////////////////////////////////////////

    var _myPressedKeys        = [];
    var _myLastTime           = null;

    var _myPosition           = new Vector3f(0,0,0);
    var _myVelocity           = new Vector3f(0,0,0); // meter/sec
    var _myEulerOrientation   = new Vector3f(0,0,0);

    var _myWalkSpeed          = INITIAL_WALK_SPEED;
    var _myGroundContactFlag  = false;
    var _myEyeHeight          = INITIAL_EYEHEIGHT;

    var _myUpVector           = new Vector3f(0,1,0);
    var _myFrontVector        = new Vector3f(0,1,0);
    var _myProjectedFrontVector = new Vector3f(0,1,0);
    var _myRightVector        = new Vector3f(0,0,0);
    var _myProjectedRightVector = new Vector3f(0,0,0);

    var _myGroundNormal       = null;
    var _myGroundPlane        = null;

    var _prevNormalizedMousePosition = new Vector3f(0,0,0); // [-1..1]

    //////////////////////////////////////////////////////////////////////
    //
    // public
    //
    //////////////////////////////////////////////////////////////////////

    self.name = "WalkMover";

    self.Mover.reset = self.reset;
    self.reset = function() {
        self.Mover.reset();

        var myCamera          = self.getMoverObject();
        myCamera.orientation = new Quaternionf(0,0,0,1);

        _myPosition          = myCamera.globalmatrix.getTranslation();
        _myVelocity          = new Vector3f(0,0,0);
        _myEulerOrientation  = myCamera.globalmatrix.getRotation();
        _myWalkSpeed         = INITIAL_WALK_SPEED;

        _myGroundContactFlag = false;
        _myGroundNormal      = null;
        _myGroundPlane       = null;
        _myEyeHeight         = INITIAL_EYEHEIGHT;
    }
    
    self.eyeHeight setter = function(theHeight) {
        _myEyeHeight = theHeight;
        _myEyeHeight = Math.max(_myEyeHeight,0);
    }
    
    self.eyeHeight getter = function () {
        return _myEyeHeight;
    }
    
    self.walkSpeed setter = function(theWalkSpeed) {
        _myWalkSpeed = theWalkSpeed;
    }
    
    self.walkSpeed getter = function () {
        return _myWalkSpeed;
    }
    
    self.movements.rotateXY = function(theDelta) {
        _myEulerOrientation.x += theDelta.y;
        _myEulerOrientation.y += -theDelta.x;
        self.getMoverObject().orientation = Quaternionf.createFromEuler(_myEulerOrientation);
    }
    
    self.movements.translateAlongFrontVector = function (theDelta) {
        var myDirVector;
        if (_myGroundContactFlag) {
            myDirVector = _myProjectedFrontVector;
        } else {
            myDirVector = _myFrontVector;
        }
        translate(myDirVector, theDelta);
    }
    
    self.movements.translateAlongRightVector = function (theDelta) {
        var myDirVector;
        if (_myGroundContactFlag) {
            myDirVector = _myProjectedRightVector;
        } else {
            myDirVector = _myRightVector;
        }
        translate(myDirVector, theDelta);
    }
    
    function translate(theDirection, theDelta) {
        var myNewPosition = sum (_myPosition, product(theDirection, theDelta));
        if (!positionAllowed(myNewPosition)) {
            myNewPosition = sum (_myPosition, product(theDirection, -5.0 * theDelta));
        }
        _myPosition = myNewPosition
    }

    self.onFrame = function(theTime) {
        if (_myLastTime == null) {
            _myLastTime = theTime;
            return;
        }
        var myDeltaTime = theTime - _myLastTime;
        for (var myKey in _myPressedKeys) {
            if (_myPressedKeys[myKey]) {
                onKeyDown(myKey, myDeltaTime);
            }
        }
        simulate(myDeltaTime);
        _myLastTime = theTime;
    }

    self.Mover.onKey = self.onKey;
    self.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag) {
        if (theKeyState && theKey == 'h') {
            printHelp();
        }
        if (theShiftFlag && theControlFlag && theAltFlag) {
            switch(theKey) {
                case "up":
                    _myEyeHeight *= 1.1;
                break;
                case "down":
                    _myEyeHeight /= 1.1;
                    _myEyeHeight = Math.max(_myEyeHeight,0);
                break;

            }
        } else {
            if((theKeyState && theControlFlag && theAltFlag) || !theKeyState) {
                _myPressedKeys[theKey] = theKeyState;
            }
        }
        self.Mover.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag);
    }

    self.Mover.onMouseButton = self.onMouseButton;
    self.onMouseButton = function(theButton, theState, theX, theY) {
        self.Mover.onMouseButton(theButton, theState, theX, theY);
        if (theButton == LEFT_BUTTON) {
            _prevNormalizedMousePosition = self.getNormalizedScreen(theX, theY);
        } else { // Button Up
            _prevNormalizedMousePosition = new Vector3f(0,0,0);
        }
    }

    self.onMouseMotion = function(theX, theY) {
        var curNormalizedMousePos = self.getNormalizedScreen(theX, theY);
        if (self.getLeftButtonFlag()) {
            var myDelta = difference(curNormalizedMousePos, _prevNormalizedMousePosition);
            self.movements.rotateXY(myDelta);
        }
        _prevNormalizedMousePosition = curNormalizedMousePos;
    }

    self.onMouseWheel = function(theDeltaX, theDeltaY) {
        if (theDeltaY < 0) {
            _myWalkSpeed *= 1.5;
        } else {
            _myWalkSpeed /= 1.5;
        }

    }
    self.setPosition = function (thePosition) {
        _myPosition = thePosition;
    }

    //////////////////////////////////////////////////////////////////////
    //
    // private
    //
    //////////////////////////////////////////////////////////////////////

    function simulate(theDeltaTime) {
        var myCamera = self.getMoverObject();

        // Get current orientation
        var myOrientationMatrix = new Matrix4f;
        myOrientationMatrix.makeIdentity();
        myOrientationMatrix.setRow(2, myCamera.globalmatrix.getRow(2).xyz0);
        myOrientationMatrix.setRow(1, myCamera.globalmatrix.getRow(1).xyz0);
        myOrientationMatrix.setRow(0, myCamera.globalmatrix.getRow(0).xyz0);

        _myFrontVector = product(MODEL_FRONT_DIRECTION, myOrientationMatrix);
        _myRightVector = product(MODEL_RIGHT_DIRECTION, myOrientationMatrix);
        
        if (_myGroundPlane) {
            _myProjectedFrontVector = normalized(projection(_myFrontVector, _myGroundPlane));
            _myProjectedRightVector = normalized(projection(_myRightVector, _myGroundPlane));
        }
        _myUpVector    = product(MODEL_UP_DIRECTION,    myOrientationMatrix);

        var myVelocity = new Vector3f(_myVelocity);

        var myPostAccelVelocity = new Vector3f(_myVelocity);
        simulateForces(theDeltaTime, myPostAccelVelocity);

        var myTranslation = product(myPostAccelVelocity, theDeltaTime);
        _myVelocity = myPostAccelVelocity;

        dropToGround(myTranslation);
        self.getMoverObject().position = _myPosition;
    }

    // returns position change resulting from all forces
    function simulateForces(myDeltaTime, myInOutVelocity) {
        var myAcceleration   = product(PERSON_WEIGHT_FORCE, 1 / PERSON_MASS);
        var myVelocityChange = product(myAcceleration, myDeltaTime);
        myInOutVelocity.add(myVelocityChange);
    }

    function positionAllowed(theNewPostion) {
        var myForwardStep = new LineSegment(_myPosition, theNewPostion);
        var myForwardIntersection = nearestIntersection(self.getWorld(), myForwardStep);
        return (myForwardIntersection == null)
    }

    function findGround(theProbe) {
        var myGroundIntersection = nearestIntersection(self.getWorld(), theProbe);
        if (myGroundIntersection) {
            _myGroundPlane  = new Planef(_myUpVector, myGroundIntersection.position);
            _myGroundNormal = normalized(myGroundIntersection.normal);
            _myGroundContactFlag = true;
            var myNewPosition = new Vector3f(myGroundIntersection.position);
            myNewPosition.y += _myEyeHeight;
            return myNewPosition;
        } else {
            _myGroundContactFlag = false;
            return null;
        }
    }

    // Reorients the Mover with the ground, pushes the Mover above the ground
    function dropToGround(myTranslation) {
        var myFallingTranslation = myTranslation.clone();
        myFallingTranslation.y = GRAVITY_DIRECTION.y;
        var myNewPosition = findGround(new Ray(_myPosition, product(myFallingTranslation, 2)));
        if (myNewPosition != null) {
            _myPosition.value = myNewPosition;
        }
    }

    function onKeyDown(theKey, theDeltaT) {
        var myNewPosition = new Vector3f(_myPosition);
        var myDirVector = null;
        var myStep = _myWalkSpeed * self.getWorldSize() * theDeltaT;
        switch (theKey) {
            case "up":
                self.movements.translateAlongFrontVector(-myStep);
                break;
            case "down":
                self.movements.translateAlongFrontVector(myStep);
                break;
            case "left":
                self.movements.translateAlongRightVector(myStep);
                break;
            case "right":
                self.movements.translateAlongRightVector(-myStep);
                break;
        }
    }

    function printHelp() {
        print("Walk Mover keys:");
        print("   <up>  walk forward");
        print("  <down> walk backward");
        print("  <left> walk to the left");
        print(" <right> walk to the right");
        print(" mouse-wheel-up    increase walk speed");
        print(" mouse-wheel-down  decrease walk speed");
    }
}
