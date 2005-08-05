//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: BSpline.js,v $
//   $Author: valentin $
//   $Revision: 1.13 $
//   $Date: 2004/08/06 15:38:57 $
//
//
//=============================================================================

// Constructor
function BSpline() {    
    this.Constructor(this);
}

BSpline.prototype.Constructor = function(obj) {
    var _myResult = new Array();

    obj.getResult = function() {
        return _myResult;
    }

    /*
        theStart       --- Startpunkt (new Vector3f)
        theStartHandle --- Anfasser-Startpunkt (new Vector3f)
        theEnd         --- Endpunkt (new Vector3f)
        theEndHandle   --- Anfasser-Endpunkt (new Vector3f)
        theResolution  --- Anzahl der schritte, von anfang bis ende
        theEaseIn      --- Wert zwischen 0 und 1. 1 für maximales theEaseIn
        theEaseOut     --- Wert zwischen 0 und 1. 1 für maximales theEaseOut
    */    
    obj.calculate = function(theStart, theStartHandle, theEnd, theEndHandle, 
                             theResolution, theEaseIn, theEaseOut) 
    {
        // Clear last result
        _myResult = [];
        
        if (theEaseIn == null) {
            theEaseIn = 0;
        }
        if (theEaseOut == null) {
            theEaseOut = 0;
        }
        
        theEaseOut = 1 - theEaseOut;
        
        // anzahl der Dimensionen
        var myDimensionCount = 3;
        
        // punkte umsortieren
        var myP0 = new Array(); // punkt 1
        var myP1 = new Array(); // anfasser 1
        var myP2 = new Array(); // punkt 2
        var myP3 = new Array(); // anfasser 2
        for (var j = 0; j < myDimensionCount; ++j) {
            myP0[j] = theStart[j];
            myP1[j] = theStartHandle[j];
            myP3[j] = theEnd[j];
            myP2[j] = theEndHandle[j];
        }
        
        // b-Funktion
        var a = new Array();
        var b = new Array();
        var c = new Array();
        
        for (var j = 0; j < myDimensionCount; j++) {
            c[j] = 3 * (myP1[j] - myP0[j]);
            b[j] = 3 * (myP2[j] - myP1[j]) - c[j];
            a[j] = myP3[j] - myP0[j] - c[j] - b[j];
        }
        
        // fill ease array 
        var myEaseArray = calcEaseModifier(theEaseIn, theEaseOut, theResolution);
        for (var i = 0; i < theResolution; ++i) {
            // ease
            var t = myEaseArray[i];
            //print("t: " + t);            
            // b-function
            var x = a[0] * t * t * t + 
                    b[0] * t * t + 
                    c[0] * t +
                    myP0[0];
            var y = a[1] * t * t * t + 
                    b[1] * t * t + 
                    c[1] * t +
                    myP0[1];
            var z = a[2] * t * t * t + 
                    b[2] * t * t + 
                    c[2] * t +
                    myP0[2];

            
            var myPunkte = new Vector3f(x, y, z);
            _myResult[i] = myPunkte;
        }     
    }
            
    function calcEaseModifier(theEaseIn, theEaseOut, theResolution) {
        var myEaseArray = new Array();
        for (var i = 0; i < theResolution; ++i) {
            var myValue = i / theResolution;
            myEaseArray[i] = smoothstep(theEaseIn, theEaseOut, myValue);
        }
        return myEaseArray;
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
}