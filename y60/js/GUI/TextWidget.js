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

function TextWidget() {
}

TextWidget.prototype.Constructor = function(Public, Protected, theParent, theTextWidgetNode, theDepth) {

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _myName   = theTextWidgetNode.name;
    var _myNode   = theTextWidgetNode;
    var _myParent = theParent;
    var _myDepth  = theDepth;

    // geometry
    var _myWidth  = Number(_myNode.width);
    var _myHeight = Number(_myNode.height);

    // scene
    var _myImage    = null;
    var _myImageId  = "";
    var _myMaterial = null;
    var _myQuad     = null;

    // background images
    var _myLeft       = null;
    var _myRight      = null;
    var _myBetween    = null;
    var _myBackground = null;

    // text positioning
    var _myTextX            = Number(_myNode.textX);
    var _myTextY            = Number(_myNode.textY);
    var _myTextWidth        = Number(_myNode.textWidth);
    var _myTextHeight       = Number(_myNode.textHeight);
    var _myTextSurfaceWidth = _myTextWidth;
    if("textSurfaceWidth" in _myNode) {
        _myTextSurfaceWidth = Number(_myNode.textSurfaceWidth);
    }

    // text rendering
    var _myTextImage   = null;
    var _myTextSurface = null;
    var _myTextStyle = fontStyleFromNode(_myNode);
    var _myText      = "";

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.__defineSetter__("visible", function(theVisible) {
        _myQuad.body.visible = theVisible;
    });

    Public.__defineSetter__("insensible", function(theInsensible) {
        _myQuad.body.insensible = theInsensible;
    });

    Public.__defineGetter__("body", function() {
        return _myQuad.body;
    });

    Public.__defineGetter__("bodies", function() {
        return [_myQuad.body];
    });

    Public.__defineSetter__("text", function(theText) {
        _myText = theText;
        Protected.redraw();
    });

    Public.__defineGetter__("text", function() {
        return _myText;
    });

    Public.__defineSetter__("background", function(theImage) {
        _myBackground = theImage;
        Protected.redraw();
    });

    Public.__defineGetter__("background", function() {
        return _myBackground;
    });

    Public.__defineGetter__("name", function() {
        return _myName;
    });

    ////////////////////////////////////////
    // Protected
    ////////////////////////////////////////

    Protected.redraw = function() {
        var mySurface = new CairoSurface(_myImage);
        var myContext = new Cairo(mySurface);

        Protected.cairoSurface   = mySurface;
        Protected.cairoContext   = myContext;
        Protected.cursorPosition = drawText();

        Protected.draw();

        mySurface.triggerUpload();

        Protected.cairoSurface   = null;
        Protected.cairoContext   = null;
        Protected.cursorPosition = null;
    }

    Protected.draw = function() {
        var mySurface = Protected.cairoSurface;
        var myContext = Protected.cairoContext;

        var myCursor  = Protected.cursorPosition;

        myContext.setOperator(Cairo.OPERATOR_CLEAR);
        myContext.setSourceRGBA(0,0,0,0);
        myContext.rectangle(0,0,_myWidth,_myHeight);
        myContext.fill();

        myContext.setOperator(Cairo.OPERATOR_OVER);

        if(_myBackground) {
             var myBackground = new CairoSurface(_myBackground);
             var myBackgroundWidth = myBackground.getWidth();
             var myBackgroundHeight = myBackground.getHeight();

             var myOffsetX = Math.round((_myWidth - myBackgroundWidth)/2);
             var myOffsetY = Math.round((_myHeight - myBackgroundHeight)/2);

             myContext.setSourceSurface(myBackground,myOffsetX,myOffsetY);
             myContext.rectangle(myOffsetX,myOffsetY,myBackgroundWidth,myBackgroundHeight);
             myContext.fill();
        } else {
             var myLeft = new CairoSurface(_myLeft);
             myContext.setSourceSurface(myLeft,0,0);
             myContext.rectangle(0,0,_myLeft.raster.width, _myHeight);
             myContext.fill();

             var myRight = new CairoSurface(_myRight);
             var myRightOffset = _myTextX + myCursor[0] + (_myTextX-_myLeft.raster.width);
             myContext.setSourceSurface(myRight,myRightOffset,0);
             myContext.rectangle(myRightOffset, 0, _myRight.raster.width, _myHeight);
             myContext.fill();

             var myBetween = new CairoSurface(_myBetween);
             var myBetweenPattern = new CairoPattern(myBetween);
             myBetweenPattern.setExtend(Cairo.EXTEND_REPEAT);
             myContext.setSource(myBetweenPattern);
             myContext.rectangle(_myLeft.raster.width,0,
                                 myRightOffset-_myLeft.raster.width,_myHeight);
             myContext.fill();
        }

        var myOffsetX = 0;
        if(myCursor[0] > _myTextWidth) {
            myOffsetX += myCursor[0] - _myTextWidth;
        }
        myContext.setSourceSurface(_myTextSurface, _myTextX - myOffsetX, _myTextY);
        myContext.rectangle(_myTextX, _myTextY, _myTextWidth, _myTextHeight);
        myContext.fill();
    }

    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////

    function drawText() {
        textToImage(_myTextImage, _myText, _myTextStyle, new Vector2f(_myTextSurfaceWidth, _myTextHeight));
        saveImage(_myTextImage, "tmp.png");

        window.scene.images.removeChild(_myTextImage);
        _myTextSurface = new CairoSurface("tmp.png");
        var myCursor = window.getTextCursorPosition();
        return myCursor;
    }

    ////////////////////////////////////////
    // setup
    ////////////////////////////////////////

    Protected.setup = function() {
        // background images
        if("leftImage" in _myNode && "rightImage" in _myNode && "betweenImage" in _myNode) {
            _myLeft = getCachedImage(CONTENT + "/" + _myNode.leftImage);
            _myLeft.resize = "none";
            _myRight = getCachedImage(CONTENT + "/" + _myNode.rightImage);
            _myRight.resize = "none";
            _myBetween = getCachedImage(CONTENT + "/" + _myNode.betweenImage);
            _myBetween.resize = "none";
        } else {
            _myBackground = getCachedImage(CONTENT + "/" + _myNode.backgroundImage);
            _myBackground.resize = "none";
        }

        if("z" in _myNode) {
            _myDepth += Number(_myNode.z);
        }

        // text image
        _myTextImage = Modelling.createImage(window.scene, _myTextSurfaceWidth, _myTextHeight, "RGBA");
        _myTextImage.name = _myName + "-text";
        _myTextImage.resize = "none";

        // our texture
        _myImage = Modelling.createImage(window.scene, _myWidth, _myHeight, "BGRA");
        _myImage.name = _myName + "-surface";
        _myImage.raster.fillRect(0, 0, _myWidth, _myHeight, [0,0,0,1]);

        // our material
        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, _myImage, true);
        _myMaterial.transparent = true;

        print(_myMaterial);
        print(_myMaterial.childNode("textureunits").firstChild.texture);
        // _myImage.resize = "none";
        // _myImage.wrapmode = "clamp";

        // our body and shape
        _myQuad = createQuad(theParent, _myName, new Vector2f(_myWidth, _myHeight),
                             new Vector3f(_myNode.x, _myNode.y, _myDepth),
                             _myMaterial, true, true);

        if("text" in _myNode) {
          Public.text = _myNode.text;
        }

        // do a redraw
        Protected.redraw();
    }
}
