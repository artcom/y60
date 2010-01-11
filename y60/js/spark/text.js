/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2009, ART+COM AG Berlin, Germany <www.artcom.de>
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
// Description: Text rendering utilities
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

spark.ourLoadedFonts = {};

spark.loadFont = function(theName, theSize, theStyle) {
    var myName = theName + "-" + theStyle + "-" + theSize;
    if(!(myName in spark.ourLoadedFonts)) {
        if(theStyle != "normal") {
            spark.loadFont(theName, theSize, "normal");
        }
        
        Logger.info("Loading font " + theName + " with size " + theSize + " and style " + theStyle);
        
        var myFontPath = null;
        if (fileExists("FONTS/" + theName + "-" + theStyle + ".otf")) {
            myFontPath = "FONTS/" + theName + "-" + theStyle + ".otf";
        } else if (fileExists("FONTS/" + theName + "-" + theStyle + ".ttf")) {
            myFontPath = "FONTS/" + theName + "-" + theStyle + ".ttf";
        } else if (fileExists("FONTS/" + theName + ".otf")) {
            myFontPath = "FONTS/" + theName + ".otf";
        } else if (fileExists("FONTS/" + theName + ".ttf")) {
            myFontPath = "FONTS/" + theName + ".ttf";
        } else {
            throw new Error("FONTS/" + theName + "-" + theStyle + ".[ttf|otf] nor " +
                    theName + ".[ttf|otf] does not exist");
        }
        
        // blurry aber gutes spacing -> No Hinting (Renderer.NOHINTING)
        // crispy aber rottenschlechtes spacing -> Natives Fonthinting (Renderer.NATIVEHINTING)
        // wenig crispy aber okes spacing -> Autohinting (Renderer.AUTOHINTING)

        // XXX: inconsistent hinting!?        
        if (theStyle != "normal") {
           // enforce loadttf of a normal font, otherwise we get an exception
           window.loadTTF(myName, myFontPath, theSize, Renderer.AUTOHINTING , spark.styleFromString("normal"));
        }
        window.loadTTF(myName, myFontPath, theSize, Renderer.NOHINTING , spark.styleFromString(theStyle));
        
        spark.ourLoadedFonts[myName] = true;

       // always load the bold variant to allow the bold tag <b>...</b> to be used
       if(spark.styleFromString(theStyle) != Renderer.BOLD) {
           var myFont = theName.split("-")[0];
           if (searchFile("FONTS/" + myFont + "-bold" + ".otf")) {
               myFontPath = searchFile("FONTS/" + myFont + "-bold" + ".otf");
           } else {
               myFontPath = searchFile("FONTS/" + myFont + "-bold" + ".ttf");
           }
           if (myFontPath != undefined) {
               Logger.info("loading bold font for " + myName + "," + myFontPath + "," + Renderer.AUTOHINTING + "," + "bold")
               window.loadTTF(myName, searchFile(myFontPath), theSize, Renderer.AUTOHINTING , Renderer.BOLD);
           }
       }
        
    }
    return myName;
};

spark.applyStyleDefaults = function(theStyle) {

    !theStyle.getAttribute("fontStyle")   ? theStyle.fontStyle    = "normal" : null;

    !theStyle.getAttribute("topPad")    ? theStyle.topPad    = 0 : null;
    !theStyle.getAttribute("bottomPad") ? theStyle.bottomPad = 0 : null;
    !theStyle.getAttribute("rightPad")  ? theStyle.rightPad  = 0 : null;
    !theStyle.getAttribute("leftPad")   ? theStyle.leftPad   = 0 : null;
    
    !theStyle.getAttribute("tracking")   ? theStyle.tracking   = 0 : null;
    !theStyle.getAttribute("lineHeight") ? theStyle.lineHeight = 0 : null;
    
    !theStyle.getAttribute("hAlign")    ? theStyle.hAlign    = "left" : null;
    !theStyle.getAttribute("vAlign")    ? theStyle.vAlign    = "top"  : null;
    
    !theStyle.getAttribute("textColor")       ? theStyle.textColor        = "000000" : null;
    !theStyle.getAttribute("backgroundColor") ? theStyle.backgroundColor  = "FFFFFF" : null;
};

spark.fontStyleFromNode = function(theNode) {
    var myStyle = new Node("<style/>");

    myStyle = myStyle.childNode(0);

    function copyAttributeIfPresent(theAttribute) {
        var myValue = theNode.getAttribute(theAttribute);
        if(myValue) {
            myStyle[theAttribute] = myValue;
        }
    }

    copyAttributeIfPresent("font");
    copyAttributeIfPresent("fontSize");
    copyAttributeIfPresent("fontStyle");

    copyAttributeIfPresent("topPad");
    copyAttributeIfPresent("bottomPad");
    copyAttributeIfPresent("leftPad");
    copyAttributeIfPresent("rightPad");

    copyAttributeIfPresent("tracking");
    copyAttributeIfPresent("lineHeight");

    copyAttributeIfPresent("hAlign");
    copyAttributeIfPresent("vAlign");

    copyAttributeIfPresent("textColor");
    copyAttributeIfPresent("backgroundColor");

    spark.applyStyleDefaults(myStyle);
    
    return myStyle;
};

spark.fontForStyle = function(theStyle) {
    !theStyle.getAttribute("font")      ? theStyle.font    = "arial" : null;
    !theStyle.getAttribute("fontSize")  ? theStyle.fontSize = 12 : null;
    !theStyle.getAttribute("fontStyle") ? theStyle.fontStyle  = "normal" : null;
    return spark.loadFont(theStyle.font, theStyle.fontSize, theStyle.fontStyle);
}

spark.alignmentFromString = function(theString) {
    if(theString == "top")
        return Renderer.TOP_ALIGNMENT;
    if(theString == "bottom")
        return Renderer.BOTTOM_ALIGNMENT;
    if(theString == "left")
        return Renderer.LEFT_ALIGNMENT;
    if(theString == "right")
        return Renderer.RIGHT_ALIGNMENT;
    if(theString == "center")
        return Renderer.CENTER_ALIGNMENT;
    throw new Error("Unknown alignment: " + theString);
};

spark.styleFromString = function(theString) {
    if(theString == "normal")
        return 0;
    if(theString == "bold")
        return Renderer.BOLD;
    if(theString == "italic")
        return Renderer.ITALIC;
    if(theString == "bolditalic")
        return Renderer.BOLDITALIC;
    throw new Error("Unknown font style: " + theString);
};

spark.createTextImage = function(theSize) {
    var myImage = Modelling.createImage(window.scene, theSize.x, theSize.y, "RGBA");
    myImage.resize = "none";
    
    return myImage;
};

spark.renderText = function(theImage, theText, theStyle, theSize, theMaxTextWidth, theLineWidths) {
    spark.applyStyleDefaults(theStyle);

    window.setTextPadding(theStyle.topPad, theStyle.bottomPad, theStyle.leftPad, theStyle.rightPad);
    window.setHTextAlignment(spark.alignmentFromString(theStyle.hAlign));
    window.setVTextAlignment(spark.alignmentFromString(theStyle.vAlign));

    window.setTextColor(asColor(theStyle.textColor));
    window.setTracking(theStyle.tracking);
    window.setLineHeight(theStyle.lineHeight);

    var myFont = spark.fontForStyle(theStyle);
    var mySize = new Vector2f(theImage.width, theImage.height);
    if(theSize != undefined) {
        mySize = theSize;
    }
    
    var myTextSize =
        window.renderTextAsImage(theImage,
                                 theText, 
                                 myFont,
                                 mySize.x, mySize.y);
    
    if (theMaxTextWidth) {
        theMaxTextWidth.width = window.getTextMaxWidth();
    }
    if (theLineWidths) {
        var myLineWidths = window.getTextLineWidths();
        for (var i = 0; i < myLineWidths.length; ++i) {
            theLineWidths.push(myLineWidths[i]);
        }
    }
    
    var myMatrix = new Matrix4f();
    myMatrix.makeScaling(new Vector3f(myTextSize.x / theImage.width, 
                                      myTextSize.y / theImage.height, 1));
    theImage.matrix = myMatrix;

    window.setHTextAlignment(Renderer.LEFT_ALIGNMENT);
    window.setVTextAlignment(Renderer.TOP_ALIGNMENT);
    window.setTextPadding(0,0,0,0);
    return myTextSize;
};
