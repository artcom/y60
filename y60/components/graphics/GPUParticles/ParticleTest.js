/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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
        _myParticleSystem.create( window.scene.world,
                                  50, // number of particles
                                  "2.png", // 3d texture
                                  [1.0, -0.5, 0.0], // initial emitting direction
                                  [0.0, 25.0], // the scattering angles
                                  [0.5, 1.5]); // the speed range

        _myParticleSystem.material.properties.timetolive = 1.0;
        _myParticleSystem.material.properties.gravity = [0.0, 0.5, 0.0, 0.0];
        _myParticleSystem.material.properties.size = [20.0, 2.0, 640.0];
        window.scene.update(Scene.ANIMATIONS_LOAD);

        window.canvas.backgroundcolor = [0.6,0.6,0.75,1.0];
    }

    self.SceneViewer.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        self.SceneViewer.onFrame(theTime);
    }

    self.onPostRender = function() {
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
