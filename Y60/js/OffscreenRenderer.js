//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Y60JSSL.js");

function OffscreenRenderer(theSize, theCamera, thePixelFormat, theImage, theCanvas, theUseFBOFlag, theMultisamples) {
    var self = this;
    
    self.overlays getter = function() {
        if (!_myViewport.childNode("overlays")) {
            var myNode = new Node("<overlays/>");
            _myViewport.appendChild(myNode.firstChild);
        }
        return _myViewport.childNode("overlays"); 
    }

    self.underlays getter = function() {
        if (!_myViewport.childNode("underlays")) {
            var myNode = new Node("<underlays/>");
            _myViewport.appendChild(myUnderlayNode.firstChild);
        }
        return _myViewport.childNode("underlays"); 
    }

    self.renderarea getter = function() {
        return _myOffscreenRenderArea;
    }
    
    self.canvas getter = function() {
        return _myCanvas;
    }
 
    self.viewport getter = function() {
        return _myViewport;
    }

    self.camera setter = function(theCamera) {
        _myCamera = theCamera;
        _myViewport.camera = theCamera.id;
    }

    self.camera getter = function() {
        return _myCamera;
    }

    self.saveScreenshot = function(theFilename) {
        _myScreenshotName = theFilename;
    }

    self.onPreViewport = function(theViewport) {
    }

    self.setImage = function(theImage) {
        self.image = theImage;
        _myCanvas.target = self.image.id;

        // Flip vertically since framebuffer content is upside-down
        var myMirrorMatrix = new Matrix4f;
        myMirrorMatrix.makeScaling(new Vector3f(1,-1,1));
        self.image.matrix.makeIdentity();
        self.image.matrix.postMultiply(myMirrorMatrix);
    }

    self.setBody = function(theNode) {
        _myOffscreenNodes.push(theNode);
        self.render();
    }

    self.addHiddenNode = function(theNode) {
        _myHiddenNodes.push(theNode);
    }

    self.render = function(theReadbackFlag) {
        if (theReadbackFlag == undefined) {
            theReadbackFlag = false;
        }

        if (_myOffscreenNodes.length == 1 && _myOffscreenNodes[0].nodeName == "world") {
            var myWasVisible = _myOffscreenNodes[0].visible 
            _myOffscreenNodes[0].visible = true;
            _myOffscreenRenderArea.renderToCanvas(theReadbackFlag);
            _myOffscreenNodes[0].visible = myWasVisible;
        } else {  
            var myVisibleNodes = [];
            for (var i = 0; i < window.scene.world.childNodesLength(); ++i) {
                var myNode = window.scene.world.childNode(i);
                if (myNode.visible && !isOffscreenNode(myNode) && myNode.nodeName != "light") {
                    myNode.visible = false;
                    myVisibleNodes.push(myNode);
                }
            }

            offscreenVisible(true);
            _myOffscreenRenderArea.renderToCanvas(theReadbackFlag || _myScreenshotName);
            offscreenVisible(false);

            for (var i = 0; i < myVisibleNodes.length; ++i) {
                myVisibleNodes[i].visible = true;
            }
        }

        if (theReadbackFlag || _myScreenshotName) {
            var myFilename = (_myScreenshotName!=null)?_myScreenshotName:"dump_"+self.image.id+".png";
            
            saveImageFiltered(self.image, myFilename, ["flip"], [[]]);
            _myScreenshotName = null;
        }
    }

    // <diediedie reason="bloody convenience functions">

    self.setCamera = function(theCamera) {
        //Logger.error("OffscreenRenderer.setCamera is deprecated, use obj.camera setter");
        self.camera = theCamera;
    }

    self.setBackgroundColor = function(theBackgroundColor) {
        //Logger.error("OffscreenRenderer.setBackgroundColor is deprecated, use obj.canvas.backgroundcolor");
        _myCanvas.backgroundcolor = theBackgroundColor;
    }

    self.appendOverlay = function(theNode) {
        //Logger.error("OffscreenRenderer.appendOverlay is deprecated, use obj.overlays.appendChild");
        self.overlays.appendChild(theNode);
    }

    self.appendUnderlay = function(theNode) {
        //Logger.error("OffscreenRenderer.appendUnderlay is deprecated, use obj.underlays.appendChild");
        self.underlays.appendChild(theNode);
    }

    // </diediedie>

    function setup() {
        // Get target image for offscreen rendering
        if (theSize == undefined) {
            theSize = [window.width, window.height];
        }

        if (thePixelFormat == undefined) {
            thePixelFormat = "rgba";
        }

        if (theImage == undefined) {
            theImage = Modelling.createImage(window.scene, theSize[0], theSize[1], thePixelFormat);
        }
        self.image = theImage;    
        self.image.name = "OffscreenBuffer_Image";

        // Flip vertically since framebuffer content is upside-down
        var myMirrorMatrix = new Matrix4f;
        myMirrorMatrix.makeScaling(new Vector3f(1,-1,1));
        self.image.matrix.postMultiply(myMirrorMatrix);  

        // Setup canvas and viewport
        if (theCanvas == undefined) {
            Logger.info("Canvas undefined, copying 1st canvas/1st viewport");

            // clone first viewport of first canvas
            _myViewport = window.canvas.childNode("viewport", 0).cloneNode(false);
            adjustNodeIds(_myViewport);
            _myViewport.name = "OffscreenBuffer_Viewport";
            _myViewport.position = [0, 0];
            _myViewport.size     = [1, 1];

            // clone first canvas
            _myCanvas = window.canvas.cloneNode(false);
            adjustNodeIds(_myCanvas);
            _myCanvas.name = "OffscreenBuffer_Canvas";
            _myCanvas.appendChild(_myViewport);
            window.scene.canvases.appendChild(_myCanvas);
        } else {
            _myCanvas = theCanvas;
            _myViewport = theCanvas.childNode("viewport", 0);
        }

        // Setup camera
        if (theCamera == undefined) {
            Logger.info("Camera undefined, copying camera of viewport");

            var myCameraId;
            if (_myViewport.camera.length == 0) {
                Logger.warning("Viewport has no camera, cloning that of 1st canvas/1st viewport");
                myCameraId = window.canvas.childNode("viewport", 0).camera;
            } else {
                myCameraId = _myViewport.camera;
            }

            var myCamera = _myViewport.getElementById(myCameraId).cloneNode(true);
            adjustNodeIds(myCamera);
            myCamera.name = "OffscreenBuffer_Camera";

            window.scene.world.appendChild(myCamera);
            self.camera = myCamera;
        } else {
            self.camera = theCamera;
        }

        // Setup target
        _myCanvas.target = self.image.id;
        _myViewport.camera = self.camera.id;
        //_myViewport.wireframe = true;

        // Setup offscreen render area
        _myOffscreenRenderArea = new OffscreenRenderArea();
        if (theUseFBOFlag) {

            _myOffscreenRenderArea.renderingCaps = Renderer.FRAMEBUFFER_SUPPORT;
            if (isFinite(theMultisamples)) {
                _myOffscreenRenderArea.multisamples = theMultisamples;
            } else {
                _myOffscreenRenderArea.multisamples = 0;
            }
        }

        _myOffscreenRenderArea.scene = window.scene;
        _myOffscreenRenderArea.canvas = _myCanvas;
        _myOffscreenRenderArea.eventListener = self;
    }
  
    function isOffscreenNode(theNode) {
        for (var i=0; i<_myOffscreenNodes.length; ++i) {
            if (theNode.id == _myOffscreenNodes[i].id) {
                return true;
            }
        }
        return false;
    }

    function offscreenVisible(theFlag) {
        for (var i=0; i<_myOffscreenNodes.length; ++i) {
            _myOffscreenNodes[i].visible = theFlag;
        }
    }

    self.image                 = null;
    var _myOffscreenRenderArea = null;
    var _myHiddenNodes         = [];
    var _myOffscreenNodes      = [];

    var _myCanvas         = null;
    var _myViewport       = null;
    var _myCamera         = null;
    var _myScreenshotName = null;

    setup(theSize, theCanvas);
}
