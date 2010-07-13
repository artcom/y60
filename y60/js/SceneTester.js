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

/*jslint white:false, nomen:false, plusplus:false*/
/*globals use, print, window, ourShow, exit, Logger, Exception,
          OffscreenRenderer, SceneViewer, expandEnvironment, Scene,
          writeStringToFile, adjustNodeIds, Modelling, saveImageFiltered*/

use("SceneViewer.js");
use("OffscreenRenderer.js");
use("Exception.js");

function SceneTester(theArguments) {
    this.Constructor(this, theArguments);
}

SceneTester.WINDOW_WIDTH = 640;
SceneTester.WINDOW_HEIGHT = 480;

SceneTester.prototype.Constructor = function(obj, theArguments) {
    SceneViewer.prototype.Constructor(obj, theArguments);
    var Base = {};

    /////////////////////
    // private members //
    /////////////////////

    var _myTestDurationInFrames = 3;
    var _myOutputImageName      = null;
    var _myOutputSuffix         = null;
    // Default is rendering offscreen!
    var _myOffscreenFlag        = (expandEnvironment("${AC_NO_OFFSCREEN_TEST}") !== '1');
    var _mySaveLoadFlag         = false;
    var _myOffscreenRenderer    = null;

    /////////////////////
    // Private Methods //
    /////////////////////
    
    function registerHeadlights(theClonedCanvas) {
        var myViewport, myCamera, myHeadlight, i, j;
        var myLightmanager = ourShow.getLightManager();
        for (i = 0; i < theClonedCanvas.childNodesLength(); ++i) {
            myViewport = theClonedCanvas.childNode(i);
            myCamera = myViewport.getElementById(myViewport.camera);
            for(j = 0; j < myCamera.childNodesLength(); ++j) {
                myHeadlight = myCamera.childNode(j);
                myLightmanager.registerHeadlightWithViewport(myViewport, myHeadlight);
            }
        }
    }

    function deregisterHeadlights(theClonedCanvas) {
        var i, myViewport;
        var myLightmanager = ourShow.getLightManager();
        for (i = 0; i < theClonedCanvas.childNodesLength(); ++i) {
            myViewport = theClonedCanvas.childNode(i);
            myLightmanager.deregisterHeadlightFromViewport(myViewport);
        }
    }

    ////////////////////
    // Public Members //
    ////////////////////

    obj.MAX_FRAME_COUNT = 1000; // Terminate test after this many frames.
    obj.myFrameCount = 0;
    obj.myImageCount = 0;

    obj.__defineSetter__("saveLoadFlag", function(theFlag) {
        _mySaveLoadFlag = theFlag;
    });

    obj.__defineSetter__("offscreenFlag", function(theFlag) {
        _myOffscreenFlag = theFlag;
    });

    /*Base.onFrame = obj.onFrame;
    obj.onFrame = function (theTime) {
        Base.onFrame(theTime);
    }*/

    obj.setTestDurationInFrames = function(theFrameCount) {
        _myTestDurationInFrames = theFrameCount;
    };

    obj.getTestImageName = function() {
        var myImageFilename = "";
        if (obj.myImageCount > 0) {
            myImageFilename = _myOutputImageName + '.' + obj.myImageCount + _myOutputSuffix;
        } else {
            myImageFilename = _myOutputImageName + _myOutputSuffix;
        }
        obj.myImageCount++;
        return myImageFilename;
    };

    Base.getActiveViewport = obj.getActiveViewport;
    obj.getActiveViewport = function() {
        if (_myOffscreenRenderer) {
            return _myOffscreenRenderer.viewport;
        } else {
            return Base.getActiveViewport();
        }
    };

    obj.getRenderer = function() {
        if (_myOffscreenRenderer) {
            return _myOffscreenRenderer.renderarea.getRenderer();
        } else {
            return window.getRenderer();
        }
    };

    Base.getRenderWindow = obj.getRenderWindow;
    obj.getRenderWindow = function() {
        if (_myOffscreenRenderer) {
            return _myOffscreenRenderer.renderarea;
        } else {
            return window;
        }
    };

    obj.saveTestImage = function(theCount) {
        var myImageFilename = obj.getTestImageName();
        print("Writing test image to file: " + myImageFilename);
        if (_myOffscreenFlag) {
            var myImage = Modelling.createImage(window.scene, window.width, window.height, "rgba");
            myImage.resize = "none";

            var myCanvas = window.scene.canvases.firstChild;
            if (!myCanvas) {
                throw new Exception("No canvas found in scene");
            }
            var myClonedCanvas = myCanvas.cloneNode(true);
            adjustNodeIds(myClonedCanvas);
            window.scene.canvases.appendChild(myClonedCanvas);

            var myViewport = myClonedCanvas.firstChild;
            if (!myViewport) {
                throw new Exception("No viewport found");
            }
            var myCamera = window.scene.dom.getElementById(myViewport.camera);

            _myOffscreenRenderer = new OffscreenRenderer([window.width, window.height], myCamera, "rgba", myImage, myClonedCanvas, true, 0);
            _myOffscreenRenderer.setBody(window.scene.world);
            ourShow.activeWindow = _myOffscreenRenderer.renderarea;

            _myOffscreenRenderer.onPreViewport  = ourShow.onPreViewport;
            _myOffscreenRenderer.onPostViewport = ourShow.onPostViewport;
            _myOffscreenRenderer.onPreRender    = ourShow.onPreRender;
            _myOffscreenRenderer.onPostRender   = ourShow.onPostRender;

            registerHeadlights(myClonedCanvas);
            _myOffscreenRenderer.render(true);
            deregisterHeadlights(myClonedCanvas);

            saveImageFiltered(_myOffscreenRenderer.image, myImageFilename, ["flip"], [[]]);

            window.scene.canvases.removeChild(myClonedCanvas);
            window.scene.images.removeChild(myImage);

            _myOffscreenRenderer = null;
        } else {
            window.saveBuffer(myImageFilename);
        }
    };

    obj.testSaveLoad = function() {
        var myXmlFilename = 'saved_testscene.x60';
        print("Writing scene to xml file: " + myXmlFilename);
        window.scene.save(myXmlFilename, false, false);

        var myBinXmlFilename = 'saved_testscene.b60';
        print("Writing scene to binary file: " + myBinXmlFilename);
        window.scene.save(myBinXmlFilename, true);

        print("Loading xml scene");
        var myNewXmlScene = new Scene(myXmlFilename);

        print("Setting up xml scene");
        myNewXmlScene.setup();

        print("Loading binary scene");
        var myNewBinScene = new Scene(myBinXmlFilename);

        print("Setting up binary scene");
        myNewBinScene.setup();

        if (myNewXmlScene.dom.toString() !== myNewBinScene.dom.toString()) {
            print("comparing " + myXmlFilename + " and " + myBinXmlFilename + " failed.");
            writeStringToFile("saved_binary_testscene.x60", myNewBinScene.dom.toString());
            writeStringToFile("saved_xml_testscene.x60", myNewXmlScene.dom.toString());
            print("inspect 'saved_binary_testscene.x60' and 'saved_xml_testscene.x60' for differences");
            exit(1);
        }
    };

    Base.onPostRender = obj.onPostRender;
    obj.onPostRender = function() {
        Base.onPostRender();
        // avoid calling of this block when rendering test offscreen
        try {
            if (_myOutputImageName && !_myOffscreenRenderer) {
                ++obj.myFrameCount;
                if (obj.myFrameCount > obj.MAX_FRAME_COUNT) {
                    if (!_myOutputImageName) {
                        print ("Maximum frame count " + obj.myFrameCount + " reached - something went wrong. Terminating test");
                        exit(1);
                    }
                }

                obj.onFrameDone(obj.myFrameCount);
            }
        } catch(ex) {
            print("### ERROR: Exception caught: " + ex);
            exit(1);
        }
    };

    obj.onFrameDone = function(theFrameCount) {
        if (_myOutputImageName &&
            theFrameCount === _myTestDurationInFrames) {
            obj.saveTestImage();
            if (_mySaveLoadFlag) {
                obj.testSaveLoad();
            }
            exit(0);
        }
    };
    
    Base.setup = obj.setup;
    obj.setup = function(theWidth, theHeight) {
        theWidth = theWidth || SceneTester.WINDOW_WIDTH;
        theHeight = theHeight || SceneTester.WINDOW_HEIGHT;
        
        Base.setup(theWidth, theHeight, false, "Scene Tester");
        window.position = [0,0]; // why?
        if (_myOffscreenFlag) {
            Logger.info("tester running in offscreen mode");
        } else {
            Logger.info("tester not running in offscreen mode");
        }
    };
    
    for (var i = 0; i < theArguments.length; ++i) {
        var lastEqualPosition;
        var myArgument = theArguments[i];
        
        if (myArgument.search(/^outputimage/) !== -1) {
            lastEqualPosition = myArgument.lastIndexOf('=');
            _myOutputImageName = myArgument.substr(lastEqualPosition + 1);
        }
        if (myArgument.search(/^outputsuffix/) !== -1) {
            lastEqualPosition = myArgument.lastIndexOf('=');
            _myOutputSuffix = myArgument.substr(lastEqualPosition + 1);
        }
    }
};
