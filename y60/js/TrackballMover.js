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

    const MAX_DISTANCE_FACTOR = 10.0;
    const MIN_DISTANCE_FACTOR = 0.1;

    //////////////////////////////////////////////////////////////////////

    var _myTrackball        = new Trackball();
    var _myTrackballCenter  = new Point3f(0,0,0);
    var _prevMousePosition  = new Vector3f(0,0,0);

    //////////////////////////////////////////////////////////////////////
    //
    // public
    //
    //////////////////////////////////////////////////////////////////////

    obj.name = "TrackballMover";
    
    obj.setup = function() {
        var myTrackballBody = obj.getMoverObject().parentNode;
        obj.selectBody(myTrackballBody);
    };

    // XXX TODO: the Eventhandling should not be inside the Mover
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
                    obj.selectBody(myTrackedBody);
                    applyRotation();
                }
                _prevMousePosition = new Vector3f(theX, theY, 0);
            } else { // Button Up
                _prevMousePosition = new Vector3f(0,0,0);
            }
        }
    };

    // XXX TODO: the Eventhandling should not be inside the Mover
    obj.onMouseMotion = function(theX, theY) {
        if (obj.getRightButtonFlag()) {
            obj.movements.zoomByScreenCoordinates(_prevMousePosition, new Vector3f(theX, theY, 0));
        } else if (obj.getMiddleButtonFlag()) {
            obj.movements.panByScreenCoordinates(_prevMousePosition, new Vector3f(theX, theY, 0));
        } else if (obj.getLeftButtonFlag()) {
            obj.movements.rotateByScreenCoordinates(_prevMousePosition, new Vector3f(theX, theY, 0));
        }
        _prevMousePosition = new Vector3f(theX, theY, 0);
    };

    obj.movements.rotateByScreenCoordinates = function(thePrevMousePos, theCurMousePos) {
        var prevNormalizedMousePos = obj.getNormalizedScreen(thePrevMousePos[0], thePrevMousePos[1]);
        var curNormalizedMousePos = obj.getNormalizedScreen(theCurMousePos[0], theCurMousePos[1]);
        _myTrackball.rotate(prevNormalizedMousePos, curNormalizedMousePos);
        applyRotation();
    };

    obj.movements.rotateByQuaternion = function( theQuaternion ) {
        _myTrackball.setQuaternion( product( theQuaternion, _myTrackball.getQuaternion()));
        applyRotation();
    };
    
    obj.movements.zoomByScreenCoordinates = function(thePrevMousePos, theCurMousePos) {
        var prevNormalizedMousePos = obj.getNormalizedScreen(thePrevMousePos[0], thePrevMousePos[1]);
        var curNormalizedMousePos = obj.getNormalizedScreen(theCurMousePos[0], theCurMousePos[1]);
        var myDelta = difference(curNormalizedMousePos, prevNormalizedMousePos);
        obj.movements.zoom(myDelta[1]);
    };

    obj.movements.zoom = function(theDelta) {
        var myWorldSize = obj.getWorldSize();

        // scale the zoom by distance from camera to picked object
        var myZoomFactor =  getDistanceDependentFactor();
        var myScreenTranslation = new Vector3f(0, 0, -theDelta * myWorldSize * myZoomFactor);

        obj.update(myScreenTranslation, 0);
    };

    obj.movements.panByScreenCoordinates = function(thePrevMousePos, theCurMousePos) {
        var prevNormalizedMousePos = obj.getNormalizedScreen(thePrevMousePos[0], thePrevMousePos[1]);
        var curNormalizedMousePos = obj.getNormalizedScreen(theCurMousePos[0], theCurMousePos[1]);
        var myDelta = difference(curNormalizedMousePos, prevNormalizedMousePos);
        obj.movements.pan(myDelta[0], myDelta[1]);
    };

    obj.movements.pan = function(theDeltaX, theDeltaY) {
        var myScreenTranslation;
        var myCamera = obj.getViewportCamera();
        if(myCamera.frustum.hfov == 0) {
            // ortho camera
            var myOrthoHeight = (obj.getViewport().height / obj.getViewport().width  ) * myCamera.width;
            myScreenTranslation = new Vector3f( myCamera.width * theDeltaX,
                                                myOrthoHeight * theDeltaY,
                                                0);
        } else {
            // divide by two to compensate for range [-1,1] => [0,1]
            var myPanFactor =  getDistanceDependentFactor() / 2;
            var myWorldSize = obj.getWorldSize();
            var myScreenTranslation = new Vector3f(0, 0, 0);
            // negate to move camera (not object)
            myScreenTranslation.x = -theDeltaX * myWorldSize * myPanFactor;
            myScreenTranslation.y = -theDeltaY * myWorldSize * myPanFactor;
        }
        obj.update(myScreenTranslation, 0);
        //Logger.warning("Pan:"+obj.getScreenPanVector());
    };

    obj.selectBody = function(theBody) {
        _myTrackballCenter = getTrackballCenterFromBody(theBody);
        setupTrackball();
    };

    obj.setTrackballCenter = function(theCenter) {
        _myTrackballCenter = theCenter;
        setupTrackball();
    };

    obj.getOrientation = function() {
        return _myTrackball.getQuaternion();
    };

    obj.setOrientation = function(theOrientation) {
        _myTrackball.setQuaternion(theOrientation);
        applyRotation();
    };

    obj.getScreenPanVector = function() {
        var myRightVector = obj.getMoverObject().globalmatrix.getRow(0).xyz;
        var myUpVector = obj.getMoverObject().globalmatrix.getRow(1).xyz;
        var myMoverObjectWorldPos = obj.getMoverObject().globalmatrix.getRow(3).xyz;

        var myObjectToCenter = difference(myMoverObjectWorldPos, _myTrackballCenter);
        var myPanVector = new Vector3f(dot(myObjectToCenter, myRightVector), dot(myObjectToCenter, myUpVector), 0);
        return myPanVector;
    };

    obj.setTrackballRadius = function( theRadius ) {
        _myTrackball.setSphereRadius( theRadius );
    };

    obj.set = function(theOrientation, theRadius, thePanVector) {
        var myNewMatrix = new Matrix4f();
        myNewMatrix.makeIdentity();
        myNewMatrix.translate(new Vector3f(0,0,theRadius));
        myNewMatrix.postMultiply(new Matrix4f(theOrientation));
        myNewMatrix.translate(_myTrackballCenter);

        var parentMatrixInv = obj.getMoverObject().parentNode.inverseglobalmatrix;
        myNewMatrix.postMultiply(parentMatrixInv);
        // now set the move object's values
        var myDecomposition = myNewMatrix.decompose();

        obj.getMoverObject().position = myDecomposition.position;
        myDecomposition.orientation.normalize();
        obj.getMoverObject().orientation = myDecomposition.orientation;
        _myTrackball.setQuaternion(myDecomposition.orientation);
        obj.getMoverObject().shear = myDecomposition.shear;
        obj.getMoverObject().scale = myDecomposition.scale;
        obj.update(thePanVector, 0);
    };
    
    
    //////////////////////////////////////////////////////////////////////
    //
    // private
    //
    //////////////////////////////////////////////////////////////////////

    function getDistanceDependentFactor() {
        var toObject = difference(obj.getMoverObject().position, _myTrackballCenter);
        var myDistanceFactor =  clamp(magnitude(toObject) / obj.getWorldSize(),
                                MIN_DISTANCE_FACTOR, MAX_DISTANCE_FACTOR);
        return myDistanceFactor;
    }

    function setupTrackball() {
        var myDecomposition = obj.getMoverObject().globalmatrix.decompose();
        _myTrackball.setQuaternion(myDecomposition.orientation);
    }

    function getTrackballCenterFromBody(theBody) {
        if (theBody) {
            if (theBody.boundingbox.isEmpty) {
                return theBody.globalmatrix.getRow(3).xyz;
            } else {
                return theBody.boundingbox.center;
            }
        } else {
            var myViewVector = product(obj.getMoverObject().globalmatrix.getRow(2).xyz, -1);
            var myPosition   = obj.getMoverObject().globalmatrix.getTranslation();
            var myCenterRay  = new Ray(myPosition, myViewVector);

            var myTrackballRadius = 1.0; //_myTrackball.getSphereRadius();
            var myIntersection = nearestIntersection(obj.getWorld(), myCenterRay);
            if (myIntersection) {
                myTrackballRadius = myIntersection.distance;
            }
            return product(myViewVector, myTrackballRadius);
        }
    }

    function applyRotation() {
        var myPanVector = obj.getScreenPanVector();
        var myWorldTranslation = obj.rotateWithObject(myPanVector).xyz;
        var myMoverObjectWorldPos = obj.getMoverObject().globalmatrix.getRow(3).xyz;
        var myDistance = distance(difference(myMoverObjectWorldPos, myWorldTranslation), _myTrackballCenter);
        
        obj.set(_myTrackball.getQuaternion(), myDistance, myPanVector);
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
        var myCamera = obj.getViewportCamera();
        var myProjectionMatrix = myCamera.frustum.projectionmatrix;
        myProjectionMatrix.invert();
        myProjectionMatrix.postMultiply(myCamera.globalmatrix);

        var myWorldNearPos = product(myClipNearPos, myProjectionMatrix);
        var myWorldFarPos = product(myClipFarPos, myProjectionMatrix);
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
};
