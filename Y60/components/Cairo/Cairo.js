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
    
    new ImageOverlay(window.scene, "test.png", [0,0]); 

    var myTestOverlay = new ImageOverlay(window.scene, myImage, [0,0]); 
    myTestOverlay.width = 1024;
    myTestOverlay.height = 768;

    var myRSvg = new RSvg();
    myRSvg.renderFromFile(myTestOverlay.image, "test2.svg");
    // myRSvg.renderFromFile(myTestOverlay.image, "../../SVGS/popup.svg");
    
    // -- replacing svg content as follows
    // var myNode = new Node();
    // myNode.parseFile("../../SVGS/popup.svg");
    // 
    // var myInfotext = myNode.getElementById("infotext");
    // myInfotext.firstChild.firstChild.nodeValue = "aaa bbb ccc";
    // 
    // var myInfotext = myNode.getElementById("nickname");
    // myInfotext.firstChild.firstChild.nodeValue = "openlaszlo";
    // 
    // myRSvg.renderFromNode(myTestOverlay.image, myNode);
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
