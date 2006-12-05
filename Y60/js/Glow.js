//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Y60JSSL.js");
use("Overlay.js");

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
            // copy viewport attributes from main viewport
            _myOffscreenRenderArea.canvas.firstChild.lighting = window.lighting;
            _myOffscreenRenderArea.canvas.firstChild.texturing = window.texturing;

            // render scene
            _myOffscreenRenderArea.activate();
            _myOffscreenRenderArea.clearBuffers( _myOffscreenRenderArea.constructor.GL_COLOR_BUFFER_BIT | _myOffscreenRenderArea.constructor.GL_DEPTH_BUFFER_BIT );
            _myOffscreenRenderArea.preRender();
            _myOffscreenRenderArea.render();
            _myOffscreenRenderArea.postRender();
            _myOffscreenRenderArea.deactivate();
            window.scene.world.visible = false;

            if (_myDebugImage) {
                _myOffscreenRenderArea.downloadFromViewport(_myDebugImage);
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
        window.swapBuffers();

        window.scene.world.visible = true;
    }

    //////////////////////////////////////////////////////////////////////
    //
    // fuConstructor
    //
    //////////////////////////////////////////////////////////////////////

    function cloneCanvas(theCanvas, theName) {

        var myCanvas = theCanvas.cloneNode();
        myCanvas.id = createUniqueId();
        myCanvas.name = theName;

        var myViewport = getDescendantByTagName(theCanvas, "viewport");
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
        var myWidth = nextPowerOfTwo(myOffscreenSize[0]) / OFFSCREEN_RESOLUTION_RATIO;
        var myHeight = nextPowerOfTwo(myOffscreenSize[1]) / OFFSCREEN_RESOLUTION_RATIO;

        var myMirrorMatrix = new Matrix4f;
        myMirrorMatrix.scale(new Vector3f(1,-1,1));
        myMirrorMatrix.translate( new Vector3f(0.0,1.0,0.0));

        // make sure glow is off for main canvas
        var myViewport = getDescendantByTagName(window.canvas, "viewport");
        if (myViewport.glow == true) {
            Logger.warning("Disabling glow on viewport '" + myViewport.name + "' id=" + myViewport.id);
            //myViewport.glow = false;
        }

        // blurKernelImage
        var myBlurKernel = generateGaussianWeights( theKernelSize );
        theKernelSize = myBlurKernel.length;

        // _myDebugImage = Modelling.createImage(window.scene, nextPowerOfTwo(window.width), nextPowerOfTwo(window.height),"RGBA");

        var myBlurKernelImage = Modelling.createImage(window.scene, nextPowerOfTwo(theKernelSize),1,"RGBA");
        myBlurKernelImage.id = createUniqueId();
        myBlurKernelImage.name = "BlurKernel";
        myBlurKernelImage.resize = 'scale';
        window.scene.images.appendChild(myBlurKernelImage);
        window.scene.update(Scene.MATERIALS);

        var myRaster = myBlurKernelImage.firstChild.firstChild.nodeValue;
        for (var i = 0; i < theKernelSize; ++i) {
            myRaster.setPixel(i,0, [myBlurKernel[i],
                                    myBlurKernel[i],
                                    myBlurKernel[i],
                                    myBlurKernel[i]]);
            //print( i, myRaster.getPixel(i,0));
        }
        //print(myBlurKernelImage.width);

        /*
         * Offscreen
         * renders scene into myOffscreenImage
         */
        var myOffscreenImage = Modelling.createImage(window.scene, myOffscreenSize[0], myOffscreenSize[1], "RGBA");
        myOffscreenImage.id = createUniqueId();
        myOffscreenImage.name = "Offscreen";
        myOffscreenImage.resize = "pad";
        myOffscreenImage.matrix.postMultiply(myMirrorMatrix);
        window.scene.images.appendChild(myOffscreenImage);
        myOffscreenImage.wrapmode = "clamp";

        var myOffscreenCanvas = cloneCanvas(window.canvas, "Offscreen");
        myOffscreenCanvas.backgroundcolor = [0,0,0,1];
        myOffscreenCanvas.target = myOffscreenImage.id;

        var myOffscreenViewport = getDescendantByTagName(myOffscreenCanvas, "viewport");
        myOffscreenViewport.glow = 1;

        var myHeadlight = getDescendantByTagName(window.camera, "light");
        theViewer.getLightManager().registerHeadlightWithViewport(myOffscreenViewport, myHeadlight);

        _myOffscreenRenderArea = new OffscreenRenderArea();
        _myOffscreenRenderArea.setSceneAndCanvas( window.scene, myOffscreenCanvas);
        _myOffscreenRenderArea.eventListener = theViewer;
        _myBufferBits = _myOffscreenRenderArea.constructor.GL_COLOR_BUFFER_BIT | _myOffscreenRenderArea.constructor.GL_DEPTH_BUFFER_BIT;

        _myOffscreenOverlay = new ImageOverlay(window.scene, myOffscreenImage, null, getDescendantByTagName(myOffscreenCanvas, "overlays", true));
        _myOffscreenOverlay.name = "Offscreen";
        _myOffscreenOverlay.srcsize = new Vector2f(myOffscreenImage.width / nextPowerOfTwo(myOffscreenImage.width),
                                                   myOffscreenImage.height / nextPowerOfTwo(myOffscreenImage.height));
        _myOffscreenOverlay.srcorigin = new Vector2f( 0, 1 - myOffscreenImage.height / nextPowerOfTwo(myOffscreenImage.height));
        _myOffscreenOverlay.material.name = "Offscreen";
        _myOffscreenOverlay.material.properties.blendfunction = "[one,zero]";
        var myTextures = getDescendantByTagName(_myOffscreenOverlay.material, "textures", false);

        /*
         * Blur_X
         * renders myOffscreenImage into myBlurXImage, with X blur
         * shader enabled
         */
        var myBlurXImage = Modelling.createImage(window.scene, myWidth, myHeight, "RGB");
        myBlurXImage.id = createUniqueId();
        myBlurXImage.name = "BlurX";
        myBlurXImage.resize = "pad";
        myBlurXImage.mipmap = myMipmapFlag;
        myBlurXImage.matrix.postMultiply(myMirrorMatrix);
        myBlurXImage.wrapmode="clamp";
        
        window.scene.images.appendChild(myBlurXImage);

        var myBlurXCanvas = cloneCanvas(window.canvas, "BlurX");
        myBlurXCanvas.target = myBlurXImage.id;

        var myBlurXViewport = getDescendantByTagName(myBlurXCanvas, "viewport");
        myBlurXViewport.glow = 1;

        _myBlurXRenderArea = new OffscreenRenderArea();
        _myBlurXRenderArea.renderingCaps = Renderer.FRAMEBUFFER_SUPPORT;
        _myBlurXRenderArea.setSceneAndCanvas( window.scene, myBlurXCanvas);

        _myBlurXOverlay = new ImageOverlay(window.scene, myOffscreenImage, null, getDescendantByTagName(myBlurXCanvas, "overlays", true));
        _myBlurXOverlay.name = "BlurX";
        _myBlurXOverlay.width = myWidth;
        _myBlurXOverlay.height = myHeight;

        var myBlurXMaterial = _myBlurXOverlay.material;
        myBlurXMaterial.name = "BlurX";
        myBlurXMaterial.requires.textures = "[10[glow,glow]]";

        var myTexture = new Node("<texture/>").firstChild;
        myTexture.image = myBlurKernelImage.id;
        var myBlurXTextures  = getDescendantByTagName(myBlurXMaterial, "textures");
        myBlurXTextures.appendChild(myTexture);

        window.scene.update(Scene.MATERIALS);
        myBlurXMaterial.properties.texelSize = new Vector3f( 1/myBlurXImage.width, 1/myBlurXImage.height, 0.0);
        myBlurXMaterial.properties.glowScale = theGlowScale;
        myBlurXMaterial.properties.kernelSize = theKernelSize;
        myBlurXMaterial.properties.blurKernelTexSize = myBlurKernelImage.width;
        var myTextures = getDescendantByTagName(myBlurXMaterial, "textures", false);

        /*
         * Blur_Y
         * renders myBlurXImage into myBlurYImage, with Y blur shader enabled
         */
        var myBlurYImage = Modelling.createImage(window.scene, myWidth, myHeight, "RGB");
        myBlurYImage.id = createUniqueId();
        myBlurYImage.name = "BlurY";
        myBlurYImage.resize = "pad";
        myBlurYImage.mipmap = myMipmapFlag;
        myBlurYImage.matrix.postMultiply(myMirrorMatrix);
        myBlurYImage.wrapmode="clamp";
        
        window.scene.images.appendChild(myBlurYImage);

        var myBlurYCanvas =  cloneCanvas(window.canvas, "BlurY");
        myBlurYCanvas.target = myBlurYImage.id;

        var myBlurYViewport = getDescendantByTagName(myBlurYCanvas, "viewport");
        myBlurYViewport.glow = 1;

        _myBlurYRenderArea = new OffscreenRenderArea();
        _myBlurYRenderArea.renderingCaps = Renderer.FRAMEBUFFER_SUPPORT;
        _myBlurYRenderArea.setSceneAndCanvas( window.scene, myBlurYCanvas);

        _myBlurYOverlay = new ImageOverlay(window.scene, myBlurXImage, null, getDescendantByTagName(myBlurYCanvas, "overlays", true));
        _myBlurYOverlay.name = "BlurY";

        var myBlurYMaterial = _myBlurYOverlay.material;
        myBlurYMaterial.name = "BlurY";
        myBlurYMaterial.requires.textures = "[10[glow,glow]]";

        var myTexture = new Node("<texture/>").firstChild;
        myTexture.image = myBlurKernelImage.id;
        var myBlurYTextures  = getDescendantByTagName(myBlurYMaterial, "textures");
        myBlurYTextures.appendChild(myTexture);

        window.scene.update(Scene.MATERIALS);
        myBlurYMaterial.properties.texelSize = new Vector3f( 1/myBlurYImage.width, 1/myBlurYImage.height,1.0);
        myBlurYMaterial.properties.glowScale = theGlowScale;
        myBlurYMaterial.properties.kernelSize = theKernelSize;
        myBlurYMaterial.properties.blurKernelTexSize = myBlurKernelImage.width;
        myBlurYMaterial.properties.blendfunction = "[one,one]";
        var myTextures = getDescendantByTagName(myBlurYMaterial, "textures", false);

        /*
         * Glow
         * renders myBlurYImage onto screen
         */
        _myGlowOverlay = new ImageOverlay(window.scene, myBlurYImage, null, getDescendantByTagName(myBlurYCanvas, "overlays", true));
        _myGlowOverlay.name = "Glow";
        _myGlowOverlay.width = _myOffscreenOverlay.width;
        _myGlowOverlay.height = _myOffscreenOverlay.height;
        _myGlowOverlay.material.properties.blendfunction = "[one,one]";
        _myGlowOverlay.srcsize = new Vector2f(myOffscreenImage.width / nextPowerOfTwo(myOffscreenImage.width),
                                                   myOffscreenImage.height / nextPowerOfTwo(myOffscreenImage.height));
        _myGlowOverlay.srcorigin = new Vector2f( 0, 1 - myOffscreenImage.height / nextPowerOfTwo(myOffscreenImage.height));

        // prepare compositing
        window.scene.overlays.appendChild(_myOffscreenOverlay.node);

        _myDebugOverlay = null;//_myOffscreenOverlay.node; // _myBlurXOverlay.node;
        if (_myDebugOverlay) {
            window.scene.overlays.appendChild(_myDebugOverlay);
        } else {
            window.scene.overlays.appendChild(_myGlowOverlay.node);
        }

    }



    setupGlow();
}
