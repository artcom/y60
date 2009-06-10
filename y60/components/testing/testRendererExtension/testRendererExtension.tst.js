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

// ScriptablePlugin's can have their own methods now ...
_myExtension.foo();

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
