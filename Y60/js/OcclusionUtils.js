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


function setupOcclusionMaterials(theScene) {

    var myRegex = new RegExp(".*shadowmap.*");

    var myMaterials = theScene.materials;
    for (var i = 0; i < myMaterials.childNodes.length; ++i) {
        var myMaterial = myMaterials.childNode(i);
        if (myMaterial.name == "lack" || myMaterial.name.search(/_shadowmapM/) != -1) {
            Logger.info("Skipping material '" + myMaterial.name + "'");
            continue;
        }

        var myTextures = myMaterial.childNode("textures");
        if (myTextures.childNodes.length <= 1) {
            Logger.info("Skipping material '" + myMaterial.name + "'");
            continue;
        }

        var myTexUsage = getDescendantByName(myMaterial.childNode("requires"), "textures");
        if (!myTexUsage) {
            continue;
        }
        var myVectorOfRankedFeature = parseVectorOfRankedFeature(myTexUsage.firstChild.nodeValue);

        var myTexture = myTextures.childNode(0);
        var myImage = myTexture.getElementById(myTexture.image);
        if (myRegex.exec(myImage.src)) {
            myVectorOfRankedFeature[0].features[0] = "occlusion";

            Logger.warning("Setup material '" + myMaterial.name + "' for occlusion mapping");
            myTexUsage.firstChild.nodeValue = stringVectorOfRankedFeature(myVectorOfRankedFeature);

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
