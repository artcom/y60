//=============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Y60JSSL.js");
use("Playlist.js");

// build a <transform> node
function buildGroupNode(theName) {
    var myTransform = '<transform visible="true" name="' + theName + '"/>';
    var myTransformDoc = new Node(myTransform);
    return myTransformDoc.childNodes[0];
}

// build a <lod> node
function buildLodNode(theName, theVisibleFlag, theRanges) {
    /*var myLodArray = new Array();
    myLodArray.push(theRanges);
    print (myLodArray);*/
    var myLod = "<lod visible=\"" + theVisibleFlag + "\" name=\"" + theName + "\" ranges=\"[" + theRanges +"]\"/>";
    var myLodDoc = new Node(myLod);
    return myLodDoc.childNodes[0];
}

// build a <shape> node from a given string
function buildShapeNode(theShapeString) {
    var myShapeDoc = new Node(theShapeString);
    return myShapeDoc.childNodes[0];
}

// build a <body> node from a given shape name
function buildBodyNode(theBodyName, theShapeId) {
    var myId = createUniqueId();
    var myBody = "<body visible=\"1\" name=\"" + theBodyName + "\" shape=\"" + theShapeId + "\" id=\"" + myId + "\"/>";
    var myBodyDoc = new Node(myBody);
    return myBodyDoc.firstChild;
}

// build a <transform> node
function buildTransformNode(theTransformId) {
    var myTransform = "<transform visible=\"1\" name=\"" + theTransformId + "\" id=\"" + theTransformId + "\"/>";
    var myTransformDoc = new Node(myTransform);
    return myTransformDoc.firstChild;
}

function buildBillboardNode(theBodyName, theShapeId, theType) {
    var myBody = "<body visible=\"1\" name=\"" + theBodyName + "\" shape=\"" + theShapeId + "\" billboard=\"" + theType + "\"/>";
    var myBodyDoc = new Node(myBody);
    return myBodyDoc.firstChild;
}

// build a <material> node
function buildMaterialNode(theMaterialName,
                           theDiffuseColor, theAmbientColor,
                           theTextureFeatures, theLightingFeatures,)
{
    var myMaterialNode = Node.createElement('material');
    myMaterialNode.id = createUniqueId(); //"m" + theMaterialName;
    myMaterialNode.name = theMaterialName;

    var myMaterialParent = getDescendantByTagName(window.scene.dom, "materials", true);
    myMaterialParent.appendChild(myMaterialNode);

    if (theDiffuseColor == undefined) {
        theDiffuseColor = new Vector4f(0.6,0.6,0.6,1);
    }
    myMaterialNode.properties.diffuse = theDiffuseColor;

    if (theAmbientColor == undefined) {
        theAmbientColor = new Vector4f(1,1,1,1);
    }
    myMaterialNode.properties.ambient = theAmbientColor;

    if (theLightingFeatures != undefined && theLightingFeatures != null) {
        myMaterialNode.requires.lighting = theLightingFeatures;
    }
    if (theTextureFeatures != undefined && theTextureFeatures != null) {
        var myTextureFeatures = new Node('<feature name="textures">[10[]]</feature>\n').firstChild;
        myMaterialNode.requires.appendChild(myTextureFeatures);
        myMaterialNode.requires.textures = theTextureFeatures;

        var myTexturesString =
            '<textures>\n' +
            '    <texture image="i' + theName + '" applymode="modulate"/>\n' +
            '</textures>';
        var myTexturesDoc = new Node(myTexturesString);
        var myTexturesNode = myTexturesDoc.firstChild;
        myMaterialNode.appendChild(myTexturesNode);
    }

    return myMaterialNode;
}

function buildUnlitTextureMaterialNode(theName, theImageId) {
    var myImageArrayFlag = theImageId instanceof Array;
    var myImageIds = [];
    if (!myImageArrayFlag) {
        myImageIds.push(theImageId);
    } else {
        myImageIds = theImageId ;
    }


    var myMaterialNode = Node.createElement('material');
    myMaterialNode.id = createUniqueId(); //"m" + theName;
    myMaterialNode.name = theName;
    var myMaterialParent = getDescendantByTagName(window.scene.dom, "materials", true);
    myMaterialParent.appendChild(myMaterialNode);
    var myTextureFeatures = new Node('<feature name="textures">[10[]]</feature>\n').firstChild;
    myMaterialNode.requires.appendChild(myTextureFeatures);
    var myTexCoordFeatures = new Node('<feature name="texcoord">[10[]]</feature>\n').firstChild;
    myMaterialNode.requires.appendChild(myTexCoordFeatures);
    myMaterialNode.requires.lighting = "[10[unlit]]";

    var myTexturesReqString = "[100["
    var myTexCoordReqString = "[100["
    var myTexturesString = '<textures>\n';
    for (var myImageIndex = 0; myImageIndex < myImageIds.length; myImageIndex++) {
        var myImageId = myImageIds[myImageIndex];
        if (myImageId == undefined) {
            myImageId = createUniqueId(); //"i" + theName;
        }

        // add textures
        myTexturesString += '    <texture image="' + myImageId  + '" applymode="modulate"/>\n';

        // add requirements
        if (myImageIndex > 0) {
            myTexturesReqString += ",";
            myTexCoordReqString += ",";
        }
        myTexturesReqString += "paint";
        myTexCoordReqString += "uv_map";
    }
    myTexturesString += '</textures>';
    var myTexturesDoc = new Node(myTexturesString);
    var myTexturesNode = myTexturesDoc.firstChild;
    myMaterialNode.appendChild(myTexturesNode);

    myTexturesReqString += "]]";
    myMaterialNode.requires.textures = myTexturesReqString;

    myTexCoordReqString += "]]";
    myMaterialNode.requires.texcoord = myTexCoordReqString;
    myMaterialNode.properties.surfacecolor = new Vector4f(1,1,1,1);
    return myMaterialNode;

}
/*
function addMaterialProperty(theMaterialNode, thePropertyType, theProperty, theValue) {
    var myPropertyString = "<" + thePropertyType +" name='" + theProperty + "'>" + theValue + "</" + thePropertyType + ">";
    var myPropertyDoc = new Node(myPropertyString);
    var myPropertyNode = myPropertyDoc.childNodes[0];

    var myPropertiesNode = getDescendantByTagName(theMaterialNode, "properties", false);
    if (!myPropertiesNode) {
        var myPropertiesString = "<properties/>";
        var myPropertiesDoc = new Node(myPropertiesString);
        myPropertiesNode = myPropertiesDoc.childNodes[0];
        theMaterialNode.append(myPropertiesNode);
    }
    myPropertiesNode.appendChild(myPropertyNode);
}
*/

function addMaterialRequirement(theMaterialNode, theRequirementClass, theRequirementValue) {
    var myRequirementString = "<feature name='" + theRequirementClass + "'>" + theRequirementValue + "</feature>";
    var myRequirementDoc = new Node(myRequirementString);
    var myRequirementNode = myRequirementDoc.childNodes[0];
    var myRequiresDocNode = getDescendantByTagName(theMaterialNode, "requires", false);
    if (!myRequiresDocNode) {
        var myRequiresString = "<requires/>";
        var myRequiresDoc = new Node(myRequiresString);
        myRequiressNode = myRequiresDoc.childNodes[0];
        theMaterialNode.append(myRequiressNode);
    }
    myRequiresDocNode.appendChild(myRequirementNode);
}

// build an <image> node
function buildImageNode(theName, theFilename, theMipMapFlag)
{
    var myPixelFormat = "BGR";
    var myType = "single";

    var myImage = "";
    myImage += "<image name=\"" + theName + "\" id=\"i" + theName + "\" src=\"" + theFilename + "\" type=\"" + myType + "\" mipmap=\"" + theMipMapFlag + "\"/>";

    var myImageDoc = new Node(myImage);
    return myImageDoc.childNodes[0];
}

// build a <movie> node
function buildMovieNode(theName, theFilename)
{
    var myDecoderHint = new Playlist().getVideoDecoderHintFromURL(theFilename, false);
    var myMovie = '<movie name="' + theName + '" id="i' + theName + '" src="' + theFilename + '" decoderhint="' + myDecoderHint + '"/>';

    var myMovieDoc = new Node(myMovie);
    return myMovieDoc.childNodes[0];
}
