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
*/

use("Y60JSSL.js");
use("Overlay.js");

/*

How does our Glow it work?

To avoid rendering the whole scene multiple times, our "glow" feature uses the
alpha channel to store information about what pixels are supposed to glow, and how bright.

When glow is switched on, the scene is rendered into an offscreen buffer first,
accumulating glow information in the alpha channel.

This offscreen buffer is then used as texture for an overlay that uses a special
shader to extract and filter the glow color information from the scene.
This shader however does only horizontal filtering, so a third pass is performed
to to the vertical filtering.

The main scene rendered consists just of two overlays,
the scene underlay, and the glow texture is then rendered as overlay.

*/

function Glow(theViewer, theKernelSize, theGlowScale) {
    this.Constructor(this, theViewer, theKernelSize, theGlowScale);
}

Glow.prototype.Constructor = function(obj, theViewer, theKernelSize, theGlowScale) {

    var _myGlowEnabled = true;

    var _myOffscreenOverlay = null;
    var _myBlurXOverlay = null;
    var _myBlurYOverlay = null;
    var _myGlowOverlay = null;
    var _myDebugOverlay = null;
    var _myDebugImage = null;

    var _myViewportVersion = null;
    var _myOffscreenRenderArea = null;
    var _myBlurXRenderArea = null;
    var _myBlurYRenderArea = null;

    var _myBufferBits = null;
    const OFFSCREEN_RESOLUTION_RATIO = 4;

    obj.getEnabled = function() {
        return _myGlowEnabled;
    }
    obj.setEnabled = function(theFlag) {
        _myGlowEnabled = theFlag;
        if (!_myGlowEnabled) {
            _myGlowOverlay.visible = false;
            _myOffscreenOverlay.visible = false;
        }
    }

    obj.onResize = function(theWidth, theHeight) {
    }

    obj.onRender = function() {

        window.scene.world.visible = true;
        _myOffscreenOverlay.visible = false;
        _myBlurXOverlay.visible = false;
        _myBlurYOverlay.visible = false;
        _myGlowOverlay.visible = false;

        if (_myGlowEnabled) {
            // copy background color with alpha=0 so the BG doesn't glow but the color is right
            _myOffscreenRenderArea.canvas.backgroundcolor = window.canvas.backgroundcolor;
            _myOffscreenRenderArea.canvas.backgroundcolor[3] = 0.0;

            // copy attributes from 'window'
            const COPY_ATTRIBS = [ "backfaceculling", "wireframe", "lighting", "texturing",
                                   "flatshading", "culling", "debugculling", "drawnormals" ];
            var myViewport = _myOffscreenRenderArea.canvas.firstChild;
            var myWindowViewport = theViewer.getActiveViewport(); //window.canvas.firstChild;
            if (_myViewportVersion != myWindowViewport.nodeVersion) {
                for (var i in COPY_ATTRIBS) {
                    var myAttrib = COPY_ATTRIBS[i];
                    myViewport[myAttrib] = myWindowViewport[myAttrib];
                }
                _myViewportVersion = myWindowViewport.nodeVersion;
            }

            // render scene
            _myOffscreenRenderArea.activate();
            _myOffscreenRenderArea.clearBuffers( _myBufferBits );
            //_myOffscreenRenderArea.preRender();
            _myOffscreenRenderArea.render();
            //_myOffscreenRenderArea.postRender();
            _myOffscreenRenderArea.deactivate();
            window.scene.world.visible = false;

            if (_myDebugImage) {
                //_myOffscreenRenderArea.downloadFromViewport(_myDebugImage); // XXX must be given a texture node
                saveImage(_myDebugImage, "debug.png");
            }

            // render blur_x
            _myBlurXOverlay.visible = true;
            _myBlurXRenderArea.activate();
            _myBlurXRenderArea.clearBuffers( _myBufferBits );
            _myBlurXRenderArea.render();
            _myBlurXRenderArea.deactivate();
            _myBlurXOverlay.visible = false;

            // render blur_y
            _myBlurYOverlay.visible = true;
            _myBlurYRenderArea.activate();
            _myBlurYRenderArea.clearBuffers( _myBufferBits );
            _myBlurYRenderArea.render();
            _myBlurYRenderArea.deactivate();
            _myBlurYOverlay.visible = false;

            // compositing
            if (_myDebugOverlay) {
                _myDebugOverlay.visible = true;
            } else {
                _myOffscreenOverlay.visible = true;
                _myGlowOverlay.visible = true;
            }
        }

        window.clearBuffers( _myBufferBits );
        window.preRender();
        window.render();
        window.postRender();
        
        window.scene.world.visible = true;

    }

    //////////////////////////////////////////////////////////////////////
    //
    // Private
    //
    //////////////////////////////////////////////////////////////////////

    function cloneCanvas(theCanvas, theName) {

        var myCanvas = theCanvas.cloneNode();
        myCanvas.id = createUniqueId();
        myCanvas.name = theName;

        var myViewport = theCanvas.find("viewport");
        myViewport = myViewport.cloneNode();
        myViewport.id = createUniqueId();
        myViewport.name = theName;
        myViewport.appendChild(new Node("<overlays/>").firstChild);

        myCanvas.appendChild(myViewport);
        window.scene.canvases.appendChild(myCanvas);
        window.scene.update(Scene.ALL);

        return myCanvas;
    }

    function gaussian( x, s) {
        return Math.exp(-x*x/(2*s*s)) / (s*Math.sqrt(2*Math.PI));
    }

    // generate array of weights for Gaussian blur
    function generateGaussianWeights( s )
    {
        var myWidth = Math.floor(3.0*s)-1;
        var size = myWidth*2+1;
        var myWeights = [];

        var sum = 0.0;
        for(var x=0; x<size; x++) {
            myWeights.push(gaussian( x-myWidth, s));
            sum += myWeights[x];
        }

        //print("sum = " +  sum);
        //printf("gaussian weights, s = %f, n = %d\n", s, n);
        for(var y=0; y<size; y++) {
            myWeights[y] /= sum;
            //print("weights: ", myWeights[y]);
        }
        return myWeights;
    }

    function generateBlurKernel( theLength ) {

        var myKernel = [];
        for( var i=0; i<theLength; ++i) {
            var myValue = 1 / (theLength);
            myKernel.push(myValue);
        }

        return myKernel;
    }

    function setupGlow() {

        var myMipmapFlag = false;

        var myOffscreenSize = new Vector2i(window.width, window.height);
        var myWidth = myOffscreenSize[0] / OFFSCREEN_RESOLUTION_RATIO; //nextPowerOfTwo(myOffscreenSize[0]) / OFFSCREEN_RESOLUTION_RATIO;
        var myHeight = myOffscreenSize[1] / OFFSCREEN_RESOLUTION_RATIO; //nextPowerOfTwo(myOffscreenSize[1]) / OFFSCREEN_RESOLUTION_RATIO;

        var myMirrorMatrix = new Matrix4f;
        myMirrorMatrix.scale(new Vector3f(1,-1,1));
        myMirrorMatrix.translate(new Vector3f(0.0,1.0,0.0));

        // make sure glow is off for main canvas
        var myViewport = window.canvas.find("viewport");
        if (myViewport.glow == true) {
            Logger.warning("Disabling glow on viewport '" + myViewport.name + "' id=" + myViewport.id);
            myViewport.glow = false;
        }

        /*
         * blurKernelImage
         */
        var myBlurKernel = generateGaussianWeights( theKernelSize );
        theKernelSize = myBlurKernel.length;

        // _myDebugImage = Modelling.createImage(window.scene, nextPowerOfTwo(window.width), nextPowerOfTwo(window.height),"RGBA");

        var myBlurKernelImage = Modelling.createImage(window.scene, nextPowerOfTwo(theKernelSize),1,"RGBA");
        myBlurKernelImage.name = "BlurKernel";
        window.scene.update(Scene.MATERIALS);

        var myRaster = myBlurKernelImage.firstChild.firstChild.nodeValue;
        for (var i = 0; i < theKernelSize; ++i) {
            myRaster.setPixel(i,0, [myBlurKernel[i],
                                    myBlurKernel[i],
                                    myBlurKernel[i],
                                    myBlurKernel[i]]);
        }

        var myBlurKernelTexture = Modelling.createTexture(window.scene, myBlurKernelImage);

        /*
         * Offscreen
         * renders scene into myOffscreenImage
         */
        var myOffscreenImage = Modelling.createImage(window.scene, myOffscreenSize[0], myOffscreenSize[1], "RGBA");
        myOffscreenImage.name = "Offscreen";
        myOffscreenImage.matrix.postMultiply(myMirrorMatrix);

        var myOffscreenTexture = Modelling.createTexture(window.scene, myOffscreenImage);
        myOffscreenTexture.wrapmode = "clamp";

        var myOffscreenCanvas = cloneCanvas(window.canvas, "Offscreen");
        myOffscreenCanvas.backgroundcolor = [0,0,0,1];
        myOffscreenCanvas.targets = [myOffscreenTexture.id];

        var myOffscreenViewport = myOffscreenCanvas.find("viewport");
        myOffscreenViewport.glow = 1;

        var myHeadlight = window.camera.find("light");
        theViewer.getLightManager().registerHeadlightWithViewport(myOffscreenViewport, myHeadlight);

        _myOffscreenRenderArea = new OffscreenRenderArea();
        _myOffscreenRenderArea.setSceneAndCanvas( window.scene, myOffscreenCanvas);
        _myOffscreenRenderArea.eventListener = theViewer;
        _myBufferBits = _myOffscreenRenderArea.constructor.GL_COLOR_BUFFER_BIT | _myOffscreenRenderArea.constructor.GL_DEPTH_BUFFER_BIT;

        _myOffscreenOverlay = new ImageOverlay(window.scene, myOffscreenImage, null, myOffscreenCanvas.find(".//overlays"));
        _myOffscreenOverlay.name = "Offscreen";
        /*_myOffscreenOverlay.srcsize = new Vector2f(myOffscreenImage.width / nextPowerOfTwo(myOffscreenImage.width),
                                                   myOffscreenImage.height / nextPowerOfTwo(myOffscreenImage.height));
        _myOffscreenOverlay.srcorigin = new Vector2f( 0, 1 - myOffscreenImage.height / nextPowerOfTwo(myOffscreenImage.height));*/
        _myOffscreenOverlay.material.name = "Offscreen";
        _myOffscreenOverlay.material.properties.blendfunction = "[one,zero]";
        //_myOffscreenOverlay.material.properties.blendfunction = "[src_color,zero]";
        //_myOffscreenOverlay.material.properties.blendequation = "add";
        //_myOffscreenOverlay.material.transparent = 0;
        //_myOffscreenOverlay.material.childNode("textureunits").firstChild.applymode = "replace";

        /*
         * Blur_X
         * renders myOffscreenImage into myBlurXImage, with X blur
         * shader enabled
         */
        var myBlurXImage = Modelling.createImage(window.scene, myWidth, myHeight, "RGB");
        myBlurXImage.name = "BlurX";
        myBlurXImage.matrix.postMultiply(myMirrorMatrix);

        var myBlurXTexture = Modelling.createTexture(window.scene, myBlurXImage);
        myBlurXTexture.mipmap = myMipmapFlag;
        myBlurXTexture.wrapmode = "clamp";

        var myBlurXCanvas = cloneCanvas(window.canvas, "BlurX");
        myBlurXCanvas.backgroundcolor = [0,0,0,1];
        myBlurXCanvas.targets = [myBlurXTexture.id];

        var myBlurXViewport = myBlurXCanvas.find("viewport");
        myBlurXViewport.glow = 1;

        _myBlurXRenderArea = new OffscreenRenderArea();
        _myBlurXRenderArea.renderingCaps = Renderer.FRAMEBUFFER_SUPPORT;
        _myBlurXRenderArea.setSceneAndCanvas( window.scene, myBlurXCanvas);

        _myBlurXOverlay = new ImageOverlay(window.scene, myOffscreenImage, null, myBlurXCanvas.find(".//overlays"));
        _myBlurXOverlay.name = "BlurX";
        _myBlurXOverlay.width = myWidth;
        _myBlurXOverlay.height = myHeight;

        var myBlurXMaterial = _myBlurXOverlay.material;
        myBlurXMaterial.enabled = false;
        myBlurXMaterial.name = "BlurX";
        myBlurXMaterial.requires.textures = "[10[glow,glow]]";

        var myTextureUnit = Node.createElement("textureunit");
        myTextureUnit.texture = myBlurKernelTexture.id;
        var myBlurXTextureUnits  = myBlurXMaterial.find("textureunits");
        myBlurXTextureUnits.appendChild(myTextureUnit);
        myBlurXMaterial.enabled = true;
        myBlurXMaterial.properties.texelSize = new Vector3f( 1/myBlurXImage.width, 1/myBlurXImage.height, 0.0);
        myBlurXMaterial.properties.glowScale = theGlowScale;
        myBlurXMaterial.properties.kernelSize = theKernelSize;
        myBlurXMaterial.properties.blurKernelTexSize = myBlurKernelImage.width;

        /*
         * Blur_Y
         * renders myBlurXImage into myBlurYImage, with Y blur shader enabled
         */
        var myBlurYImage = Modelling.createImage(window.scene, myWidth, myHeight, "RGB");
        myBlurYImage.name = "BlurY";
        myBlurYImage.matrix.postMultiply(myMirrorMatrix);

        var myBlurYTexture = Modelling.createTexture(window.scene, myBlurYImage);
        myBlurYTexture.wrapmode = "clamp";
        myBlurYTexture.mipmap = myMipmapFlag;

        var myBlurYCanvas =  cloneCanvas(window.canvas, "BlurY");
        myBlurYCanvas.backgroundcolor = [0,0,0,1];
        myBlurYCanvas.targets = [myBlurYTexture.id];

        var myBlurYViewport = myBlurYCanvas.find("viewport");
        myBlurYViewport.glow = 1;

        _myBlurYRenderArea = new OffscreenRenderArea();
        _myBlurYRenderArea.renderingCaps = Renderer.FRAMEBUFFER_SUPPORT;
        _myBlurYRenderArea.setSceneAndCanvas( window.scene, myBlurYCanvas);

        _myBlurYOverlay = new ImageOverlay(window.scene, myBlurXImage, null, myBlurYCanvas.find(".//overlays"));
        _myBlurYOverlay.name = "BlurY";

        var myBlurYMaterial = _myBlurYOverlay.material;
        myBlurYMaterial.enabled = false;
        myBlurYMaterial.name = "BlurY";
        myBlurYMaterial.requires.textures = "[10[glow,glow]]";

        var myTextureUnit = Node.createElement("textureunit");
        myTextureUnit.texture = myBlurKernelTexture.id;
        var myBlurYTextureUnits  = myBlurYMaterial.find("textureunits");
        myBlurYTextureUnits.appendChild(myTextureUnit);

        myBlurYMaterial.enabled = true;
        myBlurYMaterial.properties.texelSize = new Vector3f( 1/myBlurYImage.width, 1/myBlurYImage.height,1.0);
        myBlurYMaterial.properties.glowScale = theGlowScale;
        myBlurYMaterial.properties.kernelSize = theKernelSize;
        myBlurYMaterial.properties.blurKernelTexSize = myBlurKernelImage.width;
        myBlurYMaterial.properties.blendfunction = "[one,one]";

        /*
         * Glow
         * renders myBlurYImage onto screen
         */
        _myGlowOverlay = new ImageOverlay(window.scene, myBlurYImage, null, myBlurYCanvas.find(".//overlays"));
        _myGlowOverlay.name = "Glow";
        _myGlowOverlay.width = _myOffscreenOverlay.width;
        _myGlowOverlay.height = _myOffscreenOverlay.height;
        _myGlowOverlay.material.properties.blendfunction = "[one,one]";
        /*_myGlowOverlay.srcsize = new Vector2f(myOffscreenImage.width / nextPowerOfTwo(myOffscreenImage.width),
                                                   myOffscreenImage.height / nextPowerOfTwo(myOffscreenImage.height));
        _myGlowOverlay.srcorigin = new Vector2f( 0, 1 - myOffscreenImage.height / nextPowerOfTwo(myOffscreenImage.height));*/

        // prepare compositing
        var myUnderlays = myViewport.find("underlays");
        if (!myUnderlays) {
            myUnderlays = new Node("<underlays/>").firstChild;
            myViewport.appendChild(myUnderlays);
        }
        myUnderlays.appendChild(_myOffscreenOverlay.node);

        var myDisplayOverlay = _myGlowOverlay.node;
        _myDebugOverlay = null;//_myOffscreenOverlay.node; // _myBlurXOverlay.node;
        //_myDebugOverlay = _myBlurYOverlay.node;
        myUnderlays.appendChild(_myDebugOverlay ? _myDebugOverlay : _myGlowOverlay.node);
    }

    setupGlow();
}
