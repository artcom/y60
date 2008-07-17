//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("LayerBase.js");

function WorkspaceLayer(theDepth, theName, theType) {
    this.Constructor(this, theDepth, theName, theType);
}

WorkspaceLayer.prototype.Constructor = function(Public, theDepth, theName, theType) {

    var Protected = {};
    LayerBase.prototype.Constructor(Public, Protected, theDepth, theName, theType);
    Public.LayerBase = [];

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////////////////////////////
    Public.getWorkspaceByName = function(theName) {
        for(var i=0; i< _myWorkspaces.length; i++){
            if(_myWorkspaces[i].name == theName){
                return _myWorkspaces[i];
            }
        }    
    }
    Public.LayerBase.setup = Public.setup;    
    Public.setup = function(theLayerNode) {
        Public.LayerBase.setup(theLayerNode);        
        for(var i=0; i < theLayerNode.childNodesLength("workspace"); ++i){
            var myWorkspaceNode = theLayerNode.childNode("workspace", i);
            print("workspace " + myWorkspaceNode);
            _myWorkspaces.push(new Workspace(myWorkspaceNode, Protected.getDepth()));
        }
    }
    Public.LayerBase.onPostRender = Public.onPostRender;    
    Public.onPostRender = function() {
        Public.LayerBase.onPostRender();
        for (var i = 0; i< _myWorkspaces.length ; i++) {
            _myWorkspaces[i].onPostRender();
        }        
    }
    Public.LayerBase.onFrame = Public.onFrame;    
    Public.onFrame = function(theTime) {
        Public.LayerBase.onFrame(theTime);
        for (var i = 0; i< _myWorkspaces.length ; i++) {
            _myWorkspaces[i].onFrame(theTime);
        }
    }

    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////
    

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Member
    ///////////////////////////////////////////////////////////////////////////////////////////    
    var _myWorkspaces = [];
}
