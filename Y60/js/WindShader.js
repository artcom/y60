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
//   $RCSfile: WindShader.js,v $
//   $Author: danielk $
//   $Revision: 1.6 $
//   $Date: 2005/04/01 17:16:03 $
//
//
//=============================================================================

// theDirection: direction in degrees (0° = north = neg-z)
// theSpeed : Windspeed in something
function Wind(theDirection, theSpeed) {
    this.frequency = TWO_PI/10;
    this.amplitude = 1; // looks good;
    this.direction = radFromDeg(theDirection);
    this.waveSpeed = theSpeed * theSpeed;

    this.getFreqTime = function(theTime) {
        return fmod(theTime * this.frequency * this.waveSpeed, TWO_PI);
    }

    this.setMaterialParam = function(theNode, theTime, currentAmplitude) {
        theNode.nodeValue = "[" + this.getFreqTime(theTime) + ","
                                + this.frequency + ","
                                + this.amplitude + ",1.0]";
    }
}

function WindShader(theWind, theBodyNames, theWorld)
{
    this.Constructor(this, theWind, theBodyNames, theWorld);
}

WindShader.prototype.Constructor = function(obj, theWind, theBodyNames, theWorld)
{
    var _myWind             = theWind;
    var _myParameterNodes   = [];
    var _myBodys            = findBodys(theBodyNames, theWorld)

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    obj.addMaterial = function(theMaterialNode) {
        var newParameterNode = getDescendantByName(theMaterialNode, "WindParams", true).childNodes[0];
        _myParameterNodes.push(newParameterNode);
    }

    obj.setWindDirection = function(theDirection) {
        _myWind.direction = radFromDeg(theDirection);
    }

    obj.onIdle = function(theTime) {
        // set Cg Paramaters
        for (var i = 0; i < _myParameterNodes.length; i++) {
             _myWind.setMaterialParam(_myParameterNodes[i], theTime);
        }
        // set body orientation
        var myBodyCount = _myBodys.length;
        for (var i = 0; i < myBodyCount; i++) {
            if (_myBodys[i]) {
                var myBody = _myBodys[i];
                // clear local heading
                myBody.orientation.y = 0;
                window.updateGlobalMatrix(myBody);

                // get global heading in rad
                var tempVec = myBody.globalmatrix.getRow(0);
                var x = tempVec[0];
                var z = tempVec[2];
                var curHeading = Math.asin(x/Math.sqrt(x*x+z*z));
                if (z>=0) {
                    curHeading = Math.PI-curHeading;
                }
                //  set new local heading so that the new global heading is the wind direction
                var newHeading = _myWind.direction - curHeading;

                // somewhere, there is a negation error,
                // no time to find it, we'll just use the negative newHeading
                myBody.orientation.y = -newHeading;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // private members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    function findBodys(theBodyNames, theWorld) {
        var theBodyCount = theBodyNames.length;
        var theBodys = [];
        for (var i = 0; i < theBodyCount; ++i) {
            var myNode = getDescendantByName(theWorld, theBodyNames[i], true);
            if (myNode) {
                print ("WindShader loading body " + myNode.name);
                theBodys.push(myNode);
            } else {
                print ("### WARNING: WindShader can not find body '"+theBodyNames[i]+"'");
            }
        }
        return theBodys;
    }
}
