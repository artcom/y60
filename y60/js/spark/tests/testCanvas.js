use("SceneTester.js");

var ourShow = new SceneTester(arguments);

try {
    var Base = [];
    var myFrameCounter = 0;

    Base.setup = ourShow.setup;
    ourShow.setup = function() {
        Base.setup();
    }

    Base.onFrame = ourShow.onFrame;
    ourShow.onFrame = function(theTime) {
        Base.onFrame(theTime);
    }

    ourShow.setup();
    ourShow.setTestDurationInFrames(30);
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}