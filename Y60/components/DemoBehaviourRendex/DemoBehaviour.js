//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: DemoBehaviour.js,v $
//   $Author: christian $
//   $Revision: 1.2 $
//   $Date: 2004/10/27 03:25:49 $
//
//
//=============================================================================

if (__main__ == undefined) var __main__ = "DemoBehaviourRendex";

use("SceneViewer.js");

plug("DemoBehaviour");

function DemoBehaviourRendexApp(theArguments) {
    this.Constructor(this, theArguments);
}

DemoBehaviourRendexApp.prototype.Constructor = function(self, theArguments) {
    var _myTextOverlay = null;
    var _myCubeBehaviour = null;

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];
    var _myCubeBehaviour = new DemoBehaviour();
    window.addExtension(_myCubeBehaviour);

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////

    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {
        Base.setup(theWidth, theHeight, theTitle);

        var myCube = getElementById(window.scene.world, "b0", true);
        _myCubeBehaviour.node = myCube;
		self.registerSettingsListener(_myCubeBehaviour);
        createTextOverlay();
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theCtrlFlag) {
            return Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }

        if (!theState) {
            return true;
        }

        if (theKey == "space") {
            print("current state is: " + _myCubeBehaviour.state);
            print("current position is: " + _myCubeBehaviour.position);
            _myCubeBehaviour.position.add(new Vector2f(1,1));
            print("added position is: " + _myCubeBehaviour.position);
        } else {
            print("setting state to: " + theKey);
            _myCubeBehaviour.state = theKey;
        }
        return true;
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);
    }

    Base.onMouseMotion = self.onMouseMotion;
    self.onMouseMotion = function(theX, theY) {
        if (Base.onMouseMotion) {
            Base.onMouseMotion(theX, theY);
        }

        print("setting state to: " + new Vector2i(theX, theY));
        _myCubeBehaviour.position = new Vector2i(theX, theY);
    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function(theButton, theState, theX, theY) {
        if (Base.onMouseButton) {
            Base.onMouseButton(theButton, theState, theX, theY);
        }
    }

    Base.onMouseWheel = self.onMouseWheel;
    self.onMouseWheel = function(theDeltaX, theDeltaY) {
        if (Base.onMouseWheel) {
            Base.onMouseWheel(theDeltaX, theDeltaY);
        }
    }

    ////////////// private members ////////////

    function createTextOverlay() {
        window.loadTTF("Arial", "${PRO}/src/Y60/gl/text/fonts/arial.ttf", 18);
        window.setTextPadding(10,10,10,10);
        window.setTextColor([0.2,0.2,1,1]);

        var myTextImage = self.getImageManager().getImageNode("infobox");
        _myTextOverlay = new ImageOverlay(window.getScene(), myTextImage.id, [0,0]);
        _myTextOverlay.texture.applymode = "decal";
        _myTextOverlay.color = new Vector4f(0.8,0.8,1,0.6);

        var myTextSize = window.renderTextAsImage(myTextImage, "Press Ctrl-Q to quit.", "Arial", 300, 0);
        _myTextOverlay.width  = myTextSize.x;
        _myTextOverlay.height = myTextSize.y;
        _myTextOverlay.srcsize.x = myTextSize.x / myTextImage.width;
        _myTextOverlay.srcsize.y = myTextSize.y / myTextImage.height;

        _myTextOverlay.position = new Vector2f(10, 30);
        _myTextOverlay.visible = true;
    }

}

//
// main
//

if (__main__ == "DemoBehaviourRendex") {

    try {
        var ourDemoBehaviourRendexApp = new DemoBehaviourRendexApp(arguments);

        ourDemoBehaviourRendexApp.setup(320, 200, "DemoBehaviourRendex");
        ourDemoBehaviourRendexApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }


        /*
        NEXT STEP:

        myCube.behaviour = new Behaviour("Explosion");

        myCube.behaviour.velocity = new Vector3f(0,0,0);

        <body ... behavior="bhv0">

        <behaviors>
            <behavior id="bhv0" class="Explosion">
                <vector4f name="velocity">[1,1,1,1]</vector4f>
                <float name="duration">100</float>
            </behavior>
             <behavior id="bhv0" class="Explosions.FastExplode">
                <vector4f name="velocity">[1,1,1,1]</vector4f>
                <float name="duration">100</float>
            </behavior>
        </behaviors>
        */

}


