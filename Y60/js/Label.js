//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////////////////
//
//  Label Style properties:
//
//  string              font          (Path to font)
//  int                 fontsize      (Size of font)
//  Vector4f            textColor
//  enum                HTextAlign    (Renderer.CENTER_ALIGNMENT | LEFT_ALIGNMENT | RIGHT_ALIGNMENT)
//                                    (optional, default LEFT_ALIGNMENT)
//  enum                VTextAlign    (Renderer.TOP_ALIGNMENT | CENTER_ALIGNMENT | BOTTOM_ALIGNMENT)
//                                    (optional, default TOP_ALIGNMENT)
//  Vector4f            color         (background color)
//  int                 topPad        (optional)
//  int                 bottomPad     (optional)
//  int                 rightPad      (optional)
//  int                 leftPad       (optional)
//  float               tracking      (optional, default 0)
//
///////////////////////////////////////////////////////////////////////////////////////////

use("Overlay.js");

var ourFontCache = [];

function LabelBase(Public, Protected, theScene,
                   theSize, thePosition, theStyle, theParent)
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Inheritance
    ///////////////////////////////////////////////////////////////////////////////////////////
    ImageOverlayBase(Public, Protected, theScene, null, thePosition, theParent);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.setText = function(theText, theStyle) {
        if (theText == "") {
            theText = " ";
        }
        var myStyle = null;
        if (theStyle) {
            myStyle = clone(theStyle);
        } else {
            myStyle = Public.style;
        }

        var topPad     = "topPad"      in myStyle ? myStyle.topPad : 0;
        var bottomPad  = "bottomPad"   in myStyle ? myStyle.bottomPad : 0;
        var rightPad   = "rightPad"    in myStyle ? myStyle.rightPad : 0;
        var leftPad    = "leftPad"     in myStyle ? myStyle.leftPad : 0;
        var tracking   = "tracking"    in myStyle ? myStyle.tracking : 0;
        var lineHeight = "lineHeight"  in myStyle ? myStyle.lineHeight : 0;
        var HTextAlign = "HTextAlign"  in myStyle ? myStyle.HTextAlign : Renderer.LEFT_ALIGNMENT;
        var VTextAlign = "VTextAlign"  in myStyle ? myStyle.VTextAlign : Renderer.TOP_ALIGNMENT;

        window.setTextPadding(topPad, bottomPad, leftPad, rightPad);
        window.setHTextAlignment(HTextAlign);
        window.setVTextAlignment(VTextAlign);
        window.setTextColor(myStyle.textColor);
        window.setTracking(tracking);
        window.setLineHeight(lineHeight);

        var myFontName = loadFont(myStyle.font, myStyle.fontsize);

        var myImage = Protected.getImageNode();
        _myTextSize = window.renderTextAsImage(myImage, theText, myFontName,
                        theSize&&theSize[0]>0?Public.width:null,
                        theSize&&theSize[1]>0?Public.height:null);

        Public.srcsize.x = _myTextSize[0] / myImage.width;
        Public.srcsize.y = _myTextSize[1] / myImage.height;
        window.setHTextAlignment(Renderer.LEFT_ALIGNMENT);
        window.setVTextAlignment(Renderer.TOP_ALIGNMENT);
        window.setTextPadding(0,0,0,0);

        if (theSize == null) {
            Public.width = _myTextSize[0];
            Public.height = _myTextSize[1];
        } else if (theSize[0] <= 0) {
            Public.width = _myTextSize[0];
        } else if (theSize[1] <= 0) {
            Public.height = _myTextSize[1];
        }

        return _myTextSize;
    }

    // Get the size of the rendered text.
    Public.getTextSize = function() {
        return _myTextSize;
    }

    // Use this function if you need a foreground text that is independently
    // transparent from the background
    Public.setChildText = function(theText, theStyle) {
        if (_myChildLabel) {
            _myChildLabel.setText(theText, theStyle);
        } else {
            _myChildLabel = new Label(theScene, theText, theSize, [0,0], theStyle, Public.node);
            _myChildLabel.color = new Vector4f(1,1,1,1);
        }
    }

    // Use this function if you need a foreground image that is independently
    // transparent from the background
    Public.setChildImage = function(theSource) {
        if (_myChildLabel) {
            _myChildLabel.setImage(theSource);
        } else {
            _myChildLabel = new ImageLabel(theScene, theSource, [0, 0], theStyle, Public.node);
            _myChildLabel.color = new Vector4f(1,1,1,1);
        }
    }

    Public.setImage = function(theSource) {
        var myImage = Protected.getImageNode();
        myImage.src = theSource;
        var mySize = getImageSize(myImage);
        Public.width  = mySize.x;
        Public.height = mySize.y;
        Public.srcsize.x = 1;
        Public.srcsize.y = 1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Protected
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.style = clone(theStyle);

    Protected.getImageNode = function() {
        if (Public.image == null) {
            var myImage = theScene.images.appendChild(new Node("<image/>").firstChild);
            myImage.resize = "pad";
            Protected.addImage(myImage);
            Public.textures.lastChild.applymode = "modulate";
        }
        return Public.image;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Loads a font into the fontcache.
     * @param {string|Object} theFont The path to the ttf font file.
     * Or {regular: "regular.ttf", bold: "bold.ttf", italic: "italic.ttf"}
     * @param {int} theSize The font size.
     */
    function loadFont(theFont, theSize) {
        var myFontName = null;
        if (typeof(theFont) == "string") {
            myFontName = theFont + "_" + theSize;
            loadFontFace(myFontName, theFont, theSize, 0);
        } else {
            myFontName = theFont.regular + "_" + theSize;
            loadFontFace(myFontName, theFont.regular, theSize, 0);

            if ("bold" in theFont) {
                loadFontFace(myFontName, theFont.bold, theSize, Renderer.BOLD);
            }
            if ("italic" in theFont) {
                loadFontFace(myFontName, theFont.italic, theSize, Renderer.ITALIC);
            }
            if ("bolditalic" in theFont) {
                loadFontFace(myFontName, theFont.bolditalic, theSize, Renderer.BOLDITALIC);
            }
        }

        return myFontName;
    }

    function loadFontFace(theFontName, theFont, theSize, theFace) {
        var myFontCacheName = theFontName + "_" + theSize + "_" + theFace;
        if (!(myFontCacheName in ourFontCache)) {
            window.loadTTF(theFontName, theFont, theSize, theFace);
            ourFontCache[myFontCacheName] = true;
        }
    }

    function setup() {
        Public.color = Public.style.color;
        if (theSize) {
            Public.width  = theSize[0];
            Public.height = theSize[1];
        }
        if (thePosition) {
            Public.position.x = thePosition[0];
            Public.position.y = thePosition[1];
        }
    }

    var _myChildLabel = null;
    var _myTextSize = [-1,-1];

    setup();
}

function Label(theScene, theText, theSize, thePosition, theStyle, theParent) {
    var Public    = this;
    var Protected = {}
    LabelBase(Public, Protected, theScene, theSize, thePosition, theStyle, theParent);
    Public.setText(theText);
}

function ImageLabel(theScene, theSource, thePosition, theStyle, theParent) {
    var Public    = this;
    var Protected = {}
    LabelBase(Public, Protected, theScene, null, thePosition, theStyle, theParent);
    Public.setImage(theSource);
}

function BackgroundImageLabel(theScene, theText, theBackgroundImageSrc, theSize,
    thePosition, theStyle, theParent)
{
    var Public    = this;
    var Protected = {}
    LabelBase(Public, Protected, theScene, theSize, thePosition, theStyle, theParent);

    Protected.getImageNode = function() {
        if (Public.image == null) {
            Protected.addImage(theBackgroundImageSrc);
            var myImage = theScene.createImage("");
            Protected.addImage(myImage);
        }
        return Public.images[Public.images.length - 1];
    }

    Public.setText(theText);

    Public.setBackgroundImage = function(theImageSrc) {
        Public.image.src = theImageSrc;
    }
}
