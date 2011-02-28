//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

use("SceneTester.js");

/*
 * The scene is setup so that:
 * - a red quad lies behind a green quad
 * - the red quad is transparent to force it to be drawn after the green quad
 * - for the red quad 'ignore_depth' is activated
 *
 * if 'ignore_depth' does work then the green quad in front will be blended by the red
 * quad behind it, which wouldn't happen if depth test is enabled.
 */
try {
    var ourShow = new SceneTester(arguments);
    var SceneViewer;

    ourShow.setupTests = function() {
        var myShapeName = "behindShape";
        var myShape = window.scene.shapes.find("shape[@name = '" + myShapeName + "']");
        if (!myShape) {
            Logger.error("Unable to find shape '" + myShapeName + "'");
            exit(-1);
        }
        myShape.renderstyle.ignore_depth = true;
    }

    ourShow.setup();
    ourShow.setupTests();

    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
