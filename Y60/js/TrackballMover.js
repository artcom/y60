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
//   $RCSfile: TrackballMover.js,v $
//   $Author: christian $
//   $Revision: 1.32 $
//   $Date: 2005/04/20 16:49:06 $
//
//=============================================================================

// Possible improvements:
// - Use swept sphere for trackball center detection in stead of stick
// - Adapt fly/zoom/pan-speed to height above ground

use("Trackball.js");
use("MoverBase.js");
use("picking_functions.js");
use("intersection_functions.js");

function TrackballMover(theViewport) {
    this.Constructor(this, theViewport);
}

TrackballMover.prototype.Constructor = function(obj, theViewport) {
    MoverBase.prototype.Constructor(obj, theViewport);
    obj.Mover = [];


    const PAN_SPEED           = 1;
    const ZOOM_SPEED          = 1;
    const MAX_DISTANCE_FACTOR = 10.0;
    const MIN_DISTANCE_FACTOR = 0.1;

    //////////////////////////////////////////////////////////////////////

    var _myTrackballBody        = null;
    var _myTrackball            = new Trackball();
    var _myTrackballCenter      = new Point3f(0,0,0);

    var _prevNormalizedMousePosition = new Vector3f(0,0,0); // [-1..1]

    //////////////////////////////////////////////////////////////////////
    //
    // public
    //
    //////////////////////////////////////////////////////////////////////

    obj.name = "TrackballMover";

    obj.setup = function() {
        var myTrackballBody = obj.getMoverObject().parentNode;
        if (myTrackballBody.nodeName == "world") {
            setupTrackball(null);
        } else {
            setupTrackball(myTrackballBody);
            // applyRotation();
        }
    }

    obj.Mover.onMouseButton = obj.onMouseButton;
    obj.onMouseButton = function(theButton, theState, theX, theY) {
        obj.Mover.onMouseButton(theButton, theState, theX, theY);
        if (theButton == LEFT_BUTTON) {
            if (theState == BUTTON_DOWN) {
                if (obj.getDoubleLeftButtonFlag()) {
                    var myTrackedBody = obj.getMoverObject().parentNode;

                    if (obj.getMoverObject().parentNode.nodeName == "world") {
                        var myPickedBody = pickBody(theX, theY);
                        if (myPickedBody) {
                            myTrackedBody = myPickedBody;
                        }
                    } else {
                        print("  -> Trackball object picking only works with top-level cameras");
                    }
                    setupTrackball(myTrackedBody);
                    applyRotation();
                }
                _prevNormalizedMousePosition = obj.getNormalizedScreen(theX, theY);
            } else { // Button Up
                _prevNormalizedMousePosition = new Vector3f(0,0,0);
            }
        }
    }

    obj.rotate = function(thePrevMousePos, theCurMousePos) {
        _myTrackball.rotate(thePrevMousePos, theCurMousePos);
        applyRotation();
    }

    obj.zoom = function(theDelta) {
        var myWorldSize = obj.getWorldSize();

        // scale the zoom by distance from camera to picked object
        var myZoomFactor =  getDistanceDependentFactor();
        var myWorldTranslation = new Vector3f(0, 0, -theDelta * myWorldSize * myZoomFactor / ZOOM_SPEED);

        obj.update(myWorldTranslation, 0);
    }

    obj.pan = function(theDeltaX, theDeltaY) {
        if (1 || obj.getMoverObject().parentNode.nodeName == "world") {
            var myWorldTranslation;
            var myCamera = obj.getViewportCamera();
            if(myCamera.hfov == 0) {
                // ortho camera
                var myOrthoHeight = (obj.getViewport().height / obj.getViewport().width  ) * myCamera.width;
                myWorldTranslation = new Vector3f(myCamera.width * theDeltaX,
                                                  myOrthoHeight * theDeltaY,
                                                  0);
            } else {
                // divide by two to compensate for range [-1,1] => [0,1]
                var myPanFactor =  getDistanceDependentFactor() /2 ;
                var myWorldSize = obj.getWorldSize();
                var myWorldTranslation = new Vector3f(0, 0, 0);
                // negate to move camera (not object)
                myWorldTranslation.x = -theDeltaX * myWorldSize * myPanFactor / PAN_SPEED;
                myWorldTranslation.y = -theDeltaY * myWorldSize * myPanFactor / PAN_SPEED;
                _myTrackballBody = null;
            }
            obj.update(myWorldTranslation, 0);
        }
    }

    obj.onMouseMotion = function(theX, theY) {
        var curNormalizedMousePos = obj.getNormalizedScreen(theX, theY);
        var myDelta = difference(curNormalizedMousePos, _prevNormalizedMousePosition);
        if (obj.getRightButtonFlag()) {
            obj.zoom(myDelta[1]);
        } else if (obj.getMiddleButtonFlag()) {
            obj.pan(myDelta[0], myDelta[1]);
        } else if (obj.getLeftButtonFlag()) {
            obj.rotate(_prevNormalizedMousePosition, curNormalizedMousePos);
        }
        _prevNormalizedMousePosition = curNormalizedMousePos;
    }

    obj.selectBody = function(theBody) {
        setupTrackball(theBody);
    }

    //////////////////////////////////////////////////////////////////////
    //
    // private
    //
    //////////////////////////////////////////////////////////////////////

    function getDistanceDependentFactor(){
        var toObject = difference(obj.getMoverObject().position, _myTrackballCenter);
        var myDistanceFactor =  clamp(magnitude(toObject) / obj.getWorldSize(),
                                MIN_DISTANCE_FACTOR, MAX_DISTANCE_FACTOR);
        return myDistanceFactor;
    }

    function setupTrackball(theBody) {
        if (theBody) {
            _myTrackballBody = theBody;
        } else {
            _myTrackballBody = null;
        }
        _myTrackballCenter = getTrackballCenter();

        var myDecomposition = obj.getMoverObject().globalmatrix.decompose();
        _myTrackball.setQuaternion(myDecomposition.orientation);
    }

    function getTrackballCenter() {
        if (_myTrackballBody) {
            return _myTrackballBody.boundingbox.center;
        } else {
            var myViewVector = product(obj.getMoverObject().globalmatrix.getRow(2).xyz, -1);
            var myPosition   = obj.getMoverObject().globalmatrix.getTranslation();
            var myCenterRay  = new Ray(myPosition, myViewVector);

            var myTrackballRadius = 1;
            var myIntersection = nearestIntersection(obj.getWorld(), myCenterRay);
            if (myIntersection) {
            	myTrackballRadius = myIntersection.distance;
            }
            return product(myViewVector, myTrackballRadius);
        }
    }

    function applyRotation() {
        var myNewMatrix = new Matrix4f();
        myNewMatrix.makeIdentity();

        var myDistance = distance(obj.getMoverObject().position, _myTrackballCenter);
        var deltaRotationMatrix = new Matrix4f(_myTrackball.getQuaternion());
        myNewMatrix.translate(new Vector3f(0,0,myDistance));
        myNewMatrix.postMultiply(deltaRotationMatrix);
        myNewMatrix.translate(_myTrackballCenter);

        // now set the move object's values
        var myDecomposition = myNewMatrix.decompose();

        obj.getMoverObject().position = myDecomposition.position;
        myDecomposition.orientation.normalize();
        obj.getMoverObject().orientation = myDecomposition.orientation;
        obj.getMoverObject().shear = myDecomposition.shear;
        obj.getMoverObject().scale = myDecomposition.scale;
    }

    function pickBody(theX, theY) {
        if (obj.getMoverObject().parentNode.nodeName != "world") {
            print("  -> Trackball object picking only works with top-level cameras");
            return obj.getMoverObject().parentNode;
        }

        // TODO: This is not portrait orientation aware.
        // Implement function:
        // window.screenToWorldSpace(theX, theY, NEAR_PLANE);
        // normalize to [-1..1]
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

        var myWorld = obj.getWorld();
    	var myIntersection = nearestIntersection(myWorld, myMouseRay);
    	if (myIntersection) {
    	    print("  -> You picked trackball object: " + myIntersection.info.body.name);
    	    return myIntersection.info.body;
    	} else {
    	    return obj.getMoverObject().parentNode;
    	}
    }
}
