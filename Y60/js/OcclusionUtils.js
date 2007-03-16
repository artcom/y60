//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Y60JSSL.js");


// XXX all of this should go once the proper flag in Maya 'ac_occlusion' is used...
function setupOcclusionMaterials(theScene) {

    var myRegex = new RegExp(".*shadowmap.*");

    var myMaterials = theScene.materials;
    for (var i = 0; i < myMaterials.childNodes.length; ++i) {
        var myMaterial = myMaterials.childNode(i);

        // XXX these are from the BMW Welt project
        if (myMaterial.name == "lack" || myMaterial.name.search(/_shadowmapM/) != -1) {
            Logger.info("Skipping material '" + myMaterial.name + "': name blacklisted");
            continue;
        }

        var myTextures = myMaterial.childNode("textures");
        if (myTextures.childNodes.length <= 1) {
            Logger.info("Skipping material '" + myMaterial.name + "': only one texture");
            continue;
        }

        var myRequiresNode = myMaterial.childNode("requires");
        var myTexUsage = getDescendantByName(myRequiresNode, "textures");
        if (!myTexUsage) {
            continue;
        }
        var myTexUsageFeature = parseVectorOfRankedFeature(myTexUsage.firstChild.nodeValue);

        var myTexCoords = getDescendantByName(myRequiresNode, "texcoord");
        if (!myTexCoords) {
            continue;
        }
        var myTexCoordsFeature = parseVectorOfRankedFeature(myTexCoords.firstChild.nodeValue);
        if (myTexCoordsFeature[0].features[myTexCoordsFeature[0].features.length-1] != "reflective") {
            Logger.info("Skipping material '" + myMaterial.name + "': last texture is not reflective");
            continue;
        }

        var myTexture = myTextures.childNode(0);
        var myImage = myTexture.getElementById(myTexture.image);
        if (myRegex.exec(myImage.src)) {
            myTexUsageFeature[0].features[0] = "occlusion";

            Logger.warning("Setup material '" + myMaterial.name + "' for occlusion mapping");
            myTexUsage.firstChild.nodeValue = stringVectorOfRankedFeature(myTexUsageFeature);

            window.scene.update(Scene.MATERIALS);
            //print(myMaterial);
        }
    }
}


/*
 * RankedFeature conversion
 */
function stringRankedFeature(theRankedFeature) {

    if (theRankedFeature == null) {
        return "";
    }

    var myString = theRankedFeature.rank + "[";
    for (var i = 0; i < theRankedFeature.features.length; ++i) {
        if (i > 0) {
            myString += ",";
        }
        myString += theRankedFeature.features[i];
    }
    myString += "]";

    return myString;
}

function parseRankedFeature(theString) {

    var myRegex = new RegExp("^\([0-9]*\)\\[\(.*\)\\]$");
    var myMatch = myRegex.exec(theString);
    if (!myMatch || myMatch.length != 3) {
        return null;
    }

    var myRank = new Number(myMatch[1]);
    var myFeatures = myMatch[2].split(",");
    var myRankedFeature = {rank:myRank, features:myFeatures};
    //print(theString, stringRankedFeature(myRankedFeature));

    return myRankedFeature;
}


/*
 * VectorOfRankedFeature conversion
 */
function stringVectorOfRankedFeature(theVectorOfRankedFeature) {

    if (theVectorOfRankedFeature == null) {
        return "";
    }

    var myString = "[";
    for (var i = 0; i < theVectorOfRankedFeature.length; ++i) {
        myString += stringRankedFeature(theVectorOfRankedFeature[i]);
    }
    myString += "]";

    return myString;
}

function parseVectorOfRankedFeature(theVectorOfString) {
    var myVectorOfRankedFeature = [];
    for (var i = 0; i < theVectorOfString.length; ++i) {
        var myRankedFeature = theVectorOfString[i];
        myVectorOfRankedFeature.push(parseRankedFeature(myRankedFeature));
    }
    return myVectorOfRankedFeature;
}
