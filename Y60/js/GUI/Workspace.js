//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Animation.js");
use("Internationalization.js");

function Workspace(theWorkspaceNode, theDepth) {
    this.Constructor(this, theWorkspaceNode, theDepth);
}

Workspace.prototype.Constructor = function(Public, theWorkspaceNode, theDepth) 
{

    var _myImages = [];
    var _myTexts = [];
    var _myButtons = [];
    
    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    
    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.onFrame = function(theTime) {

    }

    

    ////////////////////////////////////////
    // Settings handler
    ////////////////////////////////////////

    ////////////////////////////////////////
    // callbacks
    ////////////////////////////////////////

    ////////////////////////////////////////
    // visibility/sensibility handling
    ////////////////////////////////////////
    
   
    ////////////////////////////////////////
    // labels and other texts
    ////////////////////////////////////////    
    

    ////////////////////////////////////////
    // timeout
    ////////////////////////////////////////
    

    ////////////////////////////////////////
    // setup
    ////////////////////////////////////////

    function setup() {
        Logger.warning("Setting up workspace " + _myName);

    
    
        for(var i=0; i < theWorkspaceNode.childNodesLength("image"); ++i){
            var myImageNode = theWorkspaceNode.childNode("image", i);
            var myImage = new Image(window.scene, myImageNode, 0);
            _myImages.push(myImage);
        }

        for(var i=0; i < theWorkspaceNode.childNodesLength("text"); ++i){
            var myTextNode = theWorkspaceNode.childNode("text", i);
            var myText = new Text(window.scene, myTextNode, 0);
            _myTexts.push(myText);
        }

    
        for(var i=0; i < theWorkspaceNode.childNodesLength("textbutton"); ++i){
            var myButtonNode = theWorkspaceNode.childNode("textbutton", i);
            var myButton = new TextButton(window.scene, myButtonNode, 2);
            //myButton.onPointerClickWithSelf = outroButtonPressed;
            _myButtons.push(myButton);
        }

    
    }
    
    print("new workspace");
    setup();
}
