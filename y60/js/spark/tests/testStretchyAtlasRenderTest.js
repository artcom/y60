/*jslint*/
/*globals use, spark, Vector3f, print, exit*/

use("spark/spark.js");

var ourShow = spark.loadFile("fixtures/testStretchyAtlasRenderTest.spark");
ourShow.parseArguments(arguments);

try {
    var Base = [];
    var myFrameCounter = 0;

    var myStretchyAtlas  = ourShow.getChildByName("stretchyatlas1");
    myStretchyAtlas.setTexture("2Dslider_thumb.png", "fixtures/atlasses/variousAtlas.xml");
    myStretchyAtlas.size = new Vector2f(300,300);

    var myStretchyAtlas  = ourShow.getChildByName("stretchyatlas2");
    myStretchyAtlas.edgeTop = 1;
    
    var myStretchyAtlas  = ourShow.getChildByName("stretchyatlas3");
    myStretchyAtlas.edges = [10, 10, 10,10];
    myStretchyAtlas.height = 200;

    
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
