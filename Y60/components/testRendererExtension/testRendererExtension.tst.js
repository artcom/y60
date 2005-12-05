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
//   $RCSfile: testRendererExtension.tst.js,v $
//   $Author: martin $
//   $Revision: 1.7 $
//   $Date: 2005/01/03 10:46:01 $
//
//
//=============================================================================

if (__main__ == undefined) var __main__ = "RendererExtensionTest";

use("SceneViewer.js");

plug("testRendererExtension");
var _myExtension = new testRendererExtension();
window = new RenderWindow();
window.addExtension(_myExtension);

function RendererExtensionTestApp(theArguments) {
    this.Constructor(this, theArguments);
}

RendererExtensionTestApp.prototype.Constructor = function(self, theArguments) {
    var _myCountDown = 30;

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////

    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {
        Base.setup(theWidth, theHeight, false, theTitle);
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);
        if (--_myCountDown == 0) {
            print("Test failed becausse the Extension did not call exit(0) within 30 frames");
            exit(1);
        }
    }
}

//
// main
//

if (__main__ == "RendererExtensionTest") {
    print("RendererExtensionTest");
    try {
        var ourRendererExtensionTestApp = new RendererExtensionTestApp([expandEnvironment("${PRO}") + "/src/y60/shader/shaderlibrary_nocg.xml"]);
        ourRendererExtensionTestApp.setup(320, 200, "RendererExtensionTest");
        ourRendererExtensionTestApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}