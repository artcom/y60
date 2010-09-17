/*jslint*/
/*globals use, spark, Vector3f, print, exit*/

use("spark/spark.js");

var ourShow = spark.loadFile("fixtures/testAtlasRenderTest.spark");
ourShow.parseArguments(arguments);

try {
    var Base = [];
    var myFrameCounter = 0;
    
    // Test start
    var myButton1 = ourShow.getChildByName("button1");
    myButton1.setTexture("button_pressed.png", "fixtures/atlasses/buttonAtlas.xml");
    myButton1.width = 400;
    
    var myButton3 = ourShow.getChildByName("button3");
    myButton3.setTexture("checkers.png", "fixtures/atlasses/variousAtlas.xml");
    myButton3.width  = myButton3.width * 2;
    myButton3.height = myButton3.height * 2;
    
    var myProgrammaticAtlasImage = new spark.AtlasImage();
    myProgrammaticAtlasImage.setTexture("testbild00.rgb", "fixtures/atlasses/variousAtlas.xml");
    
    ourShow.addChild(myProgrammaticAtlasImage);
    myProgrammaticAtlasImage.realize();
    myProgrammaticAtlasImage.postRealize();
    myProgrammaticAtlasImage.position = new Vector3f(275, 200, 5);
    // Test end
    
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
