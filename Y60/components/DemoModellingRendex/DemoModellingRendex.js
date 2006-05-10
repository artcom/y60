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
//   $RCSfile: DemoModellingRendex.js,v $
//   $Author: pavel $
//   $Revision: 1.4 $
//   $Date: 2004/11/18 16:20:47 $
//
//
//=============================================================================

if (__main__ == undefined) var __main__ = "DemoModellingRendex";


use("SceneViewer.js");

var window = new RenderWindow();

//window.loadExtension("demoModellingRendex");
plug("demoModellingRendex");

function DemoModellingRendexApp(theArguments) {
    this.Constructor(this, theArguments);
}

DemoModellingRendexApp.prototype.Constructor = function(self, theArguments) {
    var _myTextOverlay = null;

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];

    var _myModellingRendex = new demoModellingRendex();
    print("window1="+window);
    window.addExtension(_myModellingRendex);

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////

    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {
        Base.setup(theWidth, theHeight, theTitle);
        createTextOverlay();
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
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
        _myTextOverlay = self.getOverlayManager().create("infobox", myTextImage.id, true);
        self.getOverlayManager().setApplyMode(_myTextOverlay.name, "decal");
        self.getOverlayManager().setBackgroundColor(_myTextOverlay.name, new Vector4f(0.8,0.8,1,0.6));

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

if (__main__ == "DemoModellingRendex") {
    print("DemoModellingRendex");
    try {
        var ourDemoModellingRendexApp = new DemoModellingRendexApp(arguments);

        ourDemoModellingRendexApp.setup(320, 200, "DemoModellingRendex");
        ourDemoModellingRendexApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}


