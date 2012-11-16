/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

function matchNodeName(theChildNode, theName, theSubStringFlag, theIgnoreCaseFlag) {
    var searchName = theIgnoreCaseFlag ? theName.toLowerCase() : theName;
    var nodeName = theIgnoreCaseFlag ? theChildNode.name.toLowerCase() : theChildNode.name;
    Logger.trace("matching "+nodeName+" with "+searchName);
    if (theSubStringFlag) {
        return (nodeName.indexOf(searchName) != -1);
    } else {
        return (searchName == nodeName);
    }
}


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
        Public.activeChild.visible = false;
        Public.activeIndex = theIndex;
        Public.activeChild.visible = true;
    }

    Public.setActiveChildByName = function(theName, theSubnameFlag, theIgnoreCaseFlag) {
        Public.activeChild.visible = false;
        for (var i = 0; i < Public.childCount; ++i) {
            if (matchNodeName(Public.node.childNode(i), theName, theSubnameFlag, theIgnoreCaseFlag))
            {
                Public.activeIndex = i;
                Logger.debug("setting "+Public.node.name+" to "+theName);
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

    Public.setActiveChildByName = function(theName, theSubnameFlag, theIgnoreCaseFlag) {
        for (var i=0; i<Public.childCount; ++i) {
            var myChildNode = Public.node.childNode(i);
            if (matchNodeName(myChildNode, theName, theSubnameFlag, theIgnoreCaseFlag))
            {
                Logger.debug("setting "+Public.node.name+" to "+theName);
                setMaterial(myChildNode.name);
                Public.activeIndex = i;
                break;
            }
        }
    }

    function findTargetMaterial() {
        var myTargetMaterial = null;

        // searching for
        var myRegExp = /^(.*switch_(.*))$/;
        var myResult = myRegExp.exec(Public.switchName);

        if (myResult.length == 3) {
            // e.g. mswitch_dekorleiste
            myTargetMaterial = window.scene.materials.find("material[@name = '" + myResult[1] + "']");
            if (!myTargetMaterial) {
                // e.g. mswitch_dekorleisteM
                myTargetMaterial = window.scene.materials.find("material[@name = '" + myResult[1] + "M']");
            }
            if (!myTargetMaterial) {
                // e.g. dekorleiste
                myTargetMaterial = window.scene.materials.find("material[@name = '" + myResult[2] + "']");
            }
            if (!myTargetMaterial) {
                // e.g. dekorleisteM
                myTargetMaterial = window.scene.materials.find("material[@name = '" + myResult[2] + "M']");
            }
        }

        if (!myTargetMaterial) {
            Logger.error("Could not find target material for switch " + Public.switchName);
        }

        return myTargetMaterial;
    }

    function matchTextureCoordCount(theElementsNode, theCount) {
        var myTextureCoordCount = 0;
        for (var i = 0; i < theElementsNode.childNodesLength("indices"); ++i) {
            var myIndicesNode = theElementsNode.childNode("indices", i);
            if (String(myIndicesNode.role).indexOf("texcoord") != -1) {
                myTextureCoordCount++;
            }
        }
        if (myTextureCoordCount < theCount) {
            return false;
        }
        return true;
    }

    function replaceMaterialIds(theOldId, theNewId, theRequiredCount) {
        var myNodes = window.scene.shapes.getNodesByAttribute("elements", "material", theOldId);
        var skippedAnyNodes = false;
        for (var i = 0; i < myNodes.length; ++i) {
            var myUVCountOkFlag = (theRequiredCount === undefined);

            if (!myUVCountOkFlag && matchTextureCoordCount(myNodes[i], theRequiredCount)) {
                myUVCountOkFlag = true;
            }

            if (myUVCountOkFlag) {
                myNodes[i].material = theNewId;
            } else {
                skippedAnyNodes = true;
                Logger.warning("Too few texcoord sets in shape '" + parentNode.parentNode.name + "'.");
            }
        }
        return skippedAnyNodes == false;
    }

    function findOcclusionMap(theMaterial) {
        var mySampler2d = theMaterial.getNodesByAttribute("", "name", "occlusionTex");
        if (mySampler2d.length == 1) {
            var myIndex = mySampler2d[0].childNode("#text").nodeValue;
            var myTextureUnitsNode = theMaterial.childNode("textureunits");
            if (myIndex !== null
                && myIndex < myTextureUnitsNode.childNodesLength("textureunit"))
            {
                return myTexturesNode.childNode("textureunit", myIndex);
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

    function getTexcoordCount(theMaterialNode) {

        var myRequires = theMaterialNode.childNode('requires');
        var myFeatureList = myRequires.find("*[@name = 'texcoord']").childNode("#text").nodeValue;
        var myCount = 0;
        var myRegExp = /\[\d{1,3}\[(.*)\]\]/;
        var myResults = myRegExp.exec(myFeatureList);
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
            if (myChild.name.indexOf(theMaterialCode) != -1) {
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
                var myElement = myShape.find(".//elements");

                mySwitchMat = myElement.getElementById(myElement.material);
            }
        }

        if (!mySwitchMat) {
            Logger.warning("Could not find material: " + theMaterialCode);
            return false;
        }

        //print("switch mat: " + mySwitchMat);

        var myOldTargetMat = findTargetMaterial();
        if (!myOldTargetMat) {
            return false;
        }

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
        myNewTargetMat.replaceChild(mySwitchMat.childNode("textureunits").cloneNode(true),
                                    myNewTargetMat.childNode("textureunits"));

        // replace the "requires" node
        myNewTargetMat.replaceChild(mySwitchMat.childNode("requires").cloneNode(true),
                                    myNewTargetMat.childNode("requires"));

        // append the saved occlusion map as "textures" node's first child
        if (myOcclusionMap) {
            var myOtherOcclusionMap = findOcclusionMap(mySwitchMat);
            if (myOtherOcclusionMap) {
                // there is already an occlusion map in the new material, replace it
                myNewTargetMat.childNode("textureunits").replaceChild(myOcclusionMap.cloneNode(),
                                                                     myOtherOcclusionMap);
            } else {
                //add occlusion map and adjust the requirements
                myNewTargetMat.childNode("textureunits").insertBefore(myOcclusionMap,
                        myNewTargetMat.childNode("textureunits").firstChild);
                var myRequires = myNewTargetMat.childNode('requires');
                myRequires.find("*[@name = 'textures']").childNode("#text").nodeValue =
                    prependFeature(mySwitchMat.requires.textures, "occlusion");
                myRequires.find("*[@name = 'texcoord']").childNode("#text").nodeValue =
                        prependFeature(mySwitchMat.requires.texcoord, "uv_map");
            }
            //quickfix for missing occlusionmap after more then one switch, cleanup is on it's way [jb]
            var my2dSampler = myNewTargetMat.childNode("properties").find(".//*[@name = 'occlusionTex']");
            if (!my2dSampler) {
                var myNode = new Node("<sampler2d name='occlusionTex'>0</sampler2d>");
                myNewTargetMat.childNode("properties").appendChild(myNode.firstChild);
            }
        }

        //print("results in NEW material *********************************************** : \n" + myNewTargetMat);
        // FIXME BUG 478: we convert to string & parse again to strip off any facades
        window.scene.materials.appendChild(new Node(myNewTargetMat.toString()).firstChild);
        if (replaceMaterialIds(myOldTargetMat.id, myNewTargetMat.id, getTexcoordCount(myNewTargetMat))) {
            window.scene.materials.removeChild(myOldTargetMat);
        }
        //window.scene.save("test.x60", false);
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
        var myName = new String(Public.switchName);

        if ( ! myName.match(/^tswitch_.*/)) {
            Logger.warning("TSwitch node '" + Public.switchName + " doesn't obey the " +
                "naming conventions. Name should start with 'tswitch_'.");
        }

        // find the transform node with the texture references first
        var myReferenceNode = window.scene.world.find(".//*[@name = 'textureswitches']");
        if (!myReferenceNode) {
            Logger.error("Could not find reference node for textureswitches");
            return;
        }

        for (var i=0; i<myReferenceNode.childNodesLength(); ++i) {
            var myNode = myReferenceNode.childNode(i);
            if ((myNode.name.indexOf(myName) != -1) ||
                ((myNode.name+"M").indexOf(myName) != -1))
            {
                _mySwitches = myNode;
            }
        }

        if (!_mySwitches) {
            Logger.error("Could not find reference node for textureswitch: " + Public.switchName);
            return;
        }
    }

    function switchTexture(theNode) {
        _myActiveChild = theNode;

        // get texture id for the new textureunit
        var myShape = theNode.getElementById(theNode.shape);
        var myMaterialId = myShape.childNode("primitives").childNode("elements").material;

        var myMaterial = theNode.getElementById(myMaterialId);
        var myTexture = myMaterial.childNode("textureunits").firstChild.texture;

        if (theNode.parentNode.name.search(/tswitch_occlusion/i) != -1) {
            //Occlusion map switching:
            // get texture id for the old textureunit
            var myTextureOld = Public.node.childNode("textureunits").firstChild.texture;
            switchOcclusionMap(myTextureOld, myTexture, myMaterial);
        } else {
            var myTexturesLength = Public.node.childNode("textureunits").childNodesLength();
            var myNewTexturesLength = myMaterial.childNode("textureunits").childNodesLength();

            // calculate textures offset
            var myTextureOffset = myTexturesLength - myNewTexturesLength;
            if (myTextureOffset < 0) {
                Logger.error("TSwitchnode material '"+Public.switchName+"' has " +myNewTexturesLength+
                             " textures while the original material '"+myMaterial.name+"' has " + myTexturesLength);
                return;
            }

            // set new textures
            for (var i=0; i<myMaterial.childNode("textureunits").childNodesLength(); ++i) {
                var myTexture = myMaterial.childNode("textureunits").childNode(i).texture;
                Public.node.childNode("textureunits").childNode(i+myTextureOffset).texture = myTexture;
            }
        }
    }

    function switchOcclusionMap(theOldId, theNewId, theMaterialNode) {
        //change textureunits texture ids
        var myNodes =
            window.scene.materials.getNodesByAttribute("textureunit", "texture", theOldId);
        for (var i = 0; i < myNodes.length; ++i) {
            var myNode = myNodes[i];

            var myFeatureList = myNode.parentNode.parentNode.find(".//*[@name = 'textures']").childNode("#text").nodeValue;
            Logger.info(myNode.parentNode.parentNode.name+": "+myFeatureList);
            /*TODO: activate after shader envlack is fixed
            if (String(myFeatureList).search(/occlusion/i) == -1) {
                continue;
            }*/

            //do not switch texture id for the reference material node!
            if (!myNode.parentNode.parentNode.isSameNode(theMaterialNode)) {
                myNode.texture = theNewId;
            } else {
                Logger.debug("Skipping reference material.");
            }
        }
    }

    Public.setActiveChildByName = function(theName, theSubnameFlag, theIgnoreCaseFlag) {
        if (!_mySwitches) {
            return;
        }

        for (var i=0; i<_mySwitches.childNodesLength(); ++i) {
            var myNode = _mySwitches.childNode(i);
            if (matchNodeName(myNode, theName, theSubnameFlag, theIgnoreCaseFlag)) {
                Logger.debug("setting "+Public.node.name+" to "+theName);
                switchTexture(myNode);
                return;
            }
        }

        Logger.error("Failed to activate child '" + theName + "' for switchnode '" + Public.switchName + "'.");
    }

    Public.setActiveChildBySubName = function(theSubName, theIgnoreCaseFlag) {
        Public.setActiveChildByName(theSubName, true, theIgnoreCaseFlag);
    }

    var _mySwitches = null;
    var _myActiveChild = null;

    setup();
}
