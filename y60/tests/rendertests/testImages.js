//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

use("SceneTester.js");

// What do I still have to test
// - Test font replacing
// - Test cubemaps (half done)
// - Test bumpmaps
// - Test mipmapping
// - Test exporter

// TODO:
// - same image source with different attributes

var ourResizeSteps = 5;
var ourMovies      = [];

function createTextIntoTexture(theId, theText, thePosX, thePosY, theWidth, theHeight, theFrontColor) {
    var myImageNode = Node.createElement("image");
    window.scene.images.appendChild(myImageNode);
    myImageNode.name = theId;
    myImageNode.resize = "none";
    
    window.setTextColor(theFrontColor);
    var myTextSize = window.renderTextAsImage(myImageNode, theText, "Arial60", theWidth, theHeight);

    var myOverlay = new ImageOverlay(window.scene, myImageNode);
    myOverlay.position.x = thePosX;
    myOverlay.position.y = thePosY;

    myOverlay.width      = myTextSize.x;
    myOverlay.height     = myTextSize.y;

    myOverlay.srcsize.x = myTextSize.x/myImageNode.width;
    myOverlay.srcsize.y = myTextSize.y/myImageNode.height;
    myOverlay.bordercolor = new Vector4f(1,0,0,1);
    //print("Render Text '" + theText + "' with size: " + myTextSize);
    return myOverlay;
}

var ourOverlayCounter = 0;

function showCubemap(theFilename, theMaterial) {
    var myImagename = "cubemap_" + ourOverlayCounter++;
    var myImage = window.scene.images.appendChild(new Node("<image src='" + theFilename + "' />").firstChild); 
}

function showOverlay(theFilename, theResize, thePosition, theAlpha, theParent, theRotation, thePivot) {

    var myName = "image_" + ourOverlayCounter++;
    var myImage = Node.createElement("image");
    window.scene.images.appendChild(myImage);
    myImage.name = myName;
    myImage.src = theFilename;
    myImage.resize = theResize;

    var myOverlay = new ImageOverlay(window.scene, myImage, thePosition, theParent);
    myOverlay.visible = true;
    myOverlay.width = myImage.width;
    myOverlay.height = myImage.height;
 
    if (theAlpha != undefined) {
        var myTexture = myOverlay.node.getElementById(myOverlay.textureunit.texture);
        myTexture.color_scale[3]  = theAlpha;
    }

    if (theRotation) {
        myOverlay.rotation = theRotation;
    }
    if (thePivot) {
        myOverlay.pivot = thePivot;
    }

    return myOverlay;
}


try {
    var ourShow = new SceneTester(arguments);
    var SceneViewer;
    SceneViewer.onFrame = ourShow.onFrame;
    ourShow.onFrame = function (theTime) {
        window.runAnimations(theTime);
        SceneViewer.onFrame(theTime);

        for (var i = 0; i < ourMovies.length; ++i) {
            var myMovieNode = ourMovies[i];
            myMovieNode.currentframe++;
            if (myMovieNode.currentframe >= myMovieNode.framecount) {
                myMovieNode.currentframe = 0;
            }
            window.loadMovieFrame(myMovieNode);
        }

        if (ourResizeSteps > 0) {
            window.resize(window.width + 20, window.height + 20);
            ourResizeSteps--;
        }
    }

    ourShow.setupTests = function() {
        ourShow.addSkyBoxFromFile("tex/ENVMAP/layered_cubemap.i60");
        createTextIntoTexture("testtext", "Test Text", 10, 150, 0, 0, [1,0.2,0.2,1]);
        showOverlay("tex/rgbtest_256.png", "scale", new Vector2f(280, 0));
        showOverlay("tex/rgbtest_130.png", "pad", new Vector2f(540, 0));
        showOverlay("tex/rgbtest_130.png", "scale", new Vector2f(0, 280));
        showOverlay("tex/HalfAlpha.rgb", "scale", new Vector2f(280, 280));
        showOverlay("tex/RedNoAlpha.rgb", "scale", new Vector2f(280, 410), 0.5);

        showCubemap("tex/ENVMAP/sunol_front.rgb|tex/ENVMAP/sunol_right.rgb|" +
                    "tex/ENVMAP/sunol_back.rgb|tex/ENVMAP/sunol_left.rgb|" +
                    "tex/ENVMAP/sunol_top.rgb|tex/ENVMAP/sunol_bottom.rgb");

        // rotation inherited from parent
        var myParentOverlay = showOverlay("tex/testbild00.rgb", "scale", new Vector2f(450, 280), 0.2, null, Math.PI/8.0);
        showOverlay("tex/testbild00.rgb", "scale", new Vector2f(100,0), 1.0, myParentOverlay.node);

        // pivot
        var myO = showOverlay("tex/bodenfliesen00.rgb", "scale", new Vector2f(145,90), 1.0, null, 0.0);
        showOverlay("tex/bodenfliesen00.rgb", "scale", new Vector2f(0,0), 1.0, myO, Math.PI * 0.25, new Vector2f(myO.width*0.5,myO.height*0.5));
    }

    ourShow.setup();

    const FONTFILE = searchFile("../src/y60/gltext/fonts/arial.ttf");
    window.loadTTF("Arial60", FONTFILE, 60, Renderer.NOHINTING);

    ourShow.setupTests();

    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
