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
//   $RCSfile: ClassicTrackballMover.js,v $
//   $Author: christian $
//   $Revision: 1.7 $
//   $Date: 2005/04/20 16:49:06 $
//
//=============================================================================

// Possible improvements:
// - Use swept sphere for trackball center detection in stead of stick
// - Adapt fly/zoom/pan-speed to height above ground

use("MoverBase.js");
use("picking_functions.js");
use("intersection_functions.js");

function ClassicTrackballMover(theViewport) {
    this.Constructor(this, theViewport);
}

ClassicTrackballMover.prototype.Constructor = function(obj, theViewport) {
    MoverBase.prototype.Constructor(obj, theViewport);
    obj.Mover = [];

    const PAN_SPEED           = 1;
    const ZOOM_SPEED          = 1;
    const MAX_DISTANCE_FACTOR = 10.0;

    //////////////////////////////////////////////////////////////////////

    var _myTrackballBody        = null;
    var _myTrackballOrientation = new Vector3f(0,0,0);
    var _myTrackballCenter      = new Point3f(0,0,0);

    var _myMousePosX = 0;
    var _myMousePosY = 0;
    var _myCenteredFlag         = 0;

    //////////////////////////////////////////////////////////////////////
    //
    // public
    //
    //////////////////////////////////////////////////////////////////////

    obj.name = "ClassicTrackballMover";

    obj.setup = function() {
        var myTrackballBody = obj.getMoverObject().parentNode;
        if (myTrackballBody.nodeName == "world") {
            setupTrackball(null);
        } else {
            setupTrackball(myTrackballBody);
        }
    }

    obj.Mover.onMouseButton = obj.onMouseButton;
    obj.onMouseButton = function(theButton, theState, theX, theY) {
        obj.Mover.onMouseButton(theButton, theState, theX, theY);
        if (theButton == LEFT_BUTTON && theState == BUTTON_DOWN) {
            if (obj.getDoubleLeftButtonFlag()) {
                var myTrackedBody = obj.getMoverObject().parentNode;

                var myPickedBody = pickBody(theX, theY);
                if (myPickedBody) {
                    myTrackedBody = myPickedBody;
                }
                setupTrackball(myTrackedBody);
            }
            _myTrackballCenter = getTrackballCenter();
        }
    }

    obj.setCentered = function(theFlag) {
        _myCenteredFlag = theFlag;
        setupTrackball(null);
    }

    obj.setTrackedBody = function(theBody) {
        setupTrackball(theBody);
    }

    obj.rotate = function(theDeltaX, theDeltaY) {
        _myTrackballOrientation.x += theDeltaY * TWO_PI;
        _myTrackballOrientation.y -= theDeltaX * TWO_PI;
        calculateTrackball();
    }

    obj.zoom = function(theDelta) {
        var myZoomFactor =  getDistanceDependentFactor();
        var myWorldTranslation = new Vector3f(0, 0, theDelta * obj.getWorldSize() * myZoomFactor / ZOOM_SPEED);
        obj.update(myWorldTranslation, 0);
    }

    obj.pan = function(theDeltaX, theDeltaY) {
        var myPanFactor =  getDistanceDependentFactor();
        var myWorldSize = obj.getWorldSize();
        var myWorldTranslation = new Vector3f(0, 0, 0);
        myWorldTranslation.x = - theDeltaX * myWorldSize * myPanFactor / PAN_SPEED;
        myWorldTranslation.y = - theDeltaY * myWorldSize * myPanFactor / PAN_SPEED;
        _myTrackballBody = null;
        obj.update(myWorldTranslation, 0);
    }

    obj.onMouseMotion = function(theX, theY) {
        var myDeltaX = (theX-_myMousePosX) / window.width;
        var myDeltaY = - (theY-_myMousePosY) / window.height; // flip Y

        if (obj.getRightButtonFlag()) {
            obj.zoom(-myDeltaY);
        } else if (obj.getMiddleButtonFlag()) {
            obj.pan(myDeltaX, myDeltaY);
        } else if (obj.getLeftButtonFlag()) {
            obj.rotate(myDeltaX, myDeltaY);
        }

        _myMousePosX = theX;
        _myMousePosY = theY;
    }

    //////////////////////////////////////////////////////////////////////
    //
    // private
    //
    //////////////////////////////////////////////////////////////////////

    function getDistanceDependentFactor(){
        var d = Math.min(MAX_DISTANCE_FACTOR,
                magnitude(difference(window.camera.position, _myTrackballCenter)) / obj.getWorldSize());
        return d;
    }

    function setupTrackball(theBody) {
        if (theBody) {
            _myTrackballBody = theBody;
        } else {
            _myTrackballBody = null;
        }

        _myTrackballCenter = getTrackballCenter();

        var myTrackballRadius = magnitude(difference(obj.getMoverObject().position, _myTrackballCenter));
        var myPosition  = obj.getMoverObject().position;
        if (myTrackballRadius > 0) {
            var myPosition = product(difference(obj.getMoverObject().position, _myTrackballCenter), 1 / myTrackballRadius);
        }

        var myLocalMatrix = obj.getMoverObject().localmatrix;
        if (myLocalMatrix.getRow(1).y > 0) {
            _myTrackballOrientation.x = - Math.asin(myPosition.y);
            _myTrackballOrientation.y = Math.atan2(myPosition.x, myPosition.z);
        } else {
            _myTrackballOrientation.x = Math.PI + Math.asin(myPosition.y);
            _myTrackballOrientation.y = Math.atan2(myPosition.x, myPosition.z) - Math.PI;
        }

        calculateTrackball();
    }

    function calculateTrackball() {
        var myTrackballRadius = magnitude(difference(obj.getMoverObject().position, _myTrackballCenter));
        var myX = myTrackballRadius * Math.cos(- _myTrackballOrientation.x) * Math.sin(_myTrackballOrientation.y);
        var myY = myTrackballRadius * Math.sin(- _myTrackballOrientation.x);
        var myZ = myTrackballRadius * Math.cos(- _myTrackballOrientation.x) * Math.cos(_myTrackballOrientation.y);
        obj.getMoverObject().position    = sum(_myTrackballCenter, new Vector3f(myX, myY, myZ));
        obj.getMoverObject().orientation = Quaternionf.createFromEuler(_myTrackballOrientation);
    }

    function getTrackballCenter() {
        if (_myCenteredFlag) {
            return new Point3f(0,0,0);
        }

        if (_myTrackballBody) {
            return difference(_myTrackballBody.boundingbox.center, obj.getMoverObject().parentNode.globalmatrix.getTranslation());
        } else {
            var myViewVector = product(obj.getMoverObject().globalmatrix.getRow(2).xyz, -1);
            var myPosition   = obj.getMoverObject().globalmatrix.getTranslation();
            var myCenterRay  = new Ray(myPosition, myViewVector);
            var myTrackballRadius = 1;
            var myIntersection = nearestIntersection(obj.getWorld(), myCenterRay);
            if (myIntersection) {
                myTrackballRadius = myIntersection.distance;
            }
            return sum(myPosition, product(myViewVector, myTrackballRadius));
        }
    }

    function pickBody(theX, theY) {
        // TODO: This is not portrait orientation aware.
        // Implement function:
        // window.screenToWorldSpace(theX, theY, NEAR_PLANE);
        var myViewport = obj.getViewport();
        var myPosX = 2 * (theX-myViewport.left) / myViewport.width  - 1;
        var myPosY = - (2 * (theY-myViewport.top) / myViewport.height - 1);
        var myClipNearPos = new Point3f(myPosX, myPosY, -1);
        var myClipFarPos = new Point3f(myPosX, myPosY, +1);

        var myProjectionMatrix = myViewport.projectionmatrix;
        myProjectionMatrix.invert();
        var myCamera = obj.getViewportCamera();
        myProjectionMatrix.postMultiply(myCamera.globalmatrix);

    	var myWorldNearPos = product(myClipNearPos, myProjectionMatrix);
    	var myWorldFarPos = product(myClipFarPos, myProjectionMatrix);
    	var myCameraPos = new Point3f(myCamera.globalmatrix.getTranslation());
        var myMouseRay = new Ray(myWorldNearPos, myWorldFarPos);
        var myIntersection = nearestIntersection(obj.getWorld(), myMouseRay);
        if (myIntersection) {
            //print("  -> You picked trackball object: " + myIntersection.info.body.name);
            return myIntersection.info.body;
        } else {
            return obj.getMoverObject().parentNode;
        }
    }
}
