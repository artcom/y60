//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");

/* this is not a test itself but a module for the tests
 * testOffScreenFramebuffer & testOffScreenBackBuffer
 */

function TestOffScreen(theArguments) {
    this.Constructor(this, theArguments);
}

TestOffScreen.prototype.Constructor = function(obj, theArguments) {

    SceneTester.prototype.Constructor(obj, theArguments);

    const MIRROR_PLANE = new Vector3f(0,0,-2);
    const MIRROR_ASPECT_RATIO = 2; //width : height

    var myMirror = null;
    var myMirrorCamera = null;
    var myOffscreenBuffer = null;

    var Base = [];
    var _myTextureUnit = null;

    function setCulling(theNode, theCullFlag) {
        if (theNode == undefined) {
            return;
        }
        if (theNode.cullable) {
            theNode.cullable = theCullFlag;
        }
        for (var i = 0; i < theNode.childNodes.length; ++i) {
            setCulling(theNode.childNodes[i], theCullFlag);
        }
    }

    Base.setup = obj.setup;
    obj.setup = function(theWidth, theHeight, theUseFramebufferExtensionFlag, theMultisamples) {

        Base.setup(theWidth, theHeight);//, undefined, undefined, null, true,false );
        setCulling(window.scene.world, false);

        if (theMultisamples == undefined) {
            theMultisamples = 0;
        }

        // fetch mirror
        myMirror = window.scene.world.find(".//*[@name = 'spiegel']");
        if (!myMirror) {
            Logger.error("Mirror object not found");
            exit(1);
        }

        //create new image
        var myWidth = theUseFramebufferExtensionFlag ? 2048 : 512;
        var myHeight = myWidth / MIRROR_ASPECT_RATIO;

        var myImage = Modelling.createImage(window.scene, myWidth, myHeight,"rgb");
        var myTexture = Modelling.createTexture(window.scene, myImage);
        myTexture.wrapmode = "repeat";
        myTexture.mipmap = true;

        //replace existing texture image with our new one
        var myMaterial  = window.scene.materials.find("material[@name = 'lambert4']");
        _myTextureUnit = myMaterial.childNode( "textureunits").firstChild;
        _myTextureUnit.texture = myTexture.id;

        // flip vertically since framebuffer content is upside-down
        var myMirrorMatrix = new Matrix4f;
        myMirrorMatrix.makeScaling(new Vector3f(1,-1,1));
        myImage.matrix.postMultiply(myMirrorMatrix);

        // flip horizontally for mirroring effect
        myMirrorMatrix.makeScaling(new Vector3f(-1,1,1));
        myImage.matrix.postMultiply(myMirrorMatrix);

        // setup offscreen canvas
        var myCanvas = obj.getRenderWindow().canvas.cloneNode(true);
        myCanvas.id = createUniqueId();
        myCanvas.name = "OffscreenBuffer";
        myCanvas.targets = [myTexture.id];
        myCanvas.backgroundcolor = [1,1,1,1];

        // setup offscreen camera
        var myViewport = myCanvas.find("viewport");
        myMirrorCamera = window.scene.world.getElementById(myViewport.camera);
        myMirrorCamera = myMirrorCamera.cloneNode();
        myMirrorCamera.id = createUniqueId();
        window.scene.world.appendChild(myMirrorCamera);

        myViewport.id = createUniqueId();
        myViewport.camera = myMirrorCamera.id;

        // append offscreen canvas
        var myCanvases = window.scene.canvases;
        myCanvases.appendChild(myCanvas);

        // offscreen renderer
        myOffscreenBuffer = new OffscreenRenderArea();
        if (theUseFramebufferExtensionFlag) {
            print("##### using FRAMEBUFFER OBJECT extension");
            myOffscreenBuffer.renderingCaps = Renderer.FRAMEBUFFER_SUPPORT;
            myOffscreenBuffer.multisamples = theMultisamples;
        }

        myOffscreenBuffer.scene = obj.getScene();
        myOffscreenBuffer.canvas = myCanvas;

        // reference position for mipmap comparison
        //window.camera.position = [4.04183,3.5257,11.5152];
        //window.camera.orientation = [0.0875352,-0.144218,-0.0128088,0.985583];

        // reference position for FBO multisampling comparison
        //window.camera.position = [0.357935,1.15633,-0.763257];
        //window.camera.orientation = [0.0597803,-0.140139,-0.00847679,0.988289];
    }

    Base.onFrame = obj.onFrame;
    obj.onFrame = function(theTime) {
        try {
            Base.onFrame(theTime);
            //print("cam:" + window.camera.position, window.camera.orientation);

            if (myOffscreenBuffer) {
     
                var myPlane = new Planef(new Vector3f(0,0,1), new Vector3f(0,1,-2));                 
                var myMirrorCameraHint = createMirrorCamera(window.camera, myPlane);
                myMirrorCamera.orientation = myMirrorCameraHint.orientation;
                myMirrorCamera.position    = myMirrorCameraHint.position;

                // disable mirror, render, re-enable
                myMirror.visible = false;
                myOffscreenBuffer.renderToCanvas(true);
                myMirror.visible = true;
            }
        } catch (ex) {
            reportException(ex);
            exit(1);
        }
    }
}
