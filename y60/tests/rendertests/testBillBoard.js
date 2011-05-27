/*jslint*/
/*globals window SceneTester use Scene Modelling print*/

use("SceneTester.js");

var CUBE_BODY_ID = "DxI58waogW8=";

try {
    var Base = {};
    var ourShow = new SceneTester(arguments);
    
    Base.onFrame = ourShow.onFrame;
    ourShow.onFrame = function (theTime) {
        Base.onFrame(theTime);
        myBillboardBody.position.z = myCubeBody.boundingbox.min.z - 0.1;
    };
    
    Base.onFrameDone = ourShow.onFrameDone;
    ourShow.onFrameDone = function (theFrameNum) {
        //print("onFrameDone", theFrameNum);
        Base.onFrameDone(theFrameNum);
        if (theFrameNum === 2) {
            // test classical point billboard
            ourShow.saveTestImage();
        } else if (theFrameNum === 4) {
            // test billboardlookat
            myBillboardBody.billboardlookat = CUBE_BODY_ID;
        } else if (theFrameNum === 6) {
            ourShow.saveTestImage();
        }
    };
    
    ourShow.setup();
    var myScene = new Scene("cube.x60");
    myScene.setup();
    ourShow.setScene(myScene);
    
    var myCubeBody = window.scene.world.getElementById(CUBE_BODY_ID);
    
    var myImageNode = Modelling.createImage(window.scene, "tex/testbild02.png");
    var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myImageNode);
    //myMaterial.transparent = true;
    var myShape  = Modelling.createQuad(window.scene,
                                        myMaterial.id,
                                        [-1, -1, 0],
                                        [1,   1, 0]);
    var myBillboardBody = Modelling.createBody(window.scene.world, myShape.id);
    myBillboardBody.billboard = "point";
    
    ourShow.setTestDurationInFrames(10);
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}