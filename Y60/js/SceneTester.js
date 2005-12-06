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
use("UnitTest.js");

const WINDOW_WIDTH = 640;
const WINDOW_HEIGHT = 480;

function SceneTester(theArguments) {
    this.Constructor(this, theArguments);
}

SceneTester.prototype.Constructor = function(obj, theArguments) {

    SceneViewer.prototype.Constructor(obj, theArguments);

    obj.SceneViewer = [];
    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    obj.MAX_FRAME_COUNT = 1000; // Terminate test after this many frames.
    obj.myFrameCount = 0;
    obj.myImageCount = 0;

    var _myTestDurationInFrames = 3;
    var _myOutputImageName = null;
    var _myOutputSuffix = null;

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

    obj.saveTestImage = function() {
        var myImageFilename = obj.getTestImageName();
        print("Writing test image to file: " + myImageFilename);
        window.saveBuffer(myImageFilename);
    }
    
    obj.testSaveLoad = function() {
        var myXmlFilename = _myOutputImageName + '.out.x60';
        print("Writing scene to xml file: " + myXmlFilename);
        window.scene.save(myXmlFilename, false);
        
        var myBinXmlFilename = _myOutputImageName + '.out.b60';
        print("Writing scene to binary file: " + myBinXmlFilename);
        window.scene.save(myBinXmlFilename, true);
        
        var myNewXmlScene = new Scene(myXmlFilename);
        var myNewBinScene = new Scene(myBinXmlFilename);
        
        if (myNewXmlScene.dom.toString() != myNewBinScene.dom.toString()) {
            print("comparing " + myXmlFilename + " and " + myBinXmlFilename + " failed.");
            exit(1);
        }
    }


    obj.onPostRender = function() {
        if (_myOutputImageName) {
            ++obj.myFrameCount;
            if (obj.myFrameCount > obj.MAX_FRAME_COUNT) {
                if (!_myOutputImageName) {
                    print ("Maximum frame count "+obj.myFrameCount+" reached - something went wrong. Terminating test");
                    exit(1);
                }
            }

            obj.onFrameDone(obj.myFrameCount);
        }
    }

    obj.onFrameDone = function(theFrameCount) {
        if (_myOutputImageName) {
            if (theFrameCount == _myTestDurationInFrames) {
                obj.saveTestImage();
                obj.testSaveLoad();
                exit(0);
            }
        }
    }

    obj.SceneViewer.setup = obj.setup;
    obj.setup = function(theWidth, theHeight) {
        if (!theWidth) {
            theWidth = WINDOW_WIDTH;
        }
        if (!theHeight) {
            theHeight = WINDOW_HEIGHT;
        }
        obj.SceneViewer.setup(theWidth, theHeight, false, "Scene Tester");
        window.position = [0,0];
    }
}

