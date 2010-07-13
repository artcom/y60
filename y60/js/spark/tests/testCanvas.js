use("SceneTester.js");
use("spark/spark.js");

//AC_NO_OFFSCREEN_TEST=1
//y60 testCanvas.js shaderlibrary_nocg.xml rehearsal "outputimage=testCanvas" "outputsuffix=.nocg.png"

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