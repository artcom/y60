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
*/

use("SceneViewer.js");
plug("EdgeBlender");


function EdgeBlendViewer(theArguments) {
    this.Constructor(this, theArguments);
}

EdgeBlendViewer.prototype.Constructor = function(self, theArguments) {

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];
    var _myEdgeBlender = new EdgeBlender();

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////

    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theFullscreen, theTitle) {
        window = new RenderWindow();
        window.position = new Vector2i(0,0);

        Base.setup(theWidth, theHeight, theFullscreen, theTitle);

        window.addExtension(_myEdgeBlender);
        self.registerSettingsListener(_myEdgeBlender, "EdgeBlender");

        window.camera.frustum.hfov = 0;
        window.camera.frustum.width = theWidth;
        window.camera.position = new Vector3f(0,0,1);
        window.camera.orientation.assignFromEuler( new Vector3f(0,0,0));

        window.canvas.backgroundcolor = [0.5, 0.5, 0.5, 1.0];
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theCtrlFlag) {
            Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
            return;
        }
        if (theState) {
            if (theShiftFlag) {
                theKey = theKey.toUpperCase();
            }
            switch (theKey) {
                case 'a':
                    _myEdgeBlender.blendwidth += 0.01;
                    break;
                case 'd':
                    _myEdgeBlender.blendwidth -= 0.01;
                    break;
                case 'w':
                    _myEdgeBlender.blacklevel += 0.001;
                    break;
                case 's':
                    _myEdgeBlender.blacklevel -= 0.001;
                    break;
                case 'q':
                    _myEdgeBlender.mode = (_myEdgeBlender.mode + 1) % 3;
                    break;
                case 'y':
                    _myEdgeBlender.subdivisions += 1;
                    break;
                case 'c':
                    _myEdgeBlender.subdivisions -= 1;
                    break;
                case 'h':
                    print("EdgeBlend Help:");
                    print("a/d Increment/decrement blend width");
                    print("w/s Increment/decrement black level");
                    print("q   Cycle left/right/both blend out mode");
                    print("y/c Inrement/decrement number of subdivisions");
                    break;
                default:
            }
        }
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);
    }
}

//
// main
//
try {
    var ourApp = new EdgeBlendViewer(arguments);

    ourApp.setup(1024, 800, false, "EdgeBlendViewer");
    ourApp.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(1);
}
