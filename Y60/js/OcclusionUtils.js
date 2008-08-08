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

    var myMaterials = theScene.materials;
    for (var i = 0; i < myMaterials.childNodes.length; ++i) {
        var myMaterial = myMaterials.childNode(i);

        // XXX these are from the BMW Welt project
        if (myMaterial.name == "lack" || myMaterial.name.search(/_shadowmapM/) != -1) {
            Logger.info("Skipping material '" + myMaterial.name + "': name blacklisted");
            continue;
        }

        var myTextureUnits = myMaterial.childNode("textureunits");
        if (!myTextureUnits) {
            Logger.info("Skipping material '" + myMaterial.name + "': no texture");
            continue;
        }
        if (myTextureUnits.childNodes.length <= 1) {
            Logger.info("Skipping material '" + myMaterial.name + "': only one texture");
            continue;
        }

        var myRequiresNode = myMaterial.childNode("requires");
        var myTexUsage = myRequiresNode.find("//*[@name = 'textures']");
        if (!myTexUsage) {
            continue;
        }
        var myTexUsageFeature = parseVectorOfRankedFeature(myTexUsage.firstChild.nodeValue);
        if (myTexUsageFeature[0].features[0] == "occlusion") {
            Logger.info("Skipping material '" + myMaterial.name + "': already has 'occlusion' usage");
            continue;
        }

        var myTexCoords = myRequiresNode.find("//*[@name = 'texcoord']");
        if (!myTexCoords) {
            continue;
        }
        var myTexCoordsFeature = parseVectorOfRankedFeature(myTexCoords.firstChild.nodeValue);
        if (myTexCoordsFeature[0].features[myTexCoordsFeature[0].features.length-1] != "reflective") {
            Logger.info("Skipping material '" + myMaterial.name + "': last texture is not reflective");
            continue;
        }

        var myTextureUnit = myTextureUnits.childNode(0);
        var myTexture = myTextureUnit.getElementById(myTextureUnit.texture);
        var myImage = myTexture.getElementById(myTexture.image);
        if (myImage.src.search(/shadowmap/) != -1) {
            myTexUsageFeature[0].features[0] = "occlusion";

            Logger.warning("Setup material '" + myMaterial.name + "' for occlusion mapping");
            myTexUsage.firstChild.nodeValue = stringVectorOfRankedFeature(myTexUsageFeature);

            window.scene.update(Scene.MATERIALS);
            //print(myMaterial);
        }
    }
}
