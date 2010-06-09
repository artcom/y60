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

use("SunPosition.js");

// LightManager Constructor
function LightManager(theWorld, theLightSources) {
    this.Constructor(this, theWorld, theLightSources);
}

LightManager.prototype.Constructor = function(obj, theScene, theWorld) {

    var _myEnabledFlag  = true;

    var _myScene        = theScene;
    var _myWorld        = theWorld;
    var _myLightSources = theScene.dom.find('/scene/lightsources');

    var _myLights = theWorld.findAll('.//light');

    var _myLightCursor = 0;

    var _myViewportHeadlights    = [];
    var _myViewportHeadlightsEnabled = [];

    var _myHeadLightFlag = false;  // global for all headlights

    var _mySunLight      = null;
    var _mySunSpeed      = 0;
    var _myLastSunUpdate = null;

    var _mySunPosition = new SunPositioner();
    _mySunPosition.calculateCurrentPosition();


    obj.setupDefaultLighting = function(theCanvas) {
        // =================================
        //
        // create a default sun light
        //
        // ==================================
        _mySunLight = _myWorld.find(".//light[@name='Sun']");

        if (!_mySunLight) {
            var mySunLightSource = ensureSunLightSource();
            _mySunLight = Node.createElement('light');
            _mySunLight.name = "Sun";
            _mySunLight.lightsource = mySunLightSource.id;
            // calculate the local sunlight direction
            _mySunLight.orientation = Quaternionf.createFromEuler(_mySunPosition.getRotation());
            _myWorld.appendChild(_mySunLight);
            theScene.update(Scene.ALL);
        }
        _mySunLight.visible = false;
        _mySunLight.sticky  = true;

        // =================================
        //
        // create a default headlight per viewport
        //
        // ==================================

        // XXX: This is bad, the use of names to identify nodes should be avoided in library code

        var myViewportCount = theCanvas.childNodesLength("viewport");
        _myViewportHeadlights = [];
        var myHeadLightSource = ensureHeadlightSource();
        Logger.debug("Lightmanager iterating through " + myViewportCount + " Viewports.");
        for (var i = 0; i < myViewportCount; ++i) {
            var myViewport = theCanvas.childNode("viewport", i);
            var myHeadLight = _myWorld.find(".//light[@name='Headlight_"+myViewport.id+"']");
            if (!myHeadLight) {
                myHeadLight = Node.createElement('light');
                myHeadLight.name = "Headlight_"+myViewport.id;
                myHeadLight.lightsource = myHeadLightSource.id;
                myHeadLight.position = "[0,0,0]";
                var myCamera = myViewport.getElementById(myViewport.camera);
                attachHeadlightToCamera(myCamera, myHeadLight);
                Logger.debug("Creating new Headlight with id: " + myHeadLight.id);
            } else {
                Logger.debug("Found Headlight: " + myHeadLight);
            }
            myHeadLight.visible = false;
            myHeadLight.sticky  = true;
            _myViewportHeadlights[myViewport.id] = myHeadLight;
            _myViewportHeadlightsEnabled[myViewport.id] = true;

            // obj.setupHeadlight(myViewport);
        }
        var activeLightsFound = false;
        _myLights = _myWorld.findAll(".//light");

        for (var i=0; i < _myLights.length; ++i) {
            if (_myLights[i].visible) {
                activeLightsFound = true;
                break;
            }
        }
        if (!activeLightsFound) {
            Logger.info("No active lights found, activating default lights. ");
            _mySunLight.visible = true;
            _myHeadLightFlag = true;
        }
    }

    obj.setEnabled setter = function(theFlag) {
        _myEnabledFlag = theFlag;
    }

    obj.registerHeadlightWithViewport = function(theViewportNode, theLightNode) {
        _myViewportHeadlights[theViewportNode.id] = theLightNode;
        _myViewportHeadlightsEnabled[theViewportNode.id] = true;
    }

    obj.deregisterHeadlightFromViewport = function(theViewportNode) {
        delete _myViewportHeadlights[theViewportNode.id];
        delete _myViewportHeadlightsEnabled[theViewportNode.id];
    }

    obj.setupHeadlight = function(theViewport) {
        var myCamera = theViewport.getElementById(theViewport.camera);

        var myHeadlight = null;
        if (theViewport.id in _myViewportHeadlights) {
            myHeadlight = _myViewportHeadlights[theViewport.id];
        }
        obj.registerHeadlightWithViewport(theViewport,
                        attachHeadlightToCamera(myCamera, myHeadlight));
        _myViewportHeadlightsEnabled[theViewport.id] = true;
        if (myHeadlight == null) {
            // a new headlight has been created
            Logger.trace("a new headlight has been created");
            _myViewportHeadlights[theViewport.id].name = "Headlight_"+theViewport.id;
            _myViewportHeadlights[theViewport.id].visible = false;
        }
    }
    obj.enableHeadlight = function(theFlag) {
        _myHeadLightFlag = theFlag;
    }

    obj.enableSunlight = function(theFlag) {
        _mySunLight.visible = theFlag;
    }

    obj.onFrame = function(theTime) {
        if (_myLastSunUpdate) {
            var deltaTime = theTime - _myLastSunUpdate;

            if (_mySunSpeed > 0) {
                _mySunPosition.rise(_mySunSpeed * deltaTime);
                updateSunPosition();
            } else if (_mySunSpeed < 0) {
                _mySunPosition.set( -_mySunSpeed * deltaTime);
                updateSunPosition();
            }
        }
        _myLastSunUpdate = theTime;
    }

    obj.onKey = function(theKey, theKeyState, theShiftFlag) {
        // theKeyState is true, if the key is pressed
        switch (theKey) {
            case 'd':
                if (!theKeyState) {
                    // stop sun
                    _mySunSpeed = 0;
                    return;
                }
                if (theShiftFlag) {
                    _mySunSpeed = 30;
                } else {
                    _mySunSpeed = -30;
                }
                return;
        }

        // Key handlers following this are only activated on key down
        if (!theKeyState) {
            return;
        }
        switch (theKey) {
            case 'l':
                var viewportId;
                for (viewportId in _myViewportHeadlights) {
                    var viewport = _myScene.dom.getElementById(viewportId);
                    viewport.lighting = !viewport.lighting;
                    print("Lighting: " + (viewport.lighting ? "on" : "off"));
                }
                break;
            case 'k':
                if (theShiftFlag) {
                    _mySunLight.visible = !_mySunLight.visible;
                    print("Sunlight : " + (_mySunLight.visible ? "on" : "off"));
                } else {
                    obj.enableHeadlight(!_myHeadLightFlag);
                    print("Headlight : " + (_myHeadLightFlag? "on" : "off"));
                }
                break;
            case 'j':
                if (theShiftFlag) {
                    //cycle through scene lights
                    _myLightCursor = ++_myLightCursor % _myLights.length;
                } else {
                    //toggle current scene light
                    if (_myLightCursor < _myLights.length) {
                        _myLights[_myLightCursor].visible
                            = ! _myLights[_myLightCursor].visible;
                       print ('light # ' + _myLights[_myLightCursor].name + ' now visible '
                                + _myLights[_myLightCursor].visible);
                    }
                }
                break;
            case 'h':
                printHelp();
                break;
        }
    }

    obj.onPreViewport = function(theViewport) {

        if (_myEnabledFlag && _myHeadLightFlag  && theViewport.id in _myViewportHeadlights) {
            if (_myViewportHeadlights[theViewport.id].visible != _myViewportHeadlightsEnabled[theViewport.id]) {
                _myViewportHeadlights[theViewport.id].visible = _myViewportHeadlightsEnabled[theViewport.id];
            }
        }
    }
    obj.onPostViewport = function(theViewport) {
        //var numLights = 0;
        //for (i in _myViewportHeadlights) {
        //    ++numLights;
        //}
        if (_myEnabledFlag /*&& numLights > 1*/ && _myHeadLightFlag && theViewport.id in _myViewportHeadlights && _myViewportHeadlights[theViewport.id].visible) {
            _myViewportHeadlights[theViewport.id].visible = false;
        }
    }

    obj.getHeadlight = function(theViewport) {
        if (theViewport.id in _myViewportHeadlights) {
            return _myViewportHeadlights[theViewport.id];
        }
        return null;
    }
    obj.getSunlight = function() {
        return _mySunLight;
    }

    obj.setSunPosition = function(thePercentage) {
        _mySunPosition.setRelative(thePercentage);
        updateSunPosition();
    }

    obj.createLightSource = function(theLightSourceName, theType) {
        var myLightSourceNode = Node.createElement('lightsource');
        myLightSourceNode.id = createUniqueId();
        _myLightSources.appendChild(myLightSourceNode);

        myLightSourceNode.type = theType;
        Logger.trace("Creating new Lightsource node: " + theLightSourceName+" = "+myLightSourceNode.id);
        myLightSourceNode.name = theLightSourceName;
        return myLightSourceNode;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // private members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    function ensureHeadlightSource() {
        var myLightSourceNode = _myLightSources.find(".//lightsource[@name='HeadLightSource']");
        if (!myLightSourceNode) {
            Logger.debug("HeadLightSource not found");
            myLightSourceNode = obj.createLightSource("HeadLightSource", "positional");
            myLightSourceNode.properties.ambient = "[0.1,0.1,0.1,1]";
            myLightSourceNode.properties.diffuse = "[1,1,1,1]";
            myLightSourceNode.properties.specular = "[1,1,1,1]";
            myLightSourceNode.properties.attenuation = 0;
        } else {
            Logger.debug("found "+myLightSourceNode);
        }
        return myLightSourceNode;
    }

    function ensureSunLightSource() {
        var myLightSourceNode = _myLightSources.find(".//lightsource[@name='SunLightSource']");
        if (!myLightSourceNode) {
            Logger.debug("SunLightSource not found");
            myLightSourceNode = obj.createLightSource("SunLightSource", "directional");
            myLightSourceNode.properties.ambient = "[0.1,0.1,0.1,1]";
            myLightSourceNode.properties.diffuse = "[0.5,0.5,0.5,1]";
            myLightSourceNode.properties.specular = "[0.5,0.5,0.5,1]";
            myLightSourceNode.properties.attenuation = 0;
        } else {
            Logger.debug("found "+myLightSourceNode);
        }
        return myLightSourceNode;
    }

    function updateSunPosition() {
        if (_mySunLight) {
            // Logger.warning("Current sun time: " + _mySunPosition.getCurrentTime());
            _mySunLight.orientation = Quaternionf.createFromEuler(_mySunPosition.getDirection());
            _myScene.update(Scene.WORLD);
        }
    }

    function attachHeadlightToCamera(theCamera, theHeadlightNode) {
        //if (!theHeadlightNode.parentNode || theHeadlightNode.parentNode.id != theCamera.id) {
        if (theHeadlightNode && (!theHeadlightNode.parentNode || !theHeadlightNode.parentNode.isSameNode(theCamera))) {
            Logger.trace("Attaching Headlight " + theHeadlightNode + " to Camera " + theCamera.id);
            theCamera.appendChild(theHeadlightNode);
        } else {
            Logger.trace("Not attaching Headlight. Already attached to Camera " + theCamera.id);
        }
        return theHeadlightNode;
    }

    function createHeadlight(theLightName) {
        return myHeadLight;
    }

    function printHelp() {
         print("Lights:");
         print("    l    toggle lighting");
         print("    k    toggles camera headlight");
         print("    K    toggles sunlight");
         print("    d    move sun (decrease daytime)");
         print("    D    move sun (increase daytime)");
    }
}
