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
use("BuildUtils.js");

try {
    var ourShow = new SceneTester(arguments);

    ourShow.scene = null;
    ourShow.Base = [];

    ourShow.Base.setup = ourShow.setup;
    ourShow.setup = function() {

        ourShow.Base.setup();    
        var myMaterial = 
        Modelling.createUnlitTexturedMaterial(window.scene,
                                                      "tex/wave_dot.png", 
                                                      "wave_material",
                                                      true, true);
        myMaterial.properties.targetbuffers.depth = false;
        myMaterial.enabled = false;
        addMaterialRequirement(myMaterial, "option", "[10[ottobock_waves]]");
        removeMaterialRequirement(myMaterial, "texcoord");
        myMaterial.enabled = true;

        var myShapeBuilder = new ShapeBuilder();
        var myElement = myShapeBuilder.appendElement("points", myMaterial.id);

        var DOT_SPACING = 26;
        for (var x = DOT_SPACING/2; x < window.width; x += DOT_SPACING) {
            myShapeBuilder.appendVertex(myElement, new Vector3f(x,0,0));
            myShapeBuilder.appendColor(myElement, new Vector4f(1,1,1,1));
        }

        var myShape = myShapeBuilder.buildNode("wave_shape");
        window.scene.shapes.appendChild(myShape);
        
        var myBody = Modelling.createBody(window.scene.world, 
                                          myShape.id);
        myBody.name = "wave_body";
        myBody.position = new Vector3f(0,100, 5);
        myBody = Modelling.createBody(window.scene.world, 
                                          myShape.id);
        myBody.name = "wave_body2";
        myBody.position = new Vector3f(0,200, 5);

        window.camera.frustum.width = window.width;
        window.camera.frustum.height = window.height;
        window.camera.position.x = window.width/2;
        window.camera.position.y = window.height/2;
        window.camera.position.z = 100;
        window.camera.frustum.type = ProjectionType.orthonormal;
    }

    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}

