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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

if (__main__ == undefined) var __main__ = "ImageViewer";

use("SceneViewer.js");
use("FrameRateLimiter.js");
//use("AudioController.js");
use("SoundController.js");
use("Playlist.js");
use("BuildUtils.js");

const MINZOOMFACTOR = 0.001;
const FRAME_RATE    = 100;
const OS = operatingSystem();


function ImageViewerApp(theArguments) {
    this.Constructor(this, theArguments);
}

ImageViewerApp.prototype.Constructor = function(self, theArguments) {

    var _myPlaylist      = new Playlist();
    var _myFileIndex     = 0;

    var _myTextOverlay   = null;
    var _myMovieNode     = null;
    var _myImageNode     = null;
    var _myCaptureNode   = null;
    var _myImageOverlay  = null;
    var _myMovieOverlay  = null;
    var _myZoomFactor    = 1.0;
    var _myPanning       = new Vector2f(0,0);
    var _myFullSizeMode  = false;

    var _myZoomCenter       = {image: new Vector2f(0,0), screen: new Vector2f(0,0)};
    var _myDragStart        = null;
    var _myFrameRateLimiter = new FrameRateLimiter(FRAME_RATE);

    var _myCurrentMediaType = -1;
    var _mySoundId	        = -1;
    var _myMasterVolume     = 1.0;
    var _myVideoCapturePlugged= false;
    var _myMissedFrameCounter = 0;
    var _myMaxMissedFrame     = 0;
    var _myLastFrame          = 0;
    var _myFrameCounter       = 0;
    var _myTheaterFlag        = false;

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////

    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle, theTheaterFlag) {
        self.setSplashScreen(false);
        Base.setup(theWidth, theHeight, false, theTitle);
        _myTheaterFlag    = theTheaterFlag;
        if (_myTheaterFlag) {
            window.decorations     = false;
            window.backgroundColor = [0.0, 0.0, 0.0];
            window.position        = [0.0, 0.0];
            window.showMouseCursor = false;
            window.swapInterval    = 2;

        } else {
            window.canvas.backgroundcolor = [0.5,0.5,0.5,1];
        }
        //window.fixedFrameTime = 1/60;
        //window.swapInterval    = 0;

        for(var i=0; i<theArguments.length; ++i) {
            switch(theArguments[i]) {
            case "recursive": // would like to use '--recursive' but that is rejected by the acxpshell options
                _myPlaylist.setRecursiveFlag(true);
                break;
            default:
                //print("Trying to add entry: " + theArguments[i]);
                _myPlaylist.addEntry(theArguments[i]);
                break;
            }
        }
        if (_myPlaylist.getNumEntries() == 0) {
            print("Nothing to show!");
            exit(0);
        }
        updateFileView();
        _myTextOverlay = new Overlay(window.scene, [0,0,0,0.6], [0,0], [200,200]);
        _myTextOverlay.visible = !_myTheaterFlag;
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        try {
            if (theCtrlFlag) {
                Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
                return;
            }

            if (theShiftFlag) {
                theKey = theKey.toUpperCase();
            }
            if (theState) {
                switch (theKey) {
                    case "space":
                        _myTextOverlay.visible = !_myTextOverlay.visible;
                        break;
                    case "right":
                        nextFile();
                        break;
                    case "left":
                        previousFile();
                        break;
                    case "up":
                        setVolume(true);
                        break;
                    case "down":
                        setVolume(false);
                        break;
                    case "[+]":
                        if (theShiftFlag) {
                            if (_myMovieNode && _myMovieOverlay.visible) {
                                _myMovieNode.avdelay += 0.01;
                            }
                        } else {
                            setPlaySpeed(true);
                        }
                        break;
                    case "[-]":
                        if (theShiftFlag) {
                            if (_myMovieNode && _myMovieOverlay.visible) {
                                _myMovieNode.avdelay -= 0.01;
                            }
                        } else {
                            setPlaySpeed(false);
                        }
                        break;
                    case "p":
                    case "s":
                        setPlayMode(theKey);
                        break;
                    case "f":
                        _myFullSizeMode = !_myFullSizeMode;
                        updateFileView();
                        break;
                    case "page up":
                        stepFrame(true);
                        break;
                    case "page down":
                        stepFrame(false);
                        break;
                    case ".":
                        seekToOffset(true, 100);
                        break;
                    case "/":
                        seekToOffset(false, 100);
                        break;
                    case "1":
                        if (_myMovieNode && _myMovieNode.nodeName == "movie") {
                            if (_myMovieNode.audio_stream > 0) {
                                _myMovieNode.audio_stream--;
                            }
                        }                    
                        break;                        
                    case "2":
                        if (_myMovieNode && _myMovieNode.nodeName == "movie") {
                            _myMovieNode.audio_stream++;
                        }                    
                        break;                        
                    case "h":
                        self.setMessage("ImageViewer help:");
                        self.setMessage("   space           - show/hide text overlay");
                        self.setMessage("   right           - next file");
                        self.setMessage("   left            - previous file");
                        self.setMessage("   f               - show in full size mode");
                        self.setMessage("   right-button    - pan");
                        self.setMessage("   mouse-wheel     - zoom");
                        self.setMessage("MoviePlayer help:");
                        self.setMessage("   +/- (Numberpad) - increase/decrease playbackspeed");
                        self.setMessage("   pageup/pagedown - next/previous image during pause");
                        self.setMessage("   p/p             - play/pause");
                        self.setMessage("   s               - stop");
                        self.setMessage("   arrow up/down   - increase/decrease volume");
                        break;
                }
            }
        } catch (ex) {
            reportException(ex);
            exit(1);
        }
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {

        Base.onFrame(theTime);
        /* if (window.fps < 60) {
            print("miss: " + window.fps + " / " + millisec());
        }*/
        if (_myMovieNode && _myMovieOverlay && _myMovieOverlay.visible) {            
            //_myFrameRateLimiter.onFrame(theTime);
            var myFrameDiff = _myMovieNode.currentframe - _myLastFrame;
            if (myFrameDiff > 1) {
                var myMissedFrameDiff = myFrameDiff-1;
                if (myMissedFrameDiff > _myMaxMissedFrame ) {
                    _myMaxMissedFrame = myMissedFrameDiff;
                    print("frames total : " + _myFrameCounter + ", misses: " + myMissedFrameDiff + ", sum : " + _myMissedFrameCounter + ", max : " + _myMaxMissedFrame);
                }
                _myMissedFrameCounter += myMissedFrameDiff;
            }

            if (myFrameDiff >0) {
                _myFrameCounter += myFrameDiff;
            }
            _myLastFrame = _myMovieNode.currentframe;
        }
        if (_myCaptureNode && _myMovieOverlay && _myMovieOverlay.visible) {
            //_myFrameRateLimiter.onFrame(theTime);
            window.scene.loadCaptureFrame(_myCaptureNode);
        }
        if (_mySoundId != -1 && !isPlaying(_mySoundId)) {
			nextFile();
        }
    }

    self.getMovieNode = function() {
        return _myMovieNode;
    }

    self.getCaptureNode = function() {
        return _myCaptureNode;
    }

    self.getMovieOverlay = function() {
        return _myMovieOverlay;
    }

    Base.onPostRender = self.onPostRender;
    self.onPostRender = function() {
        Base.onPostRender();
        if (_myTextOverlay.visible) {
            var myText  = getDisplayText();
            var myWidth = 0;
            for (var i = 0; i < myText.length; ++i) {
                window.renderText(new Node("<style textColor='[1,1,1,1]'/>").childNode(0), [30, (40 + i * 20)], myText[i], "Screen15");
                myWidth = Math.max(myWidth, myText[i].length * 10);
            }
            _myTextOverlay.width  = myWidth;
            _myTextOverlay.height = myText.length * 20 + 35;
        }
    }

    Base.onMouseMotion = self.onMouseMotion;
    self.onMouseMotion = function(theX, theY) {

        if (_myDragStart != null) {
            _myPanning.x -= theX - _myDragStart.x;
            _myPanning.y -= theY - _myDragStart.y;
            _myDragStart = new Vector2f(theX, theY);
            applyViewport();
        }

        // calculate pixel beneath mouse pointer
        _myZoomCenter.image.x = (_myPanning.x + theX) / _myZoomFactor;
        _myZoomCenter.image.y = (_myPanning.y + theY) / _myZoomFactor;

        _myZoomCenter.screen.x = theX;
        _myZoomCenter.screen.y = theY;

        if (Base.onMouseMotion) {
            Base.onMouseMotion(theX, theY);
        }
    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function(theButton, theState, theX, theY) {
        if (theButton == LEFT_BUTTON) {
            if (theState == BUTTON_DOWN) {
                _myDragStart = new Vector2f(theX, theY);
            } else {
                _myDragStart = null;
                if (_myImageOverlay) {
                    var myPixelX = (theX-_myImageOverlay.position.x)/_myZoomFactor;
                    var myPixelY = (theY-_myImageOverlay.position.y)/_myZoomFactor;
                    if (myPixelX >= 0 && myPixelY >= 0) {
                        print("Color: " + _myImageOverlay.image.raster.getPixel(myPixelX, myPixelY));
                    }
                }
            }
        }

        Base.onMouseButton(theButton, theState, theX, theY);
    }

    Base.onMouseWheel = self.onMouseWheel;
    self.onMouseWheel = function(theDeltaX, theDeltaY) {
        _myZoomFactor += (-theDeltaY) * 0.05;

        if (_myZoomFactor < MINZOOMFACTOR) {
            _myZoomFactor = MINZOOMFACTOR;
        }

        applyViewport();
        Base.onMouseWheel(theDeltaX, theDeltaY);
    }

    self.showFile = function (theIndex) {
        var myEntry = _myPlaylist.getEntry(theIndex);
        if (myEntry == null) {
            return;
        }

        // Stop current media
        if (_myMovieNode || _myCaptureNode) {
            stopMovie();
        }
        if (_mySoundId != -1 && isPlaying(_mySoundId)) {
            print("Stopping " + _mySoundId);
            stopSound(_mySoundId);
            _mySoundId = -1;
        }

        var myFilename = urlDecode(myEntry.href);
        _myCurrentMediaType = myEntry.mediaType;
        print("Now showing '" + myFilename + "'");
        window.title = "iv: '" + myFilename + "'";

        /// plug audio and capture plugs, video is handled inside y60 engine via decoderhint
        var myPlaylist = new Playlist();
        switch (myPlaylist.getMediaHintFromURL(myFilename)) {
            case IMAGE_MEDIA:
                print("Media: Image");
                break;                
            case AUDIO_MEDIA:
                print("Media: audio");
                break;
            case STREAMING_MEDIA:
                print("Media: stream");
                break;
            case CAPTURE_MEDIA:
                print("Media: Capture Video");
                if (!_myVideoCapturePlugged &&
                    (myFilename.search(/^video:\/\//i) != -1 || myFilename.search(/^dshow:\/\//i) != -1) ) {
                    if (OS == "LINUX") {
                        plug("DC1394");
                        print("plugged dc1394");
                    } else {
                        plug("DShowCapture");
                        print("plugged y60DShowCapture");
                    }
                    _myVideoCapturePlugged  = true;
                }
                break;
        }

        switch (myEntry.mediaType) {
            case IMAGE_MEDIA:
                showImage(myFilename);
                if (_myMovieOverlay) {
                    _myMovieOverlay.visible = false;
                }
                break;
            case STREAMING_MEDIA:
            case VIDEO_MEDIA:
                var mySeekableFlag = false;
                var myEnsureFramecount = false;
                if (_myTheaterFlag) {
                    myEnsureFramecount = false;
                }
                showMovie(myFilename, myPlaylist.getVideoDecoderHintFromURL(myFilename, mySeekableFlag), myEnsureFramecount);
                if (_myImageOverlay) {
                    _myImageOverlay.visible = false;
                }
                break;
            case AUDIO_MEDIA:
                _mySoundId = playSound(myFilename, [1.0,1.0], false);
                if (_mySoundId == -1) {
                    print("Sorry, File not found : " + myFilename);
                }
                break;
            case CAPTURE_MEDIA:
                showCapture(myFilename);
                if (_myImageOverlay) {
                    _myImageOverlay.visible = false;
                }
                break;
        }
    }

    self.setFilter = function (filter, filter_params) {
        _myImageNode.filter = filter;
        _myImageNode.filter_params = filter_params;
    }

    ////////////// private members ////////////

    function stepFrame(theForwardFlag) {
        if (_myMovieNode && _myMovieOverlay.visible) {
            var myStepSize = Math.abs(_myMovieNode.playspeed);
			_myMovieNode.playmode = "pause";
            if (theForwardFlag) {
                _myMovieNode.currentframe += myStepSize;
            } else {
                _myMovieNode.currentframe -= myStepSize;
            }
        }
    }

    function seekToOffset(theForwardFlag, theSeekOffset) {
        if (_myMovieNode && _myMovieOverlay.visible) {
            _myMovieNode.playmode = "pause";
            if (theForwardFlag) {
                _myMovieNode.currentframe += theSeekOffset;
            } else {
                _myMovieNode.currentframe -= theSeekOffset;
            }
        }
    }
    function setPlayMode(theKey) {
        if (_myMovieNode && _myMovieOverlay.visible) {
            if (theKey == "s") {
                _myMovieNode.playmode = "stop";
            } else if (theKey == "p") {
                if (_myMovieNode.playmode == "play") {
                    _myMovieNode.playmode = "pause";
                } else {
                    _myMovieNode.playmode = "play";
                }
            }
        } else if (_myCaptureNode && _myMovieOverlay.visible) {
            if (theKey == "s") {
                _myCaptureNode.playmode = "stop";
            } else if (theKey == "p") {
                if (_myCaptureNode.playmode == "play") {
                    _myCaptureNode.playmode = "pause";
                } else {
                    _myCaptureNode.playmode = "play";
                }
            }
        } else if (_mySoundId != -1) {
            if (theKey == "s") {
                stopSound(_mySoundId);
            } else if (theKey == "p") {
                pauseSound(_mySoundId);
            }
        }
    }

    function setPlaySpeed(theIncreaseFlag) {
        if (_myMovieNode && _myMovieOverlay.visible) {
            if (theIncreaseFlag) {
                _myMovieNode.playspeed++;
                if (_myMovieNode.playspeed > 10) {
                    _myMovieNode.playspeed = 10;
                } else if (_myMovieNode.playspeed > -1 && _myMovieNode.playspeed < 1) {
                    _myMovieNode.playspeed = 1;
                }
            } else {
                _myMovieNode.playspeed--;
                if (_myMovieNode.playspeed < -10) {
                    _myMovieNode.playspeed = -10;
                } else if (_myMovieNode.playspeed > -1 && _myMovieNode.playspeed < 1) {
                    _myMovieNode.playspeed = -1;
                }
            }
        }
    }

    function setVolume(theIncreaseFlag) {
        if (_myMovieNode && _myMovieOverlay.visible) {
            if (theIncreaseFlag) {
                _myMovieNode.volume[0] *= 1.1;
                _myMovieNode.volume[1] *= 1.1;
                if (_myMovieNode.volume[0] > 1) {
                    _myMovieNode.volume[0] = 1;
                }
                if (_myMovieNode.volume[1] > 1) {
                    _myMovieNode.volume[1] = 1;
                }
            } else {
                _myMovieNode.volume[0] *= 0.9;
                _myMovieNode.volume[1] *= 0.9;
            }
        } else if (_myCurrentMediaType == AUDIO_MEDIA) {
            if (theIncreaseFlag) {
                _myMasterVolume *= 1.1;
            } else {
                _myMasterVolume *= 0.9;
            }
            setMasterVolume(_myMasterVolume);
            print("Set mastervolume to : " + _myMasterVolume);
        }

    }

    function getSize() {
        if (_myImageOverlay && _myImageOverlay.visible) {
            return getImageSize(_myImageNode);
        } else if (_myMovieNode) {
            if (_myMovieNode.src.search(/\.m60/) != -1) {
                return getImageSize(_myMovieNode);
            } else {
                var height = _myMovieNode.height;
                var width = Math.round(_myMovieNode.height * _myMovieNode.aspectratio);
                if (width > _myMovieNode.width) {
                    width = _myMovieNode.width;
                    height = Math.round(width / _myMovieNode.aspectratio);
                }
                return new Vector2i(width, height);
            }
        } else if (_myCaptureNode) {
            return new Vector2i(_myCaptureNode.width, _myCaptureNode.height);
        }
        return null;
    }

    function getDisplayText() {
        if (_myCurrentMediaType == AUDIO_MEDIA) {
            var myTitle = urlDecode(_myPlaylist.getEntry(_myFileIndex).title);
            if (myTitle == "" || myTitle == undefined) {
                myString = "playing audio: " + urlDecode(_myPlaylist.getEntry(_myFileIndex).href);
            } else {
                myString = "playing audio: " + urlDecode(_myPlaylist.getEntry(_myFileIndex).title);
            }
        } else {
            var myNode;
            var myTexture;
            if (_myImageOverlay && _myImageOverlay.visible) {
                myNode = _myImageNode;
                myTexture = _myImageOverlay.node.getElementById(_myImageOverlay.textureunit.texture);
            } else {
                myNode = _myMovieNode;
                myTexture = _myMovieOverlay.node.getElementById(_myMovieOverlay.textureunit.texture);
            }
            if (!myNode) {
                return ["neither AUDIO nor VIDEO"];
            }

            var myString = myNode.src + "\n";
            if (_myPlaylist.getNumEntries() > 1) {
                myString += " (" + (_myFileIndex+1) + "/" + _myPlaylist.getNumEntries() + ")";
            }

            var mySize = getSize();
            myString += "\n";
            myString += "Size:                 " + mySize.x + "x" + mySize.y +"\n";
            myString += "Raster Pixelformat:   " + myNode.rasterpixelformat +"\n";
            myString += "Texture Pixelformat:  " + myTexture.texturepixelformat +"\n";
            myString += "Internal Pixelformat: " + myTexture.internal_format +"\n";
            if (myNode.nodeName == "movie") {
                myString += "Frame:         " + (myNode.currentframe + 1) + " / " + myNode.framecount +"\n";
                myString += "Time:          " + ((myNode.currentframe + 1)/myNode.fps).toFixed(3) + " / " + ((myNode.framecount == -1) ? -1 : (myNode.framecount/myNode.fps).toFixed(3)) +"\n";
                myString += "Framerate:     " + myNode.fps.toPrecision(5) +"\n";
                myString += "Playmode:      " + myNode.playmode +"\n";
                myString += "Playspeed:     " + myNode.playspeed.toPrecision(5) +"\n";
                myString += "Volume:        " + myNode.volume +"\n";
                myString += "cachesize:     " + myNode.cachesize +"\n";
                myString += "avdelay:       " + myNode.avdelay.toPrecision(3) + "\n";
                myString += "Total frames:  " + _myFrameCounter + "\n";
                myString += "Decoder:       " + myNode.decoder + "\n";
                myString += "AudioStream:   " + myNode.audio_stream + "\n";
                myString += "Misses:  sum:  " + _myMissedFrameCounter + ", max: " + _myMaxMissedFrame + "\n";
            }
            myString += "Zoom:          " + (_myZoomFactor*100).toFixed(1) + "%\n";
        }
        return myString.split("\n");
    }

    function stopMovie() {
        if (_myMovieNode) {
            print("Removing MovieNode");
            _myMovieNode.playmode = "stop";
            window.scene.images.removeChild(_myMovieNode);
            _myMovieNode = null;
        }
        if (_myCaptureNode) {
            print("Removing CaptureNode");
            window.scene.images.removeChild(_myCaptureNode);
            _myCaptureNode = null;
        }
    }

    function showCapture(theURL) {
        if (!_myCaptureNode) {
            _myCaptureNode = Node.createElement("capture");
            window.scene.images.appendChild(_myCaptureNode);
            _myCaptureNode.name = "IVCaptureNode";
            _myCaptureNode.id = createUniqueId();
            _myCaptureNode.playmode = "play";
            _myCaptureNode.src = theURL;
            _myMovieOverlay = new MovieOverlay(window.scene, _myCaptureNode);
        }

        if (_myFullSizeMode) {
            _myCaptureNode.matrix = new Matrix4f();
        }
        _myCaptureNode.src = theURL;
        window.scene.loadCaptureFrame(_myCaptureNode)
        _myCaptureNode.playmode = "play";
        _myMovieOverlay.width  = _myCaptureNode.width;
        _myMovieOverlay.height = _myCaptureNode.height;

        _myMovieOverlay.visible = true;
        applyViewport();
    }

    function showMovie(theFilename, theDecoderHint, theEnsureFrameCount) {
        if (!_myMovieNode) {
            _myMovieNode = Node.createElement("movie");
            window.scene.images.appendChild(_myMovieNode);
            _myMovieNode.name = "IVMovieNode";
            _myMovieNode.resize="none";
            _myMovieNode.id = createUniqueId();
            _myMovieNode.loopcount = 0;
            _myMovieNode.audio = 1;
            _myMovieNode.decoderhint = theDecoderHint;
            _myMovieNode.audio_stream = 0;
            //_myMovieNode.maxcachesize = 32;
            _myMovieNode.targetpixelformat = "RGB";//"YUVA420";//"YUV420";//"ALPHA";//"LUMINANCE8";//"RGB" // "ALPHA";//"RGBA8";
        }

        if (_myFullSizeMode) {
            _myMovieNode.matrix = new Matrix4f();
        }

        _myMovieNode.src = theFilename;
        _myMovieNode.playmode = "play";
        _myMovieNode.volume = [1.0, 1.0];
        if (theEnsureFrameCount) {
            window.scene.ensureMovieFramecount(_myMovieNode);
        }
        
        if (!_myMovieOverlay) {
            _myMovieOverlay = new MovieOverlay(window.scene, _myMovieNode);
        } else {
            _myMovieOverlay.movie = _myMovieNode;
        }

        _myMovieOverlay.width  = _myMovieNode.width;
        _myMovieOverlay.height = _myMovieNode.height;

        _myMovieOverlay.visible = true;
        applyViewport();
    }

    function showImage(theFilename) {
        if (!_myImageNode) {
            _myImageOverlay = new ImageOverlay(window.scene, theFilename);
            _myImageOverlay.leftborder   = 1;
            _myImageOverlay.rightborder  = 1;
            _myImageOverlay.bottomborder = 1;
            _myImageOverlay.topborder    = 1;
            _myImageNode = _myImageOverlay.image;
        }

        _myImageNode.src = theFilename;

        if (_myFullSizeMode) {
            _myImageNode.matrix = new Matrix4f();
        }
        _myImageOverlay.visible = true;
        applyViewport();
    }

    function updateFileView() {
        self.showFile(_myFileIndex);
    }

    function applyViewport() {
        var myOverlay = null;
        if (_myImageOverlay && _myImageOverlay.visible) {
            myOverlay = _myImageOverlay;
        } else {
            myOverlay = _myMovieOverlay;
        }

        var mySize = getSize();
        if (_myTheaterFlag) {
            var myRatio = mySize.x/mySize.y;
            if (myRatio > 1.0) {
                if (window.width > mySize.x) {
                    myOverlay.width = window.width * _myZoomFactor;
                    myOverlay.height = window.width / myRatio * _myZoomFactor;
                } else {
                    myOverlay.width = mySize.x * _myZoomFactor;
                    myOverlay.height = mySize.x / myRatio * _myZoomFactor;
                }
            } else {
                if (window.height > mySize.y) {
                    myOverlay.width = window.height / myRatio * _myZoomFactor;
                    myOverlay.height = window.height * _myZoomFactor;
                } else {
                    myOverlay.width = mySize.y / myRatio * _myZoomFactor;
                    myOverlay.height = mySize.y * _myZoomFactor;
                }
            }
        } else {
            myOverlay.width  = mySize.x * _myZoomFactor;
            myOverlay.height = mySize.y * _myZoomFactor;
        }
        myOverlay.position = new Vector2f((window.width - myOverlay.width) / 2,
            (window.height - myOverlay.height) /2);
        myOverlay.position.x -= _myPanning.x;
        myOverlay.position.y -= _myPanning.y;
    }

    function previousFile() {
        --_myFileIndex;
        if (_myFileIndex < 0) {
            _myFileIndex = _myPlaylist.getNumEntries() - 1;
        }
        updateFileView();
    }

    function nextFile() {
        ++_myFileIndex;
        if (_myFileIndex >= _myPlaylist.getNumEntries()) {
            _myFileIndex = 0;
        }
        updateFileView();
    }
}

//
// main
//
try {
    if (__main__ == "ImageViewer") {
        var myWidth = 800;
        var myHeight = 600;
        var myTheaterFlag = false;
        var ourImageViewerApp = new ImageViewerApp(arguments);

        print(arguments)
        for (var i = 0; i < arguments.length; ++i) {
            if (arguments[i].search(/resolution/) != -1) {
                myWidth = arguments[i + 1];
                myHeight = arguments[i + 2];
                arguments.splice(i,3);
            }
            if (arguments[i].search(/theater/) != -1) {
                myTheaterFlag = arguments[i + 1] == "1" ? true:false;
                arguments.splice(i,2);
            }
        }
        ourImageViewerApp.setup(myWidth, myHeight, "iv", myTheaterFlag);
        ourImageViewerApp.go();
    }
} catch (ex) {
    reportException(ex);
}
