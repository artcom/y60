//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneViewer.js");
use("Overlay.js");

plug("Cairo");

var ourShow = new SceneViewer(arguments);
var _myImage = null;

ourShow.SceneViewer = [];

ourShow.SceneViewer.setup = ourShow.setup;
ourShow.setup = function() {
    ourShow.SceneViewer.setup();
    window.resize(1024, 768);
    window.canvas.backgroundcolor = new Vector4f(0.8, 0.7, 0.9, 1.0);

    var myImage = Modelling.createImage(window.scene, 1024, 768, "BGRA");
    myImage.resize = "none";
    
    var myTestOverlay = new ImageOverlay(window.scene, myImage, [0,0]); 
    myTestOverlay.width = 1024;
    myTestOverlay.height = 768;

    var xc = 128.0;
    var yc = 128.0;
    var radius = 100.0;
    var angle1 = radFromDeg(45.0);
    var angle2 = radFromDeg(180.0);

    var myCairo = new CairoContext(myImage);
    myCairo.setAntialias(CairoContext.ANTIALIAS_NONE);
    myCairo.setSourceRGB(255,0,0);
    myCairo.setLineWidth(10.0);
    myCairo.arc(xc, yc, radius, angle1, angle2);
    myCairo.stroke();
}

try {
    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
