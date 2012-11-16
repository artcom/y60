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
//   $RCSfile: WaterShader.js,v $
//   $Author: danielk $
//   $Revision: 1.48 $
//   $Date: 2005/04/01 17:16:03 $
//
//
//=============================================================================

// source:: http://www.physics.nmt.edu/~raymond/classes/ph13xbook/node7.html

function calcWaveSpeed(theWaveNumber, theGravity, theWaterDepth) {

    return Math.sqrt( (theGravity * tanh(theWaveNumber * theWaterDepth)) / theWaveNumber );
}


function Wave(theWavelength, theAmplitude, theDirection, theWaterDepth, theGravity, theWaveSpeed) {
    this.frequency = TWO_PI/theWavelength;
    this.amplitude = theAmplitude;
    this.direction = theDirection;
    if (theWaveSpeed !== undefined) {
        this.waveSpeed = theWaveSpeed;
    } else {
        if (theGravity === undefined) {
            theGravity = 9.81; // earth is default
        }
        this.waveSpeed = calcWaveSpeed(this.frequency, theGravity, theWaterDepth);
    }

    this.getFreqTime = function(theTime) {
        return fmod(theTime * this.frequency * this.waveSpeed, TWO_PI);
    }

    this.setWaveParam = function(theNode, theTime, currentAmplitude) {
        theNode.nodeValue = "[" + this.getFreqTime(theTime) + ","
                                + this.frequency + ","
                                + currentAmplitude + ",1.0]";
    }
}

function WaterShader(theWaterMaterialNode, theWaves, theShapesNode, theWorldNode, theDummyFlag)
{
    this.Constructor(this, theWaterMaterialNode, theWaves, theShapesNode, theWorldNode, theDummyFlag);

}

WaterShader.prototype.Constructor = function(obj, theWaterMaterialNode, theWaves,
                                             theShapesNode, theWorldNode, theDummyFlag)
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // Constructor code
    //
    ///////////////////////////////////////////////////////////////////////////////////////////
    var _myMaterialNode     = theWaterMaterialNode;
    var _myShapesNode       = theShapesNode;
    var _myWorldNode        = theWorldNode;
    var _myBodyNode         = null;
    var _myWaves            = theWaves;
    var _myWaveParams       = [];
    var _myActive           = true;
    var _myCgFlag           = (theDummyFlag === false);

    var _myParameterNames   = ["none", "wave 1 frequency", "wave 1 amplitude",
                               "wave 2 frequency", "wave 2 amplitude",
                               "wave 3 frequency", "wave 3 amplitude"];
    var _myTextureScaleParam = [];
    var _myCurrentTweakable = 0;
    var _myTimer            = new AutoTimer("WaterShader");

    setupWaveParameters();

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    obj.setActive = function(theFlag) {
        _myActive = theFlag;
    }
    obj.getActive = function() {
        return _myActive;
    }
    obj.getWaves = function() {
        return _myWaves;
    }

    obj.onFrame = function(theTime) {
        _myTimer.add("start");
        if (_myActive) {
            for (var i = 0; i < _myWaves.length; ++i) {
                var myCurrentAmplitude = _myWaves[i].amplitude;
                if (_myCgFlag) {
                    _myWaves[i].setWaveParam(_myWaveParams[i], theTime, myCurrentAmplitude);
                }
            }
        }

        _myTimer.add("waves");
    }
/*
    function newFromOldMatrix(m) {
        return new Matrix4f(m.getElement(0),m.getElement(1),m.getElement(2),m.getElement(3),
                            m.getElement(4),m.getElement(5),m.getElement(6),m.getElement(7),
                            m.getElement(8),m.getElement(9),m.getElement(10),m.getElement(11),
                            m.getElement(12),m.getElement(13),m.getElement(14),m.getElement(15));
    }
*/
    obj.transformWater = function() {
        if (_myCgFlag && _myBodyNode) {
            // since the floating objects (may) have moved, we have to
            // recalc the world matrixes
            var myCamera = window.camera;

            var myInverseProjectionMatrix = renderer.projectionmatrix;
            myInverseProjectionMatrix.invert();
            var myCameraWorldMatrix = window.camera.globalmatrix;
            var myCameraInverseProjectionMatrix = myInverseProjectionMatrix;
            myCameraInverseProjectionMatrix.postMultiply(myCameraWorldMatrix);

            // get Roll
            var myVecTemp = myCameraWorldMatrix.getRow(0);
            var myRightVector = normalized(new Vector3f(myVecTemp[0], myVecTemp[1], -myVecTemp[2]));
            var myRoll = Math.asin(-dot(myRightVector, new Vector3f(0,1,0)));
            // get Pitch
            myVecTemp = myCameraWorldMatrix.getRow(2);
            var myViewVector = normalized(new Vector3f(myVecTemp[0], myVecTemp[1], -myVecTemp[2]));
            var myPitch = Math.asin(-dot(myViewVector, new Vector3f(0,1,0)));

            /*
            print("Roll=" + degFromRad(myRoll));
            print("Pitch=" + degFromRad(myPitch));
            print("VFOV=" + renderer.verticalfov + ", HFOV=" + renderer.horizontalfov);
            print("renderer.width=" +renderer.width);
            print("renderer.height=" +renderer.height);
            */

            var myAspect = renderer.width / renderer.height;
            var myVerticalFOV = renderer.horizontalfov/myAspect;
            //print("Aspect" + myAspect);
            var myYOffset = Math.min(0.0,(Math.tan(-myPitch) / Math.tan(radFromDeg(myVerticalFOV/2)))-1);
            // print("myYOffset=" + myYOffset);
            // myYOffset = 0;

            var myTranslation = new Matrix4f();
            myTranslation.makeTranslating(new Vector3f(0,myYOffset,0));
            var myRotateMatrix = new Matrix4f();
            // myRoll = 0;
            var myTilt = Math.atan(Math.tan(myRoll)*myAspect);

            //print ("myRotateMatrix = "+myRotateMatrix);
            //print ("tilting by "+degFromRad(myTilt)+" deg");
            myRotateMatrix.makeZRotating(myTilt);

            //  T * R * PC
            myRotateMatrix.postMultiply(myCameraInverseProjectionMatrix);
            myTranslation.postMultiply(myRotateMatrix);
            _myBodyNode.globalmatrix = myTranslation;

            // set bounding-box
            var myMaxAmplitude = _myWaves[0].amplitude +  _myWaves[1].amplitude +  _myWaves[2].amplitude;
            _myBodyNode.boundingbox ='[[-1E+20,'+(-myMaxAmplitude)+',-1E+20],[+1E+20,'+myMaxAmplitude+',+1E+20]]';
        }
        _myTimer.add("transWater");
    }

    obj.onKey = function(theKey) {
        switch (theKey) {
            case "s":
                _myTimer.print();
                break;
            case 'h':
                print("WaterShader keys:");
                print("    j    select the wave param you want to tweak");
                print("    +    increase selected parameter");
                print("    -    decrease selected parameter");
                break;
            case 'j':
                selectTweakableParameter();
                return true;
                break;
            case '+':
                return tweakParameter(1);
                break;
            case '-':
                return tweakParameter(-1);
                break;
            default:
        }
        return false;
    }

    obj.createWaterPlane = function(theShapeId, theXSize, theYSize) {
        if (!_myCgFlag) {
            return;
        }
        const myOverscan = 1.5;

        // remove old water bodies
        var oldWaterBody = _myWorldNode.find(".//*[@name = 'wellen']");
        if (oldWaterBody) {
            oldWaterBody.parentNode.removeChild(oldWaterBody);
        }
        oldWaterBody = _myWorldNode.find(".//*[@name = 'bigwater']");
        if (oldWaterBody) {
            oldWaterBody.parentNode.removeChild(oldWaterBody);
        }

        // create vertex positions
        var myPositionsString="";
        for (var x=0; x < theXSize; ++x) {
            var myX = ((2.0*myOverscan)/(theXSize-1))*x - myOverscan;
            for (var y=0; y < theYSize; ++y) {
                var myY =  ((2.0*myOverscan)/(theYSize-1))*y - myOverscan;
                myPositionsString += "["+ myX + "," + (-myY) +",-1],";
            }
        }
        myPositionsString = myPositionsString.substr(0,myPositionsString.length-1); // discard trailing comma

        // create geometry
        var myElementString=[];

        //var myPrimitiveType = "triangles";
        var myPrimitiveType = "trianglestrip";
        //var myPrimitiveType = "quadstrip";
        var myElementCount = 0;
        myElementString[0] = "";
        if (myPrimitiveType == "triangles") {
            myElementCount = 1;
            for (var x=0; x < theXSize-1; ++x) {
                for (var y=0; y < theYSize-1; ++y) {
                    var myTLIndex = theYSize * x + y;
                    var myTRIndex = myTLIndex + 1;
                    var myBLIndex = myTLIndex + theYSize;
                    var myBRIndex = myBLIndex + 1;
                    myElementString[0] += myTLIndex + "," + myTRIndex + "," + myBLIndex + "," +
                                        myTRIndex + "," + myBRIndex + "," + myBLIndex + ",";
                }
            }
            myElementString[0] = myElementString[0].substr(0,myElementString[0].length-1); // discard trailing comma
        }
        if (myPrimitiveType != "triangles") {
            myElementCount = theXSize-1;
            for (var x=0; x < theXSize-1; ++x) {
                myElementString[x] = "";
                for (var y=0; y < theYSize-1; ++y) {
                    var myTLIndex = theYSize * x + y;
                    var myTRIndex = myTLIndex + theYSize;
                    myElementString[x] += myTRIndex + "," + myTLIndex + ",";
                }
                var myBLIndex = myTLIndex + 1;
                var myBRIndex = myBLIndex + theYSize;
                myElementString[x] += myBRIndex + "," + myBLIndex;
            }
        }

        var myShapeString =
        '<shape name="WaterplaneShape" renderstyle="[frontfacing]" id="'+theShapeId+'">' +
            '<vertexdata>'+
                '<vectorofvector3f name="position">['+myPositionsString+']</vectorofvector3f>' +
            '</vertexdata>'+
            '<primitives>';
        for (var e = 0; e < myElementCount; ++e) {
            myShapeString+=
                '<elements type="'+myPrimitiveType+'" material="'+_myMaterialNode.id+'">' +
                    '<indices vertexdata="position" role="position">['+myElementString[e]+']</indices>' +
                '</elements>';
        };
        myShapeString+=
            '</primitives>'+
        '</shape>';

        var myDocument = new Node(myShapeString);
        //print(myDocument);
        var myOldShapeNode = _myShapesNode.getElementById(theShapeId);
        var myNewShapeNode = myDocument.childNodes[0];

        if (myOldShapeNode) {
            print ("replacing water shape "+theShapeId);
            _myShapesNode.replaceChild(myNewShapeNode, myOldShapeNode);
        } else {
            _myShapesNode.appendChild(myNewShapeNode);
        }

        // create Body
        var myBodyId = 'b_'+theShapeId;
        var myBodyString = '<body position="[0,0,0]" scale="[1,1,1]" orientation="[0,0,0]" name="wellen" ' +
            'shape="'+theShapeId+'" id="'+myBodyId+'" '+
            '/>';

       myDocument = new Node(myBodyString);
       _myBodyNode = myDocument.childNodes[0];
       var myOldBodyNode = _myWorldNode.getElementById(myBodyId);

       if (myOldBodyNode) {
           print ("replacing waves body");
           _myWorldNode.replaceChild(myOldBodyNode, _myBodyNode);
           print ("replaced waves body");
       } else {
           print ("creating waves body");
           _myWorldNode.appendChild(_myBodyNode);
           print ("created waves body");
      }

       return _myBodyNode;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // private members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    function selectTweakableParameter() {
        _myCurrentTweakable = (_myCurrentTweakable + 1) % _myParameterNames.length;
        print("Parameter " + _myParameterNames[_myCurrentTweakable] + " selected.");
    }

    function tweakParameter(theDirection) {
        switch (_myCurrentTweakable) {
            case 0:
                return false;
                break;
            case 1:
                _myWaves[0].frequency += theDirection * 0.001;
                print(_myParameterNames[_myCurrentTweakable] + " = " + _myWaves[0].frequency.toFixed(3));
                break;
            case 2:
                _myWaves[0].amplitude += theDirection* 0.1;
                print(_myParameterNames[_myCurrentTweakable] + " = " + _myWaves[0].amplitude.toFixed(3));
                break;
            case 3:
                _myWaves[1].frequency += theDirection * 0.001;
                print(_myParameterNames[_myCurrentTweakable] + " = " + _myWaves[1].frequency.toFixed(4));
                break;
            case 4:
                _myWaves[1].amplitude += theDirection * 0.1;
                print(_myParameterNames[_myCurrentTweakable] + " = " + _myWaves[1].amplitude.toFixed(4));
                break;
            case 5:
                _myWaves[2].frequency += theDirection * 0.001;
                print(_myParameterNames[_myCurrentTweakable] + " = " + _myWaves[2].frequency.toFixed(4));
                break;
            case 6:
                _myWaves[2].amplitude += theDirection * 0.1;
                print(_myParameterNames[_myCurrentTweakable] + " = " + _myWaves[2].amplitude.toFixed(4));
                break;
        }
        return true;
    }

    function setupWaveParameters() {
        // We use the dummy mode to test floating shape animation on non fx machines
        if (_myCgFlag) {
            // Cg uniform parameter handles
            for (var i = 0; i < _myWaves.length; ++i) {
                _myWaveParams[i] = _myMaterialNode.find(".//*[@name = 'Wave" + Number(i+1) + "Params']").childNodes[0];
                var myWaveDirection = _myMaterialNode.find(".//*[@name = 'Wave" + Number(i+1) + "Direction']").childNodes[0];

                var myDirection3f = _myWaves[i].direction;
                myWaveDirection.nodeValue = new Vector4f(myDirection3f[0], myDirection3f[1], myDirection3f[2], 0);

                _myTextureScaleParam[0] = _myMaterialNode.find(".//*[@name = 'Tex1NearFarScale']").childNodes[0];
                _myTextureScaleParam[1] = _myMaterialNode.find(".//*[@name = 'Tex2NearFarScale']").childNodes[0];
            }
        } else {
            for (var i = 0; i < _myWaves.length; ++i) {
                _myWaveParams[i] = Node.createTextNode("");
            }
            _myTextureScaleParam[0] = 0;
            _myTextureScaleParam[1] = 0;
        }
    }
}


