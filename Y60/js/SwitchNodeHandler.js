//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


function SwitchNodeHandlerBase( Public, Protected, theNode) {

    Public.node getter = function() {
        return _myNode;
    }

    Public.switchName getter = function() {
        return _myNode.name;
    }

    Public.childCount getter = function() {
        return _myNode.childNodesLength();
    }
    
    Public.activeChild getter = function() {
        return _myNode.childNode( _myActiveChild ); 
    }

    Public.activeIndex getter = function() {
        return _myActiveChild;
    }

    Public.activeName getter = function() { 
        return _myNode.childNode( _myActiveChild ).name;
    }
    
    Public.activeIndex setter = function(theIndex) {
        _myActiveChild = theIndex;
    }

    var _myNode = theNode;
    var _myActiveChild = 0;
}

/***********************************************************
 *                                                         *
 *               Geometry switch handler                   *
 *                                                         *
 ***********************************************************/
function SwitchNodeHandler( theNode, theActiveIndex) {
    this.Constructor( this, theNode, theActiveIndex);
}

SwitchNodeHandler.prototype.Constructor = function( obj, theNode, theActiveIndex) {
    var Public    = obj;
    var Protected = {}
    
    SwitchNodeHandlerBase(Public, Protected, theNode);

    function setup( theActiveChild ) {
        var myName = new String( Public.switchName );
        if ( ! myName.match(/^switch_.*/)) {
            Logger.warning("Switch node '" + Public.switchName + " doesn't obey the " +
                "naming conventions. Name should start with 'switch_'.");
        }
        if ( theActiveChild == undefined) {
            var myActiveChildCount = 0;
            for (var i = 0; i < Public.childCount; ++i) {
                if (Public.node.childNode( i ).visible) {
                    if (myActiveChildCount > 1) {
                        Logger.warning("Switch node '" + Public.switchName + "' has more " +
                                "than one active child. Using first.");
                    } else {
                        Public.activeIndex = i;
                        //_myActiveChild = i;
                    }
                    myActiveChildCount += 1;
                }
            }
        } else {
            for (var i = 0; i < Public.childCount; ++i) {
                if ( i == theActiveChild) {
                    Public.node.childNode( i ).visible = true;
                } else {
                    Public.node.childNode( i ).visible = false;
                }
            }
        }
    }

    Public.setActiveChild = function( theIndex ) {
        Public.activeChild.visible.visible = false;
        Public.activeIndex = theIndex;
        //_myActiveChild = theIndex;
        Public.activeChild.visible = true;
    }

    Public.setActiveChildByName = function( theName ) {
        Public.activeChild.visible = false;
        for (var i = 0; i < Public.childCount; ++i) {
            if (Public.node.childNode( i ).name == theName) {
                Public.activeIndex = i;
                //_myActiveChild = i;
                break;
            }
        }
        Public.activeChild.visible = true;
    }

    Public.setActiveChildBySubName = function( theString ) {
        Public.activeChild.visible = false;
        for (var i = 0; i < Public.childCount; ++i) {
            if (Public.node.childNode( i ).name.indexOf(theString) != -1) {
                Public.activeIndex = i;
                //_myActiveChild = i;
                break;
            }
        }
        Public.activeChild.visible = true;
        
    }

    setup( theActiveIndex );
}



/***********************************************************
 *                                                         *
 *               Material switch handler                   *
 *                                                         *
 ***********************************************************/
function MSwitchNodeHandler( theNode ) {
    this.Constructor( this, theNode );
}


MSwitchNodeHandler.prototype.Constructor = function( obj, theNode ) {
    var Public    = obj;
    var Protected = {}
    
    SwitchNodeHandlerBase(Public, Protected, theNode);

    function setup() {
        _myTargetMaterial = findTargetMaterial();
    }
        
    Public.setActiveChild = function( theIndex ) {
        setMaterial(Public.activeChild.name);
        Public.activeIndex = theIndex;
        //_myActiveChild = theIndex;
    }
    Public.setActiveChildByName = function( theName ) {
        setMaterial( theName );
        for (var i = 0; i < Public.childCount; ++i) {
            if (Public.node.childNode( i ).name == theName) {
                Public.activeIndex = i;
                //_myActiveChild = i;
                break;
            }
        }
    }

    function findTargetMaterial() {
        var myUnderlineIndex = Public.switchName.search(/_/);
        var myTargetMaterialName = Public.switchName.substring(myUnderlineIndex + 1);
        var myTargetMaterial = getDescendantByName(window.scene.materials, myTargetMaterialName, false);
        if (!myTargetMaterial) {
            // Try adding an 'M'
            var myTargetMaterial = getDescendantByName(window.scene.materials, myTargetMaterialName + "M", false);
        }

        if (!myTargetMaterial) {
            Logger.error("Could not find target material: " + myTargetMaterialName + " for material switch " + Public.switchName);
        }
        
        return myTargetMaterial;
    }
    
    function setMaterial(theMaterialCode) {
        //print("switchMaterials(", theMaterialCode, " ", Public.node);
        // First Step: Search inside the switch for the material with the materialcode
        
        var myNewMaterial = null;
        for (var i = 0; i < Public.childCount; ++i) {
            var myChild = Public.node.childNode( i );
            if (myChild.name == theMaterialCode) {
                var myShapeId = "";
                if (myChild.nodeName == "body") {
                    myShapeId = myChild.shape;
                } else if ((myChild.nodeName == "transform")
                            && myChild.firstChild
                            && myChild.firstChild.nodeName == "body")
                {
                    myShapeId = myChild.firstChild.shape;
                } else {
                    continue;
                }
                var myShape = Public.node.getElementById(myShapeId);
                if (!myShape) {
                    Logger.warning("Could not find shape with id: " + myShapeId);
                    return;
                }
                var myElement = getDescendantByTagName(myShape, "elements", true);
                myNewMaterial = myElement.getElementById(myElement.material);
            }
        }

        //print("new mat: " + myNewMaterial);

        if (!myNewMaterial) {
            Logger.warning("Could not find material: " + theMaterialCode);
            return;
        }

        //print("target mat: " + myTargetMaterial);
        // Third step: Setup target material
        var myNewProperties = myNewMaterial.properties;
        var myTargetProperties = _myTargetMaterial.properties;

        myTargetProperties.ambient       = myNewProperties.ambient;
        myTargetProperties.diffuse       = myNewProperties.diffuse;
        myTargetProperties.specular      = myNewProperties.specular;
        myTargetProperties.shininess     = myNewProperties.shininess;
        myTargetProperties.emissive      = myNewProperties.emissive;
        myTargetProperties.surfacecolor  = myNewProperties.surfacecolor;
        myTargetProperties.blendfunction = myNewProperties.blendfunction;
        myTargetProperties.linewidth     = myNewProperties.linewidth;
        myTargetProperties.pointsize     = myNewProperties.pointsize;
        myTargetProperties.linesmooth    = myNewProperties.linesmooth;
        if ("reflectivity" in myNewProperties) {
            myTargetProperties.reflectivity = myNewProperties.reflectivity;
        }
        if ("eccentricity" in myNewProperties) {
            myTargetProperties.eccentricity = myNewProperties.eccentricity;
        }
        if ("specularrolloff" in myNewProperties) {
            myTargetProperties.specularrolloff = myNewProperties.specularrolloff;
        }
        if ("glow" in myNewProperties) {
            myTargetProperties.glow = myNewProperties.glow;
        }
    }

    var _myTargetMaterial = null;

    setup();
}


/***********************************************************
 *                                                         *
 *                Texture switch handler                   *
 *                                                         *
 ***********************************************************/
function TSwitchNodeHandler( theNode ) {
    this.Constructor( this, theNode );
}


TSwitchNodeHandler.prototype.Constructor = function( obj, theNode) {
    var Public    = obj;
    var Protected = {}
    
    SwitchNodeHandlerBase(Public, Protected, theNode);

    // overwrite some getters and setters because they have to
    // be handled different from model/materialswitches
    Public.activeChild getter = function() {
        return _myActiveChild;
    }

    Public.activeIndex getter = function() {
        Logger.error("Implement me!"); 
    }

    Public.activeName getter = function() { 
        if (_myActiveChild && "name" in _myActiveChild) {
            return _myActiveChild.name;
        }
        return null;
    }
    
    function setup() {
        var myName = new String( Public.switchName );
        if ( ! myName.match(/^tswitch_.*/)) {
            Logger.warning("TSwitch node '" + Public.switchName + " doesn't obey the " +
                "naming conventions. Name should start with 'tswitch_'.");
        }
        
        // find the transform node with the texture references first
        var myReferenceNode = getDescendantByName(window.scene.world, "textureswitches", true);
        if (!myReferenceNode) {
            Logger.error("Could not find reference node for textureswitches");
            return;
        }
        
        for (var i=0; i<myReferenceNode.childNodesLength(); ++i) {
            var myNode = myReferenceNode.childNode(i);
            if (myNode.name.indexOf(myName) != -1) {
                _mySwitches = myNode;
                continue;
            }
        }

        if (!_mySwitches) {
            Logger.error("Could not find reference node for textureswitch: " + Public.switchName);
            return;
        }
    }
    
    function switchTexture(theNode) {
        _myActiveChild = theNode;
      
        // get image id for the new texture
        var myShape = theNode.getElementById(theNode.shape);
        var myMaterialId = myShape.childNode("primitives").childNode("elements").material;
        
        var myMaterial = theNode.getElementById(myMaterialId);
        var myImage = myMaterial.childNode("textures").firstChild.image;

        // set new image 
        Public.node.childNode("textures").firstChild.image = myImage;
    }

    Public.setActiveChildByName = function(theName) {
        if (!_mySwitches) {
            return;
        }
    
        var myNode = getDescendantByName(_mySwitches, theName, true);
        if (!myNode) {
            Logger.error("Could not find corresponding texture reference for: " + theName);
            return;
        }

        switchTexture(myNode);
    }
    
    Public.setActiveChildBySubName = function(theSubName) {
        if (!_mySwitches) {
            return;
        }

        var myNode = null;
        for (var i=0; i<_mySwitches.childNodesLength(); ++i) {
            myNode = _mySwitches.childNode(i);
            if (myNode.name.indexOf(theSubName) != -1) {
                switchTexture(myNode);
                break;
            }
        }
    }

    var _mySwitches = null;
    var _myActiveChild = null;

    setup();
}
