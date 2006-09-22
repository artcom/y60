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

function OffscreenRenderer(theSize, theCamera, thePixelFormat, theImage) {
    const DUMP_IMAGE = false;

    this.overlays getter = function() {
        return _myCanvas.firstChild.childNode("overlays"); 
    }

    this.underlays getter = function() {
        return _myCanvas.firstChild.childNode("underlays"); 
    }
       
    function setup(theSize) {
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
            theImage = window.scene.createImage(theSize[0], theSize[1], thePixelFormat);
        }

        self.image = theImage;    
        self.image.name = "Offscreen Buffer";
        
        // Flip vertically since framebuffer content is upside-down
        var myMirrorMatrix = new Matrix4f;
        myMirrorMatrix.makeScaling(new Vector3f(1,-1,1));
        self.image.matrix.postMultiply(myMirrorMatrix);

        // Create canvas for offscreen render area
        _myCanvas = window.canvas.cloneNode(true);
        // clear overlays and underlays
        for(var i=0; i<_myCanvas.firstChild.childNodes.length; ++i) {
            while(_myCanvas.firstChild.childNode(i).childNodes.length) {
                _myCanvas.firstChild.childNode(i).removeChild(_myCanvas.firstChild.childNode(i).firstChild);
            }
        }

        _myCanvas.backgroundcolor = [1,1,1,1];
        adjustNodeIds(_myCanvas);
        _myCanvas.target = self.image.id;
        window.scene.canvases.appendChild(_myCanvas);
        _myCanvas.firstChild.camera = self.camera.id;

        // Setup offscreen render area
        _myOffscreenRenderArea = new OffscreenRenderArea();
        _myOffscreenRenderArea.scene = window.scene;
        _myOffscreenRenderArea.canvas = _myCanvas;

        _myOffscreenRenderArea.eventListener = _myOffscreenRenderArea;
    }

    this.appendOverlay = function(theNode) {
        _myCanvas.firstChild.childNode("overlays").appendChild(theNode);
    }

    this.appendUnderlay = function(theNode) {
        _myCanvas.firstChild.childNode("underlays").appendChild(theNode);
    }

    this.setImage = function(theImage) {
        self.image = theImage;
        _myCanvas.target = self.image.id;

        // Flip vertically since framebuffer content is upside-down
        var myMirrorMatrix = new Matrix4f;
        myMirrorMatrix.makeScaling(new Vector3f(1,-1,1));
        self.image.matrix.makeIdentity();
        self.image.matrix.postMultiply(myMirrorMatrix);
    }
    
    this.setBody = function(theNode) {
        _myOffscreenNodes.push(theNode);
        this.render();
    }

    this.addHiddenNode = function(theNode) {
        _myHiddenNodes.push(theNode);
    }

    this.setCamera = function(theCamera) {
        self.camera = theCamera;
        _myCanvas.firstChild.camera = self.camera.id;
    }

    this.render = function() {
        var myVisibleNodes = [];
        for (var i = 0; i < window.scene.world.childNodesLength(); ++i) {
            var myNode = window.scene.world.childNode(i);
            if (myNode.visible && !isOffscreenNode(myNode) && myNode.nodeName != "light") {
                myNode.visible = false;
                myVisibleNodes.push(myNode);
            }
        }
        offscreenVisible(true);
        _myOffscreenRenderArea.renderToCanvas(DUMP_IMAGE);
        offscreenVisible(false);

        for (var i = 0; i < myVisibleNodes.length; ++i) {
            myVisibleNodes[i].visible = true;
        }

        if (DUMP_IMAGE) {
            saveImage(this.image, "test.png");
        }
    }

    this.setBackgroundColor = function(theBackgroundColor) {
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

    var self = this;
    this.camera                = null;
    this.image                 = null;
    var _myOffscreenRenderArea = null;
    var _myHiddenNodes         = [];
    var _myOffscreenNodes      = [];
    var _myCanvas              = null;

    setup(theSize);
}
