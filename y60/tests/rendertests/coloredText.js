//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: testText.js,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/03/18 11:48:31 $
//
//
//=============================================================================

use("SceneTester.js");
//use("ImageManager.js");

try {
    var ourShow = new SceneTester(arguments);

    ourShow.SceneTester = [];
    ourCounter = 0;
    function renderTextPart(theText, thePosition, theTextColor) {
        //var myImageNode = ourShow.getImageManager().getImageNode(ourCounter++);
        var myImageNode = Node.createElement("image");
        window.scene.images.appendChild(myImageNode);
        myImageNode.name = ourCounter++;
        myImageNode.resize = "none";
        
        var myTextSize = window.renderTextAsImage(myImageNode, theText, "Arial60", 
                                                  new Node("<style textColor='" + theTextColor + "'/>").childNode(0), 600, 400, thePosition);
        var myCursorPos = window.getTextCursorPosition();
        var myOverlay = new ImageOverlay(window.scene, myImageNode);
        myOverlay.position.x = 100;
        myOverlay.position.y = 100;
        myOverlay.width      = myTextSize.x;
        myOverlay.height     = myTextSize.y;

        myOverlay.srcsize.x = myTextSize.x/myImageNode.width;
        myOverlay.srcsize.y = myTextSize.y/myImageNode.height;
        myOverlay.bordercolor = new Vector4f(1,0,0,1);
    }

    ourShow.setupTests = function() {
        window.loadTTF("Arial60", "fonts/arial.ttf", 60, Renderer.NOHINTING);
        var myTextPos = [0,0];

        renderTextPart("The quick ", myTextPos, new Vector4f(1,1,1,1));
        myTextPos = window.getTextCursorPosition();

        renderTextPart("red ", myTextPos, new Vector4f(1,0,0,1));
        myTextPos = window.getTextCursorPosition();

        renderTextPart("fox jumps over the lazy ", myTextPos, new Vector4f(1,1,1,1));
        myTextPos = window.getTextCursorPosition();

        renderTextPart("brown ", myTextPos, new Vector4f(0.5,0.3,0,0.3));
        myTextPos = window.getTextCursorPosition();

        renderTextPart("dog.", myTextPos, new Vector4f(1,1,1,1));

    }
    ourShow.setup(800, 600);
    ourShow.setupTests();
    ourShow.go();
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(-1);
}
