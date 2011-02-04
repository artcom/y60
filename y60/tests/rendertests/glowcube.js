//=============================================================================
// Copyright (C) 2006-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");
use("Y60JSSL.js");
use("Glow.js");

var ourShow = new SceneTester(arguments);

try {
    var Base = [];
    
    Base.setup = ourShow.setup;
    ourShow.setup = function() {
        Base.setup();
        window.lighting = false;
        window.canvas.backgroundcolor = new Vector4f(0,0,1,1);
        ourShow.offscreenFlag = false;
        ourShow.glow = true;
    }

    ourShow.setup();
    ourShow.setTestDurationInFrames(32);

    //window.scene.save("glow-dump.x60");
    ourShow.go()
}
catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
