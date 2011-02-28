//===========================================================================
// Copyright (C) 2004-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");
plug("GPUParticles");

var ourShow = new SceneTester(arguments);
var _myParticleSystem = null;

ourShow.SceneTester = [];
var ourFrameCounter = -1;

ourShow.SceneTester.setup = ourShow.setup;
ourShow.setup = function() {
    ourShow.SceneTester.setup();

    // position camera
    window.camera.position    = [-1.07385, 2.57901, 0.630398];
    window.camera.orientation = [0.615412, 0, 0, 0.788205]; 
    window.fixedFrameTime = 0.04;
    // create particles
    _myParticleSystem = new GPUParticles(window.scene);
    
    _myParticleSystem.create( window.scene.world, 
            5000, // number of particles
            "tex/particleAnim.png", // 3d texture
            [1.0, -0.5, 0.0], // initial emitting direction
            [0.0, 0.0], // the scattering angles
            [1.0, 1.0]); // the speed range

    _myParticleSystem.material.properties.timetolive = 3;
    _myParticleSystem.material.properties.gravity = [0.0, 0.0, 0.0, 0.0];
    _myParticleSystem.material.properties.size = [45.0, 2.0, 640.0];
    _myParticleSystem.material.transparent = true;
    window.scene.update(Scene.ANIMATIONS_LOAD);
    //window.scene.save("dump.x60"); 
}

ourShow.onFrameDone = function (theFrameCount) {
    if (theFrameCount == 1) {
        //_myParticleSystem.animation.begin = (ourShow.getCurrentTime()-3 + window.fixedFrameTime);
    }
    if (theFrameCount == 2) {
        ourShow.saveTestImage();
    }
    /*if (theFrameCount == 3) {
        _myParticleSystem.material.properties.stoptime = ourShow.getCurrentTime() - 2.5;
    }*/
    if (theFrameCount == 20) {
        ourShow.saveTestImage();
        exit(0);
    }
    window.scene.update(Scene.ANIMATIONS); 
}

try {
    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}

