/*jslint*/
/*globals window SceneTester use Scene Modelling print*/

use("SceneTester.js");

var CUBE_BODY_NAME = "pCube1";

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
            myBillboardBody.billboardlookat = myCubeBody.id;
        } else if (theFrameNum === 6) {
            ourShow.saveTestImage();
        } else if (theFrameNum === 8) {
            // test Axis with billboardlookat
            myBillboardBody.billboard = "axis";
        } else if (theFrameNum === 10) {
            ourShow.saveTestImage();
        } else if (theFrameNum === 12) {
            // test axis with defaulf billboardlookat
            myBillboardBody.billboardlookat = "";
        } else if (theFrameNum === 14) {
            ourShow.saveTestImage();
        }
    };
    
    ourShow.setup();
    var myScene = new Scene("cube.x60");
    myScene.setup();
    ourShow.setScene(myScene);
    
    var myCubeBody = window.scene.world.find("body[@name='" + CUBE_BODY_NAME + "']");
    myCubeBody.orientation = Quaternionf.createFromEuler([0.3,0.3,0.3]);
    
    var myImageNode = Modelling.createImage(window.scene, "tex/testbild02.png");
    var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myImageNode);
    var myShape  = Modelling.createQuad(window.scene,
                                        myMaterial.id,
                                        [-1, -1, 0],
                                        [1,   1, 0]);
    var myBillboardBody = Modelling.createBody(window.scene.world, myShape.id);
    myBillboardBody.billboard = "point";
    
    ourShow.setTestDurationInFrames(16);
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}