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
//   $RCSfile: TextureAnimation.js,v $
//   $Author: martin $
//   $Revision: 1.15 $
//   $Date: 2005/03/30 17:45:57 $
//
//
//=============================================================================

use("Y60JSSL.js");

function TextureAnimation(theImageNode, theImageIndex) {
    this.Constructor(this, theImageNode, theImageIndex);
}

TextureAnimation.prototype.Constructor = function(obj, theImageNode, theImageIndex) {

    // private member
    var _myLoops          = -1; // -1 endless, otherwise n-times
    var _myPingPong       = false; // true = loop alternating forward / backward
    var _myPingPongDir    = true;  // true = forward, false = backwards
    var _myCurrentFrame   = 0;
    //var _myTextureManager = window.getTextureManager();
    var _myTextureManager = window;
    var _myFrames         = new Array();
    var _myImageNode      = theImageNode;
    var _myDuration       = 2;
    var _myImageIndex     = 0;
    if (theImageIndex) {
        _myImageIndex = theImageIndex;
    }

    obj.addFrame    = function(theFilename) {
        _myFrames.push(theFilename);
    }

    // use like this: _myTextureAnimation.loadByBaseName("texanim/ripple_????.jpg", 60);
    obj.addFramesByBaseName = function(theBaseName, theCount) {
        var myRegExpr = new RegExp("\?+");
        var myDigits = String(myRegExpr.exec(theBaseName)).length;

        for(var i = 1; i < theCount + 1; ++i) {
            var myPlaceholder = "";
            for(var j = 0; j < myDigits; ++j) {
                if (i < (Math.pow(10, j))) {
                    myPlaceholder = myPlaceholder + "0";
                }
            }
            var myFileName = String(theBaseName).replace(myRegExpr, myPlaceholder + i);
            obj.addFrame(myFileName);
        }
    }

    obj.setDuration = function(theDuration) {
        _myDuration = theDuration;
    }

    obj.setLooping = function(theLoop) {
        _myLoops = theLoop;
    }

    obj.setPingPong = function(thePingPong) {
        _myPingPong = thePingPong;
        _myPingPongDir = true;
    }

    obj.setStartFrame = function(theFrameNum) {
        if (theFrameNum < _myFrames.length) {
            _myCurrentFrame = theFrameNum;
        } else {
            throw new Exception("Request for frame " + theFrameNum +
                        " but animation only has " + _myFrames.length + " frames.",
                        fileline());
        }
    }

    function checkLoops() {
        if (_myLoops == -1 || _myLoops > 0) {
            if (_myLoops > 0 ) {
                _myLoops--;
            }

            return true;
        } else {
            return false;
        }
    }

    obj.updatePerFrame = function() {
        // calc new currentFrame
        if (_myPingPongDir) {
            _myCurrentFrame++;
            if (_myCurrentFrame > _myFrames.length - 1) {
                if (checkLoops()) {
                    // Start next loop
                    if (_myPingPong) {
                        // Reverse direction
                        _myCurrentFrame = _myFrames.length - 1;
                        _myPingPongDir = !_myPingPongDir;
                    } else {
                        _myCurrentFrame = 0;
                    }
                } else {
                    // Stop looping
                    _myCurrentFrame = _myFrames.length - 1;
                }
            }
        } else {
            _myCurrentFrame--;
            if (_myCurrentFrame < 0) {
                if (checkLoops()) {
                    // Start next loop
                    if (_myPingPong) {
                        // Reverse direction
                        _myCurrentFrame = 0;
                        _myPingPongDir = !_myPingPongDir;
                    } else {
                        _myCurrentFrame = _myFrames.length - 1;
                    }
                } else {
                    // Stop looping
                    _myCurrentFrame = 0;
                }
            }
        }

        update();
    }

    // does not work with ping pong, yet
    var _myLoopTime = 0;
    obj.updatePerTime = function(theTime) {
        theTime = theTime - _myLoopTime;
        var myTime = fmod(theTime, _myDuration);
        if (myTime != theTime) {
            if (_myLoops < 0 || _myLoops > 0) {
                if (_myLoops > 0 ) {
                    _myLoops--;
                    _myLoopTime += theTime;
                }
            } else {
                myTime = _myDuration;
            }
        }
        // calc new currentFrame
        if (_myDuration != 0) {
            _myCurrentFrame = Math.floor((myTime / _myDuration) * (_myFrames.length-1));
        } else {
            _myCurrentFrame = 0;
        }
        update();
    }

    // private methods
    var update = function() {
        //print ("TextAnim:" + _myImageNode);
        _myImageNode.src = _myFrames[_myCurrentFrame];
        while (_myImageNode.firstChild) {
            _myImageNode.removeChild(_myImageNode.firstChild);
        }
    }
}



