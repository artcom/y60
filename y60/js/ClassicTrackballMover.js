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
//   $RCSfile: ClassicTrackballMover.js,v $
//   $Author: christian $
//   $Revision: 1.7 $
//   $Date: 2005/04/20 16:49:06 $
//
//=============================================================================

/*jslint nomen:false*/
/*globals use, MoverBase, Vector3f, BUTTON_UP, Point3f, print, sum, Ray,
          product, Matrix4f, difference, magnitude, pickBody, Quaternionf,
          normalized, window, nearestIntersection, PI_2
          BUTTON_DOWN, LEFT_BUTTON*/

// Possible improvements:
// - Use swept sphere for trackball center detection in stead of stick
// - Adapt fly/zoom/pan-speed to height above ground

use("MoverBase.js");
use("picking_functions.js");
use("intersection_functions.js");

function ClassicTrackballMover(theViewport) {
    this.Constructor(this, theViewport);
}

function CenteredTrackballMover(theViewport) {
    ClassicTrackballMover.prototype.Constructor(this, theViewport, true);
    this.name = "CenteredTrackballMover";
}

ClassicTrackballMover.prototype.Constructor = function (obj, theViewport, theCenteredFlag) {
    MoverBase.prototype.Constructor(obj, theViewport);
    obj.Mover = [];

    var PAN_SPEED           = 1;
    var ZOOM_SPEED          = 1;
    var ROTATE_SPEED        = 4;
    var MAX_DISTANCE_FACTOR = 10.0;

    //////////////////////////////////////////////////////////////////////

    theCenteredFlag = theCenteredFlag || false;

    var _myTrackballBody        = null;
    var _myTrackballOrientation = new Vector3f(0, 0, 0);
    var _myTrackBallCenter      = new Point3f(0, 0, 0);

    var _myMousePosX = 0;
    var _myMousePosY = 0;
    var _myCenteredFlag = theCenteredFlag;
    var _myFixedCenter  = false;
    //var _myPowerMateButtonState = BUTTON_UP;
    var _myPickInvisibleBodies = true;

    var _myZoomSpeed = ZOOM_SPEED;
    var _myRotateSpeed = ROTATE_SPEED;
    
    /////////////////////
    // Private Methods //
    /////////////////////

    function getDistanceDependentFactor() {
        var d = Math.min(MAX_DISTANCE_FACTOR,
                magnitude(difference(window.camera.globalmatrix.getTranslation(), _myTrackBallCenter)) / obj.getWorldSize());
        return d;
    }

    // Returns center in global coordinates
    function getTrackballCenter() {
        var myPos = null;
        if (_myCenteredFlag) {
            myPos = new Point3f(0, 0, 0);
        }
        else if (_myFixedCenter) {
            myPos = _myTrackBallCenter;
        }
        else if (_myTrackballBody) {
            myPos = _myTrackballBody.boundingbox.center;
        } else {
            var myViewVector = product(obj.getMoverObject().globalmatrix.getRow(2).xyz, -1);
            var myPosition   = obj.getMoverObject().globalmatrix.getTranslation();
            var myCenterRay  = new Ray(myPosition, myViewVector);
            var myTrackballRadius = 1;
            var myIntersection = nearestIntersection(obj.getWorld(), myCenterRay);
            if (myIntersection) {
                myTrackballRadius = myIntersection.distance;
            }
            myPos =  sum(myPosition, product(myViewVector, myTrackballRadius));
        }

        return myPos;
    }

    function calculateTrackball() {
        var myTrackballRadius = magnitude(difference(obj.getMoverObject().globalmatrix.getTranslation(), _myTrackBallCenter));
        var myX = myTrackballRadius * Math.cos(-_myTrackballOrientation.x) * Math.sin(_myTrackballOrientation.y);
        var myY = myTrackballRadius * Math.sin(-_myTrackballOrientation.x);
        var myZ = myTrackballRadius * Math.cos(-_myTrackballOrientation.x) * Math.cos(_myTrackballOrientation.y);
        var myGlobalMatrix = new Matrix4f(Quaternionf.createFromEuler(_myTrackballOrientation));

        myGlobalMatrix.translate(sum(_myTrackBallCenter, new Vector3f(myX, myY, myZ)));
        var myParentMatrix = new Matrix4f(obj.getMoverObject().parentNode.globalmatrix);
        myParentMatrix.invert();
        myGlobalMatrix.postMultiply(myParentMatrix);

        var myDecomposition = myGlobalMatrix.decompose();
        obj.getMoverObject().orientation = myDecomposition.orientation;
        obj.getMoverObject().position = myDecomposition.position;
    }

    function setupTrackball(theBody) {
        if (theBody) {
            _myTrackballBody = theBody;
        } else {
            _myTrackballBody = null;
        }

        _myTrackBallCenter = getTrackballCenter();
        var myGlobalPosition = obj.getMoverObject().globalmatrix.getTranslation();
        //var myRotation = obj.getMoverObject().globalmatrix.getRotation();
        var myRadiusVector = normalized(difference(myGlobalPosition, _myTrackBallCenter));

        if (obj.getMoverObject().globalmatrix.getRow(1).y > 0) {
            _myTrackballOrientation.x = - Math.asin(myRadiusVector.y);
            _myTrackballOrientation.y = Math.atan2(myRadiusVector.x, myRadiusVector.z);
        } else {
            _myTrackballOrientation.x = Math.PI + Math.asin(myRadiusVector.y);
            _myTrackballOrientation.y = Math.atan2(myRadiusVector.x, myRadiusVector.z) - Math.PI;
        }

        calculateTrackball();
    }

    function pickBody(theX, theY) {
        // TODO: This is not portrait orientation aware.
        // Implement function:
        // window.screenToWorldSpace(theX, theY, NEAR_PLANE);
        var myPickedBody = null;

        var myViewport = obj.getViewport();
        var myPosX = 2 * (theX - myViewport.left) / myViewport.width  - 1;
        var myPosY = - (2 * (theY - myViewport.top) / myViewport.height - 1);
        var myClipNearPos = new Point3f(myPosX, myPosY, -1);
        var myClipFarPos = new Point3f(myPosX, myPosY, +1);

        var myCamera = obj.getViewportCamera();
        var myProjectionMatrix = myCamera.frustum.projectionmatrix;
        myProjectionMatrix.invert();
        myProjectionMatrix.postMultiply(myCamera.globalmatrix);

        var myWorldNearPos = product(myClipNearPos, myProjectionMatrix);
        var myWorldFarPos = product(myClipFarPos, myProjectionMatrix);
        //var myCameraPos = new Point3f(myCamera.globalmatrix.getTranslation());
        var myMouseRay = new Ray(myWorldNearPos, myWorldFarPos);
        var myIntersection = nearestIntersection(obj.getWorld(), myMouseRay, _myPickInvisibleBodies);
        if (myIntersection) {
            myPickedBody = myIntersection.info.body;
        } else {
            myPickedBody = window.scene.world;
        }
        print("  -> You picked trackball object: " + myPickedBody.name + " id: " + myPickedBody.id);
        return myPickedBody;
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////

    obj.name = "ClassicTrackballMover";

    obj.setup = function () {
        setupTrackball(null);
    };

    obj.__defineSetter__("zoomspeed", function (theZoomSpeed) {
        _myZoomSpeed = theZoomSpeed;
    }); 

    obj.__defineGetter__("zoomspeed", function () {
        return _myZoomSpeed;
    });

    obj.__defineSetter__("rotatespeed", function (theRotateSpeed) {
        _myRotateSpeed = theRotateSpeed;
    });

    obj.__defineGetter__("rotatespeed", function () {
        return _myRotateSpeed;
    });

    obj.Mover.onMouseButton = obj.onMouseButton;
    obj.onMouseButton = function (theButton, theState, theX, theY) {
        obj.Mover.onMouseButton(theButton, theState, theX, theY);
        if (theButton === LEFT_BUTTON && theState === !!BUTTON_DOWN) {
            if (obj.getDoubleLeftButtonFlag()) {
                print("####################################");
                var myPickedBody = pickBody(theX, theY);
                setupTrackball(myPickedBody);
            }
            _myTrackBallCenter = getTrackballCenter();
        }
    };
    
    obj.__defineSetter__("pickInvisible", function (theFlag) {
        _myPickInvisibleBodies = theFlag;
    });

    obj.setCentered = function (theFlag) {
        _myCenteredFlag = theFlag;
        setupTrackball(null);
    };

    obj.setTrackedBody = function (theBody) {
        setupTrackball(theBody);
    };
    
    obj.getTrackedBody = function () {
        return _myTrackballBody;
    };

    obj.setTrackballCenter = function (theCenter) {
        _myFixedCenter = true;
        _myTrackBallCenter = theCenter;
        setupTrackball(null);
    };

    obj.rotate = function (theDeltaX, theDeltaY) {
        _myTrackballOrientation.x += theDeltaY * PI_2 * _myRotateSpeed;
        _myTrackballOrientation.y -= theDeltaX * PI_2 * _myRotateSpeed;
        calculateTrackball();
    };

    obj.zoom = function (theDelta) {
        var myZoomFactor =  getDistanceDependentFactor();
        var myWorldTranslation = new Vector3f(0, 0, theDelta * obj.getWorldSize() * myZoomFactor / _myZoomSpeed);
        obj.update(myWorldTranslation, 0);
    };

    obj.pan = function (theDeltaX, theDeltaY) {
        var myPanFactor =  getDistanceDependentFactor();
        var myWorldSize = obj.getWorldSize();
        var myWorldTranslation = new Vector3f(0, 0, 0);
        myWorldTranslation.x = - theDeltaX * myWorldSize * myPanFactor / PAN_SPEED;
        myWorldTranslation.y = - theDeltaY * myWorldSize * myPanFactor / PAN_SPEED;
        _myTrackballBody = null;
        obj.update(myWorldTranslation, 0);
    };

    obj.onMouseMotion = function (theX, theY) {
        var myDeltaX = (theX - _myMousePosX) / window.width;
        var myDeltaY = - (theY - _myMousePosY) / window.height; // flip Y

        if (obj.getRightButtonFlag()) {
            obj.zoom(-myDeltaY);
        } else if (obj.getMiddleButtonFlag()) {
            obj.pan(myDeltaX, myDeltaY);
        } else if (obj.getLeftButtonFlag()) {
            obj.rotate(myDeltaX, myDeltaY);
        }

        _myMousePosX = theX;
        _myMousePosY = theY;
    };

    obj.onAxis = function (theDevice, theAxis, theValue) {
        /*if( _myPowerMateButtonState == BUTTON_UP) {
          _myTrackballOrientation.y += theValue / TWO_PI;
          calculateTrackball();
          } else if( _myPowerMateButtonState == BUTTON_DOWN ) {
          if( theValue != 0 ) {
          obj.zoom(theValue / TWO_PI);
          }
          }*/
    };

    obj.onButton = function (theDevice, theButton, theState) {
        /*switch(theState) {
          case BUTTON_DOWN:
          _myPowerMateButtonState = BUTTON_DOWN;
          break;
          case BUTTON_UP:
          _myPowerMateButtonState = BUTTON_UP;
          break;
          default:
          break;
          }*/
    };

};