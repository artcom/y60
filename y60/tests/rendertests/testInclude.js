//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");

try {
    var ourInclude = null;
    var ourShow = new SceneTester(arguments);
    var ourFrameCount = 0;
    var ourNodeCount  = 0;
    var SceneViewer;

    ourShow.include = function(theFile) {
        print("INCLUDE: Node count: " + countNodes(ourShow.getScene().dom) + " file: " + theFile);
        ourInclude.src = theFile;
        ourShow.getScene().update(Scene.ALL);
        if (theFile == "") {
            print("   Removed include, Nodecount: " + countNodes(ourShow.getScene().dom));
        } else {
            print("   Included " + theFile + " Nodecount: " + countNodes(ourShow.getScene().dom));
        }
    }

    SceneViewer.onFrame = ourShow.onFrame;
    ourShow.onFrame = function (theTime) {
        SceneViewer.onFrame(theTime);
        switch (ourFrameCount) {
            case 0:
                ourInclude = new Node("<include/>").firstChild;
                ourInclude.orientation = Quaternionf.createFromEuler([PI_4, PI_4, 0]);
                window.scene.world.find(".//*[@name = 'pCube1']").appendChild(ourInclude);
                this.include("testInclude.x60");
                break;
            case 1:
                this.include("");
                ourNodeCount = countNodes(ourShow.getScene().dom);
                break;
            case 2:
                this.include("material_test.x60");
                break;
            case 3:
                ourShow.getScene().save("incl1.x60", false);
                this.include("");
                if (ourNodeCount != countNodes(ourShow.getScene().dom)) {
                    print("### ERROR: Node count after include is wrong. Should be: " + ourNodeCount +
                      " but it is: " + countNodes(ourShow.getScene().dom));
                      exit(-1);
                } else {
                    print("Node count is correct!");
                }
                break;
            case 5:
                this.include("testInclude.x60");
                break;
        }
        ourFrameCount++;
    }

    ourShow.setSplashScreen(false);
    ourShow.setTestDurationInFrames(10);
    ourShow.setup();

    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
