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
//   $RCSfile: WalkMover.js,v $
//   $Author: valentin $
//   $Revision: 1.1 $
//   $Date: 2005/04/25 15:52:51 $
//
//=============================================================================


/*jslint nomen:false*/
/*globals use, MoverBase, Vector3f, cross, product, almostEqual, Matrix4f,
          Quaternionf, Vector4f, print, sum, LEFT_BUTTON, Ray, normalized,
          Planef, projection, LineSegment, distance, nearestIntersection*/

use("MoverBase.js");
use("picking_functions.js");
use("intersection_functions.js");

function WalkMover(theViewport) {
    this.Constructor(this, theViewport);
}

WalkMover.prototype.Constructor = function (self, theViewport) {
    MoverBase.prototype.Constructor(self, theViewport);
    self.Mover = [];

    /////////////////////
    // Private Members //
    /////////////////////

    var MODEL_FRONT_DIRECTION = new Vector3f(0, 0, 1);
    var MODEL_UP_DIRECTION    = new Vector3f(0, 1, 0);
    var MODEL_RIGHT_DIRECTION = cross(MODEL_FRONT_DIRECTION, MODEL_UP_DIRECTION);
    var INITIAL_WALK_SPEED    = 0.01; // percentage of world size per second
    var INITIAL_EYEHEIGHT     = 200;
    var GRAVITY               = 9.81;
    var PERSON_MASS           = 100;
    var GRAVITY_DIRECTION     = product(MODEL_UP_DIRECTION, -1);
    var GRAVITY_ACCELERATION  = product(GRAVITY_DIRECTION, GRAVITY);
    var PERSON_WEIGHT_FORCE   = product(GRAVITY_ACCELERATION, PERSON_MASS);

    var _myPressedKeys        = [];
    var _myLastTime           = null;

    var _myPosition           = new Vector3f(0, 0, 0);
    var _myVelocity           = new Vector3f(0, 0, 0); // meter/sec

    var _myWalkSpeed          = INITIAL_WALK_SPEED;
    var _myGroundContactFlag  = false;
    var _myEyeHeight          = INITIAL_EYEHEIGHT;

    var _myUpVector             = new Vector3f(0, 1, 0);
    var _myFrontVector          = new Vector3f(0, 1, 0);
    var _myProjectedFrontVector = new Vector3f(0, 1, 0);
    var _myRightVector          = new Vector3f(0, 0, 0);
    var _myProjectedRightVector = new Vector3f(0, 0, 0);

    var _myGroundNormal       = null;
    var _myGroundPlane        = null;

    var _prevMousePosition    = new Vector3f(0, 0, 0);

    var _myMinimumTiltRotation = null;
    var _myMaximumTiltRotation = null;
    
    /////////////////////
    // Private Methods //
    /////////////////////

    // returns position change resulting from all forces
    function simulateForces(myDeltaTime, myInOutVelocity) {
        var myAcceleration   = product(PERSON_WEIGHT_FORCE, 1 / PERSON_MASS);
        var myVelocityChange = product(myAcceleration, myDeltaTime);
        myInOutVelocity.add(myVelocityChange);
    }

    function positionAllowed(theNewPostion) {
        var myForwardStep = new LineSegment(_myPosition, theNewPostion);
        return !!nearestIntersection(self.getWorld(), myForwardStep);
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
        if (myNewPosition) {
            _myPosition.value = myNewPosition;
        }
    }

    function simulate(theDeltaTime) {
        var myCamera = self.getMoverObject();

        // Get current orientation
        var myOrientationMatrix = new Matrix4f();
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
        _myUpVector = product(MODEL_UP_DIRECTION, myOrientationMatrix);
        
        var myPostAccelVelocity = new Vector3f(_myVelocity);
        simulateForces(theDeltaTime, myPostAccelVelocity);

        var myTranslation = product(myPostAccelVelocity, theDeltaTime);
        _myVelocity = myPostAccelVelocity;

        dropToGround(myTranslation);
        
        if (self.defensivelyUpdate) {
            if (!almostEqual(distance(_myPosition, self.getMoverObject().position), 0)) {
                self.getMoverObject().position = _myPosition;
            }
        } else {
            self.getMoverObject().position = _myPosition;
        }
    }

    function onKeyDown(theKey, theDeltaT) {
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

    function translate(theDirection, theDelta) {
        var myNewPosition = sum(_myPosition, product(theDirection, theDelta));
        if (positionAllowed(myNewPosition)) {
            myNewPosition = sum(_myPosition, product(theDirection, -5.0 * theDelta));
        }
        _myPosition = myNewPosition;
    }

    function printHelp() {
        print("Walk Mover keys:");
        print("  CTRL+ALT+<up>     walk forward");
        print("  CTRL+ALT+<down>   walk backward");
        print("  CTRL+ALT+<left>   walk to the left");
        print("  CTRL+ALT+<right>  walk to the right");
        print("  mouse-wheel-up    increase walk speed");
        print("  mouse-wheel-down  decrease walk speed");
        print("  CTRL+ALT+SHIFT+<up>    increase eye-height");
        print("  CTRL+ALT+SHIFT+<down>  decrease eye-height");
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    self.name = "WalkMover";

    // Position and orientation are only set when really necessary
    // This is mainly needed for onOutdatedValue and onNodeValueChanged to
    // work properly - this maybe the default in the future...
    self.defensivelyUpdate = false;

    self.Mover.reset = self.reset;
    self.reset = function () {
        self.Mover.reset();

        var myCamera         = self.getMoverObject();
        _myPosition          = myCamera.globalmatrix.getTranslation();
        _myVelocity          = new Vector3f(0, 0, 0);
        _myWalkSpeed         = INITIAL_WALK_SPEED;

        _myGroundContactFlag = false;
        _myGroundNormal      = null;
        _myGroundPlane       = null;
        _myEyeHeight         = INITIAL_EYEHEIGHT;
    };
    
    self.__defineGetter__("minTiltRotation", function () {
        return _myMinimumTiltRotation;
    });
    self.__defineSetter__("minTiltRotation", function (theValue) {
        _myMinimumTiltRotation = theValue;
    });
    self.__defineGetter__("maxTiltRotation", function () {
        return _myMaximumTiltRotation;
    });
    self.__defineSetter__("maxTiltRotation", function (theValue) {
        _myMaximumTiltRotation = theValue;
    });

    self.__defineSetter__("eyeHeight", function (theHeight) {
        _myEyeHeight = Math.max(theHeight, 0);
    });
    
    self.__defineGetter__("eyeHeight", function () {
        return _myEyeHeight;
    });
    
    self.__defineSetter__("walkSpeed", function (theWalkSpeed) {
        _myWalkSpeed = theWalkSpeed;
    });
    
    self.__defineGetter__("walkSpeed", function () {
        return _myWalkSpeed;
    });
    
    self.__defineSetter__("position", function (thePosition) {
        _myPosition = thePosition;
    });

    self.__defineSetter__("rotation", function (theRotation) {
        self.getMoverObject().orientation = Quaternionf.createFromEuler(theRotation);
    });
    
    self.movements.rotateXYByScreenCoordinates = function (thePreviousPos, theCurrentPos) {
        var myDelta = self.getNormalizedDifference(thePreviousPos, theCurrentPos);
        self.movements.rotateXY(myDelta);
    };
    
    self.movements.rotateXY = function (theAnglesInRadiant) {
        var myCameraRotationMatrix = new Matrix4f(self.getMoverObject().globalmatrix);
        myCameraRotationMatrix.setRow(3, new Vector4f(0, 0, 0, myCameraRotationMatrix[3][3]));

        var applyTiltRotationFlag = true;
        if (_myMinimumTiltRotation && _myMaximumTiltRotation) {
            var myUnitVectorMatrix = new Matrix4f();
            myUnitVectorMatrix.makeTranslating(new Vector3f(0, 0, -1));
            myUnitVectorMatrix.postMultiply(myCameraRotationMatrix);
            var myRotatedUnitVector = myUnitVectorMatrix.getTranslation();
            if (((theAnglesInRadiant.y <  0) && (myRotatedUnitVector.y < Math.sin(_myMinimumTiltRotation))) ||
                ((theAnglesInRadiant.y >= 0) && (myRotatedUnitVector.y > Math.sin(_myMaximumTiltRotation)))) {
                applyTiltRotationFlag = false;
            }
        }
        var myTiltOrientation = null;
        if (applyTiltRotationFlag) {
            // compute the rotated Side Vector and rotate about it -> TILT rotation
            var myMatrix = new Matrix4f();
            myMatrix.makeTranslating(new Vector3f(1, 0, 0));
            myMatrix.postMultiply(myCameraRotationMatrix);
            //self.getMoverObject().orientation.multiply(new Quaternionf(myMatrix.getTranslation(),theAnglesInRadiant.y));
            myTiltOrientation = new Quaternionf(myMatrix.getTranslation(), theAnglesInRadiant.y);
        }
        // add rotation about the up vector -> PAN rotation
        var myPanOrientation = new Quaternionf(new Vector3f(0, 1, 0), -theAnglesInRadiant.x);
        if (myTiltOrientation) {
            myPanOrientation.multiply(myTiltOrientation);
        }
        self.getMoverObject().orientation.multiply(myPanOrientation);
    };
    
    self.movements.translateAlongFrontVector = function (theDelta) {
        if (_myGroundContactFlag) {
            translate(_myProjectedFrontVector, theDelta);
        } else {
            translate(_myFrontVector, theDelta);
        }
    };
    
    self.movements.translateAlongRightVector = function (theDelta) {
        if (_myGroundContactFlag) {
            translate(_myProjectedRightVector, theDelta);
        } else {
            translate(_myRightVector, theDelta);
        }
    };
    
    // XXX TODO: the Eventhandling should not be inside the Mover
    self.onFrame = function (theTime, theTimeDelta) {
        for (var myKey in _myPressedKeys) {
            if (_myPressedKeys[myKey]) {
                onKeyDown(myKey, theTimeDelta);
            }
        }
        simulate(theTimeDelta);
    };

    // XXX TODO: the Eventhandling should not be inside the Mover
    self.Mover.onKey = self.onKey;
    self.onKey = function (theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag) {
        if (theKeyState && theKey === 'h') {
            printHelp();
        }
        if (theShiftFlag && theControlFlag && theAltFlag) {
            switch (theKey) {
            case "up":
                _myEyeHeight *= 1.1;
                break;
            case "down":
                _myEyeHeight /= 1.1;
                _myEyeHeight = Math.max(_myEyeHeight, 0);
                break;
            }
        } else {
            if ((theKeyState && theControlFlag && theAltFlag) || !theKeyState) {
                _myPressedKeys[theKey] = theKeyState;
            }
        }
        self.Mover.onKey(theKey, theKeyState, theX, theY,
                         theShiftFlag, theControlFlag, theAltFlag);
    };

    // XXX TODO: the Eventhandling should not be inside the Mover
    self.Mover.onMouseButton = self.onMouseButton;
    self.onMouseButton = function (theButton, theState, theX, theY) {
        self.Mover.onMouseButton(theButton, theState, theX, theY);
        if (theButton === LEFT_BUTTON) {
            _prevMousePosition = new Vector3f(theX, theY, 0);
        } else { // Button Up
            _prevMousePosition = new Vector3f(0, 0, 0);
        }
    };

    // XXX TODO: the Eventhandling should not be inside the Mover
    self.onMouseMotion = function (theX, theY) {
        if (self.getLeftButtonFlag()) {
            self.movements.rotateXYByScreenCoordinates(_prevMousePosition, new Vector3f(theX, theY, 0));
        }
        _prevMousePosition = new Vector3f(theX, theY, 0);
    };

    // XXX TODO: the Eventhandling should not be inside the Mover
    self.onMouseWheel = function (theDeltaX, theDeltaY) {
        if (theDeltaY < 0) {
            _myWalkSpeed *= 1.5;
        } else {
            _myWalkSpeed /= 1.5;
        }
    };
};