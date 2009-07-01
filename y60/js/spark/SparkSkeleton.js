//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
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
// XXX: spark should do this
use("SceneViewer.js");

// load spark (with sparkish widgets)
this.useSparkWidgets = true;
use("spark/spark.js");
use("spark/widget/NewText.js");

// load animation library
use("GUI/Animation.js");
var ourAnimationManager = new GUI.AnimationManager();
function playAnimation(theAnimation) {
    ourAnimationManager.play(theAnimation);
}

spark.SkeletonImpl = spark.ComponentClass("SkeletonImpl");
spark.SkeletonImpl.Constructor = function(Protected) {
    this.Inherit(spark.Window);
}

// load layouts and run the event loop
try {
    spark.SkeletonImpl.commandlineArguments = arguments;
    var ourApp = spark.loadFile("LAYOUT/Skeleton.spark");

    // wrap onframe to integrate GUI.Animation
    var myRealOnFrame = ourApp.onFrame;
    ourApp.onFrame = function(theTime) {
        ourAnimationManager.doFrame();
        myRealOnFrame.call(ourApp, theTime);
    }

    // remove mover so input events won't cause display trouble
    ourApp.setMover(null);
    
    // go and keep going
    ourApp.go();

} catch (ex) {
    reportException(ex);
    exit(1);
}
