//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: testTextureAnimation.js,v $
//   $Author: david $
//   $Revision: 1.3 $
//   $Date: 2004/05/21 17:19:12 $
//
//
//=============================================================================

use("Y60JSSL.js");
use("SceneViewer.js");
use("TextureAnimation.js");

function TextAnimationTestApp(theSceneFile, theShaderLib) {
    this.Constructor(this, theSceneFile, theShaderLib);
}

TextAnimationTestApp.prototype.Constructor = function(obj, theSceneFile, theShaderLib) {
    SceneViewer.prototype.Constructor(obj, theSceneFile, theShaderLib);
 
    var _myTextureAnimation = 0;
    
    obj.SceneViewer = [];
       
    obj.SceneViewer.setup = obj.setup;
    obj.setup = function(theWidth, theHeight, theTitle) {
        obj.SceneViewer.setup(theWidth, theHeight, theTitle);        

        // setup TextureAnimation
        var myImageNode = getElementByName(window.scene.childNode("images"),"CanvasImage");
        _myTextureAnimation = new TextureAnimation(myImageNode,0);
        _myTextureAnimation.addFramesByBaseName("texanim/ripple_????.jpg", 60);
        //_myTextureAnimation.setLooping(-1);// endless loop
        //_myTextureAnimation.setLooping(5);// loop 5 times
        //_myTextureAnimation.setDuration(1);// set duration for per time update to 5 secs
    }

    obj.SceneViewer.onFrame = obj.onFrame;
    obj.onFrame = function (theTime) {
        _myTextureAnimation.updatePerFrame();
        //_myTextureAnimation.updatePerTime(theTime);
        return obj.SceneViewer.onFrame(theTime);
    }

    obj.SceneViewer.onKey = obj.onKey;
    obj.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        return obj.SceneViewer.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
    }

    obj.SceneViewer.onMouseButton = obj.onMouseButton;
    obj.onMouseButton = function(theButton, theState, theX, theY) {
        return obj.SceneViewer.onMouseButton(theButton, theState, theX, theY);
    };        

}

try {
    var ourTextAnimationTestApp = new TextAnimationTestApp(arguments[0], arguments[1]);    
    ourTextAnimationTestApp.setup(800, 600, "Sail away");   
    window.canvas.backgroundcolor = [0.8, 0.9, 1];        
    ourTextAnimationTestApp.go();
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
}    

    

