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

use("SceneViewer.js");
use("CharacterSoup.js");
use("PathAlign.js");

function PathText( theText, theFontSize, theCharacterSoup, thePrebuildFlag) {
    this.Constructor(this, theText, theFontSize, theCharacterSoup, thePrebuildFlag);
}

PathText.prototype.Constructor = function(self, theText, theFontSize, theCharacterSoup, thePrebuildFlag) {

    self.getLength = function() {
        return _myText.length;
    }

    self.getShape = function() {
        return _myShape;
    }
    self.getBody = function() {
        return _myBody;
    }

    self.getPositions = function() {
        var myVertexPositions = self.getPositionsNode();
        var myPositions = myVertexPositions.childNode('#text')[1];
        return myPositions;
    }
    self.getPositionsNode = function() {
        var myVertexPositions = _myShape.find(".//*[@name = 'position']");
        return myVertexPositions;
    }

    self.getColors = function() {
        var myVertexColors = _myShape.find(".//*[@name = 'color']");
        var myColors = myVertexColors.childNode('#text')[1];
        return myColors;
    }

    self.setAlignment = function(theAlignment) {
        _myAlignment = theAlignment;
    }
    self.getAlignment = function() {
        return _myAlignment;
    }

    /**
     * Align this text along given path.
     * - if given, start near 'thePos'; default is start of path.
     * - if given, start at character 'theFirstCharacter'; default is 0.
     * - if given, end at character 'theLastCharacter';
     *   default is last character in text.
     */
    self.align = function(thePathAlign, thePos, theFirstCharacter,
                          theLastCharacter, doTheWrapAroundFlag, theFlipFlag,
                          theAutoNewlineFlag)
    {
        var myXMirror = 1;
        if (theFlipFlag != undefined) {
            myXMirror = theFlipFlag ? -1:1;
        }
        if (thePos != undefined) {
            thePathAlign.setCurrentPosition(thePos);
        }
        if (theFirstCharacter == undefined) {
            theFirstCharacter = 0;
        }
        if (theLastCharacter == undefined) {
            theLastCharacter = _myText.length;
        }
        if (doTheWrapAroundFlag == undefined) {
            doTheWrapAroundFlag = true;
        }
        if (theAutoNewlineFlag == undefined) {
            theAutoNewlineFlag = false;
        }
        var myAlignedCharacter = _myText.length;
        var myBuildGeometry = false;
        var myAlphabetMap = null;
        var myShapeBuilder = null;
        var myElement = null;
        var myUVCoordX = 0;
        var myParagraphInfo = theCharacterSoup.getParagraph();
        if (!_myShape) {
            myBuildGeometry = true;
            myAlphabetMap = theCharacterSoup.getAlphabetMap(theFontSize);
            myShapeBuilder = new ShapeBuilder();
            myElement = myShapeBuilder.appendElement("quads", myAlphabetMap.material.id);
        }

        var myAlphabetMap = theCharacterSoup.getAlphabetMap(theFontSize);
        var myFontMetrics = theCharacterSoup.getFontMetrics(theFontSize);
        //print("font metrics:", "height=" + myFontMetrics.height, "ascent=" + myFontMetrics.ascent, "descent=" + myFontMetrics.descent, "lineskip=" + myFontMetrics.lineskip);
        var myLineHeight = theCharacterSoup.lineheight;
        if (myLineHeight == 0 ) {
            myLineHeight = myFontMetrics.lineskip;
        }

        var myWidth = 0.0;
        // in case of newlines, we use this offset to move character according to path
        var myLineOffset = new Vector3f(0,0,0);
        var myNewLineFlag = true;
        var myLineCorrection = new Vector3f(0,0,0);
        for (var i = theFirstCharacter; i < theLastCharacter; ++i) {
            var myChar = _myText[i];
            if (myChar == "\n" || myChar == "\r" ) {
                // line break
                var myNewLineOffset = 0;
                if (_myText.length > i+1) {
                    if (_myText[i+1] == "\n") {
                        //print("found carriage return AND a newline");
                        myNewLineOffset = 1;
                    }
                }
                if (!theAutoNewlineFlag) {
                    return i + myNewLineOffset;
                } else {
                    // do some newline magic
                    thePathAlign.resetToStartPos();
                    var myLineSkip = myLineHeight;
                    myLineSkip += myParagraphInfo.topoffset + myParagraphInfo.bottomoffset;
                    myLineOffset = sum(myLineOffset,product(thePathAlign.getNormalAtCurrentPosition(), myLineSkip));
                    myNewLineFlag = true;
                    continue;
                }
            }
            var myCharacter = _myCharacters[i];
            if (myCharacter == null) {
                continue;
            }

            // advance
            myWidth = myCharacter.metric.advance;// HACK? - 0.25;
            //print("char=" + myCharacter.unicode + " advance=" + myWidth);

            // kerning
            if (i > 0 && i < (_myText.length-1)) {
                var myNextCharacter = _myCharacters[i+1];
                if (myNextCharacter != null) {
                    var myKerning = window.getKerning(theCharacterSoup.getFontName(theFontSize),
                                                      myCharacter.unicode,
                                                      myNextCharacter.unicode);
                    if (myKerning != 0.0) {
                        //print("kern=" + myKerning, "char=" + myCharacter.unicode, "next=" + myNextCharacter.unicode, "advance="+myCharacter.metric.advance);
                    }
                    myWidth += myKerning;
                }
            }

            myWidth += theCharacterSoup.getTracking() / 64.0;
            //print("advance+kern=" + myWidth);
            /*
             * move 'advance+kern' on path but make characters 'advance' wide
             */
            if (!doTheWrapAroundFlag && !thePathAlign.fitsAdvancement(myWidth)) {
                myAlignedCharacter = i;
                break;
            }
            var mySegment = thePathAlign.advance(myWidth);
            if (mySegment == null) {
                myAlignedCharacter = i;
                break;
            }
            var myForwardVector = difference(mySegment.end, mySegment.start);
            var padding = theCharacterSoup.getPadding();
            var paddingVector = product(normalized(myForwardVector),padding);
            var myAdvanceVector = product(normalized(myForwardVector),
                                          myCharacter.metric.advance);
            var myMinVector = product(normalized(myForwardVector),
                                      myCharacter.metric.min.x);
            var myMaxVector = product(normalized(myForwardVector),
                                      myCharacter.metric.max.x+1);

            var myLeftVector = normalized(cross(UP_VECTOR, myForwardVector));

            // align
            var myTop, myBottom;
            switch (_myAlignment) {
                case TOP_ALIGNMENT:
                    myTop = (myFontMetrics.height - myFontMetrics.ascent);
                    myBottom = -(myFontMetrics.ascent - myFontMetrics.descent)
                               + myTop;
                    break;
                case BOTTOM_ALIGNMENT:
                    myTop = myFontMetrics.ascent - myFontMetrics.descent;
                    myBottom = 0;
                    break;
                case CENTER_ALIGNMENT:
                    myTop = myAlphabetMap.cellsize * 0.5
                            - (myAlphabetMap.cellsize - myFontMetrics.height)
                            * 0.5;
                    myBottom = -myTop;
                    break;
                case BASELINE_ALIGNMENT:
                    myTop = myFontMetrics.ascent;
                    myBottom = myFontMetrics.descent;
                    break;
            }
            var myTopOffset = product(myLeftVector,
                                      myXMirror * myTop + padding);
            var myBottomOffset = product(myLeftVector,
                                         myXMirror * myBottom - padding);

            var myStart = difference(sum(mySegment.start,myMinVector),
                                     paddingVector);
            var myEnd = sum(mySegment.start,sum(myMaxVector,paddingVector));
            var j = i * 4;
            var myPositions = [];

            // do not use min of glyph in case of character is first in line,
            // and correct them in the following glyphs also
            if (myNewLineFlag) {
                myLineCorrection = new Vector3f(myMinVector);
            }
            myStart = difference(myStart, myLineCorrection);
            myEnd = difference(myEnd, myLineCorrection);
            myPositions.push(sum(sum(myStart, myBottomOffset), myLineOffset));
            myPositions.push(sum(sum(myEnd, myBottomOffset), myLineOffset));
            myPositions.push(sum(sum(myEnd, myTopOffset), myLineOffset));
            myPositions.push(sum(sum(myStart, myTopOffset), myLineOffset));

            if (myBuildGeometry) {
                var myCharacter = _myCharacters[i];
                //print("char=" + myCharacter.unicode, "uv=" + myCharacter.uv, "uvsize=" + myCharacter.uvsize);
                myShapeBuilder.appendQuadWithCustomTexCoordsAndPositions(myElement,
                        myPositions, myCharacter.uv, myCharacter.uvsize, false);
                var myXDelta = (1.0/_myText.length);
                for(var myTextureIndex = 1; myTextureIndex < _myTextureCount; myTextureIndex++) {
                    myShapeBuilder.appendTexCoord(myElement, new Vector2f(myUVCoordX,1), myTextureIndex);
                    myShapeBuilder.appendTexCoord(myElement, new Vector2f(myUVCoordX + myXDelta,1), myTextureIndex);
                    myShapeBuilder.appendTexCoord(myElement, new Vector2f(myUVCoordX + myXDelta,0), myTextureIndex);
                    myShapeBuilder.appendTexCoord(myElement, new Vector2f(myUVCoordX ,0), myTextureIndex);
                    myUVCoordX += myXDelta;
                }
            } else {
                var myShapeVertexPositions = getPositions();
                myShapeVertexPositions[j + 0] = myPositions[0];
                myShapeVertexPositions[j + 1] = myPositions[1];
                myShapeVertexPositions[j + 2] = myPositions[2];
                myShapeVertexPositions[j + 3] = myPositions[3];
            }
            myNewLineFlag = false;
        }
        if (myBuildGeometry) {
            var myName = "Text_";// + urlEncode(_myText);

            _myShape = myShapeBuilder.buildNode();
            _myShape.name = myName;
            window.scene.shapes.appendChild(_myShape);

            _myBody = buildBodyNode(myName, _myShape.id);
            _myBody.insensible = true;
            window.scene.world.appendChild(_myBody);
        }
        return myAlignedCharacter;
    }

    /**********************************************************************
     *
     * Private
     *
     **********************************************************************/

    var _myText = null;
    var _myCharacters = null;
    var _myAlignment = CENTER_ALIGNMENT;

    var _myShape = null;
    var _myBody = null;
    var _myTextureCount = 1;

    function setup() {

        _myText = theText;//asUnicodeString(theText);
        _myCharacters = theCharacterSoup.createUnicodeText(_myText, theFontSize);
        var myMaterialId = theCharacterSoup.getAlphabetMap(theFontSize).material.id;
        var myMaterial = window.scene.dom.getElementById(myMaterialId);
        var myTextureUnits = myMaterial.childNode("textureunits");

        _myTextureCount = myTextureUnits.childNodesLength();
        if (thePrebuildFlag) {
            var myAlphabetMap = theCharacterSoup.getAlphabetMap(theFontSize);
            var myShapeBuilder = new ShapeBuilder();
            var myElement = myShapeBuilder.appendElement("quads", myAlphabetMap.material.id);

            var myCharacterPos = new Vector3f(-0.5, -0.5, 0.0);
            var myCharacterSize = new Vector3f(1.0, 1.0, 0.0);
            var myUVCoordX = 0.0;

            for (var i = 0; i < _myText.length; ++i) {
                var myCharacter = _myCharacters[i];
                //print("char=" + myCharacter.unicode, "uv=" + myCharacter.uv, "uvsize=" + myCharacter.uvsize);
                    myShapeBuilder.appendQuadWithCustomTexCoords(myElement,
                        myCharacterPos, myCharacterSize,
                        myCharacter.uv, myCharacter.uvsize, false);
                    var myXDelta = (1.0/_myText.length);
                    for(var myTextureIndex = 1; myTextureIndex < _myTextureCount; myTextureIndex++) {
                        myShapeBuilder.appendTexCoord(myElement, new Vector2f(myUVCoordX,1), myTextureIndex);
                        myShapeBuilder.appendTexCoord(myElement, new Vector2f(myUVCoordX + myXDelta,1), myTextureIndex);
                        myShapeBuilder.appendTexCoord(myElement, new Vector2f(myUVCoordX + myXDelta,0), myTextureIndex);
                        myShapeBuilder.appendTexCoord(myElement, new Vector2f(myUVCoordX ,0), myTextureIndex);
                        myUVCoordX += myXDelta;
                    }

            }

            var myName = "Text_";// + urlEncode(_myText);

            _myShape = myShapeBuilder.buildNode();
            _myShape.name = myName;
            window.scene.shapes.appendChild(_myShape);

            _myBody = buildBodyNode(myName, _myShape.id);
            _myBody.insensible = true;
            window.scene.world.appendChild(_myBody);
        }

    }
    if (thePrebuildFlag == undefined) {
        thePrebuildFlag = true;
    }
    setup();
}

function test_PathText() {

    var mySceneViewer = new SceneViewer(null);
    mySceneViewer.setup(800, 600, false, "PathText");

    const FONT_SIZE = 18;
    var myCharacterSoup = new CharacterSoup(mySceneViewer, "Unit", "${PRO}/testmodels/fonts/UnMd____.ttf", [FONT_SIZE]);

    var mySvgPath = null;
    var myPos = null;

    if (0) {
        const __PATHTEXT_TEST = [
            'M 0 0 l 200 0 l 200 200Z',
            'M 0,0 C 75,0 75,50 150,50 h 100 Z',
        ];

        mySvgPath = new SvgPath(__PATHTEXT_TEST[0]);
    } else {
        var mySvgFile = readFileAsString(expandEnvironment("${PRO}/testmodels/curves.svg"));
        var mySvgNode = new Node(mySvgFile);

        var mySvgPaths = []
        var myPaths = mySvgNode.findAll(".//path");

        for (var i = 0; i < myPaths.length; ++i) {
            mySvgPaths.push(new SvgPath(myPaths[i].d));
        }
        mySvgPath = mySvgPaths[0];
        myPos = new Vector3f(500, 1.15 * 100, 0);

        window.camera.position = new Vector3f(0,300,500);
    }
    var myPathAlign = new PathAlign(mySvgPath);

    //const __TEXT = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" +
                   //"abcdefghijklmnopqrstuvwxyz" +
                   //"0123456789!@#$%^&*()" +
                   //",<.>/?;:\'\"\\|[{]}-_=+";
    const __TEXT = "Longfire AV Ti THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG" +
                   "the quick brown fox jumps over the lazy dog";
    var myPathText = new PathText(mySceneViewer, __TEXT, FONT_SIZE, myCharacterSoup);
    myPathText.setTracking(0);

    var myAlignment = TOP_ALIGNMENT;
    var myRealign = true;

    var myBaseOnKey = mySceneViewer.onKey;
    mySceneViewer.onKey = function(theKey, theState, theX, theY, theShiftFlag, theControlFlag, theAltFlag) {
        if (theState && theKey == "space") {
            myAlignment += 1;
            if (myAlignment > BASELINE_ALIGNMENT) {
                myAlignment = TOP_ALIGNMENT;
            }
            myRealign = true;
        } else {
            myBaseOnKey(theKey, theState, theX, theY, theShiftFlag, theControlFlag, theAltFlag);
        }
    }

    mySceneViewer.onPostRender = function() {
        if (myRealign) {
            myPathText.setAlignment(myAlignment);
            myPathText.align(myPathAlign, myPos);
            myRealign = false;
        }
        window.getRenderer().draw(mySvgPath, [1,1,0,1]);
    }

    mySceneViewer.go();
}
//test_PathText();
