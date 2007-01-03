//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneViewer.js");
plug("GPUParticles");

function ParticlesTest(theArguments) {
    this.Constructor(this, theArguments);
}

ParticlesTest.prototype.Constructor = function(self, theArguments) {

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    self.SceneViewer = [];

    var _myMaterialsNode = null;
    var _myParticleSystem = null;

    //////////////////////////////////////////////////////////////////////
    //
    // Callbacks
    //
    //////////////////////////////////////////////////////////////////////

    self.SceneViewer.setup = self.setup;
    self.setup = function(theWidth, theHeight, theFullscreen, theTitle) {
        // must exist before setup so that 'onSceneLoaded' etc. are called correctly
        window = new RenderWindow();
        window.position = new Vector2i(0,0);
        self.setSplashScreen(false);
        window.showMouseCursor = true;

        self.SceneViewer.setup(theWidth, theHeight, theFullscreen, theTitle);

        // position camera
        window.camera.position    = [-1.07385, 2.57901, 0.630398];
        window.camera.orientation = [0.615412, 0, 0, 0.788205]; 

        // create particles
        _myParticleSystem = new GPUParticles(window.scene);
        print("_myParticleSystem: "+_myParticleSystem);
        _myParticleSystem.create( window.scene.world, 
                                  50, // number of particles
                                  "3.png", // 3d texture
                                  [1.0, -0.5, 0.0], // initial emitting direction
                                  [0.0, 25.0], // the scattering angles
                                  [0.5, 1.5]); // the speed range

        _myParticleSystem.material.properties.timetolive = 3.0;
        _myParticleSystem.material.properties.gravity = [0.0, 0.0, 0.0];
        _myParticleSystem.material.properties.size = [20.0, 2.0, 640.0];
        window.scene.update(Scene.ANIMATIONS_LOAD); 
    }

    self.onPostRender = function() {
        window.setTextColor([1,1,1,1]);
        window.renderText([10,10], window.fps);
    }

    var myBaseOnKey = self.onKey;
    self.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theCtrlFlag) {
            myBaseOnKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        } else {
            if (theKeyState) {
                switch (theKey) {
                    case "s":
                        _myParticleSystem.animation.enabled = true;
                        _myParticleSystem.animation.begin = self.getCurrentTime();
                        print("Start particles at time: " + _myParticleSystem.material.properties.time);
                        print("particlesTime: " + _myParticleSystem.animation.begin);
                    break;
                    case "x":
                        _myParticleSystem.material.properties.stoptime = self.getCurrentTime() - _myParticleSystem.animation.begin;
                        print("Stopping particles: stoptime: " + (self.getCurrentTime() - _myParticleSystem.animation.begin));
                        break;
                    case "p":
                        print("Pause particles");
                        _myParticleSystem.animation.enabled = false;
                        _myPauseTime = self.getCurrentTime();
                        break;
                    case "r":
                        print("Resume particles");
                        _myParticleSystem.animation.enabled = true;
                        _myParticleSystem.animation.begin += (self.getCurrentTime() - _myPauseTime);
                        break;
                }
                window.scene.update(Scene.ANIMATIONS);
                //print(_myParticleSystem.animation);
            }
        }
    }

    var _myPauseTime = 0;
}


//
// main
//
try {
    var ourShow = new ParticlesTest(arguments);
    var myWindowWidth  = 800;
    var myWindowHeight = 600;
    ourShow.setup(myWindowWidth, myWindowHeight, false, "test");
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(1);
}
