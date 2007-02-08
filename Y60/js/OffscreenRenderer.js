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
        return _myCanvas.firstChild.childNode("overlays"); 
    }

    self.underlays getter = function() {
        if (!_myCanvas.firstChild.childNode("underlays")) {
            var myUnderlayNode = new Node("<underlays/>");
            _myCanvas.firstChild.appendChild(myUnderlayNode.firstChild);
        }
        return _myCanvas.firstChild.childNode("underlays"); 
    }

    self.renderarea getter = function() {
        return _myOffscreenRenderArea;
    }
    
    self.canvas getter = function() {
        return _myCanvas;
    }
        
    function setup(theSize, theCanvas) {
        //add our own camera
        if (theCamera == undefined) {
            self.camera = window.camera.cloneNode(true);
            self.camera.name = "OffscreenBuffer_Camera";
            adjustNodeIds(self.camera);
            self.camera = window.scene.world.appendChild(self.camera);
        } else {
            self.camera = theCamera;
        }

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
        
        // Create canvas for offscreen render area
        if (theCanvas) {
            _myCanvas = theCanvas;
        } else {
            _myCanvas = window.canvas.cloneNode(true);
            // clear overlays and underlays
            for(var i=0; i<_myCanvas.firstChild.childNodes.length; ++i) {
                while(_myCanvas.firstChild.childNode(i).childNodes.length) {
                    _myCanvas.firstChild.childNode(i).removeChild(_myCanvas.firstChild.childNode(i).firstChild);
                }
            }

            _myCanvas.backgroundcolor = [1,1,1,1];
            adjustNodeIds(_myCanvas);
            window.scene.canvases.appendChild(_myCanvas);
            _myCanvas.firstChild.camera = self.camera.id;
        }
        _myCanvas.name = "OffscreenBuffer_Canvas";
        _myCanvas.target = self.image.id;

        // Setup offscreen render area
        _myOffscreenRenderArea = new OffscreenRenderArea();
        if (theUseFBOFlag) {
            
            _myOffscreenRenderArea.renderingCaps = Renderer.FRAMEBUFFER_SUPPORT;
            if (theMultisamples) {
                _myOffscreenRenderArea.multisamples = theMultisamples;
            } else {
                _myOffscreenRenderArea.multisamples = 0;
            }
        }

        _myOffscreenRenderArea.scene = window.scene;
        _myOffscreenRenderArea.canvas = _myCanvas;
        _myOffscreenRenderArea.eventListener = self;
    }
   
    self.onPreViewport = function(theViewport) {
    }

    self.appendOverlay = function(theNode) {
        _myCanvas.firstChild.childNode("overlays").appendChild(theNode);
    }

    self.appendUnderlay = function(theNode) {
        if( !_myCanvas.firstChild.childNode("underlays") ) {
            var myUnderlayNode = new Node("<underlays/>");
            _myCanvas.firstChild.appendChild(myUnderlayNode.firstChild);
        }
        _myCanvas.firstChild.childNode("underlays").appendChild(theNode);
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

    self.setCamera = function(theCamera) {
        self.camera = theCamera;
        _myCanvas.firstChild.camera = self.camera.id;
    }

    self.render = function(theReadbackFlag) {
        if (theReadbackFlag == undefined) {
            theReadbackFlag = false;
        }

        var myVisibleNodes = [];
        var myIsWorld = false;
        if (_myOffscreenNodes.length == 1 && _myOffscreenNodes[0].nodeName == "world") {
            var myWasVisible = _myOffscreenNodes[0].visible 
            _myOffscreenNodes[0].visible = true;
            _myOffscreenRenderArea.renderToCanvas(theReadbackFlag);
            _myOffscreenNodes[0].visible = myWasVisible;
        } else {  
            for (var i = 0; i < window.scene.world.childNodesLength(); ++i) {
                var myNode = window.scene.world.childNode(i);
                if (myNode.visible && !isOffscreenNode(myNode) && myNode.nodeName != "light") {
                    myNode.visible = false;
                    myVisibleNodes.push(myNode);
                }
            }

            offscreenVisible(true);
            _myOffscreenRenderArea.renderToCanvas(theReadbackFlag);
            offscreenVisible(false);

            for (var i = 0; i < myVisibleNodes.length; ++i) {
                myVisibleNodes[i].visible = true;
            }
        }
        if (theReadbackFlag) {
            saveImage(self.image, "dump_"+self.image.id+".png");
        }
    }

    self.setBackgroundColor = function(theBackgroundColor) {
        _myCanvas.backgroundcolor = theBackgroundColor;
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

    self.camera                = null;
    self.image                 = null;
    var _myOffscreenRenderArea = null;
    var _myHiddenNodes         = [];
    var _myOffscreenNodes      = [];
    var _myCanvas              = null;
    setup(theSize, theCanvas);
    

}
