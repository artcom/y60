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
        var newParameterNode = theMaterialNode.find(".//*[@name = 'WindParams']").childNodes[0];
        _myParameterNodes.push(newParameterNode);
    }

    obj.setWindDirection = function(theDirection) {
        _myWind.direction = radFromDeg(theDirection);
    }

    obj.onFrame = function(theTime) {
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
            var myNode = theWorld.find(".//*[@name = '" + theBodyNames[i] + "']");
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
