//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

if (__main__ == undefined) var __main__ = "ImageViewer";

use("SceneViewer.js");
use("FrameRateLimiter.js");
//use("AudioController.js");
use("SoundController.js");
use("Playlist.js");

const MINZOOMFACTOR = 0.001;
const FRAME_RATE    = 100;
const OS = operatingSystem();
print(OS);

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
    var _myWMVPlugged       = false;
    var _myWMAPlugged       = false;
    var _myMPEGPlugged      = false;
    var _myQTPlugged        = false;
    var _myVFWCapturePlugged= false;
    var _myDShowCapturePlugged = false;
    var _myMissedFrameCounter = 0;
    var _myMaxMissedFrame     = 0;
    var _myLastFrame          = 0;
    var _myFrameCounter       = 0;
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
    self.setup = function(theWidth, theHeight, theTitle) {
        self.setSplashScreen(false);
        Base.setup(theWidth, theHeight, false, theTitle);
        for(var i=1; i<theArguments.length; ++i) {
            switch(theArguments[i]) {
            case "recursive": // would like to use '--recursive' but that's rejected by the acxpshell options
                _myPlaylist.setRecursiveFlag(true);
                break;
            default:
                print("adding entry: " + theArguments[i]);
                _myPlaylist.addEntry(theArguments[i]);
                break;
            }
        }
        if (_myPlaylist.getNumEntries() == 0) {
            print("Nothing to show!");
            exit(0);
        }

        updateFileView();
        createTextOverlay();
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
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
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);
        if (_myMovieNode && _myMovieOverlay && _myMovieOverlay.visible) {
            //_myFrameRateLimiter.onFrame(theTime);
            var myFrameDiff = _myMovieNode.currentframe - _myLastFrame;
            if (myFrameDiff > 1) {
                if (myFrameDiff > _myMaxMissedFrame ) {
                    _myMaxMissedFrame = myFrameDiff;
                    print("frames total : " + _myFrameCounter + ", misses: " + myFrameDiff + ", sum : " + _myMissedFrameCounter + ", max : " + _myMaxMissedFrame);                    
                }
                _myMissedFrameCounter += myFrameDiff;                
            } 
            window.scene.loadMovieFrame(_myMovieNode, theTime);
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
            window.setTextColor([1,1,1,1], [1,1,1,1]);
            var myText  = getDisplayText();
            var myWidth = 400;
            for (var i = 0; i < myText.length; ++i) {
                window.renderText([30, (40 + i * 20)], myText[i], "Screen15");
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
        print("Now showing " + myFilename);

        if (myFilename.search(/^video:\/\//i) != -1) {
            plug("y60VFWCapture");
            print("VideoForWindows dshow");
        } else if (myFilename.search(/^dshow:\/\//i) != -1) {
            plug("y60DShowCapture");
            print("plugged dshow");
        }

        if (!_myMPEGPlugged && (myFilename.search(/\.mpg$/i)  != -1
                                || myFilename.search(/\.m2v$/i)  != -1
                                || myFilename.search(/\.avi$/i)  != -1
                                || myFilename.search(/\.mpeg$/i) != -1
                                /* || myFilename.search(/\.ra$/i)   != -1 */
                                )) {
            plug("y60FFMpegDecoder2");
            print("plugged ffmpegdecoder 2");
            _myMPEGPlugged = true;
        }
        if ( myFilename.search(/\.mov$/i) != -1) {
            if (OS == "Linux" && !_myMPEGPlugged) {
                plug("y60FFMpegDecoder");
                _myMPEGPlugged = true;
                print("y60FFMpegDecoder")
            } else if (OS == "Win32" && !_myQTPlugged){
                plug("y60QuicktimeDecoder");
                _myQTPlugged = true;
                print("y60QuicktimeDecoder")
            }
        }
        if (!_myWMVPlugged &&
            (myFilename.search(/\.wmv$/i) != -1 ||
             myFilename.search(/\.avi$/i) != -1)) {
            if (OS == "Win32") {
                plug("y60WMVDecoder");
                _myWMVPlugged = true;
            } else {
                plug("y60FFMpegDecoder2");
                _myMPEGPlugged = true;
            }
        }
        if (!_myWMAPlugged &&
            (myFilename.search(/\.wma$/i) != -1 ||
             myFilename.search(/\.asf$/i) != -1 ||
             myFilename.search(/\.mp3$/i) != -1 ||
             myFilename.search(/^mms:\/\//i) != -1 ||
             myFilename.search(/^http:\/\//i) != -1))
             // default wma fallback: i.e. http://server/dir/
        {
            plug("y60WMADecoder");
            _myWMAPlugged = true;
        }
        if (!_myVFWCapturePlugged && myFilename.search(/^video:\/\//i) != -1) {
            plug("y60VFWCapture");
            _myVFWCapturePlugged = true;
        }
        if (!_myDShowCapturePlugged && myFilename.search(/^dshow:\/\//i) != -1) {
            plug("y60DShowCapture");
            _myDShowCapturePlugged = true;
        }
        switch (myEntry.mediaType) {
        case IMAGE_MEDIA:
            showImage(myFilename);
            if (_myMovieOverlay) {
                _myMovieOverlay.visible = false;
            }
            break;
        case VIDEO_MEDIA:
            showMovie(myFilename);
            if (_myImageOverlay) {
               _myImageOverlay.visible = false;
            }
            break;
        case AUDIO_MEDIA:
            _mySoundId = playSound(myFilename, 1.0, false);
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
        getImageNode().filter = filter;
        getImageNode().filter_params = filter_params;
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
                _myMovieNode.volume *= 1.1;
            } else {
                _myMovieNode.volume *= 0.9;
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

    function createTextOverlay() {
        var myImage = self.getImageManager().getImageNode("OSD_Overlay");
        myImage.src = "shadertex/one_white_pixel.png";
        window.scene.update(Scene.IMAGES);

        _myTextOverlay = new ImageOverlay(self.getOverlayManager(), myImage);
        _myTextOverlay.width  = 200;
        _myTextOverlay.height = 200;
        _myTextOverlay.color = new Vector4f(0,0,0,0.6);
    }

    function getSize() {
        if (_myImageOverlay && _myImageOverlay.visible) {
            return getImageSize(_myImageNode);
        } else if (_myMovieNode) {
            if (_myMovieNode.src.search(/\.m60/) != -1) {
                return getImageSize(_myMovieNode);
            } else {
                return new Vector2i(_myMovieNode.width, _myMovieNode.height);
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
            if (_myImageOverlay && _myImageOverlay.visible) {
                myNode = _myImageNode;
            } else {
                myNode = _myMovieNode;
            }
            if (!myNode) {
                return "neither AUDIO nor VIDEO";
            }
            var myString = myNode.src + "\n";

            if (_myPlaylist.getNumEntries() > 1) {
                myString += " (" + (_myFileIndex+1) + "/" + _myPlaylist.getNumEntries() + ")";
            }

            var mySize = getSize();
            myString += "\nSize:         " + mySize.x + "x" + mySize.y +"\n";
            myString += "Encoding:     " + myNode.pixelformat +"\n";
            if (myNode.nodeName == "movie") {
                myString += "Frame:        " + (myNode.currentframe + 1) + " / " + myNode.framecount +"\n";
                myString += "Framerate:    " + myNode.fps.toPrecision(5) +"\n";
                myString += "Playmode:     " + myNode.playmode +"\n";
                myString += "Playspeed:    " + myNode.playspeed.toPrecision(5) +"\n";
                myString += "Volume:       " + myNode.volume.toFixed(2) +"\n";
                myString += "cachesize:    " + myNode.cachesize +"\n";
                myString += "avdelay:      " + myNode.avdelay.toPrecision(3) + "\n";
                myString += "Total frames: " + _myFrameCounter + "\n";                
                myString += "Misses:       sum: " + _myMissedFrameCounter + ", max: " + _myMaxMissedFrame + "\n";                
            }
            myString += "Zoom:         " + (_myZoomFactor*100).toFixed(1) + "%\n";
        }
        return myString.split("\n");
    }

    function stopMovie() {
        if (_myMovieNode) {
            print("Removing MovieNode");
            self.getImages().removeChild(_myMovieNode);
            _myMovieNode = null;
        }
        if (_myCaptureNode) {
            print("Removing MovieNode");
            self.getImages().removeChild(_myCaptureNode);
            _myCaptureNode = null;
        }
    }

    function showCapture(theURL) {
        if (!_myCaptureNode) {
            _myCaptureNode = Node.createElement("capture");
            self.getImages().appendChild(_myCaptureNode);
            _myCaptureNode.name = "IVCaptureNode";
            _myCaptureNode.id = createUniqueId();
            _myCaptureNode.playmode = "play";
            _myMovieOverlay = new MovieOverlay(self.getOverlayManager(), _myCaptureNode);
        }

        if (_myFullSizeMode) {
            _myCaptureNode.matrix = new Matrix4f();
        }

        _myCaptureNode.src = theURL;
        if (!window.scene.loadCaptureFrame(_myCaptureNode)) {
            self.getImages().removeChild(_myCaptureNode);
            exit(0);
        }

        _myCaptureNode.playmode = "play";
        _myMovieOverlay.width  = _myCaptureNode.width;
        _myMovieOverlay.height = _myCaptureNode.height;

        _myMovieOverlay.visible = true;
        applyViewport();
    }

    function showMovie(theFilename) {
        if (!_myMovieNode) {
            _myMovieNode = Node.createElement("movie");
            self.getImages().appendChild(_myMovieNode);
            _myMovieNode.name = "IVMovieNode";
            _myMovieNode.id = createUniqueId();
            _myMovieNode.playmode = "play";
            _myMovieNode.loopcount = 0; // indefinitively
            _myMovieNode.audio = 1;
            _myMovieOverlay = new MovieOverlay(self.getOverlayManager(), _myMovieNode);
        }

        if (_myFullSizeMode) {
            _myMovieNode.matrix = new Matrix4f();
        }

        _myMovieNode.src = theFilename;
        try {
            window.scene.loadMovieFrame(_myMovieNode);
        } catch (ex) {
            self.getImages().removeChild(_myMovieNode);
            print(ex);
            print("### ERROR: loadMovieFrame faild for file: " + theFilename);
            return;
        }

        _myMovieNode.playmode = "play";
        _myMovieOverlay.width  = _myMovieNode.width;
        _myMovieOverlay.height = _myMovieNode.height;

        _myMovieOverlay.visible = true;
        applyViewport();
    }

    function getImageNode() {
        if (!_myImageNode) {
            _myImageNode    = self.getImageManager().getImageNode("IVImageNode");
            _myImageNode.resize = "pad";
            _myImageOverlay = new ImageOverlay(self.getOverlayManager(), _myImageNode);
            _myImageOverlay.leftborder   = 1;
            _myImageOverlay.rightborder  = 1;
            _myImageOverlay.bottomborder = 1;
            _myImageOverlay.topborder    = 1;
        }
        return _myImageNode;
    }

    function showImage(theFilename) {
        var myImageNode = getImageNode();
        myImageNode.src = theFilename;
        window.scene.update(Scene.IMAGES);
        if (_myFullSizeMode) {
            myImageNode.matrix = new Matrix4f();
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
        myOverlay.width  = mySize.x * _myZoomFactor;
        myOverlay.height = mySize.y * _myZoomFactor;
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
        var ourImageViewerApp = new ImageViewerApp(arguments);
        ourImageViewerApp.setup(800, 600, "ImageViewer");
        window.backgroundColor = [0.5,0.5,0.5];
        ourImageViewerApp.go();
    }
} catch (ex) {
    reportException(ex);
}
