//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

try {
    use("TestOffScreenModule.js");

    var ourShow = new TestOffScreen(arguments);
    print("\n\n************** testOffscreen with Backbuffer support *****************\n\n");
    ourShow.setSplashScreen(false);
    ourShow.setup(800, 600, false);
    ourShow.go();
} catch (ex) {
    reportException(ex);
}
