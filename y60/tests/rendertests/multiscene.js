use("SceneTester.js");

var ourShow = new SceneTester(arguments);

ourShow.SceneTester = [];
var ourFrameCounter     = -1;
var ourCameraPosition   = null;
var ourCameraNode    = null;
var _mySceneCube = new Scene("cube.x60");
_mySceneCube.setup();
var _mySceneAlpha = new Scene("alpha_test.x60");
_mySceneAlpha.setup();
ourShow.SceneTester.setup = ourShow.setup;
ourShow.setup = function () {
    ourShow.SceneTester.setup();
    ourShow.offscreenFlag = false;
}


ourShow.onFrameDone = function (theFrameCount) {
    Logger.info("onFrameDone(" + theFrameCount + ")");
    if (theFrameCount == 1) {
        ourShow.setScene(_mySceneCube);
    }
    if (theFrameCount == 2) {
        ourShow.saveTestImage();
    }
    if (theFrameCount == 3) {
        ourShow.setScene(_mySceneAlpha);
    }
    if (theFrameCount == 4) {
        ourShow.saveTestImage();
    }
    if (theFrameCount == 5) {
        ourShow.setScene(_mySceneCube);
    }
    if (theFrameCount == 6) {
        ourShow.saveTestImage();
    }
    if (theFrameCount == 7) {
        ourShow.setScene(_mySceneAlpha);
    }
    if (theFrameCount == 8) {
        ourShow.saveTestImage();
        exit(0);
    }
}

try {
    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
