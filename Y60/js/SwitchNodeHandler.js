//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


function SwitchNodeHandler( theNode, theActiveIndex) {
    this.Constructor( this, theNode, theActiveIndex);
}


SwitchNodeHandler.prototype.Constructor = function( obj, theNode, theActiveIndex) {

    function setup( theActiveChild ) {
        var myName = new String( _myNode.name );
        if ( ! myName.match(/^switch_.*/)) {
            Logger.warning("Switch node '" + _myNode.name + " doesn't obey the " +
                "naming conventions. Name should start with 'switch_'.");
        }
        if ( theActiveChild == undefined) {
            var myActiveChildCount = 0;
            for (var i = 0; i < _myNode.childNodesLength(); ++i) {
                if (_myNode.childNode( i ).visible) {
                    if (myActiveChildCount > 1) {
                        Logger.warning("Switch node '" + _myNode.name + "' has more " +
                                "than one active child. Using first.");
                    } else {
                        _myActiveChild = i;
                    }
                    myActiveChildCount += 1;
                }
            }
        } else {
            for (var i = 0; i < _myNode.childNodesLength(); ++i) {
                if ( i == theActiveChild) {
                    _myNode.childNode( i ).visible = true;
                } else {
                    _myNode.childNode( i ).visible = false;
                }
            }
        }
    }
    
    obj.getSwitchName = function() {
        return _myNode.name;
    }

    obj.getChildCount = function() {
        return _myNode.childNodesLength();
    }
    obj.getNode = function() {
        return _myNode;
    }

    obj.getActiveChild = function() {
        return _myNode.childNode( _myActiveChild );
    }
    obj.getActiveIndex = function() {
        return _myActiveChild;
    }
    obj.getActiveName = function() {
        return _myNode.childNode( _myActiveChild ).name;
    }

    obj.setActiveChild = function( theIndex ) {
        _myNode.childNode( _myActiveChild ).visible = false;
        _myActiveChild = theIndex;
        _myNode.childNode( _myActiveChild ).visible = true;
    }
    obj.setActiveChildByName = function( theName ) {
        _myNode.childNode( _myActiveChild ).visible = false;
        for (var i = 0; i < _myNode.childNodesLength(); ++i) {
            if (_myNode.childNode( i ).name == theName) {
                _myActiveChild = i;
                break;
            }
        }
        _myNode.childNode( _myActiveChild ).visible = true;
    }

    var _myNode = theNode;
    var _myActiveChild = 0;

    setup( theActiveIndex );
}



function MSwitchNodeHandler( theNode ) {
    this.Constructor( this, theNode );
}


MSwitchNodeHandler.prototype.Constructor = function( obj, theNode ) {

    function setup() {
        _myTargetMaterial = findTargetMaterial();
    }
        
    obj.getSwitchName = function() {
        return _myNode.name;
    }

    obj.getChildCount = function() {
        return _myNode.childNodesLength();
    }
    obj.getNode = function() {
        return _myNode;
    }

    obj.getActiveChild = function() {
        return _myNode.childNode( _myActiveChild );
    }
    obj.getActiveIndex = function() {
        return _myActiveChild;
    }
    obj.getActiveName = function() {
        return _myNode.childNode( _myActiveChild ).name;
    }

    obj.setActiveChild = function( theIndex ) {
        setMaterial(_myNode.childNode( _myActiveChild ).name);
        _myActiveChild = theIndex;
    }
    obj.setActiveChildByName = function( theName ) {
        setMaterial( theName );
        for (var i = 0; i < _myNode.childNodesLength(); ++i) {
            if (_myNode.childNode( i ).name == theName) {
                _myActiveChild = i;
                break;
            }
        }
    }

    function findTargetMaterial() {
        var myUnderlineIndex = _myNode.name.search(/_/);
        var myTargetMaterialName = _myNode.name.substring(myUnderlineIndex + 1);
        var myTargetMaterial = getDescendantByName(window.scene.materials, myTargetMaterialName, false);
        if (!myTargetMaterial) {
            // Try adding an 'M'
            var myTargetMaterial = getDescendantByName(window.scene.materials, myTargetMaterialName + "M", false);
        }

        if (!myTargetMaterial) {
            Logger.error("Could not find target material: " + myTargetMaterialName + " for material switch " + _myNode.name);
        }
        
        return myTargetMaterial;
    }
    
    function setMaterial(theMaterialCode) {
        //print("switchMaterials(", theMaterialCode, " ", _myNode);
        // First Step: Search inside the switch for the material with the materialcode
        
        var myNewMaterial = null;
        for (var i = 0; i < _myNode.childNodes.length; ++i) {
            var myChild = _myNode.childNodes[i];
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
                var myShape = _myNode.getElementById(myShapeId);
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

        //print("target mat: " + myTargetMaterial);
    }

    var _myNode = theNode;
    var _myActiveChild = 0;
    var _myTargetMaterial = null;

    setup();
}


function TSwitchNodeHandler( theNode ) {
    this.Constructor( this, theNode );
}


TSwitchNodeHandler.prototype.Constructor = function( obj, theNode) {

    function setup() {
    }
        
    obj.getSwitchName = function() {
        return _myNode.name;
    }

    obj.getNode = function() {
        return _myNode;
    }


    var _myNode = theNode;

    setup();
}
