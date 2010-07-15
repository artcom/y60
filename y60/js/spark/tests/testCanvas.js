use("spark/spark.js");

//AC_NO_OFFSCREEN_TEST=1
//y60 testCanvas.js shaderlibrary_nocg.xml rehearsal "outputimage=testCanvas" "outputsuffix=.nocg.png"

var ourShow = spark.loadFile("fixtures/testCanvas.spark");
ourShow.parseArguments(arguments);

try {
    var Base = [];
    var myFrameCounter = 0;

    var myCanvas = ourShow.getChildByName("offscreen-canvas-scene");
    var myWorld = myCanvas.world;
    
    // EMPTY CANVASES SETUP ----------------------------------------------------
    var myEmptyCanvas = ourShow.getChildByName("offscreen-canvas-empty");
    var myEmptyMovertestCanvas = ourShow.getChildByName("offscreen-canvas-empty-movertest");
    var myEmptyCanvases = [myEmptyCanvas, myEmptyMovertestCanvas];
    
    // SETUP PERSPECTIVE CAMERA
    for(var i = 0; i < myEmptyCanvases.length; ++i) {
        var myCamera = Node.createElement("camera");
        myCamera.frustum = new Frustum();
        myCamera.frustum.width = myEmptyCanvases[i].width;
        myCamera.frustum.height = myEmptyCanvases[i].height;
        myCamera.frustum.type = ProjectionType.perspective;
        myCamera.position = new Vector3f(0,1,2);
        myCamera.orientation = Quaternionf.createFromEuler(new Vector3f(radFromDeg(-30),0,0));
        
        myWorld.appendChild(myCamera);
        myEmptyCanvases[i].getActiveViewport().camera = myCamera.id;
        myEmptyCanvases[i].setActiveCamera(myCamera);
    }
    
    
    // CLIPPING PLANE FOR EMPTY-CANVAS
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
    
    
    // MOVER SETTINGS/MODIFICATION FOR EMPTY-MOVERTEST-CANVAS
    myEmptyMovertestCanvas.setMover(TrackballMover, myEmptyMovertestCanvas.getActiveViewport());
    var myRotation = Quaternionf.createFromEuler(new Vector3f(radFromDeg(-20),0,0));
    myEmptyMovertestCanvas.getMover().movements.rotateByQuaternion(myRotation);
    
    
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
