//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

const MATERIALS = ["lambert1", "single_texture", "double_texture"];

use("SceneTester.js");

var ourShow = new SceneTester(arguments);
try {
    ourShow.Base = [];

    ourShow.Base.setup = ourShow.setup;
    ourShow.setup = function() {
        ourShow.Base.setup();
        for (var i=0; i<MATERIALS.length; ++i) {
            appendSpotlightFeature(MATERIALS[i]);
        }
        ourShow.getAnimationManager().enable(false);
    }

    function appendSpotlightFeature(theMaterialName) {
        var myMaterial = window.scene.materials.find("material[@name = '" + theMaterialName + "']");
        myMaterial.enabled = false;
        var myVertexParams = new Node("<feature name=\"option\">[100[spotlight]]</feature>");
        myMaterial.requires.appendChild(myVertexParams.firstChild);
        var myPenumbra = new Node("<float name=\"penumbra\">5.0</float>"); 
        myMaterial.properties.appendChild(myPenumbra.firstChild);
        myMaterial.enabled = true;
    }

    ourShow.setup();
    ourShow.go();
}
catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
