/*jslint nomen: false*/
/*globals use, spark, OffscreenRenderArea, Modelling, window, Node, Vector3f*/

/**
 * Wrapper for the Y60 offscreen renderer.
 */

spark.Canvas = spark.ComponentClass("Canvas");

spark.Canvas.Constructor = function(Protected) {
    var Public = this;
    var Base = {};
    
    this.Inherit(spark.ResizableRectangle);
    BaseViewer.prototype.Constructor(this, []);
    this.Inherit(spark.CallbackWrapper);
    
    var _myRenderArea = null;
    var _myWorld = null;
    var _myViewport = null;
    
    const PICK_RADIUS = 0.01;
    
   //XXX is this really needed that way? Cannot access spark innernode that way
   // Public.innerSceneNode getter = function() {
   //     return _myWorld;
   // };
    
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
        
        var mySceneFile = Protected.getString("sceneFile", "");
        
        if (mySceneFile) {
            var myDom = prepareMerge(mySceneFile);
            
            // XXX assumption: only one world exist/ is handled
            var myWorldId = myDom.find(".//worlds/world").id;
            var myCanvasId = myDom.find(".//canvases/canvas").id;
            
            mergeScenes(window.scene, myDom);
            
            var myCanvas = window.scene.dom.find(".//canvases/canvas[@id='" + myCanvasId + "']");
            _myViewport = myCanvas.find(".//viewport");
            myCamera   = window.scene.dom.getElementById(myWorldId).find(".//camera");
            
            _myWorld = window.scene.dom.getElementById(myWorldId);
            
        } else {
            _myWorld = Node.createElement("world");
            window.scene.worlds.appendChild(_myWorld);
            _myWorld.name = Public.name + "-world";
            
            var myCamera = Node.createElement("camera");
            _myWorld.appendChild(myCamera);
            
            spark.setupCameraOrtho(myCamera, myWidth, myHeight);
            
            var myCanvas = Node.createElement("canvas");
            window.scene.canvases.appendChild(myCanvas);
            myCanvas.name = Public.name + "-canvas";
            myCanvas.backgroundcolor[3] = 0.0;
            
            _myViewport = Node.createElement("viewport");
            _myViewport.camera = myCamera.id;
            myCanvas.appendChild(_myViewport);
            _myViewport.name = Public.name + "-viewport";
        }
        
        var myLightManager = new LightManager(window.scene, _myWorld);
        Public.setLightManager(myLightManager);
        Public.setupWindow(_myRenderArea);
        
        myCanvas.target = myTexture.id;
        _myRenderArea.setSceneAndCanvas(window.scene, myCanvas);
        
        Public.setCanvas(myCanvas);
        
        Public.parent.stage.addEventListener(spark.StageEvent.FRAME, Public.onFrame);
        Public.stage.addEventListener(spark.KeyboardEvent.KEY_DOWN, onKey);
        Public.stage.addEventListener(spark.KeyboardEvent.KEY_UP, onKey);
        Public.addEventListener(spark.MouseEvent.MOVE, Public.onMouseMotion);
        Public.addEventListener(spark.MouseEvent.BUTTON_DOWN, Public.onMouseButtonDown);
        Public.addEventListener(spark.MouseEvent.BUTTON_UP, Public.onMouseButtonUp);
        
        Base.realize(myMaterial);
        
        Public.registerMover(WalkMover);
        Public.setMover(WalkMover, _myViewport);
    };
    
    Base.onFrame = Public.onFrame;
    Public.onFrame = function (theEvent) {
        Base.onFrame(theEvent.currenttime);
        _myRenderArea.renderToCanvas();
    }
    
    Base.onMouseButtonDown = Public.onMouseButtonDown;
    Public.onMouseButtonDown = function(theEvent) {
        Base.onMouseButtonDown(theEvent);
        
        var canvasPosition = convertToCanvasCoordinates (theEvent.stageX, theEvent.stageY);
        var myBody = pickBody(canvasPosition.x,  canvasPosition.y);
        print("canvas picket body:", myBody);
    };
    
    Base.onMouseButton = Public.onMouseButton;
    Public.onMouseButton = function(theButton, theState, theX, theY) {
        var canvasPosition = convertToCanvasCoordinates (theX, theY);
        Base.onMouseButton(theButton, theState, canvasPosition.x, canvasPosition.y);
    };
    
    function convertMousePositionToCanvasCoordinates(theX, theY) {
        return new Vector2f(theX/2, theY/2);
    }
    
    function convertToCanvasCoordinates(theX, theY) {
        var myIntersection = Public.stage.picking.pickIntersection(theX, theY);
        // assumptions:
        //  - as the rectangle has no depth, there should always only appear ONE intersection
        //  - the Canvas is topmost intersection, if not, this handler would not have been called by spark
        var PointOnCanvas = new Point3f(myIntersection.info.intersections[0].position);
        var transformMatrix = new Matrix4f(Public.innerSceneNode.globalmatrix);
        transformMatrix.invert();
       
        return product(PointOnCanvas, transformMatrix);
    }
    
    function pickBody(theX, theY) {
        var myBody = Public.picking.pickBodyBySweepingSphereFromBodies(theX, theY, PICK_RADIUS, _myWorld, _myViewport);
        if(myBody) {
            return myBody;
        }
        return null;
    };
    
    function onKey(theEvent) {
        // this works because keyboard modifiers are manipulated bitwise
        if(theEvent.modifiers < spark.Keyboard.CTRL) {
            return;
        }
        var myState = (theEvent.type == "keybord-key-down");
        var myShiftFlag = (theEvent.modifiers == spark.Keyboard.CTRL_SHIFT);
        Public.getLightManager().onKey(theEvent.key, myState, myShiftFlag);
        Public.getMover(_myViewport).onKey(theEvent.key, myState, 0, 0, myShiftFlag, true, false);
    };
    
    function prepareMerge (theSceneFilePath) {
        /* adjust ids:
         *  - world, canvas, viewport ids need to be unique as we need duplicates
         *  - camera id needs to be unique, is linked in viewport, so viewport.camera needs update too
         *  -> !!! current assumption: only one World, Camera, Canvas and Viewport per scene !!!
         *  - all other duplicate ids are supposed to be same model as already exists and will share shape etc
         *
         * adjust file path of textures within scene DOM
         *  - path is originally set relative to Model file location
         *  -> due to scene merge path needs to be updated (relative to application directory)
         */
        
        Logger.info("Loading spark file " + theSceneFilePath);
        if (!fileExists(theSceneFilePath)) {
            throw Error("spark file '" + theSceneFilePath + "' does not exist.");
        }
        
        var myDom = new Node();
        myDom.parseFile(theSceneFilePath);
        
        adjustNodeId(myDom.find(".//world"), true);
        adjustNodeId(myDom.find(".//canvases/canvas"), false);
        var myViewport = myDom.find(".//viewport");
        adjustNodeId(myViewport, false);
        adjustNodeId(myDom.find(".//camera"), false);
        myViewport.camera = myDom.find(".//camera").id;
        
        // REWRITE TEXTURE SRC PATH
        var imageNode = myDom.find(".//images");
        for(var i = 0; i < imageNode.childNodesLength(); ++i) {
            var myTargetDir = getDirectoryPart(theSceneFilePath);
            var myTexSrcPath = imageNode.childNodes[i].src;
            if(myTexSrcPath.charAt(0) !== "/") {
                myTexSrcPath = myTexSrcPath.replace(/^\.\//, "");
                var myNewPath = myTargetDir + myTexSrcPath;
                if(!fileExists(myNewPath)) {
                    Logger.warning("Could not find texture within path '"+myNewPath+"'");
                } else {
                    imageNode.childNodes[i].src = myNewPath;
                }
            }
        }
        return myDom;
    }
    
    function mergeScenes(theTargetScene, theModelDom) {
        var mySceneNode = theModelDom.firstChild;
        
        while (mySceneNode.childNodesLength()) {
            var childNode = mySceneNode.firstChild;
            var receivingNode = theTargetScene.dom.getNodesByTagName(childNode.nodeName)[0];
            while (childNode.childNodesLength()) {
                var childChildNode = childNode.removeChild(childNode.firstChild);
                try {
                    receivingNode.appendChild(childChildNode);
                }
                catch(err) {
                    print("--> Node '" + childChildNode.nodeName + "' with id='"+childChildNode.id+"' already exists but is not needed. Don't worry."); 
                }
            }
            mySceneNode.removeChild(mySceneNode.firstChild);
        }
    };
};
