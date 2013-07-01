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
plug("Proximatrix2");

function Proximatrix2Demo(theArguments) {
    this.Constructor(this, theArguments);
}

Proximatrix2Demo.prototype.Constructor = function(self, theArguments) {

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];
    var _myProximatrix2 = new Proximatrix2();

    var _touches = {};
    var _colors = {};
    var _overlays = [];

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

        // self.registerSettingsListener(_myProximatrix2, "Proximatrix2");

        window.camera.frustum.hfov = 0;
        window.camera.frustum.width = theWidth;
        window.camera.position = new Vector3f(0,0,1);
        window.camera.orientation.assignFromEuler( new Vector3f(0,0,0));

        window.canvas.backgroundcolor = [0.5, 0.5, 0.5, 1.0];
        
        for (var i=0; i<10; i++) {
            var o = new ImageOverlay(window.scene, "dot.png");
            _overlays.push(o);
            
            o.visible = false;
        }
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
    }

    self.onTouch = function(theEvent) {

        if (!(theEvent.id in _colors)) {
            _colors[theEvent.id] = new Vector4f(Math.random(), 
                    Math.random(), Math.random(), 1);
        }

        print(theEvent.position);
        _touches[theEvent.id] = theEvent;
        _myProximatrix2.onUpdateSettings( new Node("<blaschnabel/>") );
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);
    
        for (var i=0; i<_overlays.length; i++) {
            _overlays[i].visible = false;
        }
  
        var j=0;

        for (var key in _touches) {

            var o = _overlays[j];
            var t = _touches[key];
            o.visible = true;

            var size = t.pressure;
            if (t.type !== "move") {
                size *= 5;
            }
            
            o.position.x = t.position.x/10 - size/2;
            o.position.y = t.position.y/10 - size/2;
            o.color = _colors[t.id];
            o.width = size; 
            o.height = size; 
        
            if (t.type === "up") {
                delete _touches[t.id];
            }
            
            j++;
        }
    }
}

//
// main
//
var ourApp = new Proximatrix2Demo(arguments);

ourApp.setup(1024, 800, false, "Proximatrix2Demo");
ourApp.go();
