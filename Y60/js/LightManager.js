//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SunPosition.js");

// LightManager Constructor
function LightManager(theWorld, theLightSources) {
    this.Constructor(this, theWorld, theLightSources);
}

LightManager.prototype.Constructor = function(obj, theScene, theWorld) {

    var _myEnabledFlag  = true;
    
    var _myScene        = theScene;
    var _myWorld        = theWorld;
    var _myLightSources = getDescendantByTagName(theScene.dom,'lightsources', false);

    var _myLights = getDescendantsByTagName(theWorld,'light', true);
    
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
        _mySunLight = getDescendantByName(_myWorld, "Sun", true);
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

        var myViewportCount = theCanvas.childNodesLength("viewport");
        _myViewportHeadlights = [];
        var myHeadLightSource = ensureHeadlightSource();
        Logger.trace("Lightmanager iterating through " + myViewportCount + " Viewports.");
        for (var i = 0; i < myViewportCount; ++i) {
            var myViewport = theCanvas.childNode("viewport", i);
            var myHeadLight = getDescendantByName(_myWorld, "Headlight_"+myViewport.id, true);
            if (!myHeadLight) {
                myHeadLight = Node.createElement('light');
                myHeadLight.name = "Headlight_"+myViewport.id;
                myHeadLight.lightsource = myHeadLightSource.id;
                myHeadLight.position = "[0,0,0]";
                var myCamera = myViewport.getElementById(myViewport.camera);
                attachHeadlightToCamera(myCamera, myHeadLight);
                Logger.trace("Creating new Headlight with id: " + myHeadLight.id);
            } else {
                Logger.trace("Found Headlight: " + myHeadLight);
            }
            myHeadLight.visible = false;
            myHeadLight.sticky  = true;
            _myViewportHeadlights[myViewport.id] = myHeadLight;
            _myViewportHeadlightsEnabled[myViewport.id] = true;

            // obj.setupHeadlight(myViewport);
        }
        var activeLightsFound = false;
        _myLights = getDescendantsByTagName(_myWorld, "light", true);
        
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
                window.lighting = !window.lighting;
                print("Lighting: " + (window.lighting ? "on" : "off"));
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
        if (_myEnabledFlag && _myHeadLightFlag && theViewport.id in _myViewportHeadlights) {
            _myViewportHeadlights[theViewport.id].visible = _myViewportHeadlightsEnabled[theViewport.id];
        }
    }
    obj.onPostViewport = function(theViewport) {
        if (_myEnabledFlag && _myHeadLightFlag && theViewport.id in _myViewportHeadlights) {
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
        var myLightSourceNode = getDescendantByName(_myLightSources, "HeadLightSource");
        if (!myLightSourceNode) {
            myLightSourceNode = obj.createLightSource("HeadLightSource", "positional");
            myLightSourceNode.properties.ambient = "[0.1,0.1,0.1,1]";
            myLightSourceNode.properties.diffuse = "[1,1,1,1]";
            myLightSourceNode.properties.specular = "[1,1,1,1]";
            myLightSourceNode.properties.attenuation = 0;
        }
        return myLightSourceNode;
    }

    function ensureSunLightSource() {
        var myLightSourceNode = getDescendantByName(_myLightSources, "SunLightSource");
        if (!myLightSourceNode) {
            myLightSourceNode = obj.createLightSource("SunLightSource", "directional");
            myLightSourceNode.properties.ambient = "[0.1,0.1,0.1,1]";
            myLightSourceNode.properties.diffuse = "[0.5,0.5,0.5,1]";
            myLightSourceNode.properties.specular = "[0.5,0.5,0.5,1]";
            myLightSourceNode.properties.attenuation = 0;
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
