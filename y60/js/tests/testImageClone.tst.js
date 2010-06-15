// ==============================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/*jslint plusplus: false*/
/*globals use, SceneTester, Modelling, window, adjustNodeId, getImageSize,
          print, exit*/

use("SceneTester.js");

var ourShow = new SceneTester(arguments);
try {
    ourShow.SceneTester = [];
    // public methods
    ourShow.setupTest = function () {
        var myImage = Modelling.createImage(window.scene, 1, 1, "RGBA");
        var myImageClone = myImage.cloneNode(true);
        adjustNodeId(myImageClone, true);
        window.scene.images.appendChild(myImageClone);
        var myImageSize = getImageSize(myImageClone);
        print("Accessing the image reveals the error, i.w. getImageSize() : " + myImageSize);
    };
    var framecount = 0;
    ourShow.onFrame = function () {
        ++framecount;
        if (framecount === 5) {
            exit(0);
        }
    };
    ourShow.setup(100, 100);
    ourShow.setupTest();
    ourShow.go();
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(-1);
}