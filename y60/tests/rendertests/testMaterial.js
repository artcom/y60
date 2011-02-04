//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");

var ourShow = new SceneTester(arguments);

try {
    var Base = [];
    var myFrameCounter = 0;

    Base.onFrame = ourShow.onFrame;
    ourShow.onFrame = function(theTime) {
        Base.onFrame(theTime);

        var myMaterial = null;
        var myRequires = null;
        var myTextureUnits = null;

        if (myFrameCounter == 25) {
            // add a texture
            print("add texture");
            myMaterial = window.scene.materials.firstChild;
            myRequires = myMaterial.requires;
            myRequires.textures = "[100[paint,paint]]";
            myRequires.texcoord = "[100[uv_map,uv_map]]";

            var myImage = Modelling.createImage(ourShow.getScene(), "tex/lightmap.rgb");

            var myTexture = Node.createElement("texture");
            myTexture.id = createUniqueId();
            myTexture.image = myImage.id;
            ourShow.getScene().textures.appendChild(myTexture);

            myTextureUnits = myMaterial.childNode("textureunits");
            var myTextureUnit = myTextureUnits.firstChild.cloneNode();
            myTextureUnit.texture = myTexture.id;
            myTextureUnits.appendChild(myTextureUnit);

            var myShape = window.scene.shapes.firstChild;
            var myElements = myShape.find(".//elements");
            var myTexIndices = myElements.find("*[@role = 'texcoord0']");
            var myTexIndices1 = myTexIndices.cloneNode(true);
            myTexIndices1.role = "texcoord1";
            myElements.appendChild(myTexIndices1);
        }
        else if (myFrameCounter == 50) {
            // remove textures
            print("remove texture");
            myMaterial = window.scene.materials.firstChild;
            myRequires = myMaterial.requires;
            myRequires.removeChild(myRequires.find("*[@name = 'textures']"));
            myRequires.removeChild(myRequires.find("*[@name = 'texcoord']"));
            
            myTextureUnits = myMaterial.childNode("textureunits");
            myTextureUnits.removeChild(myTextureUnits.firstChild);
            myTextureUnits.removeChild(myTextureUnits.firstChild);
        }
        myFrameCounter++;
    }

    ourShow.setup();
    ourShow.setTestDurationInFrames(75);
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
