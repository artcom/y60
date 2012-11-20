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

function arrayAppend(theArray, theAppended) {
    for(var i = 0; i < theAppended.length; i++) {
        theArray.push(theAppended[i]);
    }
}

var ourNameToIdMap = {};

function playerInfoToExcel(thePlayersNode) {
    try {
        if (thePlayersNode.nodeType == Node.DOCUMENT_NODE) {
            thePlayersNode = thePlayersNode.firstChild;
        }

        var myTemplateNode = new Node();
        myTemplateNode.parseFile("CONFIG/excel_template.xml");

        var myTableNode = myTemplateNode.find("//Table");
        var myRowNode = myTableNode.find("//*[@name = 'y60:name']").parentNode.parentNode;

        for (var i=0;i<thePlayersNode.childNodesLength("player");++i) {
            var myNewRow = myRowNode.cloneNode(true);
            var myPlayer = thePlayersNode.childNode("player", i);
            var myDataNodes = myNewRow.findAll("//*[@name = 'y60:name']");
            for (var j=0;j<myDataNodes.length;++j) {
                var myValue = "";
                var myDataNode = myDataNodes[j];
                var myName = myDataNode["y60:name"];
                if (myName  in myPlayer) {
                    myValue = myPlayer[myName];
                }
                myDataNode.childNode("#text").nodeValue = myValue;
            }
            myTableNode.appendChild(myNewRow);
        }
        myTableNode.removeChild(myRowNode);

        return myTemplateNode;
    } catch (ex) {
        reportException(ex);
        return null;
    }
}

function registerNode(theName, theNode) {
    if(theName in ourNameToIdMap) {
        throw new Exception("Name \"" + theName + "\" redefined.");
    }
    ourNameToIdMap[theName] = theNode.id;
}

function getNodeByName(theName) {
    if(theName in ourNameToIdMap) {
        return window.scene.dom.getElementById(ourNameToIdMap[theName]);
    } else {
        return null;
    }
}

function getCachedImage(theImagePath) {
    var myImageName = "image-" + theImagePath;
    var myImage = getNodeByName(myImageName);

    if(!myImage) {
        myImage = Modelling.createImage(window.scene, theImagePath);
        myImage.name = myImageName;
        registerNode(myImageName, myImage);
    }
    return myImage;
}

function getCachedMovie(theMoviePath) {
    var myMovieName = "movie-" + theMoviePath;
    var myMovie = getNodeByName(myMovieName);

    if(!myMovie) {
        myMovie = Node.createElement("movie");
        window.scene.images.appendChild(myMovie);
        myMovie.src = theMoviePath;
        myMovie.name = myMovieName;
        myMovie.loopcount = 0;
        window.scene.loadMovieFrame(myMovie);
        registerNode(myMovieName, myMovie);
    }
    return myMovie;
}

function getCachedTexture(theImagePath) {
    var myTextureName = "image-texture-" + theImagePath;

    Logger.info("Computing image texture for " + theImagePath);

    var myImage = getCachedImage(theImagePath);
    var myTexture = getNodeByName(myTextureName);

    if(!myTexture) {
        myTexture = Modelling.createTexture(window.scene, myImage);
        myTexture.name = myTextureName;
        registerNode(myTextureName, myTexture);
    }

    return myTexture;
}

function getCachedImageMaterial(theImagePath) {
    var myMaterialName = "image-material-" + theImagePath;

    Logger.info("Computing image material for " + theImagePath);

    var myImage = getCachedImage(theImagePath);

    var myTexture = Modelling.createTexture(window.scene, myImage);

    var myMaterial =
        Modelling.createUnlitTexturedMaterial(window.scene, myTexture, myMaterialName);

    return myMaterial;
}

function getCachedMovieMaterial(theMoviePath) {
    var myMaterialName = "movie-material-" + theMoviePath;

    Logger.info("Computing image material for " + theMoviePath);

    var myMovie = getCachedMovie(theMoviePath);

    var myTexture = Modelling.createTexture(window.scene, myMovie);

    var myMaterial =
        Modelling.createUnlitTexturedMaterial(window.scene, myTexture, myMaterialName);

    return myMaterial;
}

function getCachedTextMaterial(theId, theText, theStyle, theWidth, theHeight) {
    var myMaterialName = "text-material-" + theId;

    var myMaterial = getNodeByName(myMaterialName);

    if(!myMaterial) {
        Logger.info("Computing text material for " + theId);

        var myImage =
            textAsImage(theText, theStyle,
                        new Vector2i(theWidth, theHeight));

        myMaterial =
            Modelling.createUnlitTexturedMaterial(window.scene,
                                                  myImage,
                                                  myMaterialName);
        myMaterial.transparent = true;

        registerNode(myMaterialName, myMaterial);
    }

    return myMaterial;
}

var ourLoadedFonts = {};

function loadCachedFont(theName, theSize, theStyle) {
    var myName = theName + theSize;
    if(!(myName in ourLoadedFonts)) {
        if(theStyle != "normal") {
            loadCachedFont(theName, theSize, "normal");
        }

        Logger.info("Loading font " + theName + " with size " + theSize + " and style " + theStyle);

        window.loadTTF(myName, "FONTS/" + theName + "-" + theStyle + ".ttf", theSize, Renderer.AUTOHINTING, fontStyleFromString(theStyle));
        ourLoadedFonts[myName] = true;
    }
    return myName;
}

function copyAttributeIfPresent(theSource, theDestination, theAttribute) {
    var myValue = theSource.getAttribute(theAttribute);
    if(myValue) {
        theDestination[theAttribute] = myValue;
    }
}

function fontStyleFromNode(theNode) {
    var myStyle = new Node("<style/>");

    myStyle = myStyle.childNode(0);

    myStyle.font = theNode.font;
    myStyle.fontSize = theNode.fontSize;

    copyAttributeIfPresent(theNode, myStyle, "fontStyle");

    copyAttributeIfPresent(theNode, myStyle, "topPad");
    copyAttributeIfPresent(theNode, myStyle, "bottomPad");
    copyAttributeIfPresent(theNode, myStyle, "leftPad");
    copyAttributeIfPresent(theNode, myStyle, "rightPad");

    copyAttributeIfPresent(theNode, myStyle, "tracking");
    copyAttributeIfPresent(theNode, myStyle, "lineHeight");

    copyAttributeIfPresent(theNode, myStyle, "hAlign");
    copyAttributeIfPresent(theNode, myStyle, "vAlign");

    copyAttributeIfPresent(theNode, myStyle, "textColor");
    copyAttributeIfPresent(theNode, myStyle, "backgroundColor");

    //applyStyleDefaults(myStyle);

    return myStyle;
}

function fontForStyle(theStyle) {
    !theStyle.getAttribute("font")      ? theStyle.font    = "Times_New_Roman" : null;
    !theStyle.getAttribute("fontSize")  ? theStyle.fontSize = 12 : null;
    !theStyle.getAttribute("fontStyle") ? theStyle.fontStyle  = "normal" : null;
    return loadCachedFont(theStyle.font, theStyle.fontSize, theStyle.fontStyle);
}

function alignmentFromString(theString) {
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
    throw new Exception("Unknown alignment: " + theString);
}

function fontStyleFromString(theString) {
    if(theString == "normal")
        return 0;
    if(theString == "bold")
        return Renderer.BOLD;
    if(theString == "italic")
        return Renderer.ITALIC;
    if(theString == "bolditalic")
        return Renderer.BOLDITALIC;
    throw new Exception("Unknown font style: " + theString);
}

function stringFromFontStyle(theStyle) {
    switch(theStyle) {
    case 0:
        return "normal";
    case Renderer.BOLD:
        return "bold";
    case Renderer.ITALIC:
        return "italic";
    case Renderer.BOLDITALIC:
        return "bolditalic";
    default:
        throw new Exception("Unknown font style: " + theStyle);
    }
}

function applyStyleDefaults(theStyle) {
    !theStyle.getAttribute("fontStyle")   ? theStyle.fontStyle    = "normal" : null;

    !theStyle.getAttribute("topPad")    ? theStyle.topPad    = 0 : null;
    !theStyle.getAttribute("bottomPad") ? theStyle.bottomPad = 0 : null;
    !theStyle.getAttribute("rightPad")  ? theStyle.rightPad  = 0 : null;
    !theStyle.getAttribute("leftPad")   ? theStyle.leftPad   = 0 : null;

    !theStyle.getAttribute("tracking")   ? theStyle.tracking   = 0 : null;
    !theStyle.getAttribute("lineHeight") ? theStyle.lineHeight = 0 : null;

    !theStyle.getAttribute("hAlign")    ? theStyle.hAlign    = "left" : null;
    !theStyle.getAttribute("vAlign")    ? theStyle.vAlign    = "top"  : null;

    !theStyle.getAttribute("textColor")       ? theStyle.textColor        = "FFFFFF" : null;
    !theStyle.getAttribute("backgroundColor") ? theStyle.backgroundColor  = "000000" : null;
}

function textAsImage(theText, theStyle, theSize) {
    var myImage = Node.createElement("image");

    // TODO: port to image-texture-separation foo [sh]
    // myImage.resize = "pad";
    // myImage.wrapmode = "clamp_to_edge";
    // myImage.mipmap = false;

    window.scene.images.appendChild(myImage);

    textToImage(myImage, theText, theStyle, theSize);

    return myImage;
}

function textToImage(theImage, theText, theStyle, theSize) {

    // theImage.resize = "pad";
    // theImage.wrapmode = "clamp_to_edge"
    // theImage.mipmap = false;

    var myTextSize =
        window.renderTextAsImage( theImage,
                                 theText,
                                 fontForStyle(theStyle),
                                 theStyle,
                                 theSize.x, theSize.y,
                                 new Vector2i(0,0));

    var myMatrix = new Matrix4f();
    myMatrix.makeScaling(new Vector3f(myTextSize.x / theImage.width,
                                      myTextSize.y / theImage.height, 1));
    theImage.matrix = myMatrix;

    window.setHTextAlignment(Renderer.LEFT_ALIGNMENT);
    window.setVTextAlignment(Renderer.TOP_ALIGNMENT);
    window.setTextPadding(0,0,0,0);
}

function createQuad(theParent, theName, theSize, thePosition, theMaterial, theInsensibleFlag, theVisibleFlag) {
    var myQuad = Modelling.createQuad(window.scene, theMaterial.id,
        [0,0,0], [theSize.x, theSize.y, 0]);
    var myBody = Modelling.createBody(theParent, myQuad.id);
    myBody.position = thePosition;
    myBody.name = theName;
    myBody.insensible = theInsensibleFlag;
    myBody.visible = theVisibleFlag;
    return {body:myBody, shape:myQuad};
}

var ourBodyOwners = {};

function getBodyOwner(theBody) {
    if (theBody.id in ourBodyOwners) {
        return ourBodyOwners[theBody.id];
    }

    return null;
}

function registerBodyOwner(theOwner, theBody) {
    ourBodyOwners[theBody.id] = theOwner;
}

function unregisterBodyOwner(theBody) {
    delete ourBodyOwners[theBody.id];
}

//convert a number to a local string i.e. 1.234.567,33
function NumberToLocalString(theNumber) {
    var myNumber = Number(theNumber);
    if (myNumber == NaN) {
        Logger.warning("The parameter '" + theNumber + "'can not be convertet to Number.");
        return "";
    }

    var myInt = Math.floor(myNumber);
    var myFrc = Math.round((myNumber%1)*100);

    var myResult = "";
    while (myInt >= 1000) {
        myInt /= 1000;
        myResult = "." + padStringFront(Math.round((myInt%1)*1000), "0", 3) + myResult;
        myInt = Math.floor(myInt);
    }

    return myInt + myResult + "," + padStringFront(Math.abs(myFrc), "0", 2);
}
