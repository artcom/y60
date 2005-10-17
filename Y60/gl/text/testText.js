//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: testText.js,v $
//   $Author: christian $
//   $Revision: 1.14 $
//   $Date: 2004/06/28 16:02:47 $
//
//
//=============================================================================

use("SceneViewer.js");

function testTextApp(theSceneFile, theShaderLib) {
    this.Constructor(this, theSceneFile, theShaderLib);
}

testTextApp.prototype.Constructor = function(obj, theSceneFile, theShaderLib) {

    var _myUnicodeXML = null;

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // Constructor code
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(obj, theSceneFile, theShaderLib);
    obj.SceneViewer = [];

    // public methods
    obj.SceneViewer.setup = obj.setup;
    obj.setup = function(theWidth, theHeight, theTitle) {
        obj.SceneViewer.setup(theWidth, theHeight, theTitle);
        window.loadTTF("Arial", "fonts/arial.ttf", 80);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // callbacks
    //
    ///////////////////////////////////////////////////////////////////////////////////////////
    obj.SceneViewer.onPostRender = obj.onPostRender;
    obj.onPostRender = function () {
    }

    obj.SceneViewer.onFrame = obj.onFrame;
    obj.onFrame = function (theTime) {
        obj.SceneViewer.onFrame(theTime);

        window.setTextStyle(Renderer.SOLID_TEXT);
        window.setTextColor(new Vector4f(1,0,0,1), new Vector4f(0,0,0,1));
        window.renderText([0.1,0.1], "AVA Arial SOLID", "Arial");
        window.setTextStyle(Renderer.SHADED_TEXT);
        window.setTextColor(new Vector4f(0,1,0,1), new Vector4f(1,1,1,1));
        window.renderText([0.1,0.3], "AVA Arial SHADED", "Arial");
        window.setTextStyle(Renderer.BLENDED_TEXT);
        window.setTextColor(new Vector4f(0,0,1,1), new Vector4f(0,0,0,1));
        window.renderText([0.1,0.5], "AVA Arial BLENDED", "Arial");

        window.setTextColor(new Vector4f(0,0,1,1), new Vector4f(0,0,0,1));

        var myTestString = String.fromCharCode(231, 172, 172);
        window.renderText([0.1,0.8], "Nachtschwärmer" + myTestString, "Times12");

        window.setTextColor(new Vector4f(1,1,1,1), new Vector4f(0,0,0,1));
    }

}

try {
    var ourApp = new testTextApp(arguments);
    ourApp.setup(800, 600, "Text test");
    ourApp.go();
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(-1);
}
