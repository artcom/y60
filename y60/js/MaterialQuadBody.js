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

function MaterialQuadBody(theScene, theParentNode, theImage, theCoords) {
    this.Constructor(this, theScene, theParentNode, theImage, theCoords);
}

var ourImageMap = {};

MaterialQuadBody.prototype.Constructor = function(Public, theScene, theParentNode, theImage, theCoords) {

    var _myMaterial;

    if (!theParentNode || !("nodeName" in theParentNode)) {
        var errorMessage = "need a DOM node to create a MaterialQuadBody!";
        Logger.error(errorMessage);
        throw new Exception(errorMessage);
    }

    if (theImage == undefined) {
        var errorstring = "theImage is undefined";
        Logger.error(errorstring);
        throw new Exception(errorstring);
    } else if ((theImage instanceof String) || (typeof theImage == "string")) {
        var myNode = theParentNode.getElementById(theImage);
        if (myNode) {
            // theImage is-a image ID
            if (myNode.nodeName == "image") {
                _myMaterial = Modelling.createUnlitTexturedMaterial(theScene, myNode);
                _myMaterial.properties.surfacecolor = new Vector4f(1, 1, 1, 1);
            } else if (myNode.nodeName == "material") {
                _myMaterial = myNode;
            }
        } else if (fileExists(theImage)) {
            // theImage is-a filename

            if (theImage in ourImageMap) {
                _myMaterial =
                    Modelling.createUnlitTexturedMaterial(theScene, ourImageMap[theImage],
                                                          "MQB_Material");
            } else {
                _myMaterial = Modelling.createUnlitTexturedMaterial(theScene, theImage,
                                                                    "MQB_Material", false);
                var myTextureId = _myMaterial.childNode('textureunits').firstChild.texture;
                var myTexture = window.scene.textures.getElementById(myTextureId);
                ourImageMap[theImage] = _myMaterial.getElementById(myTexture.image);
            }
            _myMaterial.properties.surfacecolor = new Vector4f(1, 1, 1, 1);
        } else {
            throw new Exception(__FILE__() + ': Material file "'+theImage+'" does not exist.');
        }
    } else if ((theImage instanceof Node)) {
        if (theImage.nodeName == "image") {
            // theImage is-a <image> node
            _myMaterial = Modelling.createUnlitTexturedMaterial(theScene, theImage);
            _myMaterial.properties.surfacecolor = new Vector4f(1, 1, 1, 1);
        } else if (theImage.nodeName == "material") {
            _myMaterial = theImage;
        }
    } else {
        // theImage is-a color
        _myMaterial = Modelling.createColorMaterial(theScene, theImage);
    }

    var _myQuadShape = Modelling.createQuad(theScene, _myMaterial.id, theCoords[0], theCoords[1]);
    _myQuadShape.name = "MQB_Shape";

    var _myBody = Modelling.createBody(theParentNode, _myQuadShape.id);
    _myBody.name = "MQB_Body";


    Public.destruct = function() {
        _myBody.parentNode.removeChild(_myBody);
        _myQuadShape.parentNode.removeChild(_myQuadShape);
        _myMaterial.parentNode.removeChild(_myMaterial);
    }

    Public.material getter = function() {
        return _myMaterial;
    }

    Public.material setter = function(p) {
        _myMaterial = p;
    }

    Public.shape getter = function() {
        return _myQuadShape;
    }

    Public.shape setter = function(p) {
        _myQuadShape = p;
    }

    Public.quad getter = function() {
        return _myQuadShape;
    }

    Public.body getter = function() {
        return _myBody;
    }

    Public.body setter = function(p) {
        _myBody = p;
    }
}
