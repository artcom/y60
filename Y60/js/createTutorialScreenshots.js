//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

const ourFrameBreak      = 5;
const ourScreenshotSize  = [200,200];
const ourTutorialPath    = arguments[0];
const ourTargetDirectory = arguments[1];

var ourFrameCount        = 0;

if (!fileExists(ourTutorialPath) || !isDirectory(ourTargetDirectory)) {
    print("Usage: createTutorialScreenshots.js TUTORIAL_SCRIPT TARGET_DIRECTORY");
    exit(1);
}

RenderWindow.prototype.onProtoFrame = function(theTime) {
    this.fixedFrameTime = 0.04;
    if (ourFrameCount == 0) {
        print("Resizing window to " + ourScreenshotSize[0] + "x" + ourScreenshotSize[1]);
        this.resize(ourScreenshotSize[0], ourScreenshotSize[1]);
    } else if (ourFrameCount == ourFrameBreak) {
        var myTutorial = getFilenamePart(ourTutorialPath);
        var myDotIndex = myTutorial.lastIndexOf(".");
        if (myDotIndex != -1 && myTutorial.substring(myDotIndex, myTutorial.length) == ".js") {
            var myScreenshotFileName = ourTargetDirectory + "/" + myTutorial.substr(0, myDotIndex) + ".png";
            print ("Generating screenshot " + myScreenshotFileName + " size=" + window.width + "x" + window.height);
            this.saveBuffer(myScreenshotFileName);
            exit(0);
        } else {
            print("### ERROR: Not a valid js file: " + myTutorial);
            exit(1);
        }
    }
    ourFrameCount++;
}

includePath("${PRO}/tutorials");

use(ourTutorialPath);
