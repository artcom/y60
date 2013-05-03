/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2010, ART+COM AG Berlin, Germany <www.artcom.de>
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

/*jslint white: true, plusplus: false*/
/*globals spark, searchFile, Logger, window, Renderer, Node, Vector2f,
          Matrix4f, applyImageFilter, Vector3f, asColor, Modelling */

/**
 * Text rendering utilities for SPARK
 *
 * This bunch of code is used to coerce Y60 font rendering
 * into something vaguely useful. Y60 centralizes font rendering
 * state on the Window. This code manages this state.
 *
 * The main purpose of this code is recollecting all style-related
 * state into DOM nodes representing font styles.
 *
 * Apart from that, this code provides the function renderText,
 * which is a wart representing the overall interface that has been
 * found to be required for text rendering in SPARK.
 */

/**
 * Internal: table indicating loading state of fonts
 */
spark.ourLoadedFonts = {};

/**
 * sdl_ttf renders big fonts better than small ones, so this is for adjusting the rendering size
 */
spark.fontScale = 1;

/**
 * Internal: ensure loading of font appropriate for the given style
 */

spark.loadFont = function (theName, theSize, theFontScale, theFontStyle, theHinting, theAscendOffset) {
    if (theFontScale == undefined) {
        theFontScale = 1;
    }
    if (theAscendOffset == undefined) {
        theAscendOffset = 0;
    }
    var myName = theName + "-" + theFontStyle + "-" + theSize + "-" + theHinting + "-" + theAscendOffset + "-" + theFontScale;
    if (!(myName in spark.ourLoadedFonts)) {
        if (theFontStyle != "normal") {
            spark.loadFont(theName, theSize, theFontScale, "normal", theHinting, theAscendOffset);
        }

        Logger.info("Loading font " + theName + " with size " + theSize + " and fonststyle " + theFontStyle+
                    " and hinting:"+theHinting + " and ascend offset:" + theAscendOffset);

        // XXX: this is a remnant from before the introduction
        //      of include-path-based font loading.
        var myFontPath = null;
        if (searchFile(theName + "-" + theFontStyle + ".otf")) {
            myFontPath = theName + "-" + theFontStyle + ".otf";
        } else if (searchFile(theName + "-" + theFontStyle + ".ttf")) {
            myFontPath = theName + "-" + theFontStyle + ".ttf";
        } else if (searchFile("FONTS/" + theName + "-" + theFontStyle + ".otf")) {
            myFontPath = "FONTS/" + theName + "-" + theFontStyle + ".otf";
        } else if (searchFile("FONTS/" + theName + "-" + theFontStyle + ".ttf")) {
            myFontPath = "FONTS/" + theName + "-" + theFontStyle + ".ttf";
        } else if (searchFile("FONTS/" + theName + ".ttf")) {
            myFontPath = "FONTS/" + theName + ".ttf";
        } else if (searchFile("FONTS/" + theName + ".otf")) {
            myFontPath = "FONTS/" + theName + ".otf";
        } else {
            throw new Error("FONTS/" + theName + "-" + theFontStyle + ".[ttf|otf] nor " +
                    theName + ".[ttf|otf] does not exist");
        }

        // blurry aber gutes spacing -> No Hinting (Renderer.NOHINTING)
        // crispy aber rottenschlechtes spacing -> Natives Fonthinting (Renderer.NATIVEHINTING) // if compiled into freetype!!
        // wenig crispy aber okes spacing -> Autohinting (Renderer.AUTOHINTING)

        var myHinting = spark.hintingFromString(theHinting);
        // enforce loadttf of a normal font, otherwise we get an exception
        window.loadTTF(myName, searchFile(myFontPath), theSize * theFontScale, myHinting, spark.styleFromString("normal"), theAscendOffset);
        if (theFontStyle != "normal") {
            window.loadTTF(myName, searchFile(myFontPath), theSize * theFontScale, myHinting, spark.styleFromString(theFontStyle));
        }
        spark.ourLoadedFonts[myName] = true;

        // always load the bold variant to allow the bold tag <b>...</b> to be used
        if (spark.styleFromString(theFontStyle) != Renderer.BOLD) {
            var myFont = theName.split("-")[0];
            if (searchFile("FONTS/" + myFont + "-bold" + ".otf")) {
                myFontPath = searchFile("FONTS/" + myFont + "-bold" + ".otf");
            } else {
                myFontPath = searchFile("FONTS/" + myFont + "-bold" + ".ttf");
            }
            if (myFontPath) {
                Logger.debug("loading bold font for " + myName + "," + myFontPath + "," + theSize + "," + "bold");
                window.loadTTF(myName, searchFile(myFontPath), theSize * theFontScale, myHinting, Renderer.BOLD);
            }
        }
    }
    return myName;
};

/**
 * Internal: apply defaults to the given style node
 */
spark.applyStyleDefaults = function(theStyle) {
    !theStyle.getAttribute("fontStyle")  ? theStyle.fontStyle = "normal" : null;
    !theStyle.getAttribute("fontScale")  ? theStyle.fontScale = spark.fontScale : null;

    !theStyle.getAttribute("topPad")     ? theStyle.topPad    = 0 : null;
    !theStyle.getAttribute("bottomPad")  ? theStyle.bottomPad = 0 : null;
    !theStyle.getAttribute("rightPad")   ? theStyle.rightPad  = 0 : null;
    !theStyle.getAttribute("leftPad")    ? theStyle.leftPad   = 0 : null;

    !theStyle.getAttribute("tracking")   ? theStyle.tracking   = 0 : null;
    !theStyle.getAttribute("lineHeight") ? theStyle.lineHeight = 0 : null;

    !theStyle.getAttribute("hAlign")    ? theStyle.hAlign    = "left" : null;
    !theStyle.getAttribute("vAlign")    ? theStyle.vAlign    = "top"  : null;
    !theStyle.getAttribute("hinting")    ? theStyle.hinting    = spark.AUTOHINTING  : null;

    !theStyle.getAttribute("textColor")       ? theStyle.textColor        = "000000" : null;
};

spark.isFontStyleNode = function(theNode) {
    return (theNode.getAttribute("font") ||
            theNode.getAttribute("fontSize") ||
            theNode.getAttribute("fontScale") ||
            theNode.getAttribute("fontStyle") ||
            theNode.getAttribute("topPad") ||
            theNode.getAttribute("bottomPad") ||
            theNode.getAttribute("leftPad") ||
            theNode.getAttribute("rightPad") ||
            theNode.getAttribute("tracking") ||
            theNode.getAttribute("lineHeight") ||
            theNode.getAttribute("hAlign") ||
            theNode.getAttribute("vAlign") ||
            theNode.getAttribute("hinting") ||
            theNode.getAttribute("textColor"));
}

spark.mergeFontStyles = function(theOldStyle, theNewStyle) {
    if(!spark.isFontStyleNode(theOldStyle) || !spark.isFontStyleNode(theNewStyle)) {
        throw new Error("either old fontstyle or new fontstyle is no font style not!");
    }
    var myMergedStyle = new Node("<style/>");
    myMergedStyle = myMergedStyle.childNode(0);

    // init style
    spark.applyStyleDefaults(myMergedStyle);

    function mergeAttributeIfPresent(theAttribute) {
        var myNewValue = theNewStyle.getAttribute(theAttribute);
        if(myNewValue) {
            myMergedStyle[theAttribute] = myNewValue;
        } else {
            var myOldValue = theOldStyle.getAttribute(theAttribute);
            if(myOldValue) {
                myMergedStyle[theAttribute] = myOldValue;
            }
        }
    }

    mergeAttributeIfPresent("font");
    mergeAttributeIfPresent("fontSize");
    mergeAttributeIfPresent("fontStyle");

    mergeAttributeIfPresent("topPad");
    mergeAttributeIfPresent("bottomPad");
    mergeAttributeIfPresent("leftPad");
    mergeAttributeIfPresent("rightPad");

    mergeAttributeIfPresent("tracking");
    mergeAttributeIfPresent("lineHeight");

    mergeAttributeIfPresent("hAlign");
    mergeAttributeIfPresent("vAlign");
    mergeAttributeIfPresent("hinting");

    mergeAttributeIfPresent("textColor");

   return myMergedStyle;

}


/**
 * Produce a font style node from the given DOM node.
 *
 * This CAN be applied to any DOM node, but is usually
 * used on a SPARK xml node to collect text style properties.
 *
 * Returns a font style node that can be used with renderText.
 */
spark.fontStyleFromNode = function(theNode) {
    var myStyle = new Node("<style/>");
    myStyle = myStyle.childNode(0);

    function copyAttributeIfPresent(theAttribute) {
        if (theNode) {
            var myValue = theNode.getAttribute(theAttribute);
            if(myValue) {
                myStyle[theAttribute] = myValue;
            }
        }
    }

    copyAttributeIfPresent("font");
    copyAttributeIfPresent("fontSize");
    copyAttributeIfPresent("fontScale");
    copyAttributeIfPresent("ascendOffset");
    copyAttributeIfPresent("fontStyle");

    copyAttributeIfPresent("topPad");
    copyAttributeIfPresent("bottomPad");
    copyAttributeIfPresent("leftPad");
    copyAttributeIfPresent("rightPad");

    copyAttributeIfPresent("tracking");
    copyAttributeIfPresent("lineHeight");

    copyAttributeIfPresent("hAlign");
    copyAttributeIfPresent("vAlign");
    copyAttributeIfPresent("hinting");

    copyAttributeIfPresent("textColor");

    //spark.applyStyleDefaults(myStyle);
    return myStyle;
};

/**
 * Internal: determine font for the given style, ensuring that it is loaded.
 */
spark.fontForStyle = function(theStyle) {
    !theStyle.getAttribute("font")      ? theStyle.font    = "arial" : null;
    !theStyle.getAttribute("fontSize")  ? theStyle.fontSize = 12 : null;
    !theStyle.getAttribute("fontScale")  ? theStyle.fontScale =  spark.fontScale : null;
    !theStyle.getAttribute("fontStyle") ? theStyle.fontStyle  = "normal" : null;
    !theStyle.getAttribute("hinting") ? theStyle.hinting  = spark.AUTOHINTING : null;
    !theStyle.getAttribute("ascendOffset") ? theStyle.ascendOffset  = 0 : null;
    return spark.loadFont(theStyle.font, theStyle.fontSize, theStyle.fontScale, theStyle.fontStyle, theStyle.hinting, theStyle.ascendOffset);
};

/**
 * Internal: Convert text style from string to Y60 enum.
 */
spark.styleFromString = function(theString) {
    if (theString === "normal") {
        return 0;
    }
    if (theString === "bold") {
        return Renderer.BOLD;
    }
    if (theString === "italic") {
        return Renderer.ITALIC;
    }
    if (theString === "bolditalic") {
        return Renderer.BOLDITALIC;
    }
    throw new Error("Unknown font style: " + theString);
};

spark.AUTOHINTINGLIGHT = "autohinting_light";
spark.AUTOHINTINGMONO = "autohinting_mono";
spark.AUTOHINTING = "autohinting";
spark.NOHINTING = "nohinting";
spark.NATIVEHINTING = "nativehinting";

spark.hintingFromString = function(theString) {
    if (theString === spark.NOHINTING) {
        return Renderer.NOHINTING;
    }
    if (theString === spark.AUTOHINTING) {
        return Renderer.AUTOHINTING;
      if(theString == spark.AUTOHINTINGMONO)
          return Renderer.AUTOHINTINGMONO;
      if(theString == spark.AUTOHINTINGLIGHT)
          return Renderer.AUTOHINTINGLIGHT;
    }
    if (theString === spark.NATIVEHINTING) {
        return Renderer.NATIVEHINTING;
    }
    throw new Error("Unknown font hinting: " + theString);
};

/**
 * Create an image suitable for rendering text into.
 */
spark.createTextImage = function(theSize) {
    var myImage = Modelling.createImage(window.scene, theSize.x, theSize.y, "RGBA");
    myImage.name = "text_image";
    myImage.resize = "none";

    return myImage;
};

/**
 * Render the given TEXT into the given IMAGE using the given STYLE with the given SIZE.
 *
 * Returns the size of the text within the image as a Point2f.
 *
 * MAXTEXTWIDTH and LINEWIDTHS are output parameters,
 * allowing the client some layout trickery.
 */
spark.renderText = function(theImage, theText, theStyle, theSize, theMaxTextWidth, theLineWidths) {

    var myFont = spark.fontForStyle(theStyle);
    var mySize = new Vector2f(theImage.width, theImage.height);
    if(theSize != undefined) {
        mySize = theSize;
    }

    if ("lineHeight" in theStyle && theStyle.fontScale != 1.0) {
        theStyle.lineHeight *= theStyle.fontScale;
    }
    var myTextSize =
        window.renderTextAsImage(theImage,
                                 theText,
                                 myFont,
                                 theStyle,
                                 mySize.x*theStyle.fontScale, mySize.y*theStyle.fontScale);
    var myGlyphPosition = window.getTextGlyphPositions();
    if (theStyle.fontScale != 1.0) {
        if ("lineHeight" in theStyle) {
            theStyle.lineHeight /= theStyle.fontScale;
        }
        //XXX: if the image width is odd and fontscale is >1 the imagefilter can result in bad looking text
        myTextSize = new Vector2f(Math.ceil(myTextSize[0]/theStyle.fontScale), Math.ceil(myTextSize[1]/theStyle.fontScale));
        applyImageFilter(theImage, "resizehamming", [myTextSize.x, myTextSize.y, 1]);
        if (theImage.width > 0 && theImage.height > 0) {
            var myMatrix = new Matrix4f();
            myMatrix.makeScaling(new Vector3f(myTextSize.x / theImage.width,
                                              myTextSize.y / theImage.height, 1));
            theImage.matrix = myMatrix;
        }
    }
    if (theMaxTextWidth) {
        theMaxTextWidth.width = window.getTextMaxWidth();
    }
    if (theLineWidths) {
        var myLineWidths = window.getTextLineWidths();
        for (var i = 0; i < myLineWidths.length; ++i) {
            theLineWidths.push(myLineWidths[i]);
        }
    }
    return {size:myTextSize, positions:myGlyphPosition};
};
