//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");
use("BumpUtils.js");

try {
    var ourShow = new SceneTester(arguments);

    ourShow.scene = null;
    ourShow.Base = [];

    ourShow.Base.setup = ourShow.setup;
    ourShow.setup = function() {

        ourShow.Base.setup();    

        ourShow.scene = new Scene("cube.x60");
        ourShow.scene.setup();
        ourShow.setScene(ourShow.scene);

        var myBody = window.scene.world.find(".//*[@name = 'pCube1']");
        if (!myBody) {
            Logger.error("Unable to find body");
            exit(1);
        }
        setupBumpMap(window.scene, myBody, "tex/testbild00_normalmap.rgb", "tex/testbild00.rgb");

        window.camera.position = [0.934855,0.929273,1.10204];
        window.camera.orientation = [0.2646,-0.330574,-0.0971124,0.900709];
    }

    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
