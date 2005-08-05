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
                        "TextureAnimation::setStartFrame()");
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
        window.scene.update(Scene.IMAGES);
        // window.reloadImage(_myImageNode);
    }
}

    

