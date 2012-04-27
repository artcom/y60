//=============================================================================
// Copyright (C) 2006-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");
use("Y60JSSL.js");
use("BumpUtils.js");

var ourShow = new SceneTester(arguments);

try {
    var Base = [];
    
    Base.setup = ourShow.setup;
    ourShow.setup = function() {
        Base.setup();
        
        ourShow.scene = new Scene("cube.x60");
        ourShow.scene.setup();
        ourShow.setScene(ourShow.scene);
        
        window.canvas.backgroundcolor = new Vector4f(0.5,0.5,0.75,1);
        calculateAndAppendTangents(window.scene.shapes.firstChild);
        
        // create image 
        var myImage = window.scene.images.firstChild.cloneNode(false);
        myImage.id = createUniqueId();
        myImage.name = "depth_normal_map";
        myImage.src = "./tex/aphrodite_normal.png";
        window.scene.images.appendChild(myImage);
print(window.scene.images);
        // create texture
        var myTexture = window.scene.textures.firstChild.cloneNode();
        myTexture.id = createUniqueId();
        myTexture.name = "depth_normal_map";
        myTexture.image = myImage.id; 
        window.scene.textures.appendChild(myTexture);
                
        // add material props
        var myMaterial = window.scene.materials.firstChild;
        myMaterial.enabled = 0;
        myMaterial.properties.appendChild(new Node("<float name=\"depth\">0.25</float>").firstChild);        
        myMaterial.properties.appendChild(new Node("<float name=\"tile\">1.0</float>").firstChild);        
        myMaterial.properties.appendChild(new Node("<vector3f name=\"lightpos\">"+window.camera.position+"</vector3f>").firstChild);        
       
        myMaterial.properties.diffuse  = new Vector4f(0.5, 0.5, 0.5, 1.0);
        myMaterial.properties.specular = new Vector4f(0.2, 0.2, 0.2, 1.0);
        myMaterial.properties.ambient  = new Vector4f(0.2, 0.2, 0.2, 1.0);

        var myRequires = new Node(("<requires><feature name=\"textures\">[100[paint,paint]]</feature><feature name=\"texcoord\">[100[uv_map,uv_map]]</feature><feature name=\"lighting\">[10[unlit]]</feature><feature name=\"option\">[10[relief]]</feature></requires>")).firstChild;
        myMaterial.removeChild(myMaterial.childNode("requires"));
        myMaterial.appendChild(myRequires);

        var myTextureUnit = myMaterial.childNode("textureunits").firstChild.cloneNode();
        myTextureUnit.texture = myTexture.id;
        myMaterial.childNode("textureunits").insertBefore(myTextureUnit, myMaterial.childNode("textureunits").firstChild);
        myMaterial.enabled = 1;
    
        window.camera.position = new Vector3f(2.04334,0.200373,-0.396236);
        window.camera.orientation = new Vector4f(0.0407755,-0.769085,-0.049313,0.635935);
    }

    ourShow.setup();
    ourShow.setTestDurationInFrames(32);

    ourShow.go()
}
catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
