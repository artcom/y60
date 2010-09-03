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

/*jslint */
/*globals use, GUI, spark, exit, reportException*/

use("spark/spark.js");
// load animation library
use("GUI/Animation.js");

var ourAnimationManager = new GUI.AnimationManager();
function playAnimation(theAnimation) {
    ourAnimationManager.play(theAnimation);
}

spark.SkeletonImpl = spark.ComponentClass("SkeletonImpl");
spark.SkeletonImpl.Constructor = function (Protected) {
    this.Inherit(spark.Window);
};

// load layouts and run the event loop
try {
    spark.SkeletonImpl.commandlineArguments = arguments;
    var ourApp = spark.loadFile("LAYOUT/Skeleton.spark");

    // wrap onframe to integrate GUI.Animation
    var myRealOnFrame = ourApp.onFrame;
    ourApp.onFrame = function (theTime, theDeltaT) {
        ourAnimationManager.doFrame(theTime);
        myRealOnFrame.call(ourApp, theTime, theDeltaT);
    };

    // remove mover so input events won't cause display trouble
    ourApp.setMover(null);
    ourApp.go();
} catch (ex) {
    reportException(ex);
    exit(1);
}