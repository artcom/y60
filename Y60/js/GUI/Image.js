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
    var _myHeight = 0;
    var _myWidth  = 0;

    var _mySelected = false;

    var _myMaterial = null;
    var _myQuad     = null;

    var _myAlpha    = 1.0;
    var _myMaxAlpha = 1.0;

    Public.bodies getter = function() {
        return [_myQuad.body];
    }
    
    Public.clone = function() {
        var myNode = clone(_myNode);
        myNode.name = _myNode.name + "_copy";
        return new Image(_myParent, myNode, _myDepth);
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
        return _myAlpha; 
    }
    
    Public.alpha setter = function(theAlpha) {
        _myAlpha = theAlpha; 
        Modelling.setAlpha(_myQuad.body, Math.min(_myMaxAlpha, _myAlpha));
    }

    Public.maxalpha setter = function(theAlpha) {
        _myMaxAlpha = theAlpha;
        Public.alpha = _myAlpha;
    }
    
    Public.visible getter = function() {
        return Public.body.visible;
    }
    
    Public.visible setter = function(theVisibility) {
        Public.body.visible = theVisibility;
    }
    
    Public.texture getter = function() {
        var myTextureUnit = _myMaterial.childNode("textureunits").firstChild;
        return window.scene.dom.getElementById(myTextureUnit.texture);
    }
    
    Public.image setter = function(theImage) {
        Public.texture.image = theImage.id; 
    }
    
    Public.image getter = function() {
        return window.scene.dom.getElementById(Public.texture.image);
    }
    
    Public.wrapmode getter = function() {
        return Public.texture.wrapmode;
    }
    
    Public.wrapmode setter = function(theWrapmode) {
        Public.texture.wrapmode = theWrapmode;
    }
    
    Public.width getter = function() {
        return _myWidth;
    }
    
    Public.width setter = function(theWidth) {
        _myWidth = theWidth;
    }
    
    Public.height getter = function() {
        return _myHeight;
    }
    
    Public.height setter = function(theHeight) {
        _myHeight = theHeight;
    }
    
    Public.z getter = function() {
        return Public.body.position.z;
    }
    
    Public.z setter = function(v) {
        Public.body.position.z = v;
    }
    
    Public.playmode getter = function() {
        if ("playmode" in Public.image) {
            return Public.image.playmode;
        }
        return "";
    }
    Public.playmode setter = function(theMode) {
        if ("playmode" in Public.image) {
            Public.image.playmode = theMode;
        }
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
 
        _myWidth = _myNode.width;
        _myHeight = _myNode.height;
 
        var myInsensible = true;
        if ("insensible" in _myNode) {
            myInsensible = _myNode.insensible;
        }
         
        _myQuad = createQuad(_myParent, _myName,
                             new Vector2f(_myWidth,_myHeight),
                             new Vector3f(_myNode.x,_myNode.y, _myDepth),
                             _myMaterial,
                             myInsensible,
                             /* visible */    true);

        if ("angle" in _myNode) {
            _myQuad.body.orientation =
                Quaternionf.createFromEuler(new Vector3f(0,0,radFromDeg(_myNode.angle)));
        }
         
        if ("wrapmode" in _myNode) {
            Public.wrapmode = _myNode.wrapmode;
        }
    } 
    
    Public.setup();
}
