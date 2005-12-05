//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: GlurViewer.js,v $
//   $Author: david $
//   $Revision: 1.6 $
//   $Date: 2005/03/23 17:48:44 $
//
//
//=============================================================================

if (__main__ == undefined) var __main__ = "ImageViewer";

use("SceneViewer.js");
plug("GlurFilter");

const MINZOOMFACTOR = 0.001;

function ImageViewerApp(theArguments) {
    this.Constructor(this, theArguments);
}

ImageViewerApp.prototype.Constructor = function(self, theArguments) {
    var _myTextOverlay = null;
    var _myMovieNode   = null;
    var _myGlowNode = null;
    var _myGlowOverlay = null;
    var _myMovieOverlay = null;
    var _myFileList = [];
    var _myFileIndex = 0;
    var _isPlayingMovie = false;
    var _myRecursiveFlag = false;
    var _myZoomFactor = 1.0;
    var _myPanning = new Vector2f(0,0);
    var _myZoomWheelMode = 0;

    var _myZoomCenter = {image: new Vector2f(0,0), screen: new Vector2f(0,0)};
    var _myDragStart = null;


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
        Base.setup(theWidth, theHeight, theTitle);
        for(var i=1; i<theArguments.length; ++i) {
            switch(theArguments[i]) {
            case "recursive":
                _myRecursiveFlag = true;
                break;
            default:
                print(">"+theArguments[i]+"<");
                var files = buildFileList(theArguments[i]);
                _myFileList = _myFileList.concat(files);
            }
        }
        print("Files to show: " + _myFileList);
        if (_myFileList.length==0) {
            print("Nothing to show!")
            exit(0);
        }

        createTextOverlay();
        updateFileView();
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theKey == "left ctrl") {
            _myZoomWheelMode = theState;
        }
        if (theState && theKey == "space") {
            _myTextOverlay.visible = !_myTextOverlay.visible;
        } else if (theState && theKey == "right") {
            nextFile();
        } else if (theState && theKey == "left") {
            previousFile();
        } else {
            Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);
        if (_isPlayingMovie) {
            _myMovieNode.currentframe++;
            if (_myMovieNode.currentframe >= _myMovieNode.framecount) {
                _myMovieNode.currentframe = 0;
            }
            window.loadMovieFrame(_myMovieNode);
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
        //print(_myZoomCenter.image, _myZoomCenter.screen);
        if (Base.onMouseMotion) {
            Base.onMouseMotion(theX, theY);
        }
    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function(theButton, theState, theX, theY) {
        if (theButton == RIGHT_BUTTON) {
            if (theState == BUTTON_DOWN) {
                _myDragStart = new Vector2f(theX, theY);
            } else {
                _myDragStart = null;
            }
        }
        if (Base.onMouseButton) {
            Base.onMouseButton(theButton, theState, theX, theY);
        }
    }

    Base.onMouseWheel = self.onMouseWheel;
    self.onMouseWheel = function(theDeltaX, theDeltaY) {
        if (_myZoomWheelMode) {
            //print(theDeltaX, theDeltaY);
            _myZoomFactor += (-theDeltaY) * 0.05;
            //print(_myZoomFactor);
            if (_myZoomFactor < MINZOOMFACTOR) {
                _myZoomFactor = MINZOOMFACTOR;
            }

            _myPanning.x = _myZoomCenter.image.x  * _myZoomFactor - _myZoomCenter.screen.x;
            _myPanning.y = _myZoomCenter.image.y  * _myZoomFactor - _myZoomCenter.screen.y;

            applyViewport();
            updateOverlay(_myGlowNode);
        } else {
            switch(theDeltaY) {
                case -1: // UP
                    nextFile();
                    break;
                case 1: // DOWN
                    previousFile();
                    break;
            }
        }
        if (Base.onMouseWheel) {
            Base.onMouseWheel(theDeltaX, theDeltaY);
        }
    }

    self.showFile = function (filename) {
        print("Now showing "+filename);
        if (filename.search(/\.m60/) != -1) {
            showMovie(filename);
            updateOverlay(_myMovieNode);
            _isPlayingMovie = true;
            if (_myGlowOverlay) {
               _myGlowOverlay.visible = false;
            }
            _myMovieOverlay.visible = true;
        } else {
            showImage(filename);
            updateOverlay(_myGlowNode);
            _isPlayingMovie = false;
            _myGlowOverlay.visible = true;
            if (_myMovieOverlay) {
                _myMovieOverlay.visible = false;
            }
        }
    }
    self.setFilter = function (filter, filter_params) {
        _myGlowNode.filter = filter;
        _myGlowNode.filter_params = filter_params;
    }

    ////////////// private members ////////////

    function createTextOverlay() {
        window.loadTTF("Arial", "${PRO}/src/Y60/gl/text/fonts/arial.ttf", 18);
        window.setTextPadding(10,10,10,10);
        window.setTextColor([0.2,0.2,1,1], [1,1,1,1]);

        var myTextImage = self.getImageManager().getImageNode("infobox");
        _myTextOverlay = self.getOverlayManager().create("infobox", myTextImage.id, true);
        self.getOverlayManager().setApplyMode(_myTextOverlay.name, "decal");
        self.getOverlayManager().setBackgroundColor(_myTextOverlay.name, new Vector4f(0.8,0.8,1,0.6));

        var myTextSize = window.renderTextAsImage(myTextImage, "1, 2 to browse through images/movies\n<space> toggles OSD", "Arial", 300, 0);
        _myTextOverlay.width  = myTextSize.x;
        _myTextOverlay.height = myTextSize.y;
        _myTextOverlay.srcsize.x = myTextSize.x / myTextImage.width;
        _myTextOverlay.srcsize.y = myTextSize.y / myTextImage.height;

        _myTextOverlay.position = new Vector2f(10, 30);
        _myTextOverlay.visible = true;
    }

    function updateOverlay(theImage) {
        var myString = ""
        if (theImage.nodeName == "movie") {
            myString += "Movie: ";
        } else {
            myString += "Image: ";
        }
        myString += theImage.src + " (" + (_myFileIndex+1) + " of " + _myFileList.length + ")\n";
        myString += "Size: " + theImage.width + "x" + theImage.height+"\n";
        myString += "Encoding: " + theImage.pixelformat +"\n";
        if (theImage.nodeName == "movie") {
            myString += "Framecount: " + theImage.framecount +"\n";
        }
        myString += "Zoom: " + (_myZoomFactor*100).toFixed(1) + "%\n";
        var myTextImage = self.getImageManager().getImageNode("infobox");
        var myTextSize = window.renderTextAsImage(myTextImage, myString, "Arial", 300, 0);
        _myTextOverlay.width  = myTextSize.x;
        _myTextOverlay.height = myTextSize.y;
        _myTextOverlay.srcsize.x = myTextSize.x / myTextImage.width;
        _myTextOverlay.srcsize.y = myTextSize.y / myTextImage.height;
        self.getOverlayManager().moveToTop(_myTextOverlay);
    }

    function showMovie(theFilename) {
        if (!_myMovieNode) {
            _myMovieNode = Node.createElement("movie");
            self.getImages().appendChild(_myMovieNode);
            _myMovieNode.name = "IVMovieNode";
            _myMovieNode.id = window.getNextFreeNodeId("image");
            _myMovieOverlay = self.getOverlayManager().create("IVMovieOverlay", _myMovieNode.id, true);
        }

        _myMovieNode.src = theFilename;
        window.loadMovieFrame(_myMovieNode);

        _myMovieOverlay.width  = _myMovieNode.width;
        _myMovieOverlay.height = _myMovieNode.height;

        if (_myMovieOverlay.width>1024) {
            _myMovieOverlay.width=1024;
        }
        if (_myMovieOverlay.height>768) {
            _myMovieOverlay.height = 768;
        }

        //window.resizeTo(_myMovieOverlay.width, _myMovieOverlay.height);
    }

    function showImage(theFilename) {
        if (!_myGlowNode) {
            _myGlowNode = self.getImageManager().getImageNode("IVGlowNode");
            _myGlowOverlay = self.getOverlayManager().create("IVGlowOverlay", _myGlowNode.id, true);
        }
        _myGlowNode.src = theFilename;

        _myGlowNode.filter = "glur";

        print("Calculating gaussian curve");
        // taken from http://www.cazabon.com/pyCMS/PIL_usm.html
        var a = [];
        var radius = 65;    // this avtually is the diameter (must not be even!)
                            // only half of the calculated values are actually used (quick hack)
        for (var x = 0; x < radius; x++) {
            var z = ((x + 2)/(radius));
            var dev = 0.5 + (((radius * radius)) * 0.001); // you can adjust this factor to change the shape/center-weighting of the gaussian
            var y = Math.pow((1.0 / Math.sqrt(2.0 * 3.14159265359 * dev)), ((-(z - 1.0) * -(x - 1.0)) / (2.0 * dev)));
            print(y);
            if (x+1>radius/2) { // second half (including center value) is used only
                a.push( y );
            }
        }

        // make sure, all values sum up to 1.0 (play with this value!)

        var sum = 0;
        for (x = 0; x < a.length; x++) {
            sum += a[x];
        }

        for (x = 0; x < a.length; x++) {
            a[x] *= (1.0/sum);
        }

        s = "[0.5,1.0,0.5,0.0," + a + "]";
        print(s);
        _myGlowNode.filter_params = s;

        //window.updateScene(Renderer.IMAGES);

        applyViewport();
}


    function filterFilename(theFilename) {
        if (fileExists(theFilename) && theFilename[0] != '.') {
            var myDirList = getDirectoryEntries(theFilename);
            if (myDirList.length > 0) {
                print("ignoring: "+theFilename);
                print(myDirList.length);
                return false;
            }
            return true;
        } else {
            print("ignoring2: "+theFilename);
            return false;
        }
    }

    function buildFileList(theArgument) {
        var myList = []
        // find out if the argument is a directory
        // if so, add all files to the list
        var myDirList = getDirectoryEntries(theArgument);
        print(myDirList);
        if (myDirList.length!=0) {
            for(var i=0; i<myDirList.length; ++i) {
                if (_myRecursiveFlag) {
                    myList = myList.concat(buildFileList(myDirList[i]));
                } else {
                    if (filterFilename(myDirList[i])) {
                        myList.push(myDirList[i]);
                    }
                }
            }
        } else {
            // otherwise just add the argument
            if (filterFilename(theArgument)) {
                myList.push(theArgument);
            } else {
                print("Ignoring "+ theArgument);
            }
        }

        return myList;
    }

    function updateFileView() {
        self.showFile(_myFileList[_myFileIndex]);
    }

    function applyViewport() {
        var myImageSize  = getImageSize(_myGlowNode);
        var myGlowSize  = getImageSize(_myGlowNode);

        _myGlowOverlay.width  = myGlowSize.x * _myZoomFactor;
        _myGlowOverlay.height = myGlowSize.y * _myZoomFactor;
        _myGlowOverlay.position.x = -_myPanning.x;
        _myGlowOverlay.position.y = -_myPanning.y;
    }

    function previousFile() {
        --_myFileIndex;
        if (_myFileIndex<0) {
            _myFileIndex = 0;
        }
        updateFileView();
    }

    function nextFile() {
        ++_myFileIndex;
        if (_myFileIndex >= _myFileList.length) {
            _myFileIndex = _myFileList.length-1;
        }
        updateFileView();
    }

}

//
// main
//

if (__main__ == "ImageViewer") {
    print("ImageViewer");
    try {
        var ourImageViewerApp = new ImageViewerApp(arguments);

        ourImageViewerApp.setup(800, 600, "ImageViewer");
        ourImageViewerApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}
