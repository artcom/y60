use("spark/spark.js");

//AC_NO_OFFSCREEN_TEST=1
//y60 testCanvas.js shaderlibrary_nocg.xml rehearsal "outputimage=testCanvas" "outputsuffix=.nocg.png"

var ourShow = spark.loadFile("fixtures/testCanvas.spark");
ourShow.parseArguments(arguments);

try {
    var Base = [];
    var myFrameCounter = 0;

    var myCanvas = ourShow.getChildByName("offscreen-canvas-scene");
    
    // --- empty canvas setup --------------------------------------------------
    var myEmptyCanvas = ourShow.getChildByName("empty-offscreen-canvas");
    
    // SETUP PERSPECTIVE CAMERA
    var myCamera = Node.createElement("camera");
    myCamera.frustum = new Frustum();
    myCamera.frustum.width = myEmptyCanvas.width;
    myCamera.frustum.height = myEmptyCanvas.height;
    myCamera.frustum.type = ProjectionType.perspective;
    myCamera.position = new Vector3f(0,1,2);
    myCamera.orientation = Quaternionf.createFromEuler(new Vector3f(radFromDeg(-30),0,0));
    
    var myWorld = myCanvas.world;
    myWorld.appendChild(myCamera);
    myEmptyCanvas.getActiveViewport().camera = myCamera.id;
    myEmptyCanvas.setActiveCamera(myCamera);
    
    // --- clipping planes -----------------------------------------------------
    var myPlaneNode = Node.createElement("plane");
    myWorld.appendChild(myPlaneNode);
    
    var myClippingPlane = new Planef(new Vector3f(0, -1, 0), 0);
    myPlaneNode.value = myClippingPlane;
    
    myEmptyCanvas.onPreViewportFunc = function() {
        myWorld.clippingplanes = "[`" + myPlaneNode.id + "`]";
    }
    myEmptyCanvas.onPostViewportFunc = function() {
        myWorld.clippingplanes = "[]";
    }
    
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
