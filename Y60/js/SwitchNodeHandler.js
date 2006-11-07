//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


function SwitchNodeHandlerBase( Public, Protected, theNode ) {

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
function SwitchNodeHandler( theNode, theActiveIndex ) {
    this.Constructor( this, theNode, theActiveIndex );
}

SwitchNodeHandler.prototype.Constructor = function( obj, theNode, theActiveIndex ) {
    var Public    = obj;
    var Protected = {}
    
    SwitchNodeHandlerBase( Public, Protected, theNode );

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
        Public.activeChild.visible = true;
    }

    Public.setActiveChildByName = function(theName, theSubnameFlag) {
        Public.activeChild.visible = false;
        for (var i = 0; i < Public.childCount; ++i) {
            if ((Public.node.childNode(i).name == theName) 
                 || (theSubnameFlag && Public.node.childNode(i).name.indexOf(theName) != -1)) 
            {
                Public.activeIndex = i;
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
    }
        
    Public.setActiveChild = function( theIndex ) {
        setMaterial(Public.activeChild.name);
        Public.activeIndex = theIndex;
    }
    
    Public.setActiveChildByName = function(theName, theSubnameFlag) {
        for (var i=0; i<Public.childCount; ++i) {
            var myChildNode = Public.node.childNode(i);
            if ((myChildNode.name == theName) 
                 || (theSubnameFlag && myChildNode.name.indexOf(theName) != -1)) 
            {
                setMaterial(myChildNode.name);
                Public.activeIndex = i;
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
            Logger.error("Could not find target material: " + myTargetMaterialName + " for material switch " +
                         Public.switchName);
        }
        
        return myTargetMaterial;
    }

    function matchTextureCoordCount(theId, theCount) {
        var myShapesNode = window.scene.shapes;
        for (var i = 0; i < myShapesNode.childNodesLength("shape"); ++i) {
            var myPrimitives = myShapesNode.childNode("shape", i).childNode("primitives");
            for (var j = 0; j < myPrimitives.childNodesLength("elements"); ++j) {
                var myElements = myPrimitives.childNode("elements", j)
                if (myElements.material = theId) {
                    var myTextureCoordCount = 0;
                    for (var k = 0; k < myElements.childNodesLength("indices"); ++k) {
                        var myIndicesNode = myElements.childNode("indices", k);
                        if (String(myIndicesNode.role).indexOf("texcoord") != -1) {
                            myTextureCoordCount++;
                        }
                    }
                    if (myTextureCoordCount < theCount) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    
    function replaceMaterialIds(theOldId, theNewId) {
        var myNodes = window.scene.shapes.getNodesByAttribute("elements", "material", theOldId);        
        for (var i = 0; i < myNodes.length; ++i) {
            myNodes[i].material = theNewId;
        }
    }
    
    function findOcclusionMap(theMaterial) {
        var myTexturesNode = theMaterial.childNode("textures");
        for (var i = 0; i < myTexturesNode.childNodesLength(); ++i) {
            var myTexture = myTexturesNode.childNode(i);
            var myImage = theMaterial.getElementById(myTexture.image);
            if (myImage.src.search(/shadowmap/i) != -1) {
                return myTexture;
            }
        }
        
        return null;
    }
        
    function prependFeature(theFeatureList, theValue) {
        var myRegExp = /\[(\d{1,3})\[(.*)\]\]/;
        var myResults = myRegExp.exec(theFeatureList);
        if (myResults && myResults.length) {
            var myFeatures = myResults[2].split(",");
            myFeatures.unshift(theValue);
            return "[" + myResults[1] + "[" + myFeatures.toString() + "]]";
        }
    }
    
    function getNumTexcoordCount(theFeatureList) {
        var myCount = 0;
        var myRegExp = /\[\d{1,3}\[(.*)\]\]/;
        var myResults = myRegExp.exec(theFeatureList);
        if (myResults && myResults.length) {
            var myFeatures = myResults[1].split(",");
            for (var i = 0; i < myFeatures.length; ++i) {
                if (myFeatures[i] == "uv_map") {
                    myCount++;
                }
            }
            return myCount;
        }
    }
    
    function setMaterial(theMaterialCode) {
        //print("switchMaterials(", theMaterialCode, " ", Public.node);
        // First Step: Search inside the switch for the material with the materialcode
        
        var mySwitchMat = null;
        for (var i = 0; i < Public.childCount; ++i) {
            var myChild = Public.node.childNode( i );
            if (myChild.name.indexOf(theMaterialCode) != -1 &&
                myChild.name.indexOf("dark") == -1) {
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
                    return false;
                }
                var myElement = getDescendantByTagName(myShape, "elements", true);
                mySwitchMat = myElement.getElementById(myElement.material);
            }
        }

        if (!mySwitchMat) {
            Logger.warning("Could not find material: " + theMaterialCode);
            return false;
        }

        //print("switch mat: " + mySwitchMat);

        var myOldTargetMat = findTargetMaterial();
        // NOTE: the old way [jb]
 //       myOldTargetMat.replaceChild(mySwitchMat.childNode("properties").cloneNode(true),
 //                                    myOldTargetMat.childNode("properties"));

        // NOTE: the new way
        //save the occlusion map (if there is one) before replacing the textures
        var myOcclusionMap = findOcclusionMap(myOldTargetMat);

        //print("OLD material *********************************************** : \n" + myOldTargetMat);
        //print("merging with SWITCH material *********************************************** : \n" + mySwitchMat);

        // Third step: Setup target material
        var myNewTargetMat = myOldTargetMat.cloneNode(true);
        myNewTargetMat.id = createUniqueId();

        // replace the "properties" node and remove sampler properties
        myNewTargetMat.replaceChild(mySwitchMat.childNode("properties").cloneNode(true),
                                    myNewTargetMat.childNode("properties"));
        var myProperties = myNewTargetMat.childNode("properties");
        var i = 0;
        while ( i < myProperties.childNodesLength() ) {        
            if (myProperties.childNode(i).nodeName.indexOf("sampler") == 0) {
                myProperties.removeChild(myProperties.childNode(i));
            } else {
                ++i;
            }
        }
        
        // replace the "textures" node
        myNewTargetMat.replaceChild(mySwitchMat.childNode("textures").cloneNode(true),
                                    myNewTargetMat.childNode("textures"));

        // replace the "requires" node
        myNewTargetMat.replaceChild(mySwitchMat.childNode("requires").cloneNode(true),
                                    myNewTargetMat.childNode("requires"));

        // append the saved occlusion map as "textures" node's first child
        if (myOcclusionMap) {
            var myOtherOcclusionMap = findOcclusionMap(mySwitchMat);
            if (myOtherOcclusionMap) {
                // there is already an occlusion map in the new material, replace it
                myNewTargetMat.childNode("textures").replaceChild(myOcclusionMap.cloneNode(),
                                                                     myOtherOcclusionMap);
            } else {
                //add occlusion map and adjust the requirements
                myNewTargetMat.childNode("textures").insertBefore(myOcclusionMap,
                    myNewTargetMat.childNode("textures").firstChild);
                //myNewTargetMat.requires.textures = prependFeature(myOldTargetMat.requires.textures, "paint");
                //myNewTargetMat.requires.texcoord = prependFeature(myOldTargetMat.requires.texcoord, "uv_map");
            }
        }

        //print(myNewTargetMat);

        // FIXME BUG 478: we convert to string & parse again to strip off any facades
        window.scene.materials.appendChild(new Node(myNewTargetMat.toString()).firstChild);
        //myNewTargetMat.requires.textures = "[10[paint,emissive]]";
        //myNewTargetMat.requires.texcoord = "[10[reflective,reflective]]";
        replaceMaterialIds(myOldTargetMat.id, myNewTargetMat.id);
        window.scene.materials.removeChild(myOldTargetMat);
        //print("results in NEW material *********************************************** : \n" + myNewTargetMat);


        return true;
    }

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
        
        var myTexturesLength = Public.node.childNode("textures").childNodesLength();
        var myNewTexturesLength = myMaterial.childNode("textures").childNodesLength();

        // calculate textures offset
        var myTextureOffset = myTexturesLength - myNewTexturesLength;
        if (myTextureOffset < 0) {
            Logger.error("Textureswitching error. The Switchnode has " +myNewTexturesLength+
                         " textures while the original Material has " + myTexturesLength);
            return;
        }

        // set new images
        for (var i=0; i<myMaterial.childNode("textures").childNodesLength(); ++i) {
            var myImage = myMaterial.childNode("textures").childNode(i).image;
            Public.node.childNode("textures").childNode(i+myTextureOffset).image = myImage;  
        }
    }

    Public.setActiveChildByName = function(theName, theSubnameFlag) {
        if (!_mySwitches) {
            return;
        }
        
        if (theSubnameFlag) {
            Public.setActiveChildBySubName(theName);
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
