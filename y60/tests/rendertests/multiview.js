use("SceneTester.js");

var ourShow = new SceneTester(arguments);

function setupViewports() {
    var myCanvas = ourShow.getRenderWindow().canvas;
    var myRightViewport = myCanvas.find("viewport");
    myRightViewport.position = '[0.3,0]';
    myRightViewport.size = '[0.7,1]';
    // now create a small viewport on the left for each camera in the scene
    var myCameras = window.scene.world.findAll("camera");
    var n = myCameras.length;
    Logger.info("found "+n+" cameras");
    for (var i = 0; i < n; ++i) { 
        var newViewport = Node.createElement('viewport');
        newViewport.name = "customViewport"+i;
        myCanvas.appendChild(newViewport);
        newViewport.position.x = 0;
        newViewport.position.y = i/n;
        newViewport.size.x = 0.3;
        newViewport.size.y = 1/n;
        newViewport.camera = myCameras[i].id;
    }
    // call setCanvas so the lightmanager can generate new headlights for
    // the added viewports
    ourShow.setCanvas(myCanvas);
}


try {
    ourShow.setup();
    setupViewports();    
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}

