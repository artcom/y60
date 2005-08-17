//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Button.js,v $
//   $Author: christian $
//   $Revision: 1.21 $
//   $Date: 2004/07/28 09:16:54 $
//
//
//=============================================================================

///////////////////////////////////////////////////////////////////////////////////////////
//
//  Label Style properties:
//
//  string              font          (Path to font)
//  int                 fontsize      (Size of font)
//  Vector4f            textColor
//  enum                HTextAlign    (Renderer.CENTER_ALIGNMENT | LEFT_ALIGNMENT | RIGHT_ALIGNMENT)
//  enum                VTextAlign    (Renderer.TOP_ALIGNMENT | CENTER_ALIGNMENT | BOTTOM_ALIGNMENT)
//  Vector4f            color
//  int                 topPad        (optional)
//  int                 bottomPad     (optional)
//  int                 rightPad      (optional)
//  int                 leftPad       (optional)
//
///////////////////////////////////////////////////////////////////////////////////////////

var ourFontCache = [];

function LabelBase(Public, Protected, theSceneViewer, theSize, thePosition, theStyle) {
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Inheritance
    ///////////////////////////////////////////////////////////////////////////////////////////
    ImageOverlayBase(Public, Protected, theSceneViewer.getOverlayManager(), null, thePosition);
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.setText = function(theText, theStyle) {
        if (theStyle) {
            Protected.myStyle = theStyle;
        }

        var topPad    = "topPad"    in Protected.myStyle ? Protected.myStyle.topPad : 0;
        var bottomPad = "bottomPad" in Protected.myStyle ? Protected.myStyle.bottomPad : 0;
        var rightPad  = "rightPad"  in Protected.myStyle ? Protected.myStyle.rightPad : 0;
        var leftPad   = "leftPad"   in Protected.myStyle ? Protected.myStyle.leftPad : 0;
        window.setTextStyle(Renderer.BLENDED_TEXT);
        window.setTextPadding(topPad, bottomPad, leftPad, rightPad);
        window.setHTextAlignment(Protected.myStyle.HTextAlign);
        window.setVTextAlignment(Protected.myStyle.VTextAlign);
        window.setTextColor(Protected.myStyle.textColor, new Vector4f(1,1,1,1));

        var myFontName = Protected.myStyle.font + "_" + Protected.myStyle.fontsize;
        if (!(myFontName in ourFontCache)) {
            window.loadTTF(myFontName, Protected.myStyle.font, Protected.myStyle.fontsize);
            ourFontCache[myFontName] = true;
        }

        ensureImage();
        var mySize = window.renderTextAsImage(Public.image, theText, myFontName, Public.width, Public.height);

        Public.srcsize.x = mySize[0] / Public.image.width;
        Public.srcsize.y = mySize[1] / Public.image.height;
        window.setHTextAlignment(Renderer.LEFT_ALIGNMENT);
        window.setVTextAlignment(Renderer.TOP_ALIGNMENT);
        window.setTextPadding(0,0,0,0);
        Public.texture.applymode = "decal";
    }

    Public.setImage = function(theSource) {
        ensureImage();
        Public.image.src = theSource;
        window.scene.update(Scene.IMAGES);
        var mySize = getImageSize(Public.image);
        Public.width  = mySize.x;
        Public.height = mySize.y;
    }

    Public.setBackgroundTransparent = function() {
        Public.texture.applymode = "modulate";
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Protected
    ///////////////////////////////////////////////////////////////////////////////////////////

    Protected.myStyle     = theStyle;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    function setup() {
        Public.color = Protected.myStyle.color;
        Public.width  = theSize[0];
        Public.height = theSize[1];
        Public.position.x = thePosition[0];
        Public.position.y = thePosition[1];
    }


    function ensureImage() {
        if (Public.image == null) {
            Public.image = theSceneViewer.getImageManager().getImageNode(createUniqueId());
        }
    }
    
    setup();
}

function Label(theSceneViewer, theText, theSize, thePosition, theStyle) {
    var Public    = this;
    var Protected = {}
    LabelBase(Public, Protected, theSceneViewer, theSize, thePosition, theStyle);
    Public.setText(theText);
}