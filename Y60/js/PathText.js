////=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneViewer.js");
use("CharacterSoup.js");
use("PathAlign.js");

function PathText(theSceneViewer, theText, theFontSize, theCharacterSoup) {
    this.Constructor(this, theSceneViewer, theText, theFontSize, theCharacterSoup);
}

PathText.prototype.Constructor = function(self, theSceneViewer, theText, theFontSize, theCharacterSoup) {
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
        var myVertexPositions = getDescendantByAttribute(_myShape, "name", "position", true);
        var myPositions = myVertexPositions.childNode('#text')[1];
        return myPositions;
    }
    self.getPositionsNode = function() {
        var myVertexPositions = getDescendantByAttribute(_myShape, "name", "position", true);
        return myVertexPositions;
    }

    self.getColors = function() {
        var myVertexColors = getDescendantByAttribute(_myShape, "name", "color", true);
        var myColors = myVertexColors.childNode('#text')[1];
        return myColors;
    }

    self.setAlignment = function(theAlignment) {
        _myAlignment = theAlignment;
    }

    /**
     * Align this text along given path.
     * - if given, start near 'thePos'; default is start of path.
     * - if given, start at character 'theFirstCharacter'; default is 0.
     * - if given, end at character 'theLastCharacter'; default is last character in text.
     */
    self.align = function(thePathAlign, thePos, theFirstCharacter, theLastCharacter) {
        if (thePos != undefined) {
            thePathAlign.setCurrentPosition(thePos);
        }
        if (theFirstCharacter == undefined) {
            theFirstCharacter = 0;
        }
        if (theLastCharacter == undefined) {
            theLastCharacter = _myText.length;
        }

        var myAlphabetMap = theCharacterSoup.getAlphabetMap(theFontSize);
        var myFontMetrics = theCharacterSoup.getFontMetrics(theFontSize);
        var myWidth = 0.0;
        var myVertexPositions = getDescendantByAttribute(_myShape, "name", "position", true);
        var myPositions = myVertexPositions.childNode('#text')[1]; // firstChild.nodeValue;

        for (var i = theFirstCharacter; i < theLastCharacter; ++i) {
            var myChar = _myText[i];
            if (myChar == "\n" || myChar == "\r" ) {
                // line break
                var myNewLineOffet = 0;
                if (_myText.length > i+1) {
                    if (_myText[i+1] == "\n") {
                        //print("found carriage return AND a newline");
                        myNewLineOffet = 1;
                    }
                }                
                return i + myNewLineOffet;
            }
            var myCharacter = _myCharacters[i];
            if (myCharacter == null) {
                continue;
            }

            // advance
            myWidth = myCharacter.metric.advance;

            // kerning
            if (i < (_myText.length-1)) {
                var myNextCharacter = _myCharacters[i+1];
                if (myNextCharacter != null) {
                    var myKerning = window.getKerning(theCharacterSoup.getFontName(theFontSize), myCharacter.unicode, myNextCharacter.unicode);
                    myWidth += myKerning;
                    if (myKerning != 0.0) {
                        //print("kern=" + myKerning, "char=" + myCharacter.unicode, "next=" + myNextCharacter.unicode);
                    }
                }
            }

            /*
             * move 'advance+kern' on path but make characters 'advance' wide
             */
            var mySegment = thePathAlign.advance(myWidth);
            if (mySegment == null) {
                break;
            }
            var myForwardVector = difference(mySegment.end, mySegment.start);
            myForwardVector = product(normalized(myForwardVector), myCharacter.metric.advance);
            var myLeftVector = normalized(cross(UP_VECTOR, myForwardVector));

            // align
            var myTop, myBottom;
            switch (_myAlignment) {
                case TOP_ALIGNMENT:
                    myTop = (myFontMetrics.height - myFontMetrics.ascent);
                    myBottom = -(myFontMetrics.ascent - myFontMetrics.descent) + myTop;
                    break;
                case BOTTOM_ALIGNMENT:
                    myTop = myFontMetrics.ascent - myFontMetrics.descent;
                    myBottom = 0; 
                    break;
                case CENTER_ALIGNMENT:
                    myTop = myAlphabetMap.cellsize * 0.5 - (myAlphabetMap.cellsize - myFontMetrics.height) * 0.5;
                    myBottom = -myTop;
                    break;
                case BASELINE_ALIGNMENT:
                    myTop = myFontMetrics.ascent;
                    myBottom = myFontMetrics.descent;
                    break;
            }
            //print("top=" + myTop, "bottom=" + myBottom, "sum=" + (myTop - myBottom));
            var myTopOffset = product(myLeftVector, myTop);
            var myBottomOffset = product(myLeftVector, myBottom);

            var myStart = mySegment.start;
            var myEnd = sum(myStart, myForwardVector);
            myPositions[i * 4 + 0] = sum(myStart, myBottomOffset);
            myPositions[i * 4 + 1] = sum(myEnd, myBottomOffset);
            myPositions[i * 4 + 2] = sum(myEnd, myTopOffset);
            myPositions[i * 4 + 3] = sum(myStart, myTopOffset);
        }

        // write-back positions
        //myVertexPositions.childNode('#text')[1] = myPositions;
        return _myText.length;
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

    function setup() {

        _myText = asUnicodeString(theText);
        _myCharacters = theCharacterSoup.createUnicodeText(_myText, theFontSize);

        var myAlphabetMap = theCharacterSoup.getAlphabetMap(theFontSize);
        var myShapeBuilder = new ShapeBuilder();
        var myElement = myShapeBuilder.appendElement("quads", myAlphabetMap.material);

        var myCharacterPos = new Vector3f(-0.5, -0.5, 0.0);
        var myCharacterSize = new Vector3f(1.0, 1.0, 0.0);

        for (var i = 0; i < _myText.length; ++i) {
            var myCharacter = _myCharacters[i];
            myShapeBuilder.appendQuadWithCustomTexCoords(myElement,
                    myCharacterPos, myCharacterSize,
                    myCharacter.uv, myCharacter.uvsize, false);
        }

        var myName = "Text_" + urlEncode(_myText);

        _myShape = myShapeBuilder.buildNode();
        _myShape.name = myName;
        theSceneViewer.getShapes().appendChild(_myShape);

        _myBody = buildBodyNode(myName, _myShape.id);
        _myBody.insensible = true;
        theSceneViewer.getWorld().appendChild(_myBody);
    }

    setup();
}

function test_PathText() {

    var mySceneViewer = new SceneViewer(null);
    mySceneViewer.setup(800, 600, false, "PathText");

    const FONT_SIZE = 18;
    var myCharacterSoup = new CharacterSoup(mySceneViewer, "Unit", "${PRO}/testmodels/fonts/UnitMed.ttf", [FONT_SIZE]);

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
        var myPaths = getDescendantsByTagName(mySvgNode, "path", true);
        for (var i = 0; i < myPaths.length; ++i) {
            mySvgPaths.push(new SvgPath(myPaths[i].d));
        }
        mySvgPath = mySvgPaths[0];
        myPos = new Vector3f(0, 1.15 * 100, 0);

        window.camera.position = new Vector3f(0,300,500);
    }
    var myPathAlign = new PathAlign(mySvgPath);

    const __TEXT = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" +
                   "abcdefghijklmnopqrstuvwxyz" +
                   "0123456789!@#$%^&*()" +
                   ",<.>/?;:\'\"\\|[{]}-_=+";
    //const __TEXT = "hello Helo AV Ti Wgj.....";
    var myPathText = new PathText(mySceneViewer, __TEXT, FONT_SIZE, myCharacterSoup);

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
