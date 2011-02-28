//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
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

    function createOverlay(theSource, thePosition, theMipmapFlag, theColorScale, theColorBias) {
        var myOverlay = new ImageOverlay(ourShow.getScene(), theSource, thePosition);

        var myTexture = myOverlay.texture.getElementById(myOverlay.textureunit.texture);
        myTexture.mipmap = theMipmapFlag;
        myTexture.color_scale = new Vector4f(theColorScale[0], theColorScale[1], theColorScale[2], theColorScale[3]);
        if (theColorBias) {
            myTexture.color_bias = new Vector4f(theColorBias[0], theColorBias[1], theColorBias[2], theColorBias[3]);
        }
    }

    ourShow.setupTests = function() {
        createOverlay("tex/rgbtest_256.png", [0, 0],   true,    [1,1,1,1]);
        createOverlay("tex/rgbtest_256.png", [280, 0], true,    [1,1,1,0.5]);
        createOverlay("tex/rgbtest_256.png", [560, 0], true,    [1,0,1,0.5]);
        createOverlay("tex/HalfAlpha.rgb",   [64, 64], true,    [1,0,1,0.5]);

        createOverlay("tex/rgbtest_256.png", [0, 280], false,   [1,1,1,1]);
        createOverlay("tex/rgbtest_256.png", [280, 280], false, [1,1,1,0.5]);
        createOverlay("tex/rgbtest_256.png", [560, 280], false, [1,0,1,0.5]);
        createOverlay("tex/HalfAlpha.rgb",   [64, 344], true,    [1,0,1,0.5]);

        createOverlay("tex/rgbtest_256.png", [0, 560], false,   [1,1,1,1], [1,1,1,1]);
        createOverlay("tex/rgbtest_256.png", [280, 560], false, [1,1,1,1], [1,0,1,0]);
        createOverlay("tex/rgbtest_256.png", [560, 560], false, [1,1,1,1], [0.5,0.5,0.5,0.5]);
        createOverlay("tex/HalfAlpha.rgb",   [64, 624], true,    [1,1,1,1], [-0.5,-0.5,-0.5,0]);
        //window.canvas.backgroundcolor = [0,0,0,0];
        //window.scene.save("testPixelTransfer.x60", false);
    }

    ourShow.setup(1024, 960);
    ourShow.setupTests();

    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}

