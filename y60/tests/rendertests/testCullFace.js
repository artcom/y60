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
use("Label.js")

try {
    var ourShow = new SceneTester(arguments);

    var _myTests = [];

    function createImage(thePosition, theCullFaces) {
        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, "tex/rgbtest_256.png");
        var myTop  = window.height / 2 - thePosition - 90;
        var myLeft = -window.width / 2 + 400;
        var myShape = Modelling.createQuad(window.scene, myMaterial.id, [myLeft, myTop, -1], [myLeft + 64, myTop + 64, -1]);
        myShape.renderstyle = theCullFaces;
        myLeft += 200;
        var myFlippedShape = Modelling.createQuad(window.scene, myMaterial.id, [myLeft, myTop + 64, -1], [myLeft + 64, myTop, -1]);
        myFlippedShape.renderstyle = theCullFaces;
        Modelling.createBody(window.scene.world, myShape.id);
        Modelling.createBody(window.scene.world, myFlippedShape.id);
        _myTests.push(theCullFaces);
    }

    var onPreRender = ourShow.onPreRender;
    ourShow.onPreRender = function() {
        onPreRender();
        var myTextColor = new Vector4f(1,0,0,1);       
        var myTextStyle = new Node("<style textColor='" + myTextColor + "'/>").childNode(0);
        ourShow.getRenderWindow().renderText([400, 30], "Normal", myTextStyle, "Screen15");
        ourShow.getRenderWindow().renderText([600, 30], "Flipped", myTextStyle, "Screen15");

        var myPos = 90;
        for (var i = 0; i < _myTests.length; ++i) {
            ourShow.getRenderWindow().renderText([10, myPos], "Renderstyle: " + _myTests[i], 
                                                 new Node("<style textColor='" + myTextColor + "'/>").childNode(0), "Screen15");
            myPos += 100;
        }
    }

    ourShow.setupTests = function() {
        window.camera.frustum.width = 1000;
        window.camera.frustum.type = ProjectionType.orthonormal;

        createImage(0,   "[frontfacing]");
        createImage(120, "[backfacing]");
        createImage(240, "[frontfacing, backfacing]");
        createImage(360, "[]");
    }

    ourShow.setup(800, 600);
    ourShow.setupTests();

    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}

