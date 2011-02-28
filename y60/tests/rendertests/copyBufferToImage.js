//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");

try {
    var ourShow = new SceneTester(arguments);

    var Base = [];
    var _myLastFrame = null;

    Base.setup = ourShow.setup;
    ourShow.setup = function() {

        Base.setup(256,256); // incidentally the same size as the image used in the scene below...

        var myScene = new Scene("cube.x60");
        myScene.setup();
        ourShow.setScene(myScene);
        ourShow.offscreenFlag = false;

        window.canvas.backgroundcolor = [0,0.0,0.5,1];
    }

    Base.onFrameDone = ourShow.onFrameDone;
    ourShow.onFrameDone = function(theFrameNum) {

        Base.onFrameDone(theFrameNum);
        doWhatNeedsToBeDone(theFrameNum);
    }

    function doWhatNeedsToBeDone(theFrameNum) {

        var myImage = window.scene.images.find("image[@name = 'file1']");
        if (!myImage) {
            Logger.error("image not found")
            return;
        }
        var myOffset = new Vector2i(0,0);

        var myTexture = window.scene.textures.find("texture[@image = '" + myImage.id + "']");
        //print("frame=" + theFrameNum, myTexture);

        var mySnapshot = true;
        if (theFrameNum == 2) {
            // copy to image, with mipmap
            myTexture.mipmap = true;
            window.copyBufferToTexture(myTexture, myOffset, true);
        } else if (theFrameNum == 4) {
            // copy to image, without mipmap
            myTexture.mipmap = false;
            window.copyBufferToTexture(myTexture, myOffset, true);
        } else if (theFrameNum == 6) {
            // UH: when we modify the 'mipmap' attribute, we force a texture upload from the
            // image raster (via dependency graph); I have no idea why this works in the old
            // version
            // copy to texture, with mipmap
            myTexture.mipmap = true;
            window.copyBufferToTexture(myTexture, myOffset, true);
        } else if (theFrameNum == 8) {
            // copy to texture, without mipmap
            myTexture.mipmap = false;
            window.copyBufferToTexture(myTexture, myOffset, true);
        } else {
            // rotate RGB colors
            var myBackgroundColor = window.canvas.backgroundcolor;
            var t = myBackgroundColor[0];
            myBackgroundColor[0] = myBackgroundColor[1];
            myBackgroundColor[1] = myBackgroundColor[2];
            myBackgroundColor[2] = t;
            window.canvas.backgroundcolor = myBackgroundColor;
            mySnapshot = false;
            print("bgcolor=" + myBackgroundColor);
        }
        preLoad(myTexture);                                

        if (mySnapshot) {
            ourShow.saveTestImage();
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
