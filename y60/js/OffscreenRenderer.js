/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

/*jslint nomen:false plusplus:false*/
/*globals use, Node, window, Modelling, Vector3f, Logger, Matrix4f,
          adjustNodeIds, OffscreenRenderArea, Renderer,
          saveImageFiltered*/

use("Y60JSSL.js");

function OffscreenRenderer(theSize, theCamera, thePixelFormat, theImage,
                           theCanvas, theUseFBOFlag, theMultisamples) {
    var self = this;

    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myOffscreenRenderArea = null;
    var _myOffscreenNodes      = [];

    var _myCanvas         = null;
    var _myViewport       = null;
    var _myCamera         = null;
    var _myScreenshotName = null;
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function setup() {
        // Get target image for offscreen rendering
        theSize = theSize || [window.width, window.height];
        thePixelFormat = thePixelFormat || "rgba";
        if (!theImage) {
            theImage = Modelling.createImage(window.scene, theSize[0], theSize[1], thePixelFormat);
            theImage.resize = "none"; // use non-power of two textures
        }
        self.image = theImage;
        self.image.name = "OffscreenBuffer_Image";

        self.texture = Modelling.createTexture(window.scene, self.image);
        self.texture.name = "OffscreenBuffer_Texture";

        // Flip vertically since framebuffer content is upside-down
        var myMirrorMatrix = new Matrix4f();
        myMirrorMatrix.makeScaling(new Vector3f(1, -1, 1));
        self.image.matrix.postMultiply(myMirrorMatrix);

        // Setup canvas and viewport
        if (!theCanvas) {
            Logger.debug("Canvas undefined, copying 1st canvas/1st viewport");

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
        if (!theCamera) {
            Logger.info("Camera undefined, copying camera of viewport");

            var myCameraId;
            if (_myViewport.camera.length === 0) {
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
        _myCanvas.target = self.texture.id;
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
        for (var i = 0; i < _myOffscreenNodes.length; ++i) {
            if (theNode.id === _myOffscreenNodes[i].id) {
                return true;
            }
        }
        return false;
    }

    function offscreenVisible(theFlag) {
        for (var i = 0; i < _myOffscreenNodes.length; ++i) {
            _myOffscreenNodes[i].visible = theFlag;
        }
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////

    self.__defineGetter__("overlays", function () {
        if (!_myViewport.childNode("overlays")) {
            var myNode = new Node("<overlays/>");
            _myViewport.appendChild(myNode.firstChild);
        }
        return _myViewport.childNode("overlays");
    });

    self.__defineGetter__("underlays", function () {
        if (!_myViewport.childNode("underlays")) {
            var myNode = new Node("<underlays/>");
            _myViewport.appendChild(myNode.firstChild);
        }
        return _myViewport.childNode("underlays");
    });

    self.__defineGetter__("renderarea", function () {
        return _myOffscreenRenderArea;
    });

    self.__defineGetter__("canvas", function () {
        return _myCanvas;
    });

    self.__defineGetter__("viewport", function () {
        return _myViewport;
    });

    self.__defineSetter__("camera", function (theCamera) {
        _myCamera = theCamera;
        _myViewport.camera = theCamera.id;
    });

    self.__defineGetter__("camera", function () {
        return _myCamera;
    });

    self.saveScreenshot = function (theFilename) {
        _myScreenshotName = theFilename;
    };

    self.onPreViewport = function (theViewport) {
    };

    self.resize = function (theNewSize) {
        if (self.texture.width  === 0 ||
            self.texture.width  !== theNewSize.x ||
            self.texture.height !== theNewSize.y) 
        {
            window.scene.textures.removeChild(self.texture);
            self.texture = Modelling.createTexture(window.scene, self.image);
            self.texture.name = "newTexture";
            _myCanvas.target = self.texture.id;
        }
    };

    self.setImage = function (theImage) {
        if (!theImage) {
            _myCanvas.target = "";
            return;
        }

        self.image = theImage;
        self.texture.image = self.image.id;

        // Flip vertically since framebuffer content is upside-down
        var myMirrorMatrix = new Matrix4f();
        myMirrorMatrix.makeScaling(new Vector3f(1, -1, 1));
        self.image.matrix.makeIdentity();
        self.image.matrix.postMultiply(myMirrorMatrix);
    };

    self.setBody = function (theNode) {
        theNode.sticky = true;
        _myOffscreenNodes.push(theNode);
        self.render();
    };

    self.render = function (theReadbackFlag, theCubemapFace) {
        var i;
        theReadbackFlag = !!theReadbackFlag || false;
        theCubemapFace = theCubemapFace || 0;

        if (_myOffscreenNodes.length === 1 &&
            _myOffscreenNodes[0].nodeName === "world")
        {
            var myWasVisible = _myOffscreenNodes[0].visible;
            _myOffscreenNodes[0].visible = true;
            _myOffscreenRenderArea.renderToCanvas(theReadbackFlag);
            _myOffscreenNodes[0].visible = myWasVisible;
        } else {
            var myVisibleNodes = [];
            for (i = 0; i < window.scene.world.childNodesLength(); ++i) {
                var myNode = window.scene.world.childNode(i);
                if (myNode.visible && !isOffscreenNode(myNode) &&
                    myNode.nodeName !== "light") {
                    myNode.visible = false;
                    myVisibleNodes.push(myNode);
                }
            }

            offscreenVisible(true);
            _myOffscreenRenderArea.renderToCanvas(theReadbackFlag || _myScreenshotName, theCubemapFace);
            offscreenVisible(false);

            for (i = 0; i < myVisibleNodes.length; ++i) {
                myVisibleNodes[i].visible = true;
            }
        }

        // doing a readback does *not* mean saving it to disk...
        //if (theReadbackFlag || _myScreenshotName) {
        if (_myScreenshotName) {
            var myFilename = (_myScreenshotName !== null) ? _myScreenshotName : "dump_" + self.image.id + "face" + theCubemapFace + ".png";

            saveImageFiltered(self.image, myFilename, ["flip"], [[]]);
            _myScreenshotName = null;
        }
    };

    // <diediedie reason="bloody convenience functions">

    self.setCamera = function (theCamera) {
        //Logger.error("OffscreenRenderer.setCamera is deprecated, use obj.camera setter");
        self.camera = theCamera;
    };

    self.setBackgroundColor = function (theBackgroundColor) {
        //Logger.error("OffscreenRenderer.setBackgroundColor is deprecated, use obj.canvas.backgroundcolor");
        _myCanvas.backgroundcolor = theBackgroundColor;
    };

    self.appendOverlay = function (theNode) {
        //Logger.error("OffscreenRenderer.appendOverlay is deprecated,
        //use obj.overlays.appendChild");
        //print("overlay " + self.overlays + " " + theNode);
        self.overlays.appendChild(theNode);
    };

    self.appendUnderlay = function (theNode) {
        //Logger.error("OffscreenRenderer.appendUnderlay is deprecated, use obj.underlays.appendChild");
        self.underlays.appendChild(theNode);
    };

    // </diediedie>

    self.image                 = null;
    self.texture               = null;

    setup();
}
