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
use("OffscreenRenderer.js");

try {
    var ourShow = new SceneTester(arguments);

    ourShow.scene = null;
    ourShow.offscreenRenderer = null;
    ourShow.offscreenOverlay = null;
    ourShow.rescaleImage = false;
    
    if (operatingSystem() == "OSX") {
        ourShow.offscreenFlag = false;
        Logger.warning("This does does not work in offscreen mode on OSX, disabling offscreen mode");
    }

    ourShow.Base = [];

    ourShow.Base.setup = ourShow.setup;
    ourShow.setup = function() {

        ourShow.Base.setup();    
        
        ourShow.setTestDurationInFrames(1);

        ourShow.scene = new Scene("cube.x60");
        ourShow.scene.setup();
        ourShow.setScene(ourShow.scene);

        var mySize = new Vector2f(nextPowerOfTwo(window.width)/2, nextPowerOfTwo(window.height)/2);
        var myCamera = window.camera;
        
        var myOverlays = window.canvas.firstChild.childNode("overlays");
        var myScene = window.scene;
        window.canvas.backgroundcolor = [0,0.3,0,1];

        ourShow.offscreenRenderer = new OffscreenRenderer(mySize, myCamera, "rgb", null, null, true);
        ourShow.offscreenRenderer.canvas.backgroundcolor = [0.8,0.4,0.8,1];
        ourShow.offscreenRenderer.setBody(myScene.world);

        ourShow.offscreenOverlay = new ImageOverlay(myScene, ourShow.offscreenRenderer.image, [0,0], myOverlays); 
        ourShow.offscreenOverlay.width = window.width/2;
        ourShow.offscreenOverlay.height = window.height/2;
        ourShow.offscreenOverlay.srcsize = [1, -1];

        var myCubeMaterial = window.scene.materials.find("material[@name = 'lambert2']");
        myCubeMaterial.childNode("textureunits").firstChild.texture = ourShow.offscreenOverlay.texture.id; 
        ourShow.offscreenRenderer.texture.wrapmode = "repeat";
        
        ourShow.offscreenRenderer.render(false);
    }
    
    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
