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
//   $RCSfile: FlyMover.js,v $
//   $Author: christian $
//   $Revision: 1.12 $
//   $Date: 2005/04/27 17:30:36 $
//
//=============================================================================

use("MoverBase.js");

function FlyMover(theViewport) {
    this.Constructor(this, theViewport);
}

FlyMover.prototype.Constructor = function(self, theViewport) {
    MoverBase.prototype.Constructor(self, theViewport);
    self.Mover = [];

    const INITIAL_FLY_SPEED = 0.001;
    const MINIMUM_AUTO_ADJUST_WORLD_SIZE_PERCENTAGE = 0.0001;

    //////////////////////////////////////////////////////////////////////

    var _myHeadingAngle     = 0;
    var _myPitchAngle       = 0;
    var _myLastIdleTime      = null;

    var _myFlySpeed          = 0;
    var _myCurrentStartSpeed = 1;

    //////////////////////////////////////////////////////////////////////
    //
    // public
    //
    //////////////////////////////////////////////////////////////////////

    self.name = "FlyMover";
    
    self.flySpeed getter = function() {
        return _myFlySpeed;
    };
    self.flySpeed setter = function(theSpeed) {
        _myFlySpeed = theSpeed;
    };
    
    self.headingAngle setter = function(theAngleInRadiant) {
        _myHeadingAngle = theAngleInRadiant;
    };
    self.headingAngle getter = function() {
        return _myHeadingAngle;
    };
    
    self.pitchAngle setter = function(theAngleInRadiant) {
        _myPitchAngle = theAngleInRadiant;
    };
    self.pitchAngle getter = function() {
        return _myPitchAngle;
    };

    self.Mover.reset = self.reset;
    self.reset = function() {
        self.Mover.reset();
        _myFlySpeed = 0;
        _myCurrentStartSpeed = 1;
    };

    self.stop = function() {_myFlySpeed = 0;};
    
    self.simulate = function(theDeltaTime) {
        if (_myFlySpeed) {
            var myWorldSize = self.getWorldSize();
            var myDistance = magnitude(self.getMoverObject().position);
            // adjust flight speed corresponding to world center
            if (myDistance < myWorldSize * MINIMUM_AUTO_ADJUST_WORLD_SIZE_PERCENTAGE) {
                myDistance = myWorldSize * MINIMUM_AUTO_ADJUST_WORLD_SIZE_PERCENTAGE;
            }
            var myAdjustmentFactor = myDistance / myWorldSize;
            var myAdjustedFlySpeed = _myFlySpeed * myAdjustmentFactor;
            
            var deltaHeading = new Quaternionf(new Vector3f(0,1,0), theDeltaTime * _myHeadingAngle);
            var deltaPitch = new Quaternionf(new Vector3f(1,0,0), theDeltaTime * _myPitchAngle);
            
            var prevOrientaion = new Quaternionf(self.getMoverObject().orientation);
            var newOrientation = new Quaternionf(deltaPitch);
            newOrientation.multiply(prevOrientaion);
            newOrientation.multiply(deltaHeading);
            self.getMoverObject().orientation = newOrientation;
            var myWorldHeading = _myHeadingAngle * Math.abs(_myHeadingAngle);
            
            // fly in negative z (forward)
            var myWorldTranslation = new Vector3f(0, 0, -myAdjustedFlySpeed * theDeltaTime);
            self.update(myWorldTranslation, 0);
        }
    };
    
    self.increaseFlySpeed = function(theSpeedFactor) {
        if (_myFlySpeed >= _myCurrentStartSpeed) {
            _myFlySpeed = _myFlySpeed * theSpeedFactor;
        } else if (_myFlySpeed <= -_myCurrentStartSpeed) {
            _myFlySpeed = _myFlySpeed / theSpeedFactor;
        } else {
            _myFlySpeed          = self.getWorldSize() * INITIAL_FLY_SPEED;
            _myCurrentStartSpeed = self.getWorldSize() * INITIAL_FLY_SPEED;
        }
        if (_myFlySpeed < _myCurrentStartSpeed && _myFlySpeed > - _myCurrentStartSpeed) {
            _myFlySpeed = 0;
        }
    };
    
    self.decreaseFlySpeed = function(theSpeedFactor) {
        if (_myFlySpeed <= - _myCurrentStartSpeed) {
            _myFlySpeed = _myFlySpeed * theSpeedFactor;
        } else if (_myFlySpeed >= _myCurrentStartSpeed) {
            _myFlySpeed = _myFlySpeed / theSpeedFactor;
        } else {
            _myFlySpeed          = - self.getWorldSize() * INITIAL_FLY_SPEED;
            _myCurrentStartSpeed =   self.getWorldSize() * INITIAL_FLY_SPEED;
        }
        if (_myFlySpeed < _myCurrentStartSpeed && _myFlySpeed > - _myCurrentStartSpeed) {
            _myFlySpeed = 0;
        }
    };
    
    self.setCameraAnglesByScreenCoordinates = function(theX, theY) {
        _myHeadingAngle = 2 * (0.5 - (theX / self.getViewport().width));
        _myPitchAngle = 2 * (0.5 - (theY / self.getViewport().height));
        
        _myHeadingAngle = setZeroZone(_myHeadingAngle, 0.02);
        _myPitchAngle = setZeroZone(_myPitchAngle, 0.02);
    };
    
    // XXX TODO: the Eventhandling should not be inside the Mover
    self.onFrame = function(theTime) {
        if (!_myLastIdleTime) {
            _myLastIdleTime = theTime;
            return;
        }
        var myDeltaTime = theTime - _myLastIdleTime;
        self.simulate(myDeltaTime);
        _myLastIdleTime = theTime;
    };
    
    // XXX TODO: the Eventhandling should not be inside the Mover
    self.onMouseButton = function(theButton, theState, theX, theY) {
        if (theState != BUTTON_UP) {
            return;
        }
        switch (theButton) {
            case LEFT_BUTTON :
                self.increaseFlySpeed(1.5);
                break;
            case MIDDLE_BUTTON :
                _myFlySpeed = 0;
                break;
            case RIGHT_BUTTON :
                self.decreaseFlySpeed(1.5);
                break;
            default:
                return;
        }
    };
    
    // XXX TODO: the Eventhandling should not be inside the Mover
    self.onMouseMotion = function(theX, theY) {
        self.setCameraAnglesByScreenCoordinates(theX, theY);
    };

    //////////////////////////////////////////////////////////////////////
    //
    // private
    //
    //////////////////////////////////////////////////////////////////////

    function setZeroZone(theValue, theCutoff) {
        if (theValue < - theCutoff) {
            return (theValue + theCutoff) / (1 - theCutoff);
        }
        if (theValue > theCutoff) {
            return (theValue - theCutoff) / (1 - theCutoff);
        }
        return 0;
    }
};
