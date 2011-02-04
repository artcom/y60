//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");
use("Overlay.js");

var ourShow = new SceneTester(arguments);
var _myImage = null;

ourShow.SceneTester = [];

ourShow.SceneTester.setup = ourShow.setup;
ourShow.setup = function() {
    ourShow.SceneTester.setup(1024, 256);
 
    _myImage = Modelling.createImage(window.scene, "tex/ROR.png");

    var myTestOverlay = new ImageOverlay(window.scene, "tex/NVOR.png", [0,0]); 
    myTestOverlay.width = 1024;
    myTestOverlay.height = 256;

    var myMaterial = myTestOverlay.node.getElementById(myTestOverlay.node.material);
    myMaterial.enabled = false;
    var myVertexParams = new Node("<feature name=\"option\">[100[nightvision]]</feature>");
    myMaterial.requires.appendChild(myVertexParams.firstChild);

    var myNode = myMaterial.requires.find("*[@name = 'textures']");
    myNode.firstChild.nodeValue = "[100[paint, paint]]";
    myNode = myMaterial.requires.find("*[@name = 'texcoord']");
    myNode.firstChild.nodeValue = "[100[uv_map,reflective]]";
    myMaterial.enabled = true;

    var myTexture = Modelling.createTexture(window.scene, _myImage);

    // append noise texture
    var myTextureUnit = myMaterial.childNode("textureunits").firstChild.cloneNode(true);
    myTextureUnit.texture = myTexture.id;
    myMaterial.childNode("textureunits").appendChild(myTextureUnit); 

    myTestOverlay.srcsize = [1, -1];
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
