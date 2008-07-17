//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function LayerManager() {
    this.Constructor(this);
}

LayerManager.prototype.Constructor = function(Public) {
    var _myLayers = [];

    //////////////////////////////////////////////////////////////////////
    // Public
    //////////////////////////////////////////////////////////////////////

    Public.setup = function(theLayerNode) {
        for(var i=0; i < theLayerNode.childNodes.length; ++i){
            var myLayerNode  = theLayerNode.childNode(i);
            var myLayerType  = myLayerNode.type;
            var myLayerName  = myLayerNode.name;
            var myLayerDepth = i*3;

            Logger.info("Setting up layer \"" + myLayerName + "\" at depth " + myLayerDepth + "...");

            use("GUI/LAYERS/" + myLayerType + "Layer.js");
            
            var myCtorCall = "new "+myLayerType + "Layer("+myLayerDepth+",'"+myLayerName+"'"+",'"+myLayerType+"')";
            var myNewLayer = eval(myCtorCall);
            myNewLayer.setup(myLayerNode);
            _myLayers.push(myNewLayer);
        }
    }    
    
    Public.getLayerByType = function(theType){
        for (var i = 0; i< _myLayers.length; i++) {
            if (_myLayers[i].type == theType) {
                return _myLayers[i];
            }
        }
        return null;
    }

    Public.getLayerByName = function(theName) {
        for (var i=0; i<_myLayers.length; ++i) {
            if (_myLayers[i].name == theName) {
                return _myLayers[i];
            }
        }
    }
    
    Public.layers getter  = function() {
        return _myLayers;
    }
}
