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

function ImageLayer(theDepth, theName, theType) {
    this.Constructor(this, theDepth, theName, theType);
}

ImageLayer.prototype.Constructor = function(Public, theDepth, theName, theType) {

    var Protected = {};
    var Base = {};
    LayerBase.prototype.Constructor(Public, Protected, theDepth, theName, theType);

    var _myDepth = theDepth;

    var _myName = null;
    var _myMaterial = null;
    var _myQuad = null;

    Base.setup = Public.setup;
    Public.setup = function(theLayerNode) {
        
        Logger.info(theLayerNode);
        _myName = theLayerNode.name;

        _myMaterial = getCachedImageMaterial("CONTENT/" + theLayerNode.getAttribute("file"));
        _myMaterial.transparent = theLayerNode.transparent;
    
        var myX = 0;
        if ("x" in theLayerNode) {
            myX = Number(theLayerNode.x);
        }
        
        var myY = 0;
        if ("y" in theLayerNode) {
            myY = Number(theLayerNode.y);
        }
        
        var myZ = _myDepth;
        if ("z" in theLayerNode) {
            myZ = Number(theLayerNode.z);
        }

        _myQuad = createQuad(window.scene.world,
                             _myName,
                             new Vector2i(theLayerNode.width, theLayerNode.height),
                             new Vector3i(myX, myY, myZ),
                             _myMaterial,
                             true, true);

    }
    
}
