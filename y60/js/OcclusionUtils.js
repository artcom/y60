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
        var myTexUsage = myRequiresNode.find("*[@name = 'textures']");
        if (!myTexUsage) {
            continue;
        }
        var myTexUsageFeature = parseVectorOfRankedFeature(myTexUsage.firstChild.nodeValue);
        if (myTexUsageFeature[0].features[0] == "occlusion") {
            Logger.info("Skipping material '" + myMaterial.name + "': already has 'occlusion' usage");
            continue;
        }

        var myTexCoords = myRequiresNode.find("*[@name = 'texcoord']");
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
