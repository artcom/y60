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

use("LayerBase.js");

function ImageLayer(theDepth, theName, theType) {
    this.Constructor(this, theDepth, theName, theType);
}

ImageLayer.prototype.Constructor = function(Public, theDepth, theName, theType) {

    var Protected = {};
    var Base = {};
    LayerBase.prototype.Constructor(Public, Protected, theDepth, theName, theType);

    var _myDepth = theDepth;

    var _myName = null;
    var _myMaterial = null;
    var _myQuad = null;

    Base.setup = Public.setup;
    Public.setup = function(theLayerNode) {

        Logger.info(theLayerNode);
        _myName = theLayerNode.name;

        _myMaterial = getCachedImageMaterial("CONTENT/" + theLayerNode.getAttribute("file"));
        _myMaterial.transparent = theLayerNode.transparent;

        var myX = 0;
        if ("x" in theLayerNode) {
            myX = Number(theLayerNode.x);
        }

        var myY = 0;
        if ("y" in theLayerNode) {
            myY = Number(theLayerNode.y);
        }

        var myZ = _myDepth;
        if ("z" in theLayerNode) {
            myZ = Number(theLayerNode.z);
        }

        _myQuad = createQuad(window.scene.world,
                             _myName,
                             new Vector2i(theLayerNode.width, theLayerNode.height),
                             new Vector3i(myX, myY, myZ),
                             _myMaterial,
                             true, true);

    }

}
