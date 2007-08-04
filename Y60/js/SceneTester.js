//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneViewer.js");
// use("UnitTest.js");
use("OffscreenRenderer.js");
use("Exception.js");

const WINDOW_WIDTH = 640;
const WINDOW_HEIGHT = 480;

function SceneTester(theArguments) {
    this.Constructor(this, theArguments);
}

SceneTester.prototype.Constructor = function(obj, theArguments) {

    SceneViewer.prototype.Constructor(obj, theArguments);

    var Base = {};

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    obj.MAX_FRAME_COUNT = 1000; // Terminate test after this many frames.
    obj.myFrameCount = 0;
    obj.myImageCount = 0;

    obj.saveLoadFlag setter = function(theFlag) {
        _mySaveLoadFlag = theFlag;
    }

    obj.offscreenFlag setter = function(theFlag) {
        _myOffscreenFlag = theFlag;
    }

    Base.onFrame = obj.onFrame; 
    obj.onFrame = function (theTime) {
        Base.onFrame(theTime);
    }

    obj.setTestDurationInFrames = function(theFrameCount) {
        _myTestDurationInFrames = theFrameCount;
    }

    obj.getTestImageName = function() {
        var myImageFilename = "";
        if (obj.myImageCount > 0) {
            myImageFilename = _myOutputImageName + '.' + obj.myImageCount + _myOutputSuffix;
        } else {
            myImageFilename = _myOutputImageName + _myOutputSuffix;
        }
        obj.myImageCount++;
        return myImageFilename;
    }

    Base.getActiveViewport = obj.getActiveViewport;
    obj.getActiveViewport = function() {
        if (_myOffscreenRenderer) {
            return _myOffscreenRenderer.viewport;
        } else {
            return Base.getActiveViewport();
        }
    }

    obj.getRenderer = function() {
        if (_myOffscreenRenderer) {
            return _myOffscreenRenderer.renderarea.getRenderer();
        } else {
            return window.getRenderer();
        }
    }

    Base.getRenderWindow = obj.getRenderWindow;
    obj.getRenderWindow = function() {
        if (_myOffscreenRenderer) {
            return _myOffscreenRenderer.renderarea;
        } else {
            return window;
        }
    }

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

            _myOffscreenRenderer.onPreViewport = ourShow.onPreViewport;
            _myOffscreenRenderer.onPostViewport = ourShow.onPostViewport;
            _myOffscreenRenderer.onPreRender = ourShow.onPreRender;
            _myOffscreenRenderer.onPostRender = ourShow.onPostRender;

            registerHeadlights(myClonedCanvas);
            _myOffscreenRenderer.render(true);
            deregisterHeadlights(myClonedCanvas);
            
            var myCameras = window.scene.cameras;
            var myAspect = myViewport.width / myViewport.height;

            saveImageFiltered(_myOffscreenRenderer.image, myImageFilename, ["flip"], [[]]);

            window.scene.canvases.removeChild(myClonedCanvas);
            window.scene.images.removeChild(myImage);
            
            _myOffscreenRenderer = null;
        } else {
            window.saveBuffer(myImageFilename);
        }
    }

    function registerHeadlights(theClonedCanvas) {
        var myLightmanager = ourShow.getLightManager();
        for (var i=0; i<theClonedCanvas.childNodesLength(); ++i) {
            var myViewport = theClonedCanvas.childNode(i);
            var myCamera = myViewport.getElementById(myViewport.camera);
            
            for(var j=0; j<myCamera.childNodesLength(); ++j) {
                var myHeadlight = myCamera.childNode(j);
                myLightmanager.registerHeadlightWithViewport(myViewport, myHeadlight);
            }
        }
    }
    
    function deregisterHeadlights(theClonedCanvas) {
        var myLightmanager = ourShow.getLightManager();
        for (var i=0; i<theClonedCanvas.childNodesLength(); ++i) {
            var myViewport = theClonedCanvas.childNode(i);
            myLightmanager.deregisterHeadlightFromViewport(myViewport);
        }
    }

    obj.testSaveLoad = function() {
        var myXmlFilename = 'saved_testscene.x60';
        print("Writing scene to xml file: " + myXmlFilename);
        window.scene.save(myXmlFilename, false);

        var myBinXmlFilename = 'saved_testscene.b60';
        print("Writing scene to binary file: " + myBinXmlFilename);
        window.scene.save(myBinXmlFilename, true);

        var myNewXmlScene = new Scene(myXmlFilename);
        myNewXmlScene.setup();
        var myNewBinScene = new Scene(myBinXmlFilename);
        myNewBinScene.setup();

        if (myNewXmlScene.dom.toString() != myNewBinScene.dom.toString()) {
            print("comparing " + myXmlFilename + " and " + myBinXmlFilename + " failed.");
            writeStringToFile("saved_binary_testscene.x60", myNewBinScene.dom.toString());
            writeStringToFile("saved_xml_testscene.x60", myNewXmlScene.dom.toString());
            print("inspect 'saved_binary_testscene.x60' and 'saved_xml_testscene.x60' for differences");
	        exit(1);
        }
    }


    Base.onPostRender = obj.onPostRender; 
    obj.onPostRender = function() {
        Base.onPostRender();
        // avoid calling of this block when rendering test offscreen
        try {
            if (_myOutputImageName && !_myOffscreenRenderer) {
                ++obj.myFrameCount;
                if (obj.myFrameCount > obj.MAX_FRAME_COUNT) {
                    if (!_myOutputImageName) {
                        print ("Maximum frame count "+obj.myFrameCount+" reached - something went wrong. Terminating test");
                        exit(1);
                    }
                }

                obj.onFrameDone(obj.myFrameCount);
            }
        } catch(ex) {
            print("### ERROR: Exception caught: " + ex);
            exit(1);
        }
    }

    obj.onFrameDone = function(theFrameCount) {
        if (_myOutputImageName) {
            if (theFrameCount == _myTestDurationInFrames) {
                obj.saveTestImage();

                if (_mySaveLoadFlag) {
                    obj.testSaveLoad();
                }
                exit(0);
            }
        }
    }

    Base.setup = obj.setup;
    obj.setup = function(theWidth, theHeight) {
        if (!theWidth) {
            theWidth = WINDOW_WIDTH;
        }
        if (!theHeight) {
            theHeight = WINDOW_HEIGHT;
        }
        Base.setup(theWidth, theHeight, false, "Scene Tester");
        window.position = [0,0];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // private members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    var _myTestDurationInFrames = 3;
    var _myOutputImageName      = null;
    var _myOutputSuffix         = null;
    var _myOffscreenFlag        = false;
    var _mySaveLoadFlag         = true;
    var _myOffscreenRenderer    = null;

    for (var i = 0; i < theArguments.length; ++i) {
        var myArgument = theArguments[i];
        if (myArgument.search(/outputimage/) != -1) {
            var lastEqualPosition = myArgument.lastIndexOf('=');
            _myOutputImageName = myArgument.substr(lastEqualPosition + 1);
        }
        if (myArgument.search(/outputsuffix/) != -1) {
            var lastEqualPosition = myArgument.lastIndexOf('=');
            _myOutputSuffix = myArgument.substr(lastEqualPosition + 1);
        }
        if (myArgument == "offscreen" && 
	    operatingSystem() != "OSX" &&
	    expandEnvironment("${AC_NO_OFFSCREEN_TEST}") != 1) {
            _myOffscreenFlag = true;
        }
    }
}

