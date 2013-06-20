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

use("Y60JSSL.js");
use("ShapeBuilder.js");
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
                           theTextureFeatures, theLightingFeatures)
{
    var myMaterialNode = Node.createElement('material');
    myMaterialNode.id = createUniqueId(); //"m" + theMaterialName;
    myMaterialNode.name = theMaterialName;

    var myMaterialParent = window.scene.dom.find("//materials");

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
            '<textureunits>\n' +
            '    <textureunit texture="i' + theMaterialName + '" applymode="modulate"/>\n' +
            '</textureunits>';
        var myTexturesDoc = new Node(myTexturesString);
        var myTexturesNode = myTexturesDoc.firstChild;
        myMaterialNode.appendChild(myTexturesNode);
    }

    return myMaterialNode;
}

function buildUnlitTextureMaterialNode(theName, theTextureId) {
    var myTextureArrayFlag = theTextureId instanceof Array;
    var myTextureIds = [];
    if (!myTextureArrayFlag) {
        myTextureIds.push(theTextureId);
    } else {
        myTextureIds = theTextureId ;
    }


    var myMaterialNode = Node.createElement('material');
    myMaterialNode.enabled = false;
    myMaterialNode.id = createUniqueId(); //"m" + theName;
    myMaterialNode.name = theName;
    var myMaterialParent = window.scene.dom.find("//materials");

    myMaterialParent.appendChild(myMaterialNode);
    var myTextureFeatures = new Node('<feature name="textures">[10[]]</feature>\n').firstChild;
    myMaterialNode.requires.appendChild(myTextureFeatures);
    var myTexCoordFeatures = new Node('<feature name="texcoord">[10[]]</feature>\n').firstChild;
    myMaterialNode.requires.appendChild(myTexCoordFeatures);
    myMaterialNode.requires.lighting = "[10[unlit]]";
    var myTexturesReqString = "[100["
    var myTexCoordReqString = "[100["
    var myTexturesString = '<textureunits>\n';
    for (var myTextureIndex = 0; myTextureIndex < myTextureIds.length; myTextureIndex++) {
        var myTextureId = myTextureIds[myTextureIndex];
        if (myTextureId == undefined) {
            myTextureId = createUniqueId(); //"i" + theName;
        }

        // add textures
        myTexturesString += '    <textureunit texture="' + myTextureId  + '" applymode="modulate"/>\n';

        // add requirements
        if (myTextureIndex > 0) {
            myTexturesReqString += ",";
            myTexCoordReqString += ",";
        }
        myTexturesReqString += "paint";
        myTexCoordReqString += "uv_map";
    }
    myTexturesString += '</textureunits>';
    var myTexturesDoc = new Node(myTexturesString);
    var myTexturesNode = myTexturesDoc.firstChild;
    myMaterialNode.appendChild(myTexturesNode);

    myTexturesReqString += "]]";
    myMaterialNode.requires.textures = myTexturesReqString;

    myTexCoordReqString += "]]";
    myMaterialNode.requires.texcoord = myTexCoordReqString;
    myMaterialNode.properties.surfacecolor = new Vector4f(1,1,1,1);

    myMaterialNode.enabled = true;
    return myMaterialNode;

}

function addMaterialProperty(theMaterialNode, thePropertyType, theProperty, theValue) {
    var myPropertyString = "<" + thePropertyType +" name='" + theProperty + "'>" + theValue + "</" + thePropertyType + ">";
    var myPropertyDoc = new Node(myPropertyString);
    var myPropertyNode = myPropertyDoc.childNodes[0];

    var myPropertiesNode = theMaterialNode.find("properties");
    if (!myPropertiesNode) {
        var myPropertiesString = "<properties/>";
        var myPropertiesDoc = new Node(myPropertiesString);
        myPropertiesNode = myPropertiesDoc.childNodes[0];
        theMaterialNode.append(myPropertiesNode);
    }
    myPropertiesNode.appendChild(myPropertyNode);
}

function addMaterialRequirements(theMaterialNode, theRequirementlist) {
    var myRequiresNode = theMaterialNode.find(".//requires");
    if (!myRequiresNode) {
       var myRequiresString = "<requires/>";
       var myRequiresDoc = new Node(myRequiresString);
       myRequiresNode = myRequiresDoc.childNodes[0];
       theMaterialNode.append(myRequiresNode);
       myRequiresNode = theMaterialNode.find(".//requires");
    }
    for (var i = 0; i < theRequirementlist.length; i++ ) {
        var myRequirementString = "<feature name='" + theRequirementlist[i].name + "'>" + theRequirementlist[i].value + "</feature>";
        var myRequirementDoc = new Node(myRequirementString);
        var myRequirementNode = myRequirementDoc.childNodes[0];
        myRequiresNode.appendChild(myRequirementNode);
    }
}

function addMaterialRequirement(theMaterialNode, theRequirementClass, theRequirementValue) {
    var myRequirementString = "<feature name='" + theRequirementClass + "'>" + theRequirementValue + "</feature>";
    var myRequirementDoc = new Node(myRequirementString);
    var myRequirementNode = myRequirementDoc.childNodes[0];
    var myRequiresDocNode = theMaterialNode.find("requires");

    if (!myRequiresDocNode) {
        var myRequiresString = "<requires/>";
        var myRequiresDoc = new Node(myRequiresString);
        myRequiressNode = myRequiresDoc.childNodes[0];
        theMaterialNode.append(myRequiressNode);
    }
    myRequiresDocNode.appendChild(myRequirementNode);
}

function removeMaterialRequirement(theMaterialNode, theRequirementClass) {

    var myRequiresDocNode = theMaterialNode.find("requires");

    if (myRequiresDocNode) {
        var myRequirement = myRequiresDocNode.find(".//*[@name='" +
                                                   theRequirementClass +
                                                   "']");
        myRequiresDocNode.removeChild(myRequirement);
    }
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
