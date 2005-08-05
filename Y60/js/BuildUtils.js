//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: BuildUtils.js,v $
//   $Author: danielk $
//   $Revision: 1.9 $
//   $Date: 2005/04/01 17:16:03 $
//
//=============================================================================

use("Y60JSSL.js");

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
function buildMaterialNode(theName,
                           theDiffuseColor, theAmbientColor,
                           theTextureFeatures, theLightingFeatures,
                           theWrapmode)
{
    var myWrapMode = "repeat";
    if (theWrapmode != undefined) {
        myWrapMode = theWrapmode;
    }
    var myMaterial = "";
    myMaterial += "<material name=\"" + theName + "\" id=\"m" + theName + "\" transparent=\"0\">";
    myMaterial += "<properties>";
    if (theDiffuseColor != undefined && theDiffuseColor != null) {
        myMaterial += "<vector4f name=\"diffuse\">" + theDiffuseColor + "</vector4f>";
    }
    if (theAmbientColor != undefined && theAmbientColor != null) {
        myMaterial += "<vector4f name=\"ambient\">" + theAmbientColor + "</vector4f>";
    }
    myMaterial += "</properties>\n";

    // textures
    if (theTextureFeatures != undefined && theTextureFeatures != null) {
        myMaterial += "<textures>";
        myMaterial += "<texture image=\"i" + theName + "\" wrapmode=\"" + theWrapmode + "\" applymode=\"modulate\"/>";
        myMaterial += "</textures>\n";
    }
    else {
        myMaterial += "<textures/>\n";
    }

    // requires / features
    myMaterial += "<requires>";
    if (theLightingFeatures != undefined && theLightingFeatures != null) {
        myMaterial += "<feature class=\"lighting\" values=\"" + theLightingFeatures + "\"/>";
    }
    if (theTextureFeatures != undefined && theTextureFeatures != null) {
        myMaterial += "<feature class=\"textures\" values=\"" + theTextureFeatures + "\"/>";
    }
    myMaterial += "</requires>\n";
    myMaterial += "</material>";

    var myMaterialDoc = new Node(myMaterial);
    return myMaterialDoc.childNodes[0];
}

function buildUnlitTextureMaterialNode(theName, theImageId, theWrapMode) {
    if (theImageId == undefined) {
        theImageId = "i" + theName;
    }
    var myWrapMode = "repeat";
    if (theWrapMode != undefined) {
        myWrapMode = theWrapMode;
    }

    var myMaterial = '<material name="' + theName + '" id="m' + theName + '" transparent="0">\n';
    myMaterial += '<properties/>\n';

    // textures
    myMaterial += '<textures>\n';
    myMaterial += '    <texture image="' + theImageId + '" wrapmode="' + myWrapMode + '" applymode="modulate"/>\n';
    myMaterial += '</textures>\n';

    // requires / features
    myMaterial += '<requires>\n';
    myMaterial += '    <feature class="lighting" values="[10[unlit]]"/>\n';
    myMaterial += '    <feature class="textures" values="[100[paint]]"/>\n';
    myMaterial += '    <feature class="texcoord" values="[100[uv_map]]"/>\n';
    myMaterial += '</requires>\n';
    myMaterial += '</material>\n';

    var myMaterialDoc = new Node(myMaterial);
    return myMaterialDoc.childNodes[0];
}

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

function addMaterialRequirement(theMaterialNode, theRequirementClass, theRequirementValue) {
    var myRequirementString = "<feature class='" + theRequirementClass + "' values=\"" + theRequirementValue + "\"/>";
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
    myImage += "<image name=\"" + theName + "\" id=\"i" + theName + "\" src=\"" + theFilename + "\" pixelformat=\"" + myPixelFormat + "\" type=\"" + myType + "\" mipmap=\"" + theMipMapFlag + "\"/>";

    var myImageDoc = new Node(myImage);
    return myImageDoc.childNodes[0];
}

// build a <movie> node
function buildMovieNode(theName, theFilename)
{
    var myMovie = '<movie name="' + theName + '" id="i' + theName + '" src="' + theFilename + '"/>';

    var myMovieDoc = new Node(myMovie);
    return myMovieDoc.childNodes[0];
}
