//=============================================================================
// Copyright (C) 2006-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");
use("Y60JSSL.js");
use("BuildUtils.js");
use("BumpUtils.js");

var ourShow = new SceneTester(arguments);

try {
    
    GLResourceManager.prepareShaderLibrary("testMRT_shaderlibrary.xml");
    var Base = [];
    var ourFramesWritten = 0;
    var myRenderArea, i;
    var myImages = [];
        

function ENSURE(theExpression, theMessage) {
    var myResult;
    try {
        myResult = eval(theExpression);
    } catch(e) {
        myResult = false;
        print('###### EXCEPTION:'+e);
    }
    if (!myResult) {
        throw new Exception(theExpression + " FAILED");
    }
    return myResult;
}

    Base.setup = ourShow.setup;
    ourShow.setup = function() {
        Base.setup();
        var MRTS = 4;
        var myImage, myTexture, myMaterial, myFlipMatrix, myCanvasNode, myViewport, myBody;
        ourShow.scene = new Scene("cube.x60");
        ourShow.scene.setup();
        ourShow.setScene(ourShow.scene);
        ourShow.getAnimationManager().enable(false);
        ourShow.offscreenFlag = false;
        
        myCanvasNode = Node.createElement("canvas");
        myCanvasNode.name = "offscreen-canvas";
        window.scene.canvases.appendChild(myCanvasNode);
        myCanvasNode.backgroundcolor = new Vector4f(0.5,0.5,0.75,1);
        
        myViewport = Node.createElement("viewport");
        myViewport.name = "offscreen-viewport";
        myCanvasNode.appendChild(myViewport);
        myViewport.camera = window.scene.world.find(".//*[@name = 'perspShape']").id;
        var myWorld = Node.createElement("world");
        myWorld.name = "offscreen-world";
        window.scene.worlds.appendChild(myWorld);
        
        var myCamera = Node.createElement("camera");
        myWorld.appendChild(myCamera);
        setupCameraOrtho(myCamera, window.width, window.height);
        
        window.scene.canvases.firstChild.firstChild.camera = myCamera.id;
        myRenderArea = new OffscreenRenderArea();
        myRenderArea.renderingCaps = ourShow.getDefaultRenderingCapabilites() | Renderer.FRAMEBUFFER_SUPPORT;
        myRenderArea.multisamples = 0;
        
        // add shader
        for (i = 0; i < window.scene.materials.childNodesLength(); ++i) {
            myMaterial = window.scene.materials.childNode(i);
            myMaterial.enabled = false;
            addMaterialRequirement(myMaterial, "option", "[10[test_mrt]]");
            var myNode = myMaterial.find(".//*[@name='lighting']");
            myNode.childNode("#text").nodeValue = "[10[unlit]]";
            myMaterial.enabled = true;
        }
        
        var myTransform = Modelling.createTransform(myWorld);
        //prepare mrt textures
        for (i = 0; i < MRTS; ++i) {
            myImage = Modelling.createImage(window.scene, window.width, window.height, "BGRA");
            myImages.push(myImage);
            myTexture = Modelling.createTexture(window.scene, myImage);
            myTexture.wrapmode = "clamp_to_edge";
            myFlipMatrix = myTexture.matrix;
            myFlipMatrix.makeScaling(new Vector3f(1, -1, 1));
            myFlipMatrix.translate(new Vector3f(0, 1, 0));
            myCanvasNode.targets = myCanvasNode.targets.length > 0 ? stringToArray(myCanvasNode.targets.toString()).concat([myTexture.id]) : [myTexture.id];
            
            myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myTexture);
            var myShape = Modelling.createQuad(window.scene, myMaterial.id, [0,0,0], [window.width/2, window.height/2, 0]);
            myBody = Modelling.createBody(myTransform, myShape.id);
            myBody.position.x = (i%2)*window.width/2;
            myBody.position.y = i > 1 ? window.height/2 : 0;
        }
        
        myRenderArea.setSceneAndCanvas( window.scene, myCanvasNode);
        
        myWorld.visible = true;
        window.scene.world.visible = false;
    }
    Base.onPreRender = ourShow.onPreRender;
    ourShow.onPreRender = function() {
        Base.onPreRender();
        if (ourFramesWritten === 0) {
            ourFramesWritten++;
            window.scene.world.visible = true;
            myRenderArea.renderToCanvas();
            window.scene.world.visible = false;
            return;
        }
        if (ourFramesWritten === 1) {
            ourFramesWritten++;
            window.scene.world.visible = true;
            myRenderArea.renderToCanvas(true);
            ENSURE("almostEqual(myImages[1].raster.getPixel(window.width/2,window.height/2), [1,0,0,1])");
            ENSURE("almostEqual(myImages[2].raster.getPixel(window.width/2,window.height/2), [0,1,0,1])");
            ENSURE("almostEqual(myImages[3].raster.getPixel(window.width/2,window.height/2), [0,0,1,1])");
            window.scene.world.visible = false;
            return;
        }
        if (ourFramesWritten === 2) {
            ourFramesWritten++;
            window.scene.world.visible = true;
            myRenderArea.multisamples = 4;
            myRenderArea.renderToCanvas();
            window.scene.world.visible = false;
            return;
        }
        if (ourFramesWritten === 3) {
            ourFramesWritten++;
            window.scene.world.visible = true;
            myRenderArea.renderToCanvas(true);
            window.scene.world.visible = false;
            ENSURE("almostEqual(myImages[1].raster.getPixel(window.width/2,window.height/2), [1,0,0,1])");
            ENSURE("almostEqual(myImages[2].raster.getPixel(window.width/2,window.height/2), [0,1,0,1])");
            ENSURE("almostEqual(myImages[3].raster.getPixel(window.width/2,window.height/2), [0,0,1,1])");
            return;
        }
    };
    ourShow.onFrameDone = function(theFrameCount) {
        ourShow.saveTestImage();
        if (theFrameCount > 3) exit(0); 
    };
    ourShow.setup();
    ourShow.setTestDurationInFrames(4);

    ourShow.go()
}
catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
