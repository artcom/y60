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

/*jslint white:false, plusplus:false, nomen:false*/
/*globals use, exit, window, Logger, OverlayBase, Matrix4f, Vector3f,
          Box3f, Exception, fileline, Node, product, expandEnvironment,
          asColor, createUniqueId, Modelling, Vector2i, TextureApplyMode,
          Playlist, addMaterialRequirement, getImageSize*/

use("Y60JSSL.js");
use("Playlist.js");

var ourOverlayCounter = 0;

// Just a test...
var ourImageCache = [];

function removeOverlay(theOverlayNode) {
    // Remove children
    while(theOverlayNode.childNodes.length) {
        removeOverlay(theOverlayNode.firstChild);
    }

    if (theOverlayNode.material) {
        var myMaterialNode = theOverlayNode.getElementById(theOverlayNode.material);
        if (myMaterialNode) {

            // Remove texture,image
            var myTextureUnits = myMaterialNode.find("textureunits");
            if (myTextureUnits) {
                for (var i = 0; i < myTextureUnits.childNodesLength(); ++i) {
                    var myTextureUnit = myTextureUnits.childNode(i);
                    var myTexture = myTextureUnits.getElementById(myTextureUnit.texture);
                    if (window.scene.textures.findAll("texture[@image='"+ myTexture.image+ "']").length === 1) {
                        // our texture is the only one referencing this image, safe to remove
                        var myImage = myTexture.getElementById(myTexture.image);
                        myImage.parentNode.removeChild(myImage);
                    } else {
                        Logger.warning("More than one texture references image id=" + myTexture.image);
                    }

                    if (window.scene.materials.findAll(".//textureunit[@texture='"+ myTexture.id+ "']").length === 1) {
                        // our textureunit is the only one referencing this texture, safe to remove
                        myTexture.parentNode.removeChild(myTexture);
                    } else {
                        Logger.warning("More than one textureunit references texture id=" + myTexture.id);
                    }
                }
            }

            // Remove material node
            myMaterialNode.parentNode.removeChild(myMaterialNode);
        } else {
            Logger.warning("Could not remove material node " + theOverlayNode.material);
        }
    }

    // Remove overlay node
    if (theOverlayNode.parentNode) {
        theOverlayNode.parentNode.removeChild(theOverlayNode);
    }
}

// Pure virtual base class
function OverlayBase(theScene, thePosition, theParent) {
    var Protected = [];
    this.Constructor(this, Protected, theScene, thePosition, theParent);
    exit(1);
}

OverlayBase.prototype.Constructor = function(Public, Protected, theScene, thePosition, theParent) {

    /////////////////////
    // Private Members //
    /////////////////////

     var _myNode = null;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Delegation of all overlay node attributes
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.toString = function() {
        return _myNode.toString();
    };

    Public.__defineGetter__("node", function() { return _myNode; });

    Public.__defineGetter__("name", function() { return _myNode.name; });
    Public.__defineSetter__("name", function(theArgument) { _myNode.name = theArgument; });

    Public.__defineGetter__("visible", function() { return _myNode.visible; });
    Public.__defineSetter__("visible", function(theArgument) { _myNode.visible = theArgument; });

    Public.__defineGetter__("position", function() { return _myNode.position; });
    Public.__defineSetter__("position", function(theArgument) { _myNode.position = theArgument; });

    Public.__defineGetter__("scale", function() { return _myNode.scale; });
    Public.__defineSetter__("scale", function(theArgument) { _myNode.scale = theArgument; });

    Public.__defineGetter__("rotation", function() { return _myNode.rotation; });
    Public.__defineSetter__("rotation", function(theArgument) { _myNode.rotation = theArgument; });

    Public.__defineGetter__("pivot", function() { return _myNode.pivot; });
    Public.__defineSetter__("pivot", function(theArgument) { _myNode.pivot = theArgument; });

    Public.__defineGetter__("srcorigin", function() { return _myNode.srcorigin; });
    Public.__defineSetter__("srcorigin", function(theArgument) { _myNode.srcorigin = theArgument; });

    Public.__defineGetter__("srcsize", function() { return _myNode.srcsize; });
    Public.__defineSetter__("srcsize", function(theArgument) { _myNode.srcsize = theArgument; });

    Public.__defineGetter__("alpha", function() { return _myNode.alpha; });
    Public.__defineSetter__("alpha", function(theArgument) { _myNode.alpha = theArgument; });

    Public.__defineGetter__("width", function() { return _myNode.width; });
    Public.__defineSetter__("width", function(theArgument) { _myNode.width = theArgument; });

    Public.__defineGetter__("height", function() { return _myNode.height; });
    Public.__defineSetter__("height", function(theArgument) { _myNode.height = theArgument; });

    Public.__defineGetter__("bordercolor", function() { return _myNode.bordercolor; });
    Public.__defineSetter__("bordercolor", function(theArgument) { _myNode.bordercolor = theArgument; });

    Public.__defineGetter__("borderwidth", function() { return _myNode.borderwidth; });
    Public.__defineSetter__("borderwidth", function(theArgument) { _myNode.borderwidth = theArgument; });

    Public.__defineGetter__("topborder", function() { return _myNode.topborder; });
    Public.__defineSetter__("topborder", function(theArgument) { _myNode.topborder = theArgument; });

    Public.__defineGetter__("bottomborder", function() { return _myNode.bottomborder; });
    Public.__defineSetter__("bottomborder", function(theArgument) { _myNode.bottomborder = theArgument; });

    Public.__defineGetter__("leftborder", function() { return _myNode.leftborder; });
    Public.__defineSetter__("leftborder", function(theArgument) { _myNode.leftborder = theArgument; });

    Public.__defineGetter__("rightborder", function() { return _myNode.rightborder; });
    Public.__defineSetter__("rightborder", function(theArgument) { _myNode.rightborder = theArgument; });

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Utility functions
    ///////////////////////////////////////////////////////////////////////////////////////////

    /// Removes the overlay and all its children and all
    /// connected materials and images from the scene dom.
    /// Do not call this function, if you share materials or images.
    Public.removeFromScene = function() {
        removeOverlay(_myNode);
    };

    /// Moves overlay to first position in the overlay z-order
    Public.moveToTop = function() {
        var myParent = _myNode.parentNode;
        myParent.removeChild(_myNode);
        myParent.appendChild(_myNode);
    };

    /// Moves overlay to last position in the overlay z-order
    Public.moveToBottom = function() {
        var myParent = _myNode.parentNode;
        myParent.removeChild(_myNode);
        myParent.insertBefore(_myNode, myParent.firstChild);
    };

    /// This checks if the square defined by theX/theY and theSquareSize overlaps with the overlay
    //  @param theXPos        integer   X-Center of the square to check for overlap
    //  @param theXPos        integer   Y-Center of the square to check for overlap
    //  @param theSquareSize  Array[2]  Size of the square to check for overlay (optional, default is [1,1])
    //  @rval  true, if overlay overlaps with the square, false, if it does not
    Public.touches = function(theXPos, theYPos, theSquareSize) {
        var myParent = Public.node.parentNode;

        var myMatrix = new Matrix4f();
        myMatrix.scale(new Vector3f(Public.scale.x, Public.scale.y,1.0));
        myMatrix.rotateZ(Public.rotation);
        myMatrix.translate(new Vector3f(_myNode.position.x,_myNode.position.y,0));

        var myParentMatrix = new Matrix4f();
        while (myParent.nodeName === "overlay") {
            myParentMatrix.makeScaling(new Vector3f(myParent.scale.x, myParent.scale.y,1.0));

            myParentMatrix.rotateZ(myParent.rotation);
            myParentMatrix.translate(new Vector3f(myParent.position.x,myParent.position.y,0));
            myMatrix.postMultiply(myParentMatrix);

            myParent = myParent.parentNode;
        }

        myMatrix.invert();
        var myNewPoint = product(new Vector3f(theXPos, theYPos,0), myMatrix);

        if (theSquareSize !== undefined) {
            var myOverlayBox = new Box3f([0, 0, 0],
                [_myNode.width - 1, _myNode.height - 1, 1]);
            var myMouseBox = new Box3f([myNewPoint[0] - theSquareSize, myNewPoint[1] - theSquareSize, 0],
                [myNewPoint[0] + theSquareSize, myNewPoint[1] + theSquareSize, 1]);
            return myMouseBox.touches(myOverlayBox);
        } else {
            if (0 <= myNewPoint[0] && 0 <= myNewPoint[1] &&
                _myNode.width > myNewPoint[0] && _myNode.height > myNewPoint[1])
            {
                return true;
            }
        }
        return false;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    /// Create an overlay without material
    function setup() {
        var myParent = null;
        if (theParent !== null && theParent !== undefined) { // why do we get passed both?
            if ("node" in theParent) {
                myParent = theParent.node;
            } else {
                myParent = theParent;
            }
        } else {
            var myFirstCanvas = theScene.dom.childNode("canvases").firstChild;
            if (!myFirstCanvas) {
                throw new Exception("theScene has no canvas", fileline());
            }
            var myViewport = myFirstCanvas.childNode("viewport");
            if (!myViewport) {
                throw new Exception("the primary canvas has no viewports", fileline());
            }
            myParent = myViewport.childNode("overlays");
            Logger.info("Appending Overlay to first canvas in the scene.");
        }

        _myNode = myParent.appendChild(new Node("<overlay/>").firstChild);
        _myNode.name = "Overlay_" + ourOverlayCounter++;
        if (thePosition) {
            _myNode.position.x = thePosition[0];
            _myNode.position.y = thePosition[1];
        }
    }

    setup();
};

/// Creates a group overlay
// @param theScene     Scene     The scene the overlay should be appended to
// @param theName      String    Name of the overlay group (optional)
// @param thePosition  Vector2f  Pixelposition of the overlay (optional, default is [0,0])
// @param theParent    Node      Parent overlay node (optional, optional default is toplevel)
function GroupOverlay(theScene, theName, thePosition, theParent) {
    var Public    = this;
    var Protected = {};
    OverlayBase.prototype.Constructor(Public, Protected, theScene, thePosition, theParent);
    if (theName !== undefined && theName) {
        Public.name = theName;
    }
}

// Pure virtual base class
function MaterialOverlay(theScene, thePosition, theParent) {
    var Protected = [];
    this.Constructor(this, Protected, theScene, thePosition, theParent);
    exit(1);
}
MaterialOverlay.prototype.Constructor = function(Public, Protected, theScene, thePosition, theParent) {

    OverlayBase.prototype.Constructor(Public, Protected, theScene, thePosition, theParent);
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myMaterial = null;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Convenience access to referenced nodes
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.__defineGetter__("material", function() {
        return _myMaterial;
    });
    Public.__defineSetter__("material", function(theMaterial) {
        Public.node.material = theMaterial.id;
        _myMaterial      = theMaterial;

        // Notify derived classes
        Protected.onMaterialChange();
    });

    Public.__defineGetter__("color", function() {
        return _myMaterial.properties.surfacecolor;
    });

    Public.__defineSetter__("color", function(theColor) {
        _myMaterial.properties.surfacecolor = asColor(theColor);
    });

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Protected
    ///////////////////////////////////////////////////////////////////////////////////////////

    /// Overload this function to get notified on material changes
    Protected.onMaterialChange = function() {};

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    /// Create an untextured overlay
    function setup() {
        var myMaterialId = createUniqueId();
        _myMaterial = Node.createElement('material');

        theScene.materials.appendChild(_myMaterial);
        _myMaterial.id = myMaterialId;
        _myMaterial.name = Public.name + "M";
        _myMaterial.transparent = 1;
        _myMaterial.properties.surfacecolor = "[1,1,1,1]";
        Public.node.material = _myMaterial.id;
    }

    setup();
};


/// Creates a uniform colored overlay
// @param theScene     Scene     The scene the overlay should be appended to
// @param theColor     Vector4f  Color of the overlay
// @param thePosition  Vector2f  Pixelposition of the overlay (optional, default is [0,0])
// @param theSize      Vector2f  Size of the overlay (optional, default is [100,100]
// @param theParent    Node      Parent overlay node (optional, optional default is toplevel)
function Overlay(theScene, theColor, thePosition, theSize, theParent) {
    var Protected = [];
    this.Constructor(this, Protected, theScene, theColor, thePosition, theSize, theParent);
}

Overlay.prototype.Constructor = function(Public, Protected, theScene, theColor, thePosition, theSize, theParent) {

    MaterialOverlay.prototype.Constructor(Public, Protected, theScene, thePosition, theParent);
    if (theSize) {
        Public.width  = theSize[0];
        Public.height = theSize[1];
    } else {
        Public.width  = 100;
        Public.height = 100;
    }

    if (theColor) {
        Public.color = theColor;
    }
};

// Pure virtual base class
function TextureOverlay(theScene, thePosition, theParent) {
    var Protected = [];
    this.Constructor(this, Protected, theScene, thePosition, theParent);
}

TextureOverlay.prototype.Constructor = function(Public, Protected, theScene, thePosition, theParent) {
    var Base = {};
    MaterialOverlay.prototype.Constructor(Public, Protected, theScene, thePosition, theParent);

    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myTextureUnits = null;
    var _myTextures = [];

    /////////////////////
    // Private Methods //
    /////////////////////

    function createTexcoordFeatureString(theTextureCount, theMapping) {
        theMapping = theMapping || "uv_map";

        var myString = "[100[";
        for (var i = 0; i < theTextureCount; ++i) {
            if (i > 0) {
                myString += ",";
            }
            myString += theMapping;
        }
        myString += "]]";
        return myString;
    }

    function addTextureRequirements(theTextureCount) {
        /*
        var myFindString = "*[@name='textures']";
        var myTextureFeatures = Public.material.requires.find(myFindString);//getDescendantByAttribute(Public.material.requires, "name", "textures", false);
        if (myTextureFeatures == null) {
            myTextureFeatures = new Node("<feature name='textures'>[10[paint]]</feature>").firstChild;
            Public.material.requires.appendChild(myTextureFeatures);
        }
        myFindString = "*[@name='texcoord']";
        myTextureFeatures = Public.material.requires.find(myFindString);//getDescendantByAttribute(Public.material.requires, "name", "texcoord", false);
        if (myTextureFeatures == null) {
            myTextureFeatures = new Node("<feature name='texcoord'>[10[uv_map]]</feature>").firstChild;
            Public.material.requires.appendChild(myTextureFeatures);
        }
        */
        var myFragment = Node.createDocumentFragment();
        if (!("textures" in Public.material.requires)) {
            myFragment.appendChild(new Node("<feature name='texcoord'>[10[uv_map]]</feature>").firstChild);
        }
        if (!("textures" in Public.material.requires)) {
            myFragment.appendChild(new Node("<feature name='textures'>[10[paint]]</feature>").firstChild);
        }

        if (myFragment.childNodes.length) {
            // append all nodes in one call to avoid incomplete feature set
            Public.material.requires.appendChild(myFragment);
        }

        Public.material.requires.textures = Public.createTextureFeatureString(theTextureCount);
        Public.material.requires.texcoord = createTexcoordFeatureString(theTextureCount);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.__defineGetter__("src", function() { return Public.image.src; });
    Public.__defineSetter__("src", function(theArgument) { Public.image.src = expandEnvironment(theArgument); });

    /// Get image of first texture.
    Public.__defineGetter__("image", function() {
        if (Protected.myImages.length) {
            return Protected.myImages[0];
        }
        return null;
    });

    /// Set image of first texture.
    Public.__defineSetter__("image", function(theImage) {
        Protected.myImages[0] = theImage;
        if (_myTextureUnits) {
            var myTexture = Public.node.getElementById(_myTextureUnits.childNodes[0].texture);
            myTexture.image = theImage.id;
            //_myTextures[0].image = theImage.id;
        } else {
            Protected.addTexture(theImage.id);
        }
    });

    /// Set images for all textures.
    Public.__defineSetter__("images", function(theImages) {
        var myTexture;
        for (var i = 0; i < theImages.length; ++i) {
            if (i < _myTextureUnits.childNodes.length) {
                myTexture = Public.node.getElementById(_myTextureUnits.childNodes[i].texture);
                if (myTexture) {
                    myTexture.image = theImages[i].id;
                } else {
                    Protected.addTexture(theImages[i].id);
                }
            } else {
                Protected.addTexture(theImages[i].id);
            }
        }
        // remove unused textureunit/texture
        i = _myTextureUnits.childNodes.length;
        while (i-- > theImages.length) {
            var myTextureUnit = _myTextureUnits.lastChild;
            myTexture = myTextureUnit.getElementById(myTextureUnit.texture);
            _myTextureUnits.removeChild(myTextureUnit);
            myTexture.parentNode.removeChild(myTexture);
        }
        addTextureRequirements(_myTextureUnits.childNodesLength());
        Protected.myImages = theImages;
    });

    /// Get all images.
    Public.__defineGetter__("images", function() {
        return Protected.myImages;
    });

    /*
    Public.textures getter = function() {
        // XXX must be adapted for textureunit/texture/image structure.
        Logger.error("DEPRECATED Overlay.textures getter");
        exit(-1);
        return _myTextureUnits;
    }

    Public.textures setter = function(theTextures) {
        // XXX must be adapted for textureunit/texture/image structure.
        Logger.error("DEPRECATED Overlay.textures setter");
        exit(-1);
        for (var i = 0; i < theTextures.length; ++i) {
            if (i < _myTextureUnits.childNodes.length) {
                _myTextureUnits.replaceChild(theTextures[i], _myTextureUnits.childNodes[i]);
            } else {
                _myTextureUnits.appendChild(theTextures[i]);
            }
        }
        i = _myTextureUnits.childNodes.length;
        // XXX must be adapted for textureunit/texture/image structure.
        while (i-- > theTextures.length) {
            _myTextureUnits.removeChild(_myTextureUnits.lastChild);
        }
        Protected.onTextureChange();
    }
    */

    /// Get texture units.
    Public.__defineGetter__("textureunits", function() {
        return _myTextureUnits;
    });

    /// Get first texture unit.
    Public.__defineGetter__("textureunit", function() {
        return _myTextureUnits.childNodes[0];
    });

    /// Set texture unit. Default is the first texture unit.
    Public.__defineSetter__("textureunit", function(theTextureUnit, theIndex) {
        theIndex = theIndex || 0;
        _myTextureUnits.replaceChild(theTextureUnit, _myTextureUnits.childNodes[theIndex]);
        Protected.onTextureChange();
    });

    /// Get first texture.
    Public.__defineGetter__("texture", function() {
        var myTextureUnit = Public.textureunit;
        return myTextureUnit.getElementById(myTextureUnit.texture);
    });

    /*
    Public.texture setter = function(theTexture) {
        // XXX must be adapted for textureunit/texture/image structure.
        // but apparently no one uses this anyway...
        Logger.error("Deprecated Overlay.texture setter. " + theTexture);
        exit(-1);
        _myTextureUnits.replaceChild(theTexture, _myTextureUnits.firstChild);
        Protected.onTextureChange();
    }
    */

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Protected
    ///////////////////////////////////////////////////////////////////////////////////////////

    Protected.onTextureChange = function() {
        Protected.myImages = [];
        for (var i = 0; i < _myTextureUnits.childNodes.length; ++i) {
            var myId = _myTextureUnits.childNodes[i].texture;
            var myTexture = _myTextureUnits.getElementById(myId);
            var myImage = _myTextureUnits.getElementById(myTexture.image);
            if (myImage === null) {
                throw new Exception("Could not find image with id: " + myTexture.image, fileline());
            }
            Protected.myImages.push(myImage);
        }
    };

    Protected.onMaterialChange = function() {
        _myTextureUnits = Public.material.find("textureunits");
        if (_myTextureUnits.childNodes.length !== 1) {
            throw new Exception("TextureOverlay can only have one texture, but it has: " + _myTextureUnits.childNodes.length, fileline());
        }
        Protected.onTextureChange();
    };

    Protected.addTexture = function(theImageId) {
        var myImage = window.scene.images.getElementById(theImageId);
        if (!myImage) {
            Logger.error("No such image id=" + theImageId);
            return;
        }
        //var isMovieFlag = myImage.nodeName === "movie";
        var myFindString = "texture[@image='"+ theImageId+ "']";        
        var myTexture = window.scene.textures.find(myFindString);
        /*if (_myTextureUnits) {
            _myTextureUnits = getDescendantByTagName(Public.material, "textureunits", false);
            for (var i = 0; i < _myTextureUnits.childNodesLength(); i++) {
                var myTextureUnit = _myTextureUnits.childNode(i);
                myTexture  = getDescendantByAttribute(window.scene.textures, "id", myTextureUnit.texture);
                if (myTexture.image == theImageId) {
                    break;
                }
            }
        }*/

        if (!myTexture) {
            myTexture = Modelling.createTexture(window.scene, myImage);
            _myTextures.push(myTexture);
            myTexture.image = theImageId;
        } else {
            // re-use texture that references the given image -- maybe not such a good idea...
            // in fact, it will probably break when cleaning up after ourselves since we might
            // delete a texture that is in use by some other material...
        }

        // create texture unit
        if (!_myTextureUnits) {
            myFindString = "textureunits";
            _myTextureUnits = Public.material.find(myFindString);
            if (!_myTextureUnits) {
                _myTextureUnits = Public.material.appendChild(Node.createElement("textureunits"));
            }
        }

        var myTextureUnit = _myTextureUnits.appendChild(Node.createElement("textureunit"));
        myTextureUnit.applymode = TextureApplyMode.modulate;
        myTextureUnit.texture   = myTexture.id;

        addTextureRequirements(_myTextureUnits.childNodesLength());

        return myTexture;
    };

    Protected.myImages = [];

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.createTextureFeatureString = function(theTextureCount, theUsage) {
        theUsage = theUsage || "paint";

        var myString = "[100[";
        for (var i = 0; i < theTextureCount; ++i) {
            if (i > 0) {
                myString += ",";
            }
            myString += theUsage;
        }
        myString += "]]";
        return myString;
    };

};

// Pure virtual base class
function ImageOverlayBase(theScene, theSource, thePosition, theParent) {
    var Protected = [];
    this.Constructor(this, Protected, theScene, theSource, thePosition, theParent);
}

ImageOverlayBase.prototype.Constructor = function(Public, Protected, theScene, theSource, thePosition, theParent) {

    TextureOverlay.prototype.Constructor(Public, Protected, theScene, thePosition, theParent);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////
    Protected.addImage = function(theSource) {
        var myImage = null;
        if (typeof(theSource) === "string") {
            var mySource = expandEnvironment(theSource);
            if (mySource in ourImageCache) {
                myImage = ourImageCache[mySource];
            } else {
                // theSource is a string
                myImage = Node.createElement("image");
                myImage.src  = expandEnvironment(theSource);
                //myImage.resize = "pad";
                //myImage.mipmap = false;
                theScene.images.appendChild(myImage);
                myImage.name = mySource;
                //myImage.wrapmode  = TextureWrapMode.repeat;
                //myImage.type = ImageType.single;
                //ourImageCache[mySource] = myImage;
            }
        } else if (typeof(theSource) === "object" && "previousSibling" in theSource) {
            // theSource is a node
            myImage = theSource;
        } else {
            Logger.error("Invalid type of source argument in addImage");
            return;
        }

        Protected.addTexture(myImage.id);
        Protected.myImages.push(myImage);
    };

    function setup() {
        if (theSource) {
            if (typeof(theSource) === "object" && "splice" in theSource) {
                // theSource is an array
                for (var i = 0; i < theSource.length; ++i) {
                    Protected.addImage(theSource[i]);
                }
            } else {
                Protected.addImage(theSource);
            }
            var myImage = Protected.myImages[0];
            var mySize = getImageSize(myImage);

            if (myImage.src.search(/dshow:\/\//) !== -1) {
                mySize = new Vector2i(myImage.width, myImage.height);
            }
            Public.width  = mySize.x;
            Public.height = mySize.y;
        }
    }

    setup();
};

/// Creates an overlay and with an image as content
//  @param theScene     Scene        The scene the overlay should be appended to
//  @param theSource    String/Node  Image file name or image node or array thereof
//  @param thePosition  Vector2f     Pixelposition of the overlay (optional, default is [0,0])
//  @param theParent    Node         Parent overlay node (optional, default is toplevel)
function ImageOverlay(theScene, theSource, thePosition, theParent) {
    var Protected = {};
    ImageOverlayBase.prototype.Constructor(this, Protected, theScene, theSource, thePosition, theParent);
}


/// Creates an overlay and with a movie as content
//  @param theScene       Scene        The scene the overlay should be appended to
//  @param theSource      String/Node  Movie file name or movie node
//  @param thePosition    Vector2f     Pixelposition of the overlay (optional, default is [0,0])
//  @param theParent      Node         Parent overlay node (optional, default is toplevel)
//  @param theAudioFlag   Boolean      Play audio with the movie (optional, default is true)
//  @param thePixelFormat String       The texture pixel format (optional, default is RGB)
//  @param theDecoderHint String       Which movie decoder to use (optional, default depends on file-extension)

function MovieOverlay(theScene, theSource, thePosition, theParent, theAudioFlag, thePixelFormat, theDecoderHint, thePlayMode) {
    var Protected = {};
    this.Constructor(this, Protected, theScene, theSource, thePosition, theParent, theAudioFlag, thePixelFormat, theDecoderHint, thePlayMode);
}

MovieOverlay.prototype.Constructor = function(Public, Protected, theScene, theSource, thePosition, theParent,
                          theAudioFlag, thePixelFormat, theDecoderHint, thePlayMode) {

    TextureOverlay.prototype.Constructor(Public, Protected, theScene, thePosition, theParent);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////////////////////////////

    var BaseRemoveFromScene = Public.removeFromScene;
    Public.removeFromScene = function() {
        if(Public.image) {
            Public.playmode = "stop";
        }
        BaseRemoveFromScene();
    };

    Public.__defineGetter__("movie", function() {
        return Public.image;
    });
    Public.__defineSetter__("movie", function(theMovie) {
        Public.image = theMovie;
    });

    Public.__defineGetter__("loopcount", function() { return Public.image.loopcount; });
    Public.__defineSetter__("loopcount", function(theArgument) { Public.image.loopcount = theArgument; });

    Public.__defineGetter__("playspeed", function() { return Public.image.playspeed; });
    Public.__defineSetter__("playspeed", function(theArgument) { Public.image.playspeed = theArgument; });
    
    Public.__defineGetter__("playmode", function() { return Public.image.playmode; });
    Public.__defineSetter__("playmode", function(theArgument) { Public.image.playmode = theArgument; });

    Public.__defineGetter__("audio", function() { return Public.image.audio; });
    Public.__defineSetter__("audio", function(theArgument) { Logger.error("Cannot set audio flag after construction"); });

    Public.__defineGetter__("avdelay", function() { return Public.image.avdelay; });
    Public.__defineSetter__("avdelay", function(theArgument) { Public.image.avdelay = theArgument; });

    Public.__defineGetter__("cachesize", function() { return Public.image.cachesize; });
    Public.__defineSetter__("cachesize", function(theArgument) { Public.image.cachesize = theArgument; });

    Public.__defineGetter__("fps", function() { return Public.image.fps; });
    Public.__defineSetter__("fps", function(theArgument) { Logger.error("Cannot set movie fps"); });

    Public.__defineGetter__("framecount", function() { return Public.image.framecount; });
    Public.__defineSetter__("framecount", function(theArgument) { Logger.error("Cannot set movie framecount"); });

    Public.__defineGetter__("volume", function() { return Public.image.volume; });
    Public.__defineSetter__("volume", function(theArgument) { Public.image.volume = theArgument; });

    Public.__defineGetter__("currentframe", function() { return Public.image.currentframe; });
    Public.__defineSetter__("currentframe", function(theArgument) { Public.image.currentframe = theArgument; });

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    function setup() {
        var myImage = null;
        if (typeof(theSource) === "object") {
            myImage = theSource;
        } else {
            myImage = Node.createElement("movie");
            theScene.images.appendChild(myImage);
            myImage.name = theSource;
            myImage.resize = "none";
            if (theAudioFlag === undefined) {
                theAudioFlag = true;
            }
            if (theDecoderHint) {
                myImage.decoderhint = theDecoderHint;
            } else {
                myImage.decoderhint = new Playlist().getVideoDecoderHintFromURL(myImage.src, false);
            }
            myImage.audio = theAudioFlag;
            if (thePixelFormat === null) {
                thePixelFormat = "RGB";
            }
            myImage.targetpixelformat = thePixelFormat;
            myImage.src  = expandEnvironment(theSource);
            if (thePlayMode !== undefined) {
                myImage.playmode = thePlayMode;
            }
        }
        var myNodeName = myImage.nodeName;
        if (myNodeName === "movie") {
            //window.scene.loadMovieFrame(myImage); // no need anymore, playmode handles that
        } else if (myNodeName === "capture") {
            window.scene.loadCaptureFrame(myImage);
        } else {
            throw new Exception("Unknown movie image node name: " + myNodeName, fileline());
        }
        var mySize = null;
        if (myImage.src.search(/\.m60/) !== -1) {
            mySize = getImageSize(myImage);
        } else {
            mySize = new Vector2i(myImage.width, myImage.height);
        }
        Protected.myImages.push(myImage);

        Public.width  = mySize.x;
        Public.height = mySize.y;
        Protected.addTexture(myImage.id);

        // YUV targetrasterformat allows us to use a shader to convert YUV2RGB, therefore we need 3 rasters
        if (myImage.childNodesLength() > 1) {
            Public.material.enabled = false;
        }
        for (var i = 1; i < myImage.childNodesLength(); i++) {
            var myTextureUnit = Public.textureunits.appendChild(Node.createElement("textureunit"));
            myTextureUnit.applymode = TextureApplyMode.modulate;
            var myTexture = Modelling.createTexture(window.scene, myImage);
            myTexture.image = myImage.id;
            myTexture.image_index = i;
            myTextureUnit.texture   = myTexture.id;
        }
        if (myImage.childNodesLength() > 1) {
            Public.material.requires.textures = Public.createTextureFeatureString(myImage.childNodesLength());
        }
        if (myImage.targetpixelformat === "YUV420") {
            Logger.info("using yuv2rgb shader to convert pixelformat");
            addMaterialRequirement(Public.material, "option", "[10[yuv2rgb]]");
        }
        if (! Public.material.enabled) {
            Public.material.enabled = true;
        }
    }

    setup();
};
