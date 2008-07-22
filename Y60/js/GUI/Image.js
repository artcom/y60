//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function Image(theParent, theImageNode, theDepth) {
    this.Constructor(this, theParent, theImageNode, theDepth);
}

Image.prototype.Constructor = function(Public, theParent, theImageNode, theDepth) {
    
    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _myName   = theImageNode.name;
    var _myNode   = theImageNode;
    var _myParent = theParent;
    var _myDepth  = theDepth;

    var _mySelected = false;

    var _myMaterial = null;
    var _myQuad     = null;

    Public.bodies getter = function() {
        return [_myQuad.body];
    }
   
    Public.name getter = function() {
        return _myName;
    }

    Public.body getter = function() {
        return _myQuad.body;
    }
    
    Public.insensible setter = function(theInsensible) {
        _myQuad.body.insensible = theInsensible;
    }

    Public.material getter = function() {
        return _myMaterial;
    }
    
    Public.alpha getter = function() {
        return _myMaterial.properties.surfacecolor[3];
    }

    Public.alpha setter = function(theAlpha) {
        Modelling.setAlpha(_myQuad.body, theAlpha);
    }
    
    Public.visible getter = function() {
        return Public.body.visible;
    }
    
    Public.visible setter = function(theVisibility) {
        Public.body.visible = theVisibility;
    }

    Public.image setter = function(theImage) {
        var myTextureUnit = _myMaterial.childNode("textureunits").firstChild;
        var myTexture = window.scene.dom.getElementById(myTextureUnit.texture);
        myTexture.image = theImage.id; 
    }
    
    Public.image getter = function() {
        var myTextureUnit = _myMaterial.childNode("textureunits").firstChild;
        var myTexture = window.scene.dom.getElementById(myTextureUnit.texture);
        
        return window.scene.dom.getElementById(myTexture.image);
    }

    ////////////////////////////////////////
    // setup
    ////////////////////////////////////////
        
    Public.setup = function() {
        Logger.info("Setting up image " + _myName);

        if("z" in _myNode) {
            _myDepth += Number(_myNode.z);
        }

         var myFile = CONTENT + "/" + _myNode.file;
         _myMaterial = getCachedImageMaterial(myFile);
         _myMaterial.transparent = true;
 
         var myInsensible = true;
         if ("insensible" in _myNode) {
             myInsensible = _myNode.insensible;
         }
         
         _myQuad = createQuad(_myParent, _myName,
                              new Vector2f(_myNode.width,_myNode.height),
                              new Vector3f(_myNode.x,_myNode.y, _myDepth),
                              _myMaterial,
                              myInsensible,
                              /* visible */    true);

         if("angle" in _myNode) {
             _myQuad.body.orientation =
                 Quaternionf.createFromEuler(new Vector3f(0,0,radFromDeg(_myNode.angle)))
         }
    }        
    
    Public.setup();
}
