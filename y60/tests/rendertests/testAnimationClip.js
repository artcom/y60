//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: testAnimationClip.js,v $
//   $Author: jens $
//   $Revision: 1.2 $
//   $Date: 2005/04/18 14:29:39 $
//
//=============================================================================

use("AnimationManager.js");
use("SceneTester.js");

var ourFrame = 0;

try {
    var ourShow = new SceneTester(arguments);
    ourShow.setSplashScreen(false);
    ourShow.setup();
    ourShow.setTestDurationInFrames(15);
    ourShow.getAnimationManager().enable(false);
    ourShow.getAnimationManager().startClip("pCube1Ch", "clipSource");    
    
    var myBaseOnIdleFunc = ourShow.onFrame;
    ourShow.onFrame = function(theTime) {
        ourShow.getAnimationManager().setFrame(ourFrame++);
        myBaseOnIdleFunc(theTime);
    }
    
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}

