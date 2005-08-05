//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: OverlayManager.js,v $
//   $Author: martin $
//   $Revision: 1.43 $
//   $Date: 2005/04/29 13:37:56 $
//
//
//=============================================================================

use("ImageManager.js");
use("Overlay.js");

function OverlayManager(theScene, theViewport) {
    var Public = this;

    Public.scene     = theScene;
    Public.overlays  = getDescendantByTagName(theViewport, "overlays", false);
    Public.materials = getDescendantByTagName(theScene.dom, "materials", false);
    Public.images    = getDescendantByTagName(theScene.dom, "images", false);

    var _myCounter   = 0;

    // DEPRECATED: Use "new ImageOverlay()" instead.
    Public.createImageOverlay = function(theSource, thePosition, theName) {
        print("### WARNING: Public.createImageOverlay() is deprecated. Use 'new ImageOverlay()' instead.");
        return new ImageOverlay(this, theSource, thePosition);
    }

    // DEPRICATED
    Public.createMovie = function(theSource, thePosition, theParent, theAudioFlag) {
        print("### WARNING: Public.createImageOverlay() is deprecated. Use 'new MovieOverlay()' instead.");
        return new MovieOverlay(this, theSource, thePosition, theParent, theAudioFlag);
    }

    /// DEPRECATED
    Public.create = function(theName, theImageId, theOverwriteFlag, theMaterialId, theParent) {
        print("### WARNING: OverlayManager.create is deprecated use 'new ImageOverlay' instead");
        return new ImageOverlay(this, this.images.getElementById(theImageId), [0,0], theParent);
    }

    /// Creates an overlay group
    // @param theName   String  Name of the group overlay
    // @param theParent Node    Parent node the overlay group (optional, default is top-level)
    Public.createGroup = function(theName, theParent) {
        return commonCreate(theName, "", false, theParent);
    }

    Public.createMultitextured = function(theName, theImageIds, theOverwriteFlag, theParent) {
        var myMaterialString =
            '<material name="' + theName + 'M" id="'+ createUniqueId() + '" transparent="1" >\n' +
            '    <properties>\n' +
            '        <vector4f name="surfacecolor">[1.0,1.0,1.0,1]</vector4f>\n' +
            '    </properties>\n' +
            '    <textures>\n';

        for (var i = 0; i < theImageIds.length; ++i) {
            var myApplymode = "decal";
            if (i == 0) {
                myApplymode = "modulate";
            }
            myMaterialString +=
            '        <texture image="' + theImageIds[i] + '" applymode="' + myApplymode + '" wrapmode="repeat"/>\n';
        }

        myMaterialString +=
            '    </textures>\n' +
            '    <requires>\n' +
            createTextureFeatureString(theImageIds.length)+
            '        <feature class="lighting" values="[10[unlit]]"/>\n' +
            '    </requires>\n' +
            '</material>';
        var myMaterialDoc = new Node(myMaterialString);
        var myMaterialNode = insertMaterial(myMaterialDoc.childNodes[0]);

        return commonCreate(theName, myMaterialNode.id, theOverwriteFlag, theParent);
    }

    Public.createOverlayMaterial = function(theName, theImageId) {
        var myMaterialId = createUniqueId();
        var myMaterialString =
            '<material name="' + theName + 'M" id="' + myMaterialId + '" transparent="1" >\n' +
            '    <properties>\n' +
            '        <vector4f name="surfacecolor">[1.0,1.0,1.0,1]</vector4f>\n' +
            '    </properties>\n' +
            '    <textures>\n' +
            '        <texture image="' + theImageId + '" applymode="modulate" wrapmode="repeat"/>\n' +
            '    </textures>\n' +
            '    <requires>\n' +
            '        <feature class="textures" values="[100[paint]]"/>\n' +
            '        <feature class="texcoord" values="[100[uv_map]]"/>\n' +
            '        <feature class="lighting" values="[10[unlit]]"/>\n' +
            '    </requires>\n' +
            '</material>';

        var myMaterialDoc = new Node(myMaterialString);
        var myMaterialNode = myMaterialDoc.childNodes[0];
        return insertMaterial(myMaterialNode);
    }

    Public.get = function(theName) {
        var myOverlay = getDescendantByName(Public.overlays, theName, true);
        var myOverlayMaterial = getDescendantByName(Public.materials, theName + "M", false);

        if (myOverlay && !myOverlayMaterial) {
            throw new Exception("Overlay " + theName + " does not have a corresponding material node " + theName + "M.",
                "OverlayManager::get()");
        }

        return myOverlay;
    }

    Public.setImages = function(theOverlayNode, theImageIds) {
        var myMaterialId = theOverlayNode.material;
        var myOverlayMaterial = Public.materials.getElementById(myMaterialId);
        var myTextures = getDescendantByTagName(myOverlayMaterial, "textures", false);
        if (theImageIds.length != myTextures.childNodes.length) {
            var myRequiresNode = getDescendantByTagName(myOverlayMaterial, "requires", false);
            var myTextureFeaturesNode = getDescendantByAttribute(myRequiresNode, "class", "textures");
            var myTempDoc = new Node(createTextureFeatureString(theImageIds.length));
            myRequiresNode.replaceChild(myTempDoc.childNodes[0], myTextureFeaturesNode);
        }
        for (var i = 0; i < theImageIds.length; ++i) {
            var myTexture = {};
            if ( i == myTextures.childNodes.length) {
                myTexture = Node.createElement("texture");
                myTexture.applymode = "decal";
                myTexture.wrapmode = "repeat";
                myTextures.appendChild(myTexture);
            } else {
                myTexture = myTextures.childNodes[i];
            }
            myTexture.image = theImageIds[i];
        }
    }

    Public.setBlendMode = function(theOverlayName, theMode) {
        var myBlendProperty = '<vectorofstring name="blendfunction">' + theMode + '</vectorofstring>';
        var myPropertyDoc = new Node(myBlendProperty);
        Public.getMaterial(theOverlayName).childNode("properties", 0).appendChild(myPropertyDoc.firstChild);
    }

    // DEPRICATED: USE "myOverlay.material" INSTEAD
    //Public.getMaterial = function(theName) {

    // DEPRICATED: USE "myOverlay.color" INSTEAD
    //Public.setBackgroundColor = function(theOverlayName, theBackgroundColor) {

    // DEPRICATED: USE "myOverlay.removeFromScene()" instead
    //Public.remove = function(theName) {

    // DEPRICATED: USE "myOverlay.texture.wrapmode = theMode" INSTEAD
    //Public.setWrapMode = function(theOverlayName, theMode) {

    // DEPRICATED: USE "myOverlay.texture.applymode = theApplyMode" INSTEAD
    //Public.setApplyMode = function(theOverlayName, theApplyMode) {

    // DEPRICATED: USE "myOverlay.texture.image = theImageId" INSTEAD
    //Public.setTexture = function(theName, theImageId) {

    // DEPRICATED: USE "myOverlay.moveToTop()" INSTEAD
    //Public.moveToTop = function(theOverlayNode) {

    //////////////////////////////////////////////////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////////////////////////////////////////////////

    function createTextureFeatureString(theTextureCount) {
        var myPaintNodeString = '<feature class="textures" values="[100[';
        var myTexCoordNodeString = '<feature class="texcoord" values="[100[';

        for (var i = 0; i < theTextureCount; ++i) {
            myPaintNodeString += "paint";
            myTexCoordNodeString += "texcoord";
            if (i != theTextureCount - 1) {
                myPaintNodeString += ",";
                myTexCoordNodeString += ",";
            }
        }
        myPaintNodeString += ']]"/>';
        myTexCoordNodeString += ']]"/>';   
             
        return myPaintNodeString + '\n' + myTexCoordNodeString;
    }

    function commonCreate(theName, theMaterialId, theOverwriteFlag, theParent) {
        var myParent = Public.overlays;
        if (theParent) {
            myParent = theParent;
        }
        var myOldOverlay = getDescendantByName(myParent, theName, false);

        if (!theOverwriteFlag && myOldOverlay) {
            throw new Exception("Overlay " + theName + " already in use", "OverlayManager::create()");
        }

        var myOverlayString = '<overlay name="' + theName + '" material="' + theMaterialId + '"/>';
        var myOverlayNode = new Node(myOverlayString).firstChild;

        if (myOldOverlay) {
            myParent.replaceChild(myOverlayNode, myOldOverlay);
        } else {
            myOverlayNode = myParent.appendChild(myOverlayNode);
        }

        return myOverlayNode;
    }

    function insertMaterial(theMaterialNode) {
        var myOldOverlayMaterial = getDescendantByName(Public.materials, theMaterialNode.name,  false);
        if (myOldOverlayMaterial) {
            Public.materials.replaceChild(theMaterialNode, myOldOverlayMaterial);
        } else {
            Public.materials.appendChild(theMaterialNode);
        }

        return theMaterialNode;
    }
}
