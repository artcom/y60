//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
// example start code
// y60 Skeleton.js rehearsal
//=============================================================================

use("SceneViewer.js");

plug("OscReceiver");

function OscReceiverDemo(theArguments, theWidth, theHeight, theTitle) {

    //////////////////////////////////////////////////////////////////////
    // Baseclass construction
    //////////////////////////////////////////////////////////////////////

    var Base   = {};
    var Public = this;
    SceneViewer.prototype.Constructor(Public, theArguments);

    var _myOscReceiver = null;
    

    Public.Constructor = function() {
        window = new RenderWindow();
        window.position = [0, 0];

        // Call base class setup
        Public.setup(theWidth, theHeight, false, theTitle);
        window.swapInterval    = 0;

        // create and start osc receiver (will call onOscEvent 
        // to tell about messages)
        _myOscReceiver = new OscReceiver();
    }

    //////////////////////////////////////////////////////////////////////
    // Callbacks
    //////////////////////////////////////////////////////////////////////

    Public.onOscEvent = function(theNode){
        print(theNode);
    }

    //////////////////////////////////////////////////////////////////////
    // Private
    //////////////////////////////////////////////////////////////////////

    Public.Constructor();
}

//
// main
//
try {
    var myWindowWidth  = 800;
    var myWindowHeight = 640;
    var ourShow = new OscReceiverDemo(arguments, myWindowWidth, myWindowHeight, "OscReceiverDemo");
    ourShow.go();
} catch (ex) {
    reportException(ex);
    exit(1);
}
