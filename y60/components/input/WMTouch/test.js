/*jslint plusplus:false*/
/*globals use, spark, Node, Frustum, ProjectionType, Vector3f, Quaternionf,
          radFromDeg, print, Planef, TrackballMover, exit*/

plug("WMTouch");
use("spark/spark.js");

var ourShow = spark.loadFile("fixtures/testAlphaVideo.spark");
ourShow.parseArguments(arguments);

ourShow.getRenderWindow().addExtension(new WMTouch());

ourShow.onWMTouch = function(e) {
    Logger.error(e);
};
try {
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}

