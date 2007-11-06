//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy Protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("ImageManager.js");
use("Y60JSSL.js");
use("Playlist.js");

var ourOverlayCounter = 0;

// Just a test...
var ourImageCache = [];

// Pure virtual base class
function OverlayBase(theScene, thePosition, theParent) {
    var Protected = [];
    this.Constructor(this, Protected, theScene, thePosition, theParent);
    exit(0);
}

OverlayBase.prototype.Constructor = function(Public, Protected, theScene, thePosition, theParent) {

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Delegation of all overlay node attributes
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.toString = function() {
        return _myNode.toString();
    }

    Public.node getter = function() { return _myNode; }

    Public.name getter = function() { return _myNode.name; }
    Public.name setter = function(theArgument) { _myNode.name = theArgument; }

    Public.visible getter = function() { return _myNode.visible; }
    Public.visible setter = function(theArgument) { _myNode.visible = theArgument; }

    Public.position getter = function() { return _myNode.position; }
    Public.position setter = function(theArgument) { _myNode.position = theArgument; }

    Public.scale getter = function() { return _myNode.scale; }
    Public.scale setter = function(theArgument) { _myNode.scale = theArgument; }

    Public.rotation getter = function() { return _myNode.rotation; }
    Public.rotation setter = function(theArgument) { _myNode.rotation = theArgument; }

    Public.pivot getter = function() { return _myNode.pivot; }
    Public.pivot setter = function(theArgument) { _myNode.pivot = theArgument; }

    Public.srcorigin getter = function() { return _myNode.srcorigin; }
    Public.srcorigin setter = function(theArgument) { _myNode.srcorigin = theArgument; }

    Public.srcsize getter = function() { return _myNode.srcsize; }
    Public.srcsize setter = function(theArgument) { _myNode.srcsize = theArgument; }

    Public.alpha getter = function() { return _myNode.alpha; }
    Public.alpha setter = function(theArgument) { _myNode.alpha = theArgument; }

    Public.width getter = function() { return _myNode.width; }
    Public.width setter = function(theArgument) { _myNode.width = theArgument; }

    Public.height getter = function() { return _myNode.height; }
    Public.height setter = function(theArgument) { _myNode.height = theArgument; }

    Public.bordercolor getter = function() { return _myNode.bordercolor; }
    Public.bordercolor setter = function(theArgument) { _myNode.bordercolor = theArgument; }

    Public.borderwidth getter = function() { return _myNode.borderwidth; }
    Public.borderwidth setter = function(theArgument) { _myNode.borderwidth = theArgument; }

    Public.topborder getter = function() { return _myNode.topborder; }
    Public.topborder setter = function(theArgument) { _myNode.topborder = theArgument; }

    Public.bottomborder getter = function() { return _myNode.bottomborder; }
    Public.bottomborder setter = function(theArgument) { _myNode.bottomborder = theArgument; }

    Public.leftborder getter = function() { return _myNode.leftborder; }
    Public.leftborder setter = function(theArgument) { _myNode.leftborder = theArgument; }

    Public.rightborder getter = function() { return _myNode.rightborder; }
    Public.rightborder setter = function(theArgument) { _myNode.rightborder = theArgument; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Utility functions
    ///////////////////////////////////////////////////////////////////////////////////////////

    /// Removes the overlay and all its children and all
    /// connected materials and images from the scene dom.
    /// Do not call this function, if you share materials or images.
    Public.removeFromScene = function() {
        removeOverlay(_myNode);
    }

    /// Moves overlay to first position in the overlay z-order
    Public.moveToTop = function() {
        var myParent = _myNode.parentNode;
        myParent.removeChild(_myNode);
        myParent.appendChild(_myNode);
    }

    /// Moves overlay to last position in the overlay z-order
    Public.moveToBottom = function() {
        var myParent = _myNode.parentNode;
        myParent.removeChild(_myNode);
        myParent.insertBefore(_myNode, myParent.firstChild);
    }

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
        while (myParent.nodeName == "overlay") {
            myParentMatrix.makeScaling(new Vector3f(myParent.scale.x, myParent.scale.y,1.0));

            myParentMatrix.rotateZ(myParent.rotation);
            myParentMatrix.translate(new Vector3f(myParent.position.x,myParent.position.y,0));
            myMatrix.postMultiply(myParentMatrix);

            myParent = myParent.parentNode;
        }

        myMatrix.invert();
        var myNewPoint = product(new Vector3f(theXPos, theYPos,0), myMatrix);

        if (theSquareSize != undefined) {
            var myOverlayBox = new Box3f([0, 0, 0],
                [_myNode.width - 1, _myNode.height - 1, 1]);
            var myMouseBox = new Box3f([myNewPoint[0] - theSquareSize, myNewPoint[1] - theSquareSize, 0],
                [myNewPoint[0] + theSquareSize, myNewPoint[1] + theSquareSize, 1]);
            return myMouseBox.intersects(myOverlayBox);
        } else {
            if (0 <= myNewPoint[0] && 0 <= myNewPoint[1] &&
                _myNode.width > myNewPoint[0] && _myNode.height > myNewPoint[1])
            {
                return true;
            }
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    /// Create an overlay without material
    function setup() {
        var myParent = null;
        if (theParent != undefined) {
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

    var _myNode = null;

    setup();
}

// Pure virtual base class
function MaterialOverlay(theScene, thePosition, theParent) {
    var Protected = [];
    this.Constructor(this, Protected, theScene, thePosition, theParent);
    exit(0);
}
MaterialOverlay.prototype.Constructor = function(Public, Protected, theScene, thePosition, theParent) {

    OverlayBase.prototype.Constructor(Public, Protected, theScene, thePosition, theParent);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Convenience access to referenced nodes
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.material getter = function() {
        return _myMaterial;
    }
    Public.material setter = function(theMaterial) {
        Public.node.material = theMaterial.id;
        _myMaterial      = theMaterial;

        // Notify derived classes
        Protected.onMaterialChange();
    }

    Public.color getter = function() {
        return _myMaterial.properties.surfacecolor;
    }

    Public.color setter = function(theColor) {
        _myMaterial.properties.surfacecolor = asColor(theColor);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Protected
    ///////////////////////////////////////////////////////////////////////////////////////////

    /// Overload this function to get notified on material changes
    Protected.onMaterialChange = function() {}

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

    var _myMaterial = null;

    setup();
}


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
}

// Pure virtual base class
function TextureOverlay(theScene, thePosition, theParent) {
    var Protected = [];
    this.Constructor(this, Protected, theScene, thePosition, theParent);
}

TextureOverlay.prototype.Constructor = function(Public, Protected, theScene, thePosition, theParent) {
    var Base = {}
    MaterialOverlay.prototype.Constructor(Public, Protected, theScene, thePosition, theParent);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.src getter = function() { return Public.image.src; }
    Public.src setter = function(theArgument) { Public.image.src = expandEnvironment(theArgument); }

    Public.image getter = function() {
        if (Protected.myImages.length) {
            return Protected.myImages[0];
        } else {
            return null;
        }
    }
    Public.image setter = function(theImage) {
        Protected.myImages[0] = theImage;
        if (_myTextures) {
            _myTextures.childNodes[0].image = theImage.id;
        } else {
            Protected.addTexture(theImage.id);
        }
    }

    Public.images setter = function(theImages) {
        for (var i = 0; i < theImages.length; ++i) {
            if (i < _myTextures.childNodes.length) {
                _myTextures.childNodes[i].image = theImages[i].id;
            } else {
                Protected.addTexture(theImages[i].id);
            }
        }
        i = _myTextures.childNodes.length;
        while (i-- > theImages.length) {
            _myTextures.removeChild(_myTextures.lastChild);
        }
        addTextureRequirements(_myTextures.childNodesLength());
        Protected.myImages = theImages;
    }

    Public.images getter = function() {
        /*
        var myImages = [];
        for (var i = 0; i < Protected.myImages.length; ++i) {
            myImages.push(Protected.myImages[i]);
        }*/
        return Protected.myImages;
    }

    Public.textures getter = function() {
        return _myTextures;
    }

    Public.textures setter = function(theTextures) {
        for (var i = 0; i < theTextures.length; ++i) {
            if (i < _myTextures.childNodes.length) {
                _myTextures.replaceChild(theTextures[i], _myTextures.childNodes[i]);
            } else {
                _myTextures.appendChild(theTextures[i]);
            }
        }
        i = _myTextures.childNodes.length;
        while (i-- > theTextures.length) {
            _myTextures.removeChild(_myTextures.lastChild);
        }
        Protected.onTextureChange();
    }

    Public.texture getter = function() {
        return _myTextures.childNodes[0];
    }

    Public.texture setter = function(theTexture) {
        _myTextures.replaceChild(theTexture, _myTextures.firstChild);
        Protected.onTextureChange();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Protected
    ///////////////////////////////////////////////////////////////////////////////////////////

    Protected.onTextureChange = function() {
        Protected.myImages = [];
        for (var i = 0; i < _myTextures.childNodes.length; ++i) {
            var myId = _myTextures.childNodes[i].image;
            var myImage = _myTextures.getElementById(myId);
            if (myImage == null) {
                throw new Exception("Could not find image with id: " + myId, fileline());
            }
            Protected.myImages.push(myImage);
        }
    }

    Protected.onMaterialChange = function() {
        _myTextures = getDescendantByTagName(Public.material, "textures", false);
        
        if (_myTextures.childNodes.length != 1) {
            throw new Exception("TextureOverlay can only have one texture, but it has: " + _myTextures.childNodes.length, fileline());
        }

        Protected.onTextureChange();
    }

    Protected.addTexture = function(theImageId) {
        _myTextures = getDescendantByTagName(Public.material, "textures", false);
        if (!_myTextures) {
            _myTextures = Public.material.appendChild(Node.createElement("textures"));
        }
        var myTexture  = _myTextures.appendChild(Node.createElement("texture"));
        myTexture.applymode = TextureApplyMode.modulate;
        myTexture.image     = theImageId;
        addTextureRequirements(_myTextures.childNodesLength());
        return myTexture;
    }

    Protected.myImages = [];

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    function createTextureFeatureString(theTextureCount) {
        var myString = '[100[';
        for (var i = 0; i < theTextureCount; ++i) {
            myString += "paint";
            if (i != theTextureCount - 1) {
                myString += ",";
            }
        }
        myString += ']]';
        return myString;
    }
    function createTexcoordFeatureString(theTextureCount) {
        var myString = '[100[';
        for (var i = 0; i < theTextureCount; ++i) {
            myString += "uv_map";
            if (i != theTextureCount - 1) {
                myString += ",";
            }
        }
        myString += ']]';
        return myString;
    }

    function addTextureRequirements(theTextureCount) {
        var myTextureFeatures = getDescendantByAttribute(Public.material.requires, "name", "textures", false);
        if (myTextureFeatures == null) {
            myTextureFeatures = new Node('<feature name="textures">[10[sds]]</feature>\n').firstChild;
            Public.material.requires.appendChild(myTextureFeatures);
        }
        myTextureFeatures = getDescendantByAttribute(Public.material.requires, "name", "texcoord", false);
        if (myTextureFeatures == null) {
            myTextureFeatures = new Node('<feature name="texcoord">[10[sds]]</feature>\n').firstChild;
            Public.material.requires.appendChild(myTextureFeatures);
        }
        Public.material.requires.textures = createTextureFeatureString(theTextureCount);
        Public.material.requires.texcoord = createTexcoordFeatureString(theTextureCount);

    }

    var _myTextures = null;
}

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
        if (typeof(theSource) == "string") {
            var mySource = expandEnvironment(theSource);
            if (mySource in ourImageCache) {
                myImage = ourImageCache[mySource];
            } else {
                // theSource is a string
                myImage = Node.createElement("image");
                myImage.src  = expandEnvironment(theSource);
                myImage.resize = "pad";
                myImage.mipmap = false;
                theScene.images.appendChild(myImage);
                myImage.name = mySource;
                myImage.wrapmode  = TextureWrapMode.repeat;
                myImage.type = ImageType.single;                
                //ourImageCache[mySource] = myImage;
            }
        } else if (typeof(theSource) == "object" && "previousSibling" in theSource) {
            // theSource is a node
            myImage = theSource;
        } else {
            Logger.error("Invalid type of source argument in addImage");
            return;
        }

        Protected.addTexture(myImage.id);
        Protected.myImages.push(myImage);
    }

    function setup() {
        if (theSource) {
            if (typeof(theSource) == "object" && "splice" in theSource) {
                // theSource is an array
                for (var i = 0; i < theSource.length; ++i) {
                    Protected.addImage(theSource[i]);
                }
            } else {
                Protected.addImage(theSource);
            }
            var myImage = Protected.myImages[0];
            var mySize = getImageSize(myImage);

            if (myImage.src.search(/dshow:\/\//) != -1) {
                mySize = new Vector2i(myImage.width, myImage.height);
            }
            Public.width  = mySize.x;
            Public.height = mySize.y;
        }
    }

    setup();
}

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

function MovieOverlay(theScene, theSource, thePosition, theParent, theAudioFlag, thePixelFormat, theDecoderHint) {
    var Protected = {};
    this.Constructor(this, Protected, theScene, theSource, thePosition, theParent, theAudioFlag, thePixelFormat, theDecoderHint);
}

MovieOverlay.prototype.Constructor = function(Public, Protected, theScene, theSource, thePosition, theParent,
                          theAudioFlag, thePixelFormat, theDecoderHint) {

    TextureOverlay.prototype.Constructor(Public, Protected, theScene, thePosition, theParent);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.movie getter = function() {
        return Public.image;
    }
    Public.movie setter = function(theMovie) {
        Public.image = theMovie;
    }

    Public.loopcount getter = function() { return Public.image.loopcount; }
    Public.loopcount setter = function(theArgument) { Public.image.loopcount = theArgument; }

    Public.playspeed getter = function() { return Public.image.playspeed; }
    Public.playspeed setter = function(theArgument) { Public.image.playspeed = theArgument; }

    Public.playmode getter = function() { return Public.image.playmode; }
    Public.playmode setter = function(theArgument) { Public.image.playmode = theArgument; }

    Public.color_scale getter = function() { return Public.image.color_scale; }
    Public.color_scale setter = function(theArgument) { Public.image.color_scale = theArgument; }

    Public.startime getter = function() { return Public.image.startime; }
    Public.startime setter = function(theArgument) { Public.image.startime = theArgument; }

    Public.audio getter = function() { return Public.image.audio; }
    Public.audio setter = function(theArgument) { Logger.error("Cannot set audio flag after construction"); }

    Public.avdelay getter = function() { return Public.image.avdelay; }
    Public.avdelay setter = function(theArgument) { Public.image.avdelay = theArgument; }

    Public.cachesize getter = function() { return Public.image.cachesize; }
    Public.cachesize setter = function(theArgument) { Public.image.cachesize = theArgument; }

    Public.fps getter = function() { return Public.image.fps; }
    Public.fps setter = function(theArgument) { Logger.error("Cannot set movie fps"); }

    Public.framecount getter = function() { return Public.image.framecount; }
    Public.framecount setter = function(theArgument) { Logger.error("Cannot set movie framecount"); }

    Public.volume getter = function() { return Public.image.volume; }
    Public.volume setter = function(theArgument) { Public.image.volume = theArgument; }

    Public.currentframe getter = function() { return Public.image.currentframe; }
    Public.currentframe setter = function(theArgument) { Public.image.currentframe = theArgument; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    function setup() {
        var myImage = null;
        if (typeof(theSource) == "object") {
            myImage = theSource;
        } else {
            myImage = Node.createElement("movie");
            myImage.name = theSource;
            myImage.resize = "none";
            myImage.src  = expandEnvironment(theSource);
            theScene.images.appendChild(myImage);
            if (theAudioFlag == undefined) {
                theAudioFlag = true;
            }
            if (theDecoderHint) {
                myImage.decoderhint = theDecoderHint;
            } else {
                myImage.decoderhint = new Playlist().getVideoDecoderHintFromURL(myImage.src, false);
            }
            myImage.audio = theAudioFlag;
            if (thePixelFormat == undefined) {
                thePixelFormat = "RGB";
            }
            myImage.texturepixelformat = thePixelFormat;
        }
        var myNodeName = myImage.nodeName;
        if (myNodeName == "movie") {
            window.scene.loadMovieFrame(myImage);
        } else if (myNodeName == "capture") {
                window.scene.loadCaptureFrame(myImage);
        } else {
            throw new Exception("Unknown movie image node name: " + myNodeName, fileline());
        }
        var mySize = null;
        if (myImage.src.search(/\.m60/) != -1) {
            mySize = getImageSize(myImage);
        } else {
            mySize = new Vector2i(myImage.width, myImage.height);
        }
        Protected.myImages.push(myImage);

        Public.width  = mySize.x;
        Public.height = mySize.y;
        Protected.addTexture(myImage.id);
    }

    setup();
}

/// Creates a group overlay
// @param theScene     Scene     The scene the overlay should be appended to
// @param theName      String    Name of the overlay group (optional)
// @param thePosition  Vector2f  Pixelposition of the overlay (optional, default is [0,0])
// @param theParent    Node      Parent overlay node (optional, optional default is toplevel)
function GroupOverlay(theScene, theName, thePosition, theParent) {
    var Public    = this;
    var Protected = {};
    OverlayBase.prototype.Constructor(Public, Protected, theScene, thePosition, theParent);
    if (theName != undefined && theName) {
        Public.name = theName;
    }
}

function removeOverlay(theOverlayNode) {
    // Remove children
    while(theOverlayNode.childNodes.length) {
        removeOverlay(theOverlayNode.firstChild);
    }

    if (theOverlayNode.material) {
        var myMaterialNode = theOverlayNode.getElementById(theOverlayNode.material);
        if (myMaterialNode) {

            // Remove images
            var myTextures = getDescendantByTagName(myMaterialNode, "textures", false);
            
            if (myTextures) {
                for (var i = 0; i < myTextures.childNodes.length; ++i) {
                    var myImageId = myTextures.childNode(i).image;
                    var myImage = theOverlayNode.getElementById(myImageId);
                    if (myImage) {        
                        myImage.parentNode.removeChild(myImage);
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
