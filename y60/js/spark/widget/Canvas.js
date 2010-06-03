/**
 * Simple wrapper for the Y60 offscreen renderer.
 */

use("SceneViewer.js"); // XXX: bad place for this

spark.Canvas = spark.ComponentClass("Canvas");

spark.Canvas.Constructor = function(Protected) {
    var Public = this;
    var Base = {};

    this.Inherit(spark.Stage);
    this.Inherit(spark.ResizableRectangle);

    var _myRenderArea = null;
    var _myWorld = null;

    Public.innerSceneNode getter = function() {
    	return _myWorld;
    };

    Base.realize = this.realize;
    this.realize = function() {
    	_myRenderArea = new OffscreenRenderArea();

    	var myWidth = Protected.getNumber("width", 100);
    	var myHeight = Protected.getNumber("height", 100);

    	var myImage = Modelling.createImage(window.scene,
    										myWidth, myHeight, "BGRA");
    	myImage.name = Public.name + "_canvasImage";
    	var myTexture = Modelling.createTexture(window.scene, myImage);

        myTexture.wrapmode = "clamp_to_edge";

    	var myFlipMatrix = myTexture.matrix;
    	myFlipMatrix.makeScaling(new Vector3f(1,-1,1));
        myFlipMatrix.translate(new Vector3f(0,1,0));

    	var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myTexture);
        myMaterial.transparent = true;

    	_myWorld = Node.createElement("world");
    	window.scene.worlds.appendChild(_myWorld);
        _myWorld.name = Public.name + "-world";

    	var myCamera = Node.createElement("camera");
    	_myWorld.appendChild(myCamera);

    	spark.setupCameraOrtho(myCamera, myWidth, myHeight);

    	var myCanvas = Node.createElement("canvas");
    	myCanvas.target = myTexture.id;
    	window.scene.canvases.appendChild(myCanvas);
        myCanvas.name = Public.name + "-canvas";
        myCanvas.backgroundcolor[3] = 0.0;

    	var myViewport = Node.createElement("viewport");
    	myViewport.camera = myCamera.id;
    	myCanvas.appendChild(myViewport);
        myViewport.name = Public.name + "-viewport";

    	_myRenderArea.setSceneAndCanvas(window.scene, myCanvas);

    	Public.parent.stage.addEventListener(spark.StageEvent.FRAME, function(theEvent) {
    		_myRenderArea.renderToCanvas();
    	});

    	Base.realize(myMaterial);
    };

}
