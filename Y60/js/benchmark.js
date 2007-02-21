//============================================================================
//
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

const WINDOW_SIZE   = [1280, 1024]
const MULTISAMPLING = 0;
const USE_CG        = false;

print("Running benchmark test with:")
print("   Window-Size:   " + WINDOW_SIZE);
print("   Multisampling: " + MULTISAMPLING);
print("   USE_CG:        " + (USE_CG ? "true" : "false"));

GLResourceManager.prepareShaderLibrary("shaderlibrary" + (USE_CG ? ".xml" : "_nocg.xml"));

var window = new RenderWindow();
window.resize(WINDOW_SIZE[0], WINDOW_SIZE[1]);
window.position = [0,0]
window.decorations = false;
window.multisamples = MULTISAMPLING;
window.scene = new Scene(arguments[0]);
window.swapInterval = 0;

Scene.intersectBodies(window.scene.world, new Ray([0,0,0],[0,0,10]));
window.onFrame = function(theTime) {
    window.renderText([10, 20], window.fps.toFixed(1));
}

window.onKey = function(theKey, theKeyState) {
    if (theKeyState && theKey == "s") {
        window.printStatistics();
    }
}

window.go();
