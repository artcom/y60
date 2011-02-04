//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Overlay.js");
use("SceneTester.js");
use("UnitTest.js");

// the scenetest first 
try {
    var ourShow = new SceneTester(arguments);
    var ourImage = null;
    var ourOverlay = null;

    var Base = [];
    var _myLastFrame = null;

    Base.setup = ourShow.setup;
    ourShow.setup = function() {

        Base.setup(256,256); 

        var myScene = new Scene(); 
        myScene.setup();
        ourShow.setScene(myScene);
        ourShow.offscreenFlag = true;
            
        // Image setup
        ourImage = Modelling.createImage(window.scene, 8, 4, "RGB");
        ourOverlay = new ImageOverlay(window.scene, ourImage);
        ourOverlay.scale = [16,16];
        ourOverlay.position = [64,64];

        var myTexture = ourOverlay.node.getElementById(ourOverlay.textureunit.texture);
        myTexture.mag_filter = "nearest";
        myTexture.mipmap = false;
        window.canvas.backgroundcolor = [0.5,0.5,0.5,1];

        ourRaster = ourImage.raster;
    }

    Base.onFrameDone = ourShow.onFrameDone;
    ourShow.onFrameDone = function(theFrameNum) {
        Base.onFrameDone(theFrameNum);
        
        try { 
            if (theFrameNum == 2) {
                ourShow.saveTestImage();

            } else if (theFrameNum == 4) {
                ourRaster.setPixel(0,0, [1,1,1,1]);
                ourShow.saveTestImage();

            } else if (theFrameNum == 6) {
                ourRaster.clear();

                ourShow.saveTestImage();
            } else if (theFrameNum == 8) {
                ourRaster.fillRect(0,0,1,1,[1,0,0,0]);
                ourRaster.fillRect(0,0,2,2,[0,1,0,0]);

                ourShow.saveTestImage();
            } else if (theFrameNum == 9) {
                print(">>>>>> Test resize");
                ourRaster.resize(4, 8);
                ourOverlay.width = 4;
                ourOverlay.height = 8;
                ourRaster.setPixel(3,7, [0,0,1,0]);

                ourShow.saveTestImage();
            }
        } catch (ex) {
            reportException(ex);
            exit(1);
        }
    }

    ourShow.setTestDurationInFrames(10);
    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}

// now the unit test
var ourImage  = null;
var ourRaster = null;

function RasterTest() {

    UnitTest.prototype.Constructor(this, "RasterTest");

    this.runRasterTests = function() {
        try {
            // Image setup
            var myScene = new Scene();
            ourImage = Modelling.createImage(myScene, 8, 4, "RGB");

            print(">>>>>> Test image setup");
            ENSURE("ourImage.width == 8");
            ENSURE("ourImage.height == 4");
            ENSURE("ourImage.rasterpixelformat == 'RGB'");
            ourRaster = ourImage.raster;

            print(">>>>>> Test width and height getter");
            ENSURE("ourRaster.width == 8");
            ENSURE("ourRaster.height == 4");

            print(">>>>>> Test setPixel");
            ourRaster.setPixel(0,0, [1,1,1,1]);
            ENSURE("almostEqual(ourRaster.getPixel(0,0), [1,1,1,0])");

            print(">>>>>> Test clear");
            ourRaster.clear();
            ENSURE("almostEqual(ourRaster.getPixel(0,0), [0,0,0,0])");

            print(">>>>>> Test fillRect");
            ourRaster.fillRect(0,0,1,1,[1,0,0,0]);
            ENSURE("almostEqual(ourRaster.getPixel(0,0), [1,0,0,0])");
            ENSURE("almostEqual(ourRaster.getPixel(1,0), [0,0,0,0])");
            ENSURE("almostEqual(ourRaster.getPixel(0,1), [0,0,0,0])");
            ourRaster.fillRect(0,0,2,2,[0,1,0,0]);
            ENSURE("almostEqual(ourRaster.getPixel(0,0), [0,1,0,0])");
            ENSURE("almostEqual(ourRaster.getPixel(1,0), [0,1,0,0])");
            ENSURE("almostEqual(ourRaster.getPixel(2,0), [0,0,0,0])");
            ENSURE("almostEqual(ourRaster.getPixel(0,1), [0,1,0,0])");
            ENSURE("almostEqual(ourRaster.getPixel(0,2), [0,0,0,0])");

            print(">>>>>> Test resize");
            ourRaster.resize(4, 8);
            ENSURE("ourRaster.width == 4");
            ENSURE("ourRaster.height == 8");
            ENSURE("ourImage.width == 4");
            ENSURE("ourImage.height == 8");
            ourRaster.setPixel(3,7, [0,0,1,0]);
            ENSURE("almostEqual(ourRaster.getPixel(3,7), [0,0,1,0])");
        } catch (ex) {
            reportException(ex);
            exit(1);
        }
    }

    this.run = function() {
        this.runRasterTests();
    }
}

var mySuite = new UnitTestSuite("RasterTest");

mySuite.addTest(new RasterTest());
mySuite.run();
exit(mySuite.returnStatus());

