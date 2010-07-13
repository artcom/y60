use("spark/spark.js");

//AC_NO_OFFSCREEN_TEST=1
//y60 testCanvas.js shaderlibrary_nocg.xml rehearsal "outputimage=testCanvas" "outputsuffix=.nocg.png"

var ourShow = spark.loadFile("fixtures/testCanvas.spark");
ourShow.parseArguments(arguments);

try {
    var Base = [];
    var myFrameCounter = 0;

    var myCanvas = ourShow.getChildByName("offscreen-canvas-scene");

    Base.onFrame = ourShow.onFrame;
    ourShow.onFrame = function(theTime) {
        Base.onFrame(theTime);
    }

    ourShow.setTestDurationInFrames(30);
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}