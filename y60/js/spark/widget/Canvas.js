/*jslint nomen: false, plusplus: false, bitwise: false*/
/*globals use, spark, OffscreenRenderArea, Modelling, window, Node, Vector3f,
          BaseViewer, LightManager, product, Matrix4f, Point3f, Logger,
          fileExists, getDirectoryPart, print, adjustNodeId, Renderer,
          Vector4f, searchFile, createUniqueId*/

/**
 * Wrapper for the Y60 offscreen renderer.
 */

spark.Canvas = spark.ComponentClass("Canvas");

spark.Canvas.BINDING_SLOT = {
    PRE  : "PRE_VIEWPORT",
    POST : "POST_VIEWPORT",
    RESIZE : "RESIZE"
};

spark.Canvas.Constructor = function (Protected) {
    var Public = this;
    var Base = {};
    
    Public.Inherit(spark.ResizableRectangle);
    BaseViewer.prototype.Constructor(Public, []);
    Public.Inherit(spark.CallbackWrapper);
    
    var _myRenderArea       = null;
    var _myWorld            = null;
    var _myViewport         = null;
    var _myCanvasNode       = null;
    var _onPreViewportFunc  = null;
    var _onPostViewportFunc = null;
    var _myRenderFlag       = true;
    var _myPickRadius       = 0;
    var _myImage            = null;
    var _myCamera           = null;
    var _bindings = {};
    (function () {
        for (var slot in spark.Canvas.BINDING_SLOT) {
            _bindings[spark.Canvas.BINDING_SLOT[slot]] = {};
        }
    }());
    

    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function onKey(theEvent) {
        var myState = (theEvent.type === "keybord-key-down");
        var myShiftFlag = (theEvent.modifiers === spark.Keyboard.SHIFT) ||
                          (theEvent.modifiers === spark.Keyboard.ALT_SHIFT) ||
                          (theEvent.modifiers === spark.Keyboard.CTRL_SHIFT) ||
                          (theEvent.modifiers === spark.Keyboard.CTRL_ALT_SHIFT);
        var myAltFlag = (theEvent.modifiers === spark.Keyboard.ALT) ||
                        (theEvent.modifiers === spark.Keyboard.ALT_SHIFT) ||
                        (theEvent.modifiers === spark.Keyboard.CTRL_ALT) ||
                        (theEvent.modifiers === spark.Keyboard.CTRL_ALT_SHIFT);
        // this works because keyboard modifiers are manipulated bitwise
        var myCtrlFlag = (theEvent.modifiers >= spark.Keyboard.CTRL);
        
        if (Public.getLightManager()) {
            Public.getLightManager().onKey(theEvent.key, myState, myShiftFlag);
        }
        var myMover = Public.getMover(_myViewport);
        if (myMover) {
            myMover.onKey(theEvent.key, myState, 0, 0, myShiftFlag, myCtrlFlag, myAltFlag);
        }
    }
    
    function _unbind(theHandle) {
        delete _bindings[theHandle.bind_info.slot][theHandle.bind_info.id];
        return true;
    }
    
    function Handle(theBindInfo, theCb) {
        this.bind_info = theBindInfo; //back-pointer
        this.cb = theCb;
    }
    
    Handle.prototype.unbind = function () {
        _unbind(this);
    };
    
    ///////////////////////
    // Getters / Setters //
    ///////////////////////
    
    Public.bind = function (theBindingSlot, cb) {
        var bind_info = {id   : createUniqueId(),
                         slot : theBindingSlot};
        var my_handle = new Handle(bind_info, cb);
        _bindings[theBindingSlot][bind_info.id] = my_handle;
        return my_handle;
    };
    
    Public.__defineGetter__("world", function () {
        return _myWorld;
    });
    
    Public.__defineGetter__("viewport", function () {
        return _myViewport;
    });
    
    Public.__defineSetter__("onPreViewportFunc", function (theCallback) {
        _onPreViewportFunc = theCallback;
    });
    
    Public.__defineSetter__("onPostViewportFunc", function (theCallback) {
        _onPostViewportFunc = theCallback;
    });
    
    Public.__defineSetter__("render", function (theBoolean) {
        _myRenderFlag = theBoolean;
    });
    Public.__defineGetter__("render", function () {
        return _myRenderFlag;
    });
    
    Public.__defineGetter__("image", function () {
        return _myImage;
    });
    Public.__defineGetter__("renderArea", function () {
        return _myRenderArea;
    });
    Public.__defineGetter__("camera", function () {
        return _myCamera;
    });
    
    
    function applyWidth(theWidth, theBaseSetter) {
        theBaseSetter(theWidth);
        var mySize = new Vector2f(theWidth, getImageSize(_myImage).y);
        applySize(mySize);
    }

    function applyHeight(theHeight, theBaseSetter) {
        theBaseSetter(theHeight);
        var mySize = new Vector2f(getImageSize(_myImage).x, theHeight);
        applySize(mySize);
    }

    function applySize(theSize) {
        _myImage.raster.resize(theSize.x, theSize.y);
        _myRenderArea.setWidth(theSize.x);
        _myRenderArea.setHeight(theSize.y);
        for (var handleId in _bindings[spark.Canvas.BINDING_SLOT.RESIZE]) {
            var myHandle = _bindings[spark.Canvas.BINDING_SLOT.RESIZE][handleId];
            myHandle.cb(theSize);
        }
    }

    Public.SetterOverride("width", applyWidth);
    Public.SetterOverride("height", applyHeight);
    

    //pickRadius = 0 will result in fast ray intersection picking, 
    // when pickRadius > 0 slower sweepsphere picking will be activated
    Public.__defineSetter__("pickRadius", function (theNewRadius) {
        _myPickRadius = theNewRadius;
    });
    
    Public.__defineGetter__("pickRadius", function () {
        return _myPickRadius;
    });
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Public.convertToCanvasCoordinates = function (theX, theY) {
        var transformMatrix;
        var intersecPointOnCanvas = null;
        // assumptions:
        //  - as the rectangle has no depth, there should always only appear ONE intersection
        //  - the Canvas is topmost intersection, if not, this handler would not have been called by spark

        var myIntersectionInfo =  window.scene.getPickedBodyInformation(theX, theY);
        if (myIntersectionInfo && myIntersectionInfo.intersections.length > 0) {
            transformMatrix = new Matrix4f(Public.innerSceneNode.globalmatrix);
            transformMatrix.invert();
            intersecPointOnCanvas = product(myIntersectionInfo.intersections[0].position,
                                            transformMatrix);
            //due to sweeping sphere intersection in spark.Window.. values outside of canvas can result in coordinates of -1, Public.width + 1  etc. 
            intersecPointOnCanvas.x = clamp(intersecPointOnCanvas.x, 0, Public.width-1);
            intersecPointOnCanvas.y = clamp(Public.height - intersecPointOnCanvas.y, 0, Public.height-1);
        }
        return intersecPointOnCanvas;
    };
    
    Base.realize = Public.realize;
    Public.realize = function () {
        var myWorldId, myCanvasId;
        var myMultiSampling   = Protected.getNumber("multisamples", 0);
        var myBackgroundColor = Protected.getVector4f("backgroundColor", new Vector4f(0,0,0,0));
        var myWidth       = Protected.getNumber("width", 100);
        var myHeight      = Protected.getNumber("height", 100);
        
        _myRenderArea = new OffscreenRenderArea();
        _myRenderArea.renderingCaps = Public.getDefaultRenderingCapabilites() | Renderer.FRAMEBUFFER_SUPPORT;
        _myRenderArea.multisamples = myMultiSampling;
        
        _myImage = Modelling.createImage(window.scene,
                                         myWidth, myHeight, "BGRA");
        _myImage.name = Public.name + "_canvasImage";
        var myTexture = Modelling.createTexture(window.scene, _myImage);
        myTexture.wrapmode = "clamp_to_edge";
        
        var myFlipMatrix = myTexture.matrix;
        myFlipMatrix.makeScaling(new Vector3f(1, -1, 1));
        myFlipMatrix.translate(new Vector3f(0, 1, 0));
        
        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myTexture);
        myMaterial.transparent = true;
        
        var myAutoCreateTag = Protected.getBoolean("autoCreate", true);
        var mySceneFile = Protected.getString("sceneFile", "");
        if (mySceneFile) {
            // Create (merge) world from scene file
            var myDom = spark.Canvas.prepareMerge(mySceneFile);
            // XXX assumption: only one world exist/ is handled
            myWorldId  = myDom.find(".//worlds/world").id;
            myCanvasId = myDom.find(".//canvases/canvas").id;
            

            spark.Canvas.mergeScenes(window.scene, myDom);
            
            _myCanvasNode = window.scene.dom.find(".//canvases/canvas[@id='" + myCanvasId + "']");
            _myViewport = _myCanvasNode.find(".//viewport");
            _myCamera    = window.scene.dom.getElementById(myWorldId).find(".//camera");
            
            _myWorld = window.scene.dom.getElementById(myWorldId);
            _myWorld.name = Public.name + "-world";
        } else {
            _myCanvasNode = Node.createElement("canvas");
            _myCanvasNode.name = Public.name + "-canvas";
            _myCanvasNode.backgroundcolor = myBackgroundColor;
            window.scene.canvases.appendChild(_myCanvasNode);
            
            _myViewport = Node.createElement("viewport");
            _myViewport.name = Public.name + "-viewport";
            _myCanvasNode.appendChild(_myViewport);
            
            if (myAutoCreateTag) {
                // Setup default world
                _myWorld = Node.createElement("world");
                _myWorld.name = Public.name + "-world";
                window.scene.worlds.appendChild(_myWorld);
                
                _myCamera = Node.createElement("camera");
                _myWorld.appendChild(_myCamera);
                spark.setupCameraOrtho(_myCamera, myWidth, myHeight);
                
                _myViewport.camera = _myCamera.id;
            }
        }
        
        if (_myWorld) {
            var myLightManager = new LightManager(window.scene, _myWorld);
            Public.setLightManager(myLightManager);
        }
        
        Public.setupWindow(_myRenderArea, false);
        
        _myCanvasNode.target = myTexture.id;
        _myRenderArea.setSceneAndCanvas(window.scene, _myCanvasNode);

        Public.setCanvas(_myCanvasNode);
        
        Base.realize(myMaterial);
        // set the canvas material to allow proper layering of transparencies
        // see http://home.comcast.net/~tom_forsyth/blog.wiki.html#[[Premultiplied%20alpha]]
        myMaterial.properties.blendfunction = "[one,one_minus_src_alpha,one,one_minus_src_alpha]";
        if (_myWorld) {
            while (Public.innerSceneNode.firstChild) {
                _myWorld.appendChild(Public.innerSceneNode.firstChild);
            }
        }
    };
    
    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
        Public.parent.stage.addEventListener(spark.StageEvent.FRAME, Public.onFrame);
        Public.stage.addEventListener(spark.KeyboardEvent.KEY_DOWN, onKey);
        Public.stage.addEventListener(spark.KeyboardEvent.KEY_UP, onKey);
        Public.addEventListener(spark.MouseEvent.MOVE, Public.onMouseMotion);
        Public.addEventListener(spark.MouseEvent.BUTTON_DOWN, Public.onMouseButtonDown);
        Public.addEventListener(spark.MouseEvent.BUTTON_UP, Public.onMouseButtonUp);
    };
    
    Base.setActiveCamera = Public.setActiveCamera;
    Public.setActiveCamera = function (theCamera, theViewport) {
        if (!Public.getLightManager()) {
            var myWorld = theCamera.parentNode;
            while (myWorld && myWorld.nodeName !== "world") {
                myWorld = myWorld.parentNode;
            }
            var myLightManager = new LightManager(window.scene, myWorld);
            Public.setLightManager(myLightManager);
            myLightManager.setupDefaultLighting(_myCanvasNode);
        }
        
        Base.setActiveCamera(theCamera, theViewport);
    };
    
    Base.onPreViewport = Public.onPreViewport;
    Public.onPreViewport = function (theViewport) {
        Base.onPreViewport(theViewport);
        if (_onPreViewportFunc) {
            Logger.warning("spark.Canvas.onPreViewportFunc is deprecated - use Bindings now!");
            _onPreViewportFunc(theViewport);
        }
        for (var handleId in _bindings[spark.Canvas.BINDING_SLOT.PRE]) {
            var myHandle = _bindings[spark.Canvas.BINDING_SLOT.PRE][handleId];
            myHandle.cb(theViewport);
        }
    };
    
    Base.onPostViewport = Public.onPostViewport;
    Public.onPostViewport = function (theViewport) {
        if (_onPostViewportFunc) {
            Logger.warning("spark.Canvas.onPostViewportFunc is deprecated - use Bindings now!");
            _onPostViewportFunc(theViewport);
        }
        for (var handleId in _bindings[spark.Canvas.BINDING_SLOT.POST]) {
            var myHandle = _bindings[spark.Canvas.BINDING_SLOT.POST][handleId];
            myHandle.cb(theViewport);
        }
        Base.onPostViewport(theViewport);
    };
    
    Public.pickBody = function (theX, theY, theFilterFunction) {
        var pickedBody     = null;
        var canvasPosition = Public.convertToCanvasCoordinates(theX, theY);
        if (canvasPosition) {
            if (!theFilterFunction) {
                if (_myPickRadius === 0) {
                    pickedBody = window.scene.pickBody(canvasPosition.x, canvasPosition.y, _myCanvasNode); 
                }  else {
                    pickedBody = window.scene.pickBodyBySweepingSphereFromBodies(
                                    canvasPosition.x, canvasPosition.y,
                                    _myPickRadius, _myCanvasNode);
                }
            } else {  //filtered picking
                var intersectionInformation = window.scene.getPickedBodiesInformation(canvasPosition.x, canvasPosition.y, _myCanvasNode);
                if (intersectionInformation) {
                    for (var i = 0, l = intersectionInformation.length; i < l; ++i) {
                        if (theFilterFunction(intersectionInformation[i])) {
                            return intersectionInformation[i].body;
                        }
                    }
                }
            }

        }
        return pickedBody;
    };
    
    Public.pickWidget = function (theX, theY) {
        var myBody = Public.pickBody(theX, theY);
        if (myBody) {
            var myBodyId = myBody.id;
            if (myBodyId in spark.sceneNodeMap) {
                var myWidget = spark.sceneNodeMap[myBodyId];
                return myWidget;
            }
        }
        return null;
    };
    
    Base.onFrame = Public.onFrame;
    Public.onFrame = function (theEvent) {
        Base.onFrame(theEvent.currenttime);
        if (_myRenderFlag) {
            _myRenderArea.renderToCanvas();
        }
    };
    
    Base.onMouseMotion = Public.onMouseMotion;
    Public.onMouseMotion = function (theEvent) {
        var canvasPosition = Public.convertToCanvasCoordinates(theEvent.stageX, theEvent.stageY);
        if (canvasPosition) {
            Base.onMouseMotion(canvasPosition.x, canvasPosition.y);
        }
    };
    
    Base.onMouseButton = Public.onMouseButton;
    Public.onMouseButton = function (theButton, theState, theX, theY) {
        var canvasPosition = Public.convertToCanvasCoordinates(theX, theY);
        if (canvasPosition) {
            Base.onMouseButton(theButton, theState, canvasPosition.x, canvasPosition.y);
           // var myWidget = Public.pickWidget(canvasPosition.x, canvasPosition.y);
           // print("___myPickedWidget: ", myWidget);
        }
        
    };
};

spark.Canvas.prepareMerge = function prepareMerge(theSceneFilePath) {
    theSceneFilePath = searchFile(theSceneFilePath);
    var myTargetDir, myTexSrcPath, myNewPath;
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
        throw new Error("spark file '" + theSceneFilePath + "' does not exist.");
    }
    
    var myDom = new Node();
    
    myDom.parseFile(theSceneFilePath);
    
    adjustNodeId(myDom.find(".//world"), true);
    adjustNodeId(myDom.find(".//canvases/canvas"), false);
    var myViewport = myDom.find(".//viewport");
    adjustNodeId(myViewport, false);
    // adjustNodeId(myDom.find(".//camera"), false); // XXX not necessary since already done in world 
    myViewport.camera = myDom.find(".//camera").id;
    
    // REWRITE TEXTURE SRC PATH
    var imageNode = myDom.find(".//images");
    for (var i = 0; i < imageNode.childNodesLength(); ++i) {
        myTargetDir = getDirectoryPart(theSceneFilePath);
        myTexSrcPath = imageNode.childNodes[i].src;
        if (myTexSrcPath.charAt(0) !== "/") {
            var myTexSrcPaths = myTexSrcPath.split("|");
            for (var j = 0; j < myTexSrcPaths.length; j++) {
                if (myTexSrcPaths[j].indexOf("./") === 0) {
                    myTexSrcPaths[j] = myTexSrcPaths[j].replace(/^\.\//, myTargetDir);
                } else {
                    myTexSrcPaths[j] = myTargetDir + myTexSrcPaths[j]; 
                }
                if (!fileExists(myTexSrcPaths[j])) {
                    Logger.error("Could not find texture within path '" + myTexSrcPaths[j] + "'");
                }
            }
            imageNode.childNodes[i].src =  myTexSrcPaths.join("|");
        }
    }
    return myDom;
};

spark.Canvas.mergeScenes = function (theTargetScene, theModelDom) {
    var childNode, receivingNode, childChildNode;
    var mySceneNode = theModelDom.firstChild;
    while (mySceneNode.childNodesLength()) {
        childNode = mySceneNode.firstChild;
        receivingNode = theTargetScene.dom.getNodesByTagName(childNode.nodeName)[0];
        while (childNode.childNodesLength()) {
            childChildNode = childNode.removeChild(childNode.firstChild);
            try {
                receivingNode.appendChild(childChildNode);
            } catch (err) {
                print("--> Node '" + childChildNode.nodeName + "' with id='" + childChildNode.id + "' already exists but is not needed. Don't worry."); 
            }
        }
        mySceneNode.removeChild(mySceneNode.firstChild);
    }
};
