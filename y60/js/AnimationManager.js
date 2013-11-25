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

/*jslint nomen:false white:false plusplus:false*/
/*globals window, fileline, Exception, Logger, print, Renderer,
          createDirectory, getDirectoryEntries, fileExists*/

function Animation(theAnimationName, theSceneViewer) {
    this.Constructor(this, theAnimationName, theSceneViewer);
}

Animation.prototype.Constructor = function(obj, theAnimationName, theSceneViewer) {
    var _mySceneViewer   = theSceneViewer;
    var _myName  = theAnimationName;
    var _myPauseFlag = false;
    var _myAnimationNode = window.scene.animations.find(".//*[@name = '" + theAnimationName + "']");
    if (!_myAnimationNode) {
        throw new Exception("Could not find animation: " + theAnimationName, fileline());
    }
    obj.getName = function() {
        return _myName;
    };
    obj.pause = function() {
        _myPauseFlag = !_myPauseFlag; 
    };
    obj.stop = function() {
        _myAnimationNode.enabled = false;
    };
    obj.start = function(theOffset, theLoops) {
        var myCurrentTime = _mySceneViewer.getCurrentTime();
        _myAnimationNode.begin = myCurrentTime + theOffset;
        _myAnimationNode.enabled = true;
        _myAnimationNode.count   = theLoops;
        _myAnimationNode.pause = 0.0;
    };
    obj.setInOutAndStart = function(theBeginTime, theEndTime, theOffset, theLoops) {
        var myCurrentTime = _mySceneViewer.getCurrentTime();
        _myAnimationNode.clipin  = theBeginTime;
        _myAnimationNode.clipout = theEndTime;
        _myAnimationNode.begin = myCurrentTime + theOffset;
        _myAnimationNode.enabled = true;
        _myAnimationNode.count   = theLoops;
    };
    obj.setDuration = function(theDuration) {
        _myAnimationNode.duration = theDuration;
    };
    obj.getDuration = function() {
        return _myAnimationNode.duration;
    };
    obj.getAnimationNode = function() {
        return _myAnimationNode;
    };
    obj.onFrame = function(theDeltaT) {
        if (_myPauseFlag) {
            _myAnimationNode.pause = _myAnimationNode.pause + theDeltaT;
        }
    };
    obj.isRunning = function() {
        var myCurrentTime = _mySceneViewer.getCurrentTime();
        return _myAnimationNode.enabled && myCurrentTime >= _myAnimationNode.begin  &&  myCurrentTime <= _myAnimationNode.begin + _myAnimationNode.duration;
    };
    obj.reset = function() {
        obj.start(604800,1); // offset on week into the future. this resets the animation toe the first frame.  
    };
};

//=============================================================================
//
//   ANIMATION MANAGER
//
//=============================================================================

function AnimationManager(theSceneViewer) {
    this.Constructor(this, theSceneViewer);
}

AnimationManager.prototype.Constructor = function(obj, theSceneViewer) {
    var ANIMATION_FRAMES_PER_SECOND = 25;

    var _mySceneViewer       = theSceneViewer;
    var _myAnimationTime     = 0;
    var _myLastTime          = null;
    var _myAnimationTimeStep = 0.04;
    var _myEnabled           = true;
    var _myUpdated           = false;
    var _myAnimations        = [];
    var _myCreateImageStrip  = false;
    var _myImageStripOverwriteFlag = false;
    var _myRenderFrame       = 0;
    var _myImageStripPrefix  = "";
    var _myLastImageStripFrame = 0;

    /////////////////////
    // Private Methods //
    /////////////////////
    
    function getAnimatable(theAnimationName) {
        if (theAnimationName in _myAnimations) {
            return _myAnimations[theAnimationName];
        } else {
            Logger.error("AnimationManager::getAnimatable: Unable find animation '" + theAnimationName + "'");
            return 0;
        }
    }
    
    // Seens unused
    /*function getAnimation(theAnimation) {
        return getAnimatable(theAnimationName);
    }*/

    function printHelp() {
        print("Animation Keys:");
        print("    a    toggle animations");
        print("    R    reset animations");
        print("  insert step forward in animation time");
        print("  delete step backward in animation time");
    }

    ////////////////////
    // Public Methods //
    ////////////////////

    obj.setupSimpleAnimation = function(theAnimations) {
        for (var i = 0; i < theAnimations.length; i++) {
            _myAnimations[theAnimations[i]] = new Animation (theAnimations[i], _mySceneViewer);
        }
    };

    obj.enableGlobalAnimations = function(theFlag) {
        var myAnimations = window.scene.animations;
        for (var i = 0; i < myAnimations.childNodes.length; ++i) {
            myAnimations.childNode(i).enabled = theFlag;
        }
    };

    obj.setDuration = function(theAnimationName, theDuration)  {
        var myAnimation = getAnimatable(theAnimationName);
        if (myAnimation) {
            myAnimation.setDuration(theDuration);
            _myUpdated = true;
        }
    };

    obj.getDuration = function(theAnimationName)  {
        var myAnimation = getAnimatable(theAnimationName);
        if (myAnimation) {
            return myAnimation.getDuration();
        } else {
            return 0;
        }
    };

    obj.getAnimationFrameCount = function(theAnimationName) {
        var myAnimation = getAnimatable(theAnimationName);
        if (myAnimation) {
            return myAnimation.getDuration() * ANIMATION_FRAMES_PER_SECOND;
        } else {
            return 0;
        }
    };

    obj.start = function(theAnimationName, theLoops, theOffset)  {
        theOffset = theOffset || 0;
        theLoops  = theLoops  || 1;
        var myAnimation = getAnimatable(theAnimationName);
        if (myAnimation) {
            myAnimation.start(theOffset, theLoops);
            _myUpdated = true;
        }
    };

    obj.startClip = function(theCharacterName, theClipName, allowReStart) {
        if (!!allowReStart || !window.isCharacterActive(theCharacterName) ) {
            window.playClip(_myAnimationTime, theCharacterName, theClipName);
            return true;
        }
    };
    obj.isClipActive = function(theCharacterName, theClipName) {
        return window.isClipActive(theCharacterName, theClipName);
    };
    obj.isCharacterActive = function(theCharacterName) {
        return window.isCharacterActive(theCharacterName);
    };
    obj.setClipLoops = function(theCharacterName, theClipName, theLoops) {
        window.setClipLoops(theCharacterName, theClipName, theLoops);
    };
    obj.setClipForwardDirection = function(theCharacterName, theClipName, theFlag) {
        window.setClipForwardDirection(theCharacterName, theClipName, theFlag);
    };
    obj.stopAtEndOfLoop = function(theCharacterName, theClipName) {
        var myLoops = window.getLoops(theCharacterName, theClipName);
        window.setClipLoops(theCharacterName, theClipName, myLoops+1);
    };
    obj.stop = function(theCharacterName) {
        window.stopCharacter(theCharacterName);
    };

    obj.setTime = function(theTime) {
        if (theTime < _myAnimationTime) {
            _myUpdated = true;
        }
        _myAnimationTime = theTime;
        _myLastTime      = null;
        window.runAnimations(_myAnimationTime);
    };

    obj.getTime = function() {
        return _myAnimationTime;
    };

    obj.setFrame = function(theFrame) {
        _myAnimationTime = theFrame / ANIMATION_FRAMES_PER_SECOND;
        _myLastTime      = null;
        window.runAnimations(_myAnimationTime);
    };

    obj.getFrame = function() {
        return _myAnimationTime * ANIMATION_FRAMES_PER_SECOND;
    };

    obj.enable = function(theEnable) {
        _myEnabled = theEnable;
        _myUpdated = true;
    };

    obj.onFrame = function(theTime) {
        if (_myLastTime === null) {
            _myLastTime = theTime;
        }
        if (_myUpdated) {
             _mySceneViewer.getScene().update(Renderer.ANIMATIONS);
            _myUpdated = false;
        }
        if (_myCreateImageStrip) {
            if (_myRenderFrame === -1) {
                obj.setFrame(0);
            } else {
                obj.setFrame(_myRenderFrame);
                //print(" Time: " + theTime + " Frame : " +   _myRenderFrame);
            }
        } else if (_myEnabled) {
            var myDiff = theTime - _myLastTime;
            if (myDiff > 0.05) {
               // print("Animation timestep > 0.05: " + myDiff.toFixed(4));
            } else if (myDiff > 0.1) {
                Logger.warning("Animation timestep > 0.1: " + myDiff.toFixed(4));
            }
            _myAnimationTime += myDiff;
            window.runAnimations(_myAnimationTime);
        }
        _myLastTime = theTime;
    };
    
    function saveImage() {
        if (_myRenderFrame !== -1) {
            var i;
            var myDigits = Math.ceil(Math.log(_myLastImageStripFrame+1)/Math.LN10);
            if (_myRenderFrame > 0) {
                for(i = 1; i < myDigits+1; i++) {
                    var myBase = Math.pow(10,i);
                    if (_myRenderFrame < myBase) {
                        myDigits = myDigits-i;
                        break;
                    }
                }
            }
            var myCounter = "";
            for(i = 0; i < myDigits; i++) {
                myCounter = myCounter + "0";
            }
            if (_myRenderFrame >0) {
                myCounter += _myRenderFrame;
            }
            var myFilename = _myImageStripPrefix + "_" + myCounter + ".png";
            if (!fileExists(myFilename) || _myImageStripOverwriteFlag) {
                window.saveBuffer(myFilename);
                print("saved Image:" + myFilename);
            } else {
                Logger.error("File '" + myFilename + "' does exist and no overwrite flag is set");
            }
        }
        _myRenderFrame++;
        if (_myRenderFrame > _myLastImageStripFrame) {
            _myRenderFrame = -1;
            _myCreateImageStrip = false;
            print("Image strip creation is finished!");
        }
    }
    
    obj.onPostRender = function() {
        if (_myCreateImageStrip) {
            saveImage();
        }
    };

    obj.imageStripDone = function() {
        return (_myCreateImageStrip === false);
    };

    function saveImageStrip(thePrefix, theDirectory, theOverwriteFlag) {
        _myCreateImageStrip = true;
        _myImageStripOverwriteFlag = !!theOverwriteFlag;
        _myImageStripPrefix = theDirectory + "\\" + thePrefix;
        _myRenderFrame      = -1;
        // does the directory exist ?
        if (getDirectoryEntries(theDirectory) == null) {
            createDirectory(theDirectory);
        }
    }

    obj.saveStripByAnimation = function(theAnimationName, thePrefix, theDirectory, theOverwriteFlag) {
        var myCurrentTime = _mySceneViewer.getCurrentTime();
        var myAnimationNode = window.scene.animations.find(".//*[@name = '" + theAnimationName + "']");
        myAnimationNode.begin = myCurrentTime;
        myAnimationNode.enabled = true;
        myAnimationNode.count   = 0;
        _myLastImageStripFrame = myAnimationNode.duration * ANIMATION_FRAMES_PER_SECOND;
        obj.saveImageStrip(thePrefix, theDirectory, theOverwriteFlag);
    };
    obj.saveStripByAnimations = function(theAnimationNames, thePrefix, theDirectory, theOverwriteFlag) {
        var myCurrentTime = _mySceneViewer.getCurrentTime();
        for (var i = 0; i < theAnimationNames.length; i++) {
            var myAnimationNode = window.scene.animations.find(".//*[@name = '" + theAnimationNames[i] + "']");
            myAnimationNode.begin   = 0;
            myAnimationNode.enabled = true;
            myAnimationNode.count   = 1;
            _myLastImageStripFrame = myAnimationNode.duration * ANIMATION_FRAMES_PER_SECOND;
            print("Start animation : " + myAnimationNode.name);
        }

        obj.saveImageStrip(thePrefix, theDirectory, theOverwriteFlag);
    };
    obj.saveStripByFrameCount = function(theFrameCount, thePrefix, theDirectory, theOverwriteFlag) {
        _myLastImageStripFrame = theFrameCount-1;
        obj.saveImageStrip(thePrefix, theDirectory, theOverwriteFlag);
    };

    obj.saveStripByNodeId = function(theNodeId, thePrefix, theDirectory, theOverwriteFlag) {
        var myCameraAnimations = window.scene.animations.findAll(".//*[@node = '" + theNodeId + "']");
        if (myCameraAnimations.length === 0 ) {
            Logger.error("Unable to find animation for id=" + theNodeId);
            return;
        }
        var myCurrentTime = _mySceneViewer.getCurrentTime();
        for (var i = 0; i < myCameraAnimations.length; i++) {
            var myAnimationNode = myCameraAnimations[i];
            myAnimationNode.begin = myCurrentTime;
            myAnimationNode.enabled = true;
            myAnimationNode.count   = 0;
            _myLastImageStripFrame = myAnimationNode.duration * ANIMATION_FRAMES_PER_SECOND;
        }
        obj.saveImageStrip(thePrefix, theDirectory, theOverwriteFlag);
    };

    obj.saveStripByActiveCamera = function(thePrefix, theDirectory, theOverwriteFlag) {
        obj.saveStripByNodeId(window.camera.id, thePrefix, theDirectory, theOverwriteFlag);
    };

    obj.reset = function() {
        _myAnimationTime = 0;
        _myUpdated = true;
        window.runAnimations(_myAnimationTime);
    };

    obj.step = function(theDirection) {
        _myEnabled = false;
        if (theDirection) {
            _myAnimationTime +=_myAnimationTimeStep;
        } else {
            _myAnimationTime -=_myAnimationTimeStep;
            _myUpdated = true;
        }
        window.runAnimations(_myAnimationTime);
        return _myAnimationTime;
    };

    obj.onKey = function(theKey, theKeyState, theShiftFlag) {
        // theKeyState is true, if the key is pressed
        if (!theKeyState) {
            return;
        }

        if (theShiftFlag) {
            theKey = theKey.toUpperCase();
        }

        switch (theKey) {
            case "h":
                printHelp();
                break;
            case "a":
                _myEnabled = !_myEnabled;
                print("Animations " + (_myEnabled ? "on" : "off"));
                break;
            case "delete":
                obj.step(false);
                print("Animationtime: " + _myAnimationTime.toFixed(4));
                break;
            case "insert":
                obj.step(true);
                print("Animationtime: " + _myAnimationTime.toFixed(4));
                break;
            case "R":
                print("Reset Animations");
                obj.reset();
                break;
        }

        return true;
    };
};
