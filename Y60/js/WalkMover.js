//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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


const MODEL_FRONT_DIRECTION    = new Vector3f(0,0,1);
const MODEL_UP_DIRECTION       = new Vector3f(0,1,0);
const MODEL_RIGHT_DIRECTION    = cross(MODEL_FRONT_DIRECTION, MODEL_UP_DIRECTION);
const INITIAL_WALK_SPEED       = 0.1; // percentage of world size per second
const INITIAL_EYEHEIGHT        = 2.0;
const ROTATE_SPEED             = 1.0;
const GRAVITY                  = 9.81;
const PERSON_MASS              = 100;
const GRAVITY_DIRECTION        = new Vector3f(0, -1, 0);
const GRAVITY_ACCELERATION     = product(GRAVITY_DIRECTION, GRAVITY);
const PERSON_WEIGHT_FORCE      = product(GRAVITY_ACCELERATION, PERSON_MASS);

function WalkMover() {
    this.Constructor(this);
}

WalkMover.prototype.Constructor = function(self) {
    MoverBase.prototype.Constructor(self);
    self.Mover = [];

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
        Scene.updateGlobalMatrix(myCamera);

        _myPosition          = myCamera.globalmatrix.getTranslation();
        _myVelocity          = new Vector3f(0,0,0);
        _myEulerOrientation  = myCamera.globalmatrix.getRotation();
        _myWalkSpeed         = INITIAL_WALK_SPEED;

        _myGroundContactFlag = false;
        _myGroundNormal      = null;
        _myGroundPlane       = null;
        _myEyeHeight         = INITIAL_EYEHEIGHT;        
    }

    self.onIdle = function(theTime) {
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
        if (theShiftFlag) {
            switch(theKey) {
                case "up":
                    _myEyeHeight += 0.1;
                break;
                case "down":
                    _myEyeHeight -= 0.1;
                    _myEyeHeight = Math.max(_myEyeHeight,0);
                break;
                   
            }            
        } else {            
            _myPressedKeys[theKey] = theKeyState;
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
            _myEulerOrientation.x += myDelta.y;
            _myEulerOrientation.y += -myDelta.x;
            self.getMoverObject().orientation = Quaternionf.createFromEuler(_myEulerOrientation);
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

    //////////////////////////////////////////////////////////////////////
    //
    // private
    //
    //////////////////////////////////////////////////////////////////////

    function simulate(theDeltaTime) {
        var myCamera = self.getMoverObject();
        Scene.updateGlobalMatrix(myCamera);

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
        //print("Find ground with ray: " + theProbe);
        if (myGroundIntersection) {
            _myGroundPlane  = new Plane(_myUpVector, myGroundIntersection.position);
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
        myFallingTranslation.y = -1;
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
                if (_myGroundContactFlag) {
                    myDirVector = _myProjectedFrontVector;
                } else {
                    myDirVector = _myFrontVector;
                }

                myNewPosition = sum(_myPosition, product(myDirVector, -myStep));
                if (!positionAllowed(myNewPosition)) {
                    myNewPosition = sum (_myPosition, product(myDirVector, 5.0 * myStep));
                }
                break;
            case "down":
                if (_myGroundContactFlag) {
                    myDirVector = _myProjectedFrontVector;
                } else {
                    myDirVector = _myFrontVector;
                }
                myNewPosition = sum (_myPosition, product(myDirVector, myStep));
                if (!positionAllowed(myNewPosition)) {
                    myNewPosition = sum (_myPosition, product(myDirVector, -5.0 * myStep));
                }
                break;
            case "left":
                if (_myGroundContactFlag) {
                    myDirVector = _myProjectedRightVector;
                } else {
                    myDirVector = _myRightVector;
                }
                myNewPosition = sum (_myPosition, product(myDirVector, myStep))
                if (!positionAllowed(myNewPosition)) {
                    myNewPosition = sum (_myPosition, product(myDirVector, -5.0 * myStep));
                }
                break;
            case "right":
                if (_myGroundContactFlag) {
                    myDirVector = _myProjectedRightVector;
                } else {
                    myDirVector = _myRightVector;
                }
                myNewPosition = sum (_myPosition, product(myDirVector, -myStep))
                if (!positionAllowed(myNewPosition)) {
                    myNewPosition = sum (_myPosition, product(myDirVector, 5.0 * myStep));
                }
                break;
        }
        _myPosition = myNewPosition
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
