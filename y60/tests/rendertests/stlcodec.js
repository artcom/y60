use("SceneTester.js");

var ourShow = new SceneTester(arguments);
ourShow.SceneTester = [];
plug("StlImporter");

var ourFrameCounter     = -1;
var _myReferenceName = "stltest.x60";
var _myBigName = "stlcodectest_big.stl";
var _myLittleName = "stlcodectest_little.stl";
var _myScene = new Scene(_myReferenceName);
_myScene.setup();

var myShapes = _myScene.shapes.childNodes;
var _myBigScene = null;
var _myLittleScene = null;
if (1) {
    var i;
    var myCodec = new StlCodec(_myBigName, true);
    for (i=0; i < myShapes.length; ++i) {
        myCodec.exportShape(myShapes[i]);
        print(_myBigName + "shape: " + i);
    }
    myCodec.close();
    myCodec = null;
    myCodec = new StlCodec(_myLittleName, false);
    for (i=0; i < myShapes.length; ++i) {
        myCodec.exportShape(myShapes[i]);
        print(_myLittleName + "shape: " + i);
    }
    myCodec.close();
    myCodec = null;
    gc();
}

ourShow.SceneTester.setup = ourShow.setup;
ourShow.setup = function () {
    ourShow.SceneTester.setup();
}

ourShow.onFrameDone = function (theFrameCount) {
    if (theFrameCount == 10) {
        _myBigScene = new Scene(_myBigName);
        _myBigScene.setup();
        _myLittleScene = new Scene(_myLittleName);
        _myLittleScene.setup();
    }
    if (theFrameCount == 20) {
        var myStlCamera = _myBigScene.world.find("camera");
        var myBody = _myBigScene.world.find("*[@name = 'Main']");
        myStlCamera.position.z = 0.7;
        myBody.orientation = Quaternionf.createFromEuler(new Vector3f(Math.PI/4, Math.PI/4, 0));
        ourShow.setScene(_myBigScene);
        //ourShow.getRenderWindow().wireframe = true;
    }
    if (theFrameCount == 21) {
        ourShow.saveTestImage();
    }
    if (theFrameCount == 30) {
        myStlCamera = _myLittleScene.world.find("camera");
        myBody = _myLittleScene.world.find("*[@name = 'Main']");
        myStlCamera.position.z = 0.7;
        myBody.orientation = Quaternionf.createFromEuler(new Vector3f(Math.PI/4, Math.PI/4, 0));
        ourShow.setScene(_myLittleScene);
        //ourShow.getRenderWindow().wireframe = true;
    }
    if (theFrameCount == 31) {
        ourShow.saveTestImage();
        exit(0);
    }
}

try {
    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
