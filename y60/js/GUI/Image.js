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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

function Image(theParent, theImageNode, theDepth) {
    this.Constructor(this, theParent, theImageNode, theDepth);
}

Image.prototype.Constructor = function(Public, theParent, theImageNode, theDepth) {

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _myName   = theImageNode.name;
    var _myNode   = theImageNode;
    var _myParent = theParent;
    var _myDepth  = theDepth;
    var _myHeight = 0;
    var _myWidth  = 0;

    var _mySelected = false;

    var _myMaterial = null;
    var _myQuad     = null;

    var _myAlpha    = 1.0;
    var _myMaxAlpha = 1.0;

    Public.__defineGetter__("bodies", function() {
        return [_myQuad.body];
    });

    Public.clone = function() {
        var myNode = clone(_myNode);
        myNode.name = _myNode.name + "_copy";
        return new Image(_myParent, myNode, _myDepth);
    }

    Public.__defineGetter__("name", function() {
        return _myName;
    });

    Public.__defineGetter__("body", function() {
        return _myQuad.body;
    });

    Public.__defineSetter__("insensible", function(theInsensible) {
        _myQuad.body.insensible = theInsensible;
    });

    Public.__defineGetter__("material", function() {
        return _myMaterial;
    });

    Public.__defineGetter__("alpha", function() {
        return _myAlpha;
    });

    Public.__defineSetter__("alpha", function(theAlpha) {
        _myAlpha = theAlpha;
        Modelling.setAlpha(_myQuad.body, Math.min(_myMaxAlpha, _myAlpha));
    });

    Public.__defineSetter__("maxalpha", function(theAlpha) {
        _myMaxAlpha = theAlpha;
        Public.alpha = _myAlpha;
    });

    Public.__defineGetter__("visible", function() {
        return Public.body.visible;
    });

    Public.__defineSetter__("visible", function(theVisibility) {
        Public.body.visible = theVisibility;
    });

    Public.__defineGetter__("texture", function() {
        var myTextureUnit = _myMaterial.childNode("textureunits").firstChild;
        return window.scene.dom.getElementById(myTextureUnit.texture);
    });

    Public.__defineSetter__("image", function(theImage) {
        Public.texture.image = theImage.id;
    });

    Public.__defineGetter__("image", function() {
        return window.scene.dom.getElementById(Public.texture.image);
    });

    Public.__defineGetter__("wrapmode", function() {
        return Public.texture.wrapmode;
    });

    Public.__defineSetter__("wrapmode", function(theWrapmode) {
        Public.texture.wrapmode = theWrapmode;
    });

    Public.__defineGetter__("width", function() {
        return _myWidth;
    });

    Public.__defineSetter__("width", function(theWidth) {
        _myWidth = theWidth;
    });

    Public.__defineGetter__("height", function() {
        return _myHeight;
    });

    Public.__defineSetter__("height", function(theHeight) {
        _myHeight = theHeight;
    });

    Public.__defineGetter__("z", function() {
        return Public.body.position.z;
    });

    Public.__defineSetter__("z", function(v) {
        Public.body.position.z = v;
    });

    Public.__defineGetter__("playmode", function() {
        if ("playmode" in Public.image) {
            return Public.image.playmode;
        }
        return "";
    });
    Public.__defineSetter__("playmode", function(theMode) {
        if ("playmode" in Public.image) {
            Public.image.playmode = theMode;
        }
    });

    ////////////////////////////////////////
    // setup
    ////////////////////////////////////////

    Public.setup = function() {
        Logger.info("Setting up image " + _myName);

        if("z" in _myNode) {
            _myDepth += Number(_myNode.z);
        }

        var myFile = CONTENT + "/" + _myNode.file;
        _myMaterial = getCachedImageMaterial(myFile);
        _myMaterial.transparent = true;

        _myWidth = _myNode.width;
        _myHeight = _myNode.height;

        var myInsensible = true;
        if ("insensible" in _myNode) {
            myInsensible = _myNode.insensible;
        }

        _myQuad = createQuad(_myParent, _myName,
                             new Vector2f(_myWidth,_myHeight),
                             new Vector3f(_myNode.x,_myNode.y, _myDepth),
                             _myMaterial,
                             myInsensible,
                             /* visible */    true);

        if ("angle" in _myNode) {
            _myQuad.body.orientation =
                Quaternionf.createFromEuler(new Vector3f(0,0,radFromDeg(_myNode.angle)));
        }

        if ("wrapmode" in _myNode) {
            Public.wrapmode = _myNode.wrapmode;
        }
    }

    Public.setup();
}
