use("SceneTester.js");

var ourShow = new SceneTester(arguments);

ourShow.SceneTester = [];
var ourFrameCounter     = -1;
var ourCameraPosition   = null;
var ourCameraNode    = null;
var ourFramesWritten = 0;
var Base = {};

ourShow.offscreenFlag = false;
ourShow.onFrameDone = function (theFrameCount) {
    // disable base class implementation
}

Base.onFrame = ourShow.onFrame;
ourShow.onFrame = function (theTime) {
    Base.onFrame(theTime);
    // time-based screen-shots
    if (ourFramesWritten == 0 && theTime > 0.5) {
        ourShow.saveTestImage();
        ourFramesWritten++;
    }
    if (ourFramesWritten == 1 && theTime > 1.5) {
        ourShow.saveTestImage();
        ourFramesWritten++;
    }
    if (ourFramesWritten == 2 && theTime > 2.5) {
        ourShow.saveTestImage();
        ourFramesWritten++;
    }
    if (ourFramesWritten == 3 && theTime > 3.5) {
        ourShow.saveTestImage();
        ourFramesWritten++;
        exit(0);
    }
}

try {
    ourShow.setup();
    window.fixedFrameTime = 0.01;
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}

