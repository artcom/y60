//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/**
 * @fileoverview JavaScript BSpline class with ease in and out
 * @author Christian Hardenberg
 */

use("Exception.js");

/**
 * Constructs a new BSpline object
 *   @param {Vector3f} theStart       Startpunkt
 *   @param {Vector3f} theStartHandle Anfasser-Startpunkt
 *   @param {Vector3f} theEnd         Endpunkt
 *   @param {Vector3f} theEndHandle   Anfasser-Endpunkt
 **/
function BSpline(theStart, theStartHandle, theEnd, theEndHandle) {
    /**
     * Gets the point array of the BSpline, call calculate to create the point array
     * @return {ArrayOfPoint3f} The point array
     */
    this.getResult = function() {
        return _myResult;
    }

    /**
     *   Samples the spline at theResolution points. caches result
     *   @param {integer}  theResolution  Anzahl der schritte, von anfang bis ende
     *   @param {float}    theEaseIn      Wert zwischen 0 und 1. 1 für maximales theEaseIn
     *   @param {float}    theEaseOut     Wert zwischen 0 und 1. 1 für maximales theEaseOut
     *   @return {ArrayOfPoint3f} The point array
     **/
    this.calculate = function(theResolution, theEaseIn, theEaseOut) {
        if (_myPolyCoefs == null) {
            throw new Exception("you need to call setup first", fileline());
        }
        // Clear last result
        _myResult = [];

        for (var i = 0; i < theResolution; ++i) {
            var myValue = i / theResolution;
            _myResult[i] = this.evaluate(myValue, theEaseIn, theEaseOut);
            //print("t: " + t);

        }
        return _myResult;
    }

    this.setupFromPoints = function(thePoints, theSize) {
        var myDir1 = product(normalized(difference(thePoints[2], thePoints[0])), theSize);
        var myStartHandle = sum(thePoints[1], myDir1);
        var myDir2 = product(normalized(difference(thePoints[1], thePoints[3])), theSize);
        var myEndHandle   = sum(thePoints[2], myDir2);

        setup(thePoints[1], myStartHandle, thePoints[2], myEndHandle);
    }

    /**
     *   Calculates the a point along the spline
     *   @param {float} theCurveParameter Position along the curve (between 0 and 1)
     *   @param {float} theEaseIn Ease in between zero and one (optional)
     *   @param {float} theEaseOut Ease out between zero and one (optional)
     *   @return {Vector3f} The point
     **/
    this.evaluate = function(theCurveParameter, theEaseIn, theEaseOut) {
        if (theEaseIn != null && theEaseOut != null) {
            theEaseOut = 1 - theEaseOut;
            theCurveParameter = smoothstep(theEaseIn, theEaseOut, theCurveParameter);
        }
        return getValue(theCurveParameter);
    }

    /**
     *   Calculates the arc length of the spline. Caches result
     *   @return {float} The arc length
     **/
    this.getArcLength = function() {
        if (_myArcLength == null) {
            var myP1 = this.evaluate(0);
            var myP2 = this.evaluate(1);
            _myArcLength = getSegmentLength(0, 1, myP1, myP2, distance(myP1, myP2));
        }
        return _myArcLength;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    function setup(theStart, theStartHandle, theEnd, theEndHandle) {
        _myPolyCoefs = [];

        // b-Funktion
        for (var i = 0;i < 4; ++i) {
            _myPolyCoefs[i] = [];
        }

        for (var j = 0; j < 3; j++) {
            _myPolyCoefs[0][j] = theStart[j];
            _myPolyCoefs[1][j] = 3 * (theStartHandle[j] - theStart[j]);
            _myPolyCoefs[2][j] = 3 * (theEndHandle[j] - theStartHandle[j]) - _myPolyCoefs[1][j];
            _myPolyCoefs[3][j] = theEnd[j] - theStart[j] - _myPolyCoefs[1][j] - _myPolyCoefs[2][j];
        }
        _myArcLength = null;
    }

    function getValue(t) {
        var myPoint = new Vector3f();
        for (var j = 0; j < 3; j++) {
            myPoint[j] = _myPolyCoefs[3][j] * t*t*t
                + _myPolyCoefs[2][j] * t*t
                + _myPolyCoefs[1][j] * t
                + _myPolyCoefs[0][j];
        }
        return myPoint;
    }

    function getSegmentLength(theT1, theT2, theP1, theP2, theChordLength) {
        const MAX_ERROR = 0.01;

        var myCenter = 0.5*(theT1 + theT2);
        var myCenterPoint = self.evaluate(myCenter);
        var myL1 = distance(theP1, myCenterPoint);
        var myL2 = distance(theP2, myCenterPoint);
        var myError = ((myL1 + myL2) - theChordLength) / theChordLength;
        if (myError > MAX_ERROR) {
            myL1 = getSegmentLength(theT1, myCenter, theP1, myCenterPoint, myL1);
            myL2 = getSegmentLength(myCenter, theT2, myCenterPoint, theP2, myL2);
        }
        return myL1 + myL2;
    }

    var _myResult = [];
    var _myPolyCoefs = null;
    var _myArcLength;

    var self = this;

    if (theStart != undefined) {
        setup(theStart, theStartHandle, theEnd, theEndHandle);
    }
}



// Neccessary for auto-documentation
new BSpline();
