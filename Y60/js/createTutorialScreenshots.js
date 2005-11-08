
const myFrameBreak = 50;
const myScreenshotSize = [200,200];

var myFrameCount = 0;
var myTutorialPath = arguments[0];
var myTargetDirectory = arguments[1];

if (!fileExists(myTutorialPath) || !isDirectory(myTargetDirectory)) {
    print("Usage: createTutorialScreenshots.js TUTORIAL_SCRIPT TARGET_DIRECTORY");
    exit(1);
}

RenderWindow.prototype.onProtoFrame = function(theTime) {
    this.fixedFrameTime = 0.04;
    if (myFrameCount == myFrameBreak - 5) {
        this.setVideoMode(myScreenshotSize[0], myScreenshotSize[1], false);

    } else if (myFrameCount == myFrameBreak) {
        var myTutorial = basename(myTutorialPath);
        var myDotIndex = myTutorial.lastIndexOf(".");
        if (myDotIndex != -1 && myTutorial.substring(myDotIndex, myTutorial.length) == ".js") {
            var myScreenshotFileName = myTutorial.substr(0, myDotIndex) + ".png";
            print ("Generating screenshot " + myTargetDirectory + "/" + myScreenshotFileName);
            this.saveBuffer(myTargetDirectory + "/" + myScreenshotFileName);
            exit(0);
        } else {
            print("### ERROR: Not a valid js file: " + myTutorial);
            exit(1);
        }
    }
    myFrameCount++;
}

includePath("${PRO}/tutorials");

use(myTutorialPath);
