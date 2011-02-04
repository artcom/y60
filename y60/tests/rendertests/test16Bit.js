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
//   $RCSfile: testCheckers.js,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2005/03/30 15:57:22 $
//
//
//=============================================================================

use("SceneTester.js");

try {
    var ourShow = new SceneTester(arguments);
    ourShow.setSplashScreen(false);
    ourShow.setup(256, 256);

    var myImageOverlay = new ImageOverlay(ourShow.getScene(), "tex/16bit.tiff");
    myImageOverlay.position = new Vector2i(1,1);

    renderer.backgroundcolor = [1,0,0,1];
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(1);
}
