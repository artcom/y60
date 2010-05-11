
spark.OffscreenCanvas = spark.ComponentClass("OffscreenCanvas");

spark.OffscreenCanvas.Constructor = function (Protected) {
    var Public = this;
    var Base = {};
    this.Inherit(spark.Image);

    var _myWorld = null;
    var _myCanvasTexture = null;
    var _myRenderArea = null;
    var _myCamera = null;

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        setupOffscreenRenderer();
        setup3dView();
    };
    
    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
        var myMaterial = Public.sceneNode.$shape.find("./primitives/elements").$material;
        var myTextureUnit = myMaterial.find("./textureunits/textureunit");
        myTextureUnit.texture = _myCanvasTexture.id;
    }
    
    Public.camera setter = function(theCamera) {
        _myCamera = theCamera;
    };
        
    function setupOffscreenRenderer() {
        /*
        Hook the 3d-worlds camera into an ad-hoc viewport with our camera
        in the 3d-world. 
        */
        var myImage = Modelling.createImage(window.scene, window.width,
                window.height, "BGRA");
        
        _myCanvasTexture = Modelling.createTexture(window.scene, myImage);
        _myCanvasTexture.wrapmode = "clamp_to_edge";
        
        var myFlipMatrix = _myCanvasTexture.matrix;
        myFlipMatrix.makeScaling(new Vector3f(1, -1, 1));
        myFlipMatrix.translate(new Vector3f(0, 1, 0));
        
        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, _myCanvasTexture);
        myMaterial.transparent = true;
        
        var myCanvas = Node.createElement("canvas");
        myCanvas.target = _myCanvasTexture.id;
        window.scene.canvases.appendChild(myCanvas);

        var myViewport3d = Node.createElement("viewport");
        myCanvas.appendChild(myViewport3d);
        myViewport3d.camera = _myCamera.id;

        // create headlight for 3d-viewport
        var myLightManager = Public.stage.getLightManager();
        myLightManager.setupDefaultLighting(myCanvas);
        
        //XXX: This is turning lighting off for the 3d scene, depends on model, 
        // we probably do not always want this
        myViewport3d.lighting = false;
        
        _myRenderArea = new OffscreenRenderArea();
        _myRenderArea.setSceneAndCanvas(window.scene, myCanvas);
    }
    
    function setup3dView () {
        if (_myCamera !== null) {
            setupOffscreenRenderer();
        } else {
            Logger.warning("NO CAMERA DEFINED for Canvas 3d Renderer");
        }
        
        // Every frame event in the spark world is a frame event in the 3d-world
        Public.stage.addEventListener(spark.StageEvent.FRAME, function (theEvent) {
            _myRenderArea.renderToCanvas();
        });
    };

};
