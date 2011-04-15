/*jslint*/
/*globals use, spark, Vector2f, Vector3f, print, exit*/

use("spark/spark.js");

var ourShow = spark.loadFile("fixtures/testStretchyAtlasRenderTest.spark");
ourShow.parseArguments(arguments);

try {
    var Base = [];
    var myFrameCounter = 0;

    // Test START
    var myStretchyAtlas  = ourShow.getChildByName("stretchyatlas1");
    myStretchyAtlas.setTexture("2Dslider_thumb.png", "fixtures/atlasses/variousAtlas.xml");
    myStretchyAtlas.size = new Vector2f(300, 300);
    myStretchyAtlas.edgeTop    = 15;
    myStretchyAtlas.edgeBottom = 15;
    myStretchyAtlas.edgeLeft   = 15;
    myStretchyAtlas.edgeRight  = 15;

    var myStretchyAtlas  = ourShow.getChildByName("stretchyatlas2");
    myStretchyAtlas.edgeTop    = 1;
    myStretchyAtlas.edgeBottom = 18;
    myStretchyAtlas.edgeLeft   = 22;
    myStretchyAtlas.edgeRight  = 10;
    myStretchyAtlas.edges = [myStretchyAtlas.edgeLeft,  // this implicitely tests the getters as well
                             myStretchyAtlas.edgeBottom,
                             0 /*right*/,
                             myStretchyAtlas.edgeTop];
    
    myStretchyAtlas.cropTop    = 18;
    myStretchyAtlas.cropBottom = 1;
    myStretchyAtlas.cropLeft   = 10;
    myStretchyAtlas.cropRight  = 22;
    myStretchyAtlas.crop = [myStretchyAtlas.cropLeft,  // this implicitely tests the getters as well
                            myStretchyAtlas.cropBottom,
                            0 /*right*/,
                            myStretchyAtlas.cropTop];
    
    var myStretchyAtlas  = ourShow.getChildByName("stretchyatlas3");
    myStretchyAtlas.edges = [10, 10, 10, 10];
    myStretchyAtlas.crop  = [0, 0, 0, 0];
    myStretchyAtlas.height = 200;
    myStretchyAtlas.origin = new Vector3f(150, 100,0);
    myStretchyAtlas.x += 150;
    myStretchyAtlas.y += 100;

    var myStretchyAtlas  = ourShow.getChildByName("stretchyatlas4");
    myStretchyAtlas.cropTop    = 10;
    myStretchyAtlas.cropBottom = 15;
    myStretchyAtlas.cropLeft   = 10;
    myStretchyAtlas.cropRight  = 10;
    // Test END
    
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
