use("SceneTester.js");

var ourShow = new SceneTester(arguments);
ourShow.SceneTester = [];


ourShow.onFrameDone = function (theFrameCount) {
    try {
        var myPoTSize = new Vector2f( 1024, 1024);

        // create render traget
        var myImage = Modelling.createImage(window.scene, myPoTSize.x, myPoTSize.y, "rgba");
        myImage.name = "HiresImage";

        var myTexture = Modelling.createTexture(window.scene, myImage);
        myTexture.mipmap = false;

        // setup offscreen canvas
        var mySourceViewport = ourShow.getRenderWindow().canvas.find("viewport");

        var mySourceHeadlight = ourShow.getLightManager().getHeadlight(mySourceViewport);

        var myCanvas = ourShow.getRenderWindow().canvas.cloneNode(true);
        myCanvas.id = createUniqueId();
        myCanvas.name = "OffscreenBuffer";
        myCanvas.targets = [myTexture.id];
        // switching back to black background. This is core part of the test
        myCanvas.backgroundcolor[0] = 0.0;

        var myViewport = myCanvas.find("viewport");
        myViewport.id = createUniqueId();
        myViewport.name ="hires_vp";

        var myCanvases = window.scene.canvases;
        myCanvases.appendChild(myCanvas);

        var myOffscreenBuffer = new OffscreenRenderArea();
        myOffscreenBuffer.renderingCaps = Renderer.FRAMEBUFFER_SUPPORT;

        var myOffscreenViewer = new BaseViewer();
        myOffscreenViewer.attachWindow(myOffscreenBuffer);
        myOffscreenBuffer.scene = ourShow.getScene();
        myOffscreenViewer.prepareScene(ourShow.getScene(), myCanvas);

        myOffscreenBuffer.setWidth(ourShow.getRenderWindow().width);
        myOffscreenBuffer.setHeight(ourShow.getRenderWindow().height);
        myOffscreenBuffer.renderToCanvas(false);

        var myOutImage = Modelling.createImage(window.scene, myPoTSize.x, myPoTSize.y, "rgba");
        var myOutTexture = Modelling.createTexture(window.scene, myOutImage);
        myOffscreenBuffer.downloadFromViewport( myOutTexture );

        saveImageFiltered( myOutImage, ourShow.getTestImageName(), ["flip"], [[]]);

        // cleanup
        myCanvases.removeChild(myCanvas);
        ourShow.getScene().images.removeChild( myImage );
        ourShow.getScene().textures.removeChild( myTexture );

        ourShow.getScene().images.removeChild( myOutImage );
        ourShow.getScene().textures.removeChild( myOutTexture );

        exit(0);
    } catch (ex) {
        reportException(ex);
        exit(1);
    }
}

try {
    ourShow.setup();
    var myScene = new Scene("cube.x60");
    myScene.setup();
    ourShow.setScene(myScene);
    // Background color intentionally set to red. But baseline image *must* be black.
    // This is core part of the test.
    ourShow.getRenderWindow().canvas.backgroundcolor[0] = 1.0;

    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}

