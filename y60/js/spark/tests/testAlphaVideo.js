/*jslint plusplus:false*/
/*globals use, spark, Node, Frustum, ProjectionType, Vector3f, Quaternionf,
          radFromDeg, print, Planef, TrackballMover, exit*/

use("spark/spark.js");

//AC_NO_OFFSCREEN_TEST=1
//y60 testCanvas.js shaderlibrary_nocg.xml rehearsal "outputimage=testCanvas" "outputsuffix=.nocg.png"

var ourShow = spark.loadFile("fixtures/testAlphaVideo.spark");
ourShow.parseArguments(arguments);

try {
    var Base = [];
    var myFrameCounter = 0;

    var myMovie = ourShow.getChildByName("movie");
    myMovie.play();
    Base.onFrame = ourShow.onFrame;
    ourShow.onFrame = function (theTime) {
        Base.onFrame(theTime);
    };

    ourShow.setTestDurationInFrames(30);
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
