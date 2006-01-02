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

/**
 * Constructs a new BSpline object
 */
function BSpline() {
    /**
     * Gets the point array of the BSpline
     * @return {ArrayOfPoint3f} The point array
     */
    this.getResult = function() {
        return _myResult;
    }

    /**
     *   Calculate the BSpline point array
     *   @param {Vector3f} theStart       Startpunkt
     *   @param {Vector3f} theStartHandle Anfasser-Startpunkt
     *   @param {Vector3f} theEnd         Endpunkt
     *   @param {Vector3f} theEndHandle   Anfasser-Endpunkt
     *   @param {integer}  theResolution  Anzahl der schritte, von anfang bis ende
     *   @param {float}    theEaseIn      Wert zwischen 0 und 1. 1 für maximales theEaseIn
     *   @param {float}    theEaseOut     Wert zwischen 0 und 1. 1 für maximales theEaseOut
     */
    this.calculate = function(theStart, theStartHandle, theEnd, theEndHandle,
                             theResolution, theEaseIn, theEaseOut)
    {
        // Clear last result
        _myResult = [];

        this.setup(theStart, theStartHandle, theEnd, theEndHandle);
        
        for (var i = 0; i < theResolution; ++i) {
            var myValue = i / theResolution;
            _myResult[i] = this.evaluate(myValue, theEaseIn, theEaseOut);
            //print("t: " + t);

        }
    }

    this.setup = function(theStart, theStartHandle, theEnd, theEndHandle) {
        _myPolyCoefs = [];

        // b-Funktion
        for (var i = 0;i < 4; ++i) {
            _myPolyCoefs[i] = [];
        }

        for (var j = 0; j < _myDimensionCount; j++) {
            _myPolyCoefs[0][j] = theStart[j];
            _myPolyCoefs[1][j] = 3 * (theStartHandle[j] - theStart[j]);
            _myPolyCoefs[2][j] = 3 * (theEndHandle[j] - theStartHandle[j]) - _myPolyCoefs[1][j];
            _myPolyCoefs[3][j] = theEnd[j] - theStart[j] - _myPolyCoefs[1][j] - _myPolyCoefs[2][j];
        }
    }

    this.evaluate = function(theCurveParameter, theEaseIn, theEaseOut) {
        if (theEaseIn != null && theEaseOut != null) {
            theEaseOut = 1 - theEaseOut;
            theCurveParameter = smoothstep(theEaseIn, theEaseOut, theCurveParameter);
        }
        return getValue(theCurveParameter);
    }
    
    function getValue(t) {            
        var myPoint = new Vector3f();
        for (var j = 0; j < _myDimensionCount; j++) {
            myPoint[j] = _myPolyCoefs[3][j] * t*t*t
                + _myPolyCoefs[2][j] * t*t
                + _myPolyCoefs[1][j] * t
                + _myPolyCoefs[0][j];
        }
        return myPoint;
    }
            
    function smoothstep(theEaseIn, theEaseOut, theInput) {
        if (theInput < theEaseIn) {
            return 0.0;
        }
        if (theInput >= theEaseOut) {
            return 1.0;
        }
        var myOutput = (theInput - theEaseIn) / (theEaseOut - theEaseIn);
        return (myOutput * myOutput) * (3 - 2 * myOutput);
    }

    const _myDimensionCount = 3;
    var _myResult = [];
    var _myPolyCoefs;
    
}

// Neccessary for auto-documentation
new BSpline();
