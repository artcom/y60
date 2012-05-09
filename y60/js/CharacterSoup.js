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

//use("Unicode.js");
use("ShapeBuilder.js");
use("BuildUtils.js");
//use("Unicode.js");

//plug("GlurFilter");


function Character(theUnicode, theUV, theUVSize, theGlyphMetric) {
    this.unicode = String(theUnicode);
    this.uv      = new Vector2f(theUV);
    this.uvsize  = new Vector2f(theUVSize);
    this.metric  = theGlyphMetric;
}

function CharacterSoup(theFontname, theFontFilename, theSizes, theGlurRadi,
                       theGlyphPadding) {
    this.Constructor(this, theFontname, theFontFilename, theSizes, theGlurRadi,
                     theGlyphPadding);
}

CharacterSoup.prototype.Constructor = function(self, theFontname,
                                               theFontFilename, theSizes,
                                               theGlurRadi, theGlyphPadding)
{

    var _myAlphabetMap = [];
    var _myGlurRadi    = new Array();
    var _myTracking    = 0.0;

    var _myGlyphPadding = null;
    if (theGlyphPadding == undefined) {
        _myGlyphPadding = 0;
    } else {
        _myGlyphPadding = theGlyphPadding;
    }
    var _myParagraphTopOffset    = 0;
    var _myParagraphBottomOffset = 0;
    var _myLineHeight            = 0;

    const CHARACTERS_PER_LINE = 32; // must be power-of-two



    ////////////////////////////////////////////////////////////////////////////
    ///                        public functions                            /////
    ////////////////////////////////////////////////////////////////////////////

    self.getFontName = function(theSize) {
        if (theSize in _myAlphabetMap) {
            return _myAlphabetMap[theSize].fontname;
        }
        return null;
    }


    self.getFontMetrics = function(theSize) {
        if (theSize in _myAlphabetMap) {
            return _myAlphabetMap[theSize].fontmetrics;
        }
        return null;
    }


    self.getAlphabetMap = function(theSize) {
        if (!(theSize in _myAlphabetMap)) {
            self.setupFont(theSize);
        }
        return _myAlphabetMap[theSize];
    }
    self.setTracking = function(theTracking) {
        _myTracking = theTracking;
    }
    self.getTracking = function() {
        return _myTracking;
    }


    self.getPadding = function() {
        return _myGlyphPadding;
    }

    self.__defineGetter__('lineheight', function() { return _myLineHeight; } );
    self.__defineSetter__('lineheight', function(theLineHeight) { _myLineHeight = theLineHeight; } );

    self.setParagraph = function(theTopOffset, theBottomOffset) {
        _myParagraphTopOffset    = theTopOffset;
        _myParagraphBottomOffset = theBottomOffset;
    }
    self.getParagraph = function() {
        return {topoffset:_myParagraphTopOffset, bottomoffset:_myParagraphBottomOffset};
    }

    self.createText = function(theText, theSize) {
        self.createUnicodeText(theText, theSize);
        //self.createUnicodeText(asUnicodeString(theText), theSize);
    }


    self.createUnicodeText = function(theText, theSize) {

        if (!(theSize in _myAlphabetMap)) {
            Logger.warning("CharacterSoup.createText: No such size '" +
                           theSize + "'");
            self.setupFont(theSize);
        }

        var myCharacters = [];

        var myFontName = _myAlphabetMap[theSize].fontname;
        var myFontImage = _myAlphabetMap[theSize].fontimage;
        var myCellSize = _myAlphabetMap[theSize].cellsize;

        var myTexCoord = new Vector2f(0,0);
        var myTexSize = new Vector2f(0,0);


        for (var i = 0; i < theText.length; ++i) {
            var myChar = theText[i];
            var myFound = window.hasGlyph(myFontName, myChar);
            if (!myFound || myChar == "\n" || myChar == "\r" || myChar == "\t")
            {
                // map white space characters to space
                myChar = " ";
            }
            var myCharacter = null;

            if (!(myChar in _myAlphabetMap[theSize])) {

                var mySlot = new Vector2f(_myAlphabetMap[theSize].nextCharSlot);
                var myMetric = window.getGlyphMetrics(myFontName, myChar);

                //print("char="+myChar,"max="+myMetric.max,
                //      "min="+myMetric.min,"advance="+myMetric.advance,
                //      "found="+myFound);

                var myGlyphWidth = myMetric.max.x - myMetric.min.x + 1;
                var myHeight = window.getFontMetrics(myFontName).height;
                // pad with one extra pixel to prevent filtering artefacts
                var myPaddedSize = new Vector2f(myGlyphWidth +
                                                _myGlyphPadding * 2,
                                                myHeight +
                                                _myGlyphPadding * 2);

                if (_myAlphabetMap[theSize].nextCharSlot.x >=
                    (myFontImage.width - myPaddedSize[0]))
                {
                    _myAlphabetMap[theSize].nextCharSlot.x = 0;
                    _myAlphabetMap[theSize].nextCharSlot.y += myCellSize;
                    if (_myAlphabetMap[theSize].nextCharSlot.y >=
                        myFontImage.height - (2*myCellSize))
                    {
                        Logger.error("Sorry, alphabet reached " +
                                     (CHARACTERS_PER_LINE*CHARACTERS_PER_LINE) +
                                     " chars.");
                        exit(1);
                    }
                }

                var myTmpImage = Modelling.createImage(window.scene,
                                                       myPaddedSize[0],
                                                       myPaddedSize[1],
                                                       "RGBA");
                var surfaceWidth = myMetric.advance + _myGlyphPadding
                                   + Math.abs(myMetric.min.x);
                var surfaceHeight = myHeight+_myGlyphPadding;
                var myStyle = new Node("<style/>");
                var mySurfaceSize = window.renderTextAsImage(myTmpImage,
                                                             myChar,
                                                             myFontName,
                                                             myStyle,
                                                             surfaceWidth,
                                                             surfaceHeight,
                                                             [_myGlyphPadding,
                                                              _myGlyphPadding]);
                //print("myPaddedSize:",myPaddedSize);
                //print("myTmpImage.size:[",myTmpImage.width,",",
                //      myTmpImage.height,"]");
                // saveImage(myTmpImage, ""+myChar + "_" + theSize + ".png");

                if (_myGlurRadi[theSize] > 0) {
                    var myBlurParams = [_myGlurRadi[theSize],
                                        myPaddedSize[0],
                                        myPaddedSize[1],
                                        1.3];
                    applyImageFilter(myTmpImage, "gaussianblur", myBlurParams);
                }

                //saveImage(myTmpImage,""+myChar +"_"+ theSize+"_blurred.png");
                var myFontMetrics = _myAlphabetMap[theSize].fontmetrics;

                //print("char=",myChar," glyphwidth=",myGlyphWidth,
                //      " charsize=",myCharSurfaceSize,
                //      " height=",myFontMetrics.height,
                //      " ascent=",myFontMetrics.ascent,
                //      " descent=",myFontMetrics.descent);

                // blur/glow
                //if (_myGlurRadi[theSize] > 0) {
                //    applyGlurFilter(_myGlurRadi[theSize], myTmpImage);
                //}

                // The character is rendered starting with the leftmost pixel
                // of myTmpImage, so we need do take the metrics into account,
                // when blitting.
                var myBlitPos = new Vector2f(mySlot[0], mySlot[1]);
                if (myBlitPos.x < 0) {
                    Logger.error("Blit pos < 0");
                    myBlitPos.x = 0;
                }
                blitImage(myTmpImage, myFontImage, myBlitPos);
                window.scene.images.removeChild(myTmpImage);

                // Save the texture coordinates for each character
                myTexCoord[0] = mySlot.x / myFontImage.width;
                myTexCoord[1] = mySlot.y / myFontImage.height;
                myTexSize[0] = myPaddedSize[0] / myFontImage.width;
                myTexSize[1] = myPaddedSize[1] / myFontImage.height;

                //print("myTexCoord:",myTexCoord," myTexSize:",myTexSize);
                //print("\n");

                // position of next character
                //var advance = myMetric.advance + 2 * _myGlyphPadding;
                _myAlphabetMap[theSize].nextCharSlot.x += myPaddedSize[0] + 1;

                myCharacter = new Character(myChar,
                                            myTexCoord,
                                            myTexSize,
                                            myMetric);
                _myAlphabetMap[theSize][myChar] = myCharacter;
            } else {
                myCharacter = _myAlphabetMap[theSize][myChar];
            }
            myCharacters.push(myCharacter);
        }
        //saveImage(myFontImage, myFontName + "_" + theSize  + ".png");

        return myCharacters;
    }


    self.setupFont = function(theSize) {

        if (theSize in _myAlphabetMap) {
            Logger.warning("Font '" + theFontname +
                           "' size=" + theSize + " is already in map");
            return;
        }

        // load font
        var myFontName = theFontname + "_" + String(theSize);
        window.loadTTF(myFontName, expandEnvironment(theFontFilename), theSize);

        var myFontMetrics = window.getFontMetrics(myFontName);
        var myCellSize = nextPowerOfTwo(myFontMetrics.height +
                                        2 * _myGlyphPadding);
        Logger.info("CharacterSoup.setupFont " + theFontname,
                    "size=" + theSize, "cellsize=" + myCellSize);

        var myFontImageSize = myCellSize * CHARACTERS_PER_LINE;
        var myFontImage = Modelling.createImage(window.scene,
                                                myFontImageSize,
                                                myFontImageSize,
                                                "RGBA");
        myFontImage.resize = "pad";
        //myFontImage.wrapmode = "clamp";
        //myFontImage.mipmap = false;
//        var myFakeImage = Modelling.createImage(window.scene,
//                                                "dropshadow_fake.png");
//       var myMaterial =
//            buildUnlitTextureMaterialNode(myFontName + "_material",
//                                          myFontImage.id);
       var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myFontImage);
         //print(myMaterial);
         //print(myFontImage);
//       var myMaterial =
//            buildUnlitTextureMaterialNode(myFontName + "_material",
//                                          myFakeImage.id);
        myMaterial.name = myFontName + "_material";
        addMaterialRequirement(myMaterial, "vertexparams", "[10[color]]");
        myMaterial.transparent = true;
        myMaterial.properties.targetbuffers = "[red,green,blue,alpha]"
        //window.scene.materials.appendChild(myMaterial);

        _myAlphabetMap[theSize] = [];
        _myAlphabetMap[theSize].material = myMaterial;
        _myAlphabetMap[theSize].cellsize = myCellSize;
        _myAlphabetMap[theSize].fontimage = myFontImage;
        _myAlphabetMap[theSize].fontname = myFontName;
        _myAlphabetMap[theSize].fontmetrics = myFontMetrics;

        var initial_slot = new Vector2f(0,0);
        _myAlphabetMap[theSize].nextCharSlot = initial_slot;

        window.scene.update(Scene.MATERIALS);
    }



    ////////////////////////////////////////////////////////////////////////////
    ///                        private functions                           /////
    ////////////////////////////////////////////////////////////////////////////

    function applyGlurFilter(theRadius, theImageNode) {
        // taken from http://www.cazabon.com/pyCMS/PIL_usm.html
        var a = [];
        var radius = theRadius; // this actually is the diameter (must not be even!)
                            // only half of the calculated values are actually used (quick hack)
        for (var x = 0; x < radius; x++) {
            var z = ((x + 2)/(radius));
            var dev = 0.5 + (((radius * radius)) * 0.001); // you can adjust this factor to change the shape/center-weighting of the gaussian
            var y = Math.pow((1.0 / Math.sqrt(2.0 * 3.14159265359 * dev)), ((-(z - 1.0) * -(x - 1.0)) / (2.0 * dev)));
            if (x+1>radius/2) { // second half (including center value) is used only
                a.push( y );
            }
        }

        // make sure, all values sum up to 1.0 (play with this value!)
        var sum = 0;
        for (x = 0; x < a.length; x++) {
            sum += a[x];
        }

        for (x = 0; x < a.length; x++) {
            a[x] *= (1.0/sum);
        }
        var text_color = window.getTextColor();
        var s = "[";
        for (var i = 0; i < 3; i++) {
            s += text_color[i] + ",";
        }
        s += "1.0," + a + "]";
        theImageNode.filter_params = s;
        applyImageFilter(theImageNode, "glur", theImageNode.filter_params);
    }

    function setup() {
        for (var i = 0; i < theSizes.length; ++i) {
            self.setupFont(theSizes[i]);
            _myGlurRadi[theSizes[i]] = [];
            if (theGlurRadi == undefined || theGlurRadi.length < theSizes.length) {
                if (theGlurRadi && theGlurRadi.length >0) {
                    // take the first for all sizes
                    _myGlurRadi[theSizes[i]] = theGlurRadi[0];
                } else {
                    // we do not want any glur
                    _myGlurRadi[theSizes[i]] = 0;
                }
            } else {
                // we glurradi for all different sizes
                _myGlurRadi[theSizes[i]] = theGlurRadi[i];
            }
        }
        window.scene.update(Scene.ALL);
    }

    setup();
}


function test() {
    use("SceneViewer.js");

    var mySceneViewer = new SceneViewer(null);
    mySceneViewer.setup(800, 600, false, "PathText");
    window.setTextPadding(0,0,0,0);
    const FONT_FILE_INVERSE = "FONTS/TxCaAc__.ttf";
    const TICKERFONT_SIZE = 24;
    var _myCharacterSoup = new CharacterSoup("Ticker", FONT_FILE_INVERSE, [TICKERFONT_SIZE]);
    _myCharacterSoup.createUnicodeText("Schweden",  TICKERFONT_SIZE);

}

//test();
