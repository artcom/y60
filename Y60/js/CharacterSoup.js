//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Unicode.js");
use("ShapeBuilder.js");
use("BuildUtils.js");
plug("GlurFilter");


function Character(theUnicode, theUV, theUVSize, theGlyphMetric) {
    this.unicode = String(theUnicode);
    this.uv      = new Vector2f(theUV);
    this.uvsize  = new Vector2f(theUVSize);
    this.metric  = theGlyphMetric;
}

function CharacterSoup(theSceneViewer, theFontname, theFontFilename, theSizes, theGlurRadi) {
    this.Constructor(this, theSceneViewer, theFontname, theFontFilename, theSizes, theGlurRadi);
}

CharacterSoup.prototype.Constructor = function(self, theSceneViewer, theFontname, theFontFilename, theSizes, theGlurRadi) {
    
    var _myAlphabetMap = [];
    var _myGlurRadi    = new Array();

    const CHARACTERS_PER_LINE = 16; // must be power-of-two

    /////////////////////////////////////////////////////////////////////////////
    ///                        public functions                            //////
    /////////////////////////////////////////////////////////////////////////////

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

    self.createText = function(theText, theSize) {
        self.createUnicodeText(asUnicodeString(theText), theSize);
    }
    self.createUnicodeText = function(theText, theSize) {
        if (!(theSize in _myAlphabetMap)) {
            Logger.warning("CharacterSoup.createText: No such size '" + theSize + "'");
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
            if (myChar == "\n" || myChar == "\r" || myChar == "\t") {
                // map white space characters to space
                myChar = asUnicodeString(" ");
            }
            var myCharacter = null;
            if (!(myChar in _myAlphabetMap[theSize])) {
                // character is not in alphabet map -> render it into alphabet image
                var myTargetUVPosition = new Vector2f(_myAlphabetMap[theSize].nextCharSlot);
                var myMetric = window.getGlyphMetrics(myFontName, myChar);
                var myGlyphSize = difference(myMetric.max, myMetric.min);

                var myTmpImage = window.scene.createImage(myGlyphSize.x, myGlyphSize.y, "RGBA");
                var myCharSize = window.renderTextAsImage(myTmpImage, myChar, myFontName, 0, 0);

                var myFontMetrics = _myAlphabetMap[theSize].fontmetrics;
                //print("char=" + myChar, "glyphsize=" + myGlyphSize, "charsize=" + myCharSize, "height=" + myFontMetrics.height, myFontMetrics.ascent, myFontMetrics.descent);

                // blur/glow
                if (_myGlurRadi[theSize] > 0) {
                    applyGlurFilter(_myGlurRadi[theSize], myTmpImage);
                }
                blitImage(myTmpImage, myFontImage, myTargetUVPosition);
                theSceneViewer.getImages().removeChild(myTmpImage);

                // texture coordinates
                myTexCoord[0] = myTargetUVPosition.x / myFontImage.width;
                myTexCoord[1] = myTargetUVPosition.y / myFontImage.height;
                myTexSize[0] = (myCharSize[0] /*+ _myGlurRadi[theSize] * 0.5*/) / myFontImage.width;
                myTexSize[1] = (myCharSize[1] /*+ _myGlurRadi[theSize] * 0.5*/) / myFontImage.height;

                // position of next character
                _myAlphabetMap[theSize].nextCharSlot.x += myCellSize;
                if (_myAlphabetMap[theSize].nextCharSlot.x > myFontImage.width - (2*myCellSize)) {
                    _myAlphabetMap[theSize].nextCharSlot.x = 1;
                    _myAlphabetMap[theSize].nextCharSlot.y += myCellSize;
                    if (_myAlphabetMap[theSize].nextCharSlot.y > myFontImage.height - (2*myCellSize)) {
                        Logger.error("Sorry, alphabet reached " + (CHARACTERS_PER_LINE*CHARACTERS_PER_LINE)  + " chars.");
                        exit(1);                            
                    }
                }

                myCharacter = new Character(myChar, myTexCoord, myTexSize, myMetric);
                _myAlphabetMap[theSize][myChar] = myCharacter;                    
            } else {
                myCharacter = _myAlphabetMap[theSize][myChar];
            }

            myCharacters.push(myCharacter);
        }
        //saveImage(myFontImage, myFontName + ".png");

        return myCharacters;
    }
    
    self.setupFont = function(theSize) {

        if (theSize in _myAlphabetMap) {
            Logger.warning("Font '" + theFontname + "' size=" + theSize + " is already in map");
            return;
        }

        // load font
        var myFontName = theFontname + "_" + String(theSize);
        window.loadTTF(myFontName, expandEnvironment(theFontFilename), theSize);

        var myFontMetrics = window.getFontMetrics(myFontName);
        var myCellSize = nextPowerOfTwo(myFontMetrics.height);
        Logger.info("CharacterSoup.setupFont " + theFontname, "size=" + theSize, "cellsize=" + myCellSize);

        var myFontImageSize = myCellSize * CHARACTERS_PER_LINE;
        var myFontImage = window.scene.createImage(myFontImageSize, myFontImageSize, "RGBA");
        window.scene.update(Scene.IMAGES);

        var myMaterial = buildUnlitTextureMaterialNode(myFontName + "_material", myFontImage.id);
        addMaterialRequirement(myMaterial, "vertexparams", "[10[color]]");
        myMaterial.writedepthbuffer = false;
        myMaterial.transparent = true;
        theSceneViewer.getMaterials().appendChild(myMaterial);

        _myAlphabetMap[theSize] = [];
        _myAlphabetMap[theSize].material = myMaterial.id;
        _myAlphabetMap[theSize].cellsize = myCellSize;
        _myAlphabetMap[theSize].nextCharSlot = new Vector2f(1,0);
        _myAlphabetMap[theSize].fontimage = myFontImage;
        _myAlphabetMap[theSize].fontname = myFontName;
        _myAlphabetMap[theSize].fontmetrics = myFontMetrics;

        window.scene.update(Scene.MATERIALS);
    }

    /////////////////////////////////////////////////////////////////////////////
    ///                        private functions                           //////
    /////////////////////////////////////////////////////////////////////////////

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
        var s = "[1.0,1.0,1.0,0.0," + a + "]";
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
