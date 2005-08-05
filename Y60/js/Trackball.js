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
//   $RCSfile: Trackball.js,v $
//   $Author: david $
//   $Revision: 1.1 $
//   $Date: 2005/04/04 14:40:15 $
//
//=============================================================================

function Trackball() {
    this.Constructor(this);
}

Trackball.prototype.Constructor = function(obj) {
    const SPHERE_RADIUS       = 1.0;   // sphere for quat-based rotation (1=screen-width)
    var _curQuat                = new Quaternionf(0,0,0,1);

    obj.rotate = function(thePrevMousePos, theCurMousePos) {
        var prevVector = projectOnSphere(thePrevMousePos);
        var curVector = projectOnSphere(theCurMousePos);
        var moveRotation = getRotationFromMove(prevVector,curVector);
        
        _curQuat = product(moveRotation, _curQuat);
    }

    obj.getQuaternion = function() {
        return new Quaternionf(_curQuat);
    }
    obj.setQuaternion = function(theNewValue) {
        _curQuat = theNewValue;
    }

    function projectOnSphere(theVector) {
        var rsqr = SPHERE_RADIUS * SPHERE_RADIUS;
        var dsqr = theVector.x * theVector.x + theVector.y * theVector.y;
        var myProjectedVector = new Vector3f(theVector.x, theVector.y, 0);
        // if relatively "inside" sphere project to sphere else on hyperbolic sheet
        if(dsqr<(rsqr*0.5))	{ 
            myProjectedVector.z = Math.sqrt(rsqr-dsqr);
        } else {
            myProjectedVector.z = rsqr/(2*Math.sqrt(dsqr));
        }
        return myProjectedVector;
    }
    
	function getRotationFromMove ( theVector0, theVector1 ) {
        // calculate axis of rotation and correct it to avoid "near zero length" rot axis
		var myRotAxis = cross(theVector1, theVector0);
        if ( magnitude(myRotAxis) < 0.000000001 ) {
            myRotAxis = new Vector3f(1,0,0);
        }
        // find the amount of rotation
		var d = difference(theVector1, theVector0);
		var t = magnitude(d) /(2.0*SPHERE_RADIUS);
		t = clamp(t,-1.0,1.0);
		var phi = 2.0 * Math.asin(t);
		return new Quaternionf(myRotAxis, phi); 
	}
}


