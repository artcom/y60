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
//  enum                VTextAlign    (Renderer.TOP_ALIGNMENT | CENTER_ALIGNMENT | BOTTOM_ALIGNMENT)
//  Vector4f            color
//  int                 topPad        (optional)
//  int                 bottomPad     (optional)
//  int                 rightPad      (optional)
//  int                 leftPad       (optional)
//  float               tracking      (optional, default 0)
//
///////////////////////////////////////////////////////////////////////////////////////////

var ourFontCache = [];

function LabelBase(Public, Protected, theSceneViewer,
                   theSize, thePosition, theStyle, theParent)
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Inheritance
    ///////////////////////////////////////////////////////////////////////////////////////////
    ImageOverlayBase(Public, Protected, theSceneViewer.getOverlayManager(), null,
                     thePosition, theParent);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.setText = function(theText, theStyle) {

        var myStyle = null;
        if (theStyle) {
            myStyle = theStyle;
        } else {
            myStyle = Protected.myStyle;
        }

        var topPad    = "topPad"    in myStyle ? myStyle.topPad : 0;
        var bottomPad = "bottomPad" in myStyle ? myStyle.bottomPad : 0;
        var rightPad  = "rightPad"  in myStyle ? myStyle.rightPad : 0;
        var leftPad   = "leftPad"   in myStyle ? myStyle.leftPad : 0;
        var tracking  = "tracking"  in myStyle ? myStyle.tracking : 0;
        window.setTextStyle(Renderer.BLENDED_TEXT);
        window.setTextPadding(topPad, bottomPad, leftPad, rightPad);
        window.setHTextAlignment(myStyle.HTextAlign);
        window.setVTextAlignment(myStyle.VTextAlign);
        window.setTextColor(myStyle.textColor, new Vector4f(1,1,1,1));
        window.setTracking(tracking);

        var myFontName = myStyle.font + "_" + myStyle.fontsize;
        if (!(myFontName in ourFontCache)) {
            window.loadTTF(myFontName, myStyle.font, myStyle.fontsize);
            ourFontCache[myFontName] = true;
        }

        if (theSize == null || theSize[0] == 0 || theSize[1] == 0) {
            // force recreation of image
            Public.image = theSceneViewer.getImageManager().getImageNode(createUniqueId());
            if (theSize == null || theSize[0] == 0) {
                Public.width = 0;
            }
            if (theSize == null || theSize[1] == 0) {
                Public.height = 0;
            }
        }
        ensureImage();
        var mySize = window.renderTextAsImage(Public.image, theText, myFontName, Public.width, Public.height);

        Public.srcsize.x = mySize[0] / Public.image.width;
        Public.srcsize.y = mySize[1] / Public.image.height;
        window.setHTextAlignment(Renderer.LEFT_ALIGNMENT);
        window.setVTextAlignment(Renderer.TOP_ALIGNMENT);
        window.setTextPadding(0,0,0,0);

        if (theSize == null) {
            Public.width = mySize[0];
            Public.height = mySize[1];
        } else if (theSize[0] == 0) {
            Public.width = mySize[0];
        } else if (theSize[1] == 0) {
            Public.height = mySize[1];
        }
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
        if (theSize) {
            Public.width  = theSize[0];
            Public.height = theSize[1];
        }
        Public.position.x = thePosition[0];
        Public.position.y = thePosition[1];
    }

    function ensureImage() {
        if (Public.image == null) {
            Public.image = theSceneViewer.getImageManager().getImageNode(createUniqueId());
            Public.texture.applymode = "decal";
        }
    }

    setup();
}

function Label(theSceneViewer, theText,
               theSize, thePosition, theStyle, theParent)
{
    var Public    = this;
    var Protected = {}
    LabelBase(Public, Protected, theSceneViewer,
              theSize, thePosition, theStyle, theParent);
    Public.setText(theText);
}
