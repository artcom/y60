/*jslint nomen:false*/
/*globals spark, Logger, Vector2i, Modelling, window*/

/* This is a text rendering facility for spark
 * It supports autosizing: the quad has the appropriate size after
 * setting the 'text' property.
 */
spark.Text = spark.ComponentClass("Text");

spark.Text.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.ResizableRectangle);

    /////////////////////
    // Private Members //
    /////////////////////

    var _myStyle = null;
    var _myUpcase = false;

    var _myText  = "";
    var _myTextId = null;
    var _myTextItem = null;

    var _myImage = null;
    var _myMaxWidth;
    var _myMaxHeight;
    var _myMaxTextWidth = {};
    var _myGlyphPositions = []; // array of vector2f-pair per glyph of x1/y1 and x2/y2 coordinates in rendered image
    var _myLayoutHook;
    var _myTextChangedHook;
    var _myLineWidths = [];

    /////////////////////
    // Private Methods //
    /////////////////////
    
    Protected.handleI18nLanguage = function(e) {
        applyTextItemData();
    };

    function attachToI18nItem(theItemId) {
        if (_myTextItem) {
            _myTextItem.removeEventListener(spark.I18nEvent.LANGUAGE,
                                            Protected.handleI18nLanguage);
            _myTextItem = null;
        }
        if (theItemId) {
            _myTextItem = Public.getI18nItemByName(theItemId);
            if (!_myTextItem) {
                Logger.fatal("no i18n item named " + theItemId);
            }
            _myTextItem.addEventListener(spark.I18nEvent.LANGUAGE,
                    Protected.handleI18nLanguage);
            applyTextItemData();
        } else {
            Public.text = "";
        }
    }
 
    function applyTextItemData() {
        var myNewFontStyle = _myTextItem.fontStyle;
        if (myNewFontStyle) {
            if(spark.isFontStyleNode(myNewFontStyle)) {
                _myStyle = spark.mergeFontStyles(_myStyle, myNewFontStyle);
            }
        }
        Public.text = _myTextItem.text;
    }
   
    ///////////////////////
    // Protected Methods //
    ///////////////////////
    Protected.__defineGetter__("image", function () {
        return _myImage;
    });
    
    Protected.render = function () {
        var myWidth = {width : 0};
        _myLineWidths = [];
        var myTextInfo = spark.renderText(_myImage, _myText, _myStyle, new Vector2i(_myMaxWidth, _myMaxHeight), myWidth, _myLineWidths);
        _myGlyphPositions = myTextInfo.positions;
        _myMaxTextWidth =  myWidth.width;
        Public.width = myTextInfo.size.x;
        Public.height = myTextInfo.size.y;
        return myTextInfo.size;
    };
    
    ////////////////////
    // Public Methods //
    ////////////////////

    Public.__defineGetter__("text", function () {
        return _myText;
    });

    Public.__defineSetter__("text", function (theValue) {
        if (_myLayoutHook) {
            _myText = _myLayoutHook(theValue);
        } else {
            _myText = theValue;
        }
        if (_myText && _myUpcase) {
            _myText = _myText.toUpperCase();
        }        
        Protected.render();
        if (_myTextChangedHook) {
            _myTextChangedHook();
        }
    });

    Public.__defineGetter__("style", function () {
        return _myStyle;
    });

    Public.__defineSetter__("style", function (theValue) {
        _myStyle = theValue;
        Protected.render();
    });
    
    Public.__defineGetter__("upcase", function () {
        return _myUpcase;
    });
    
    Public.__defineGetter__("maxTextWidth", function () {
        return _myMaxTextWidth;
    });

    Public.__defineGetter__("glyphPositions", function () {
        return _myGlyphPositions;
    });

    Public.__defineGetter__("lineWidths", function () {
        return _myLineWidths;
    });

    Public.__defineGetter__("lineCount", function () {
        return _myLineWidths.length;
    });

    Base.realize = Public.realize;
    Public.realize = function () {
        // retrieve text
        _myText = Protected.getString("text", "");

        // handle internationalization
        _myTextId = Protected.getString("textId", "");

        // extract font style
        _myStyle = spark.fontStyleFromNode(Public.node);

        _myUpcase = Protected.getBoolean("upcase", false);

        _myMaxWidth = Protected.getNumber("maxWidth", 0);
        _myMaxHeight = Protected.getNumber("maxHeight", 0);

        _myImage = Modelling.createImage(window.scene, 16, 16, "BGRA");
        var myTexture  = Modelling.createTexture(window.scene, _myImage);
        myTexture.name = Public.name + "-texture";
        myTexture.wrapmode = "clamp_to_edge";
        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene,
                myTexture, Public.name + "-material", true);

        Base.realize(myMaterial);
        // finally, render the text
        Public.text = _myText;
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        if (_myTextId) {
            attachToI18nItem(_myTextId);
        }
        Base.postRealize();
    };

    Public.__defineGetter__("maxWidth", function () {
        return _myMaxWidth;
    });
    Public.__defineSetter__("maxWidth", function (w) {
        _myMaxWidth = w;
        Protected.render();
    });

    Public.__defineGetter__("maxHeight", function () {
        return _myMaxHeight;
    });
    Public.__defineSetter__("maxHeight", function (h) { 
        _myMaxHeight = h; 
        Protected.render();
    });

    Public.__defineGetter__("textId", function () {
        return _myTextId;
    });
    Public.__defineSetter__("textId", function (id) {
        _myTextId = id;
        attachToI18nItem(id);
    });

    Public.__defineGetter__("i18nItem", function () {
        return Public.textId;
    });
    Public.__defineSetter__("i18nItem", function (id) {
        Public.textId = id;
    });

    Public.__defineGetter__("layoutHook", function () {
        return _myLayoutHook;
    });
    Public.__defineSetter__("layoutHook", function (f) {
        _myLayoutHook = f;
        Public.text = _myText;
    });

    Public.__defineGetter__("onTextChanged", function () {
        return _myTextChangedHook;
    });
    Public.__defineSetter__("onTextChanged", function (f) {
        _myTextChangedHook = f;
    });

    Public.__defineSetter__("hinting", function (theHinting) {
        _myStyle.hinting = theHinting;
        Protected.render();
    });
    Public.__defineGetter__("hinting", function () {
        return _myStyle.hinting;
    });
    Public.__defineSetter__("font", function (theFont) {
        _myStyle.font= theFont;
        Protected.render();
    });
    Public.__defineGetter__("font", function () {
        return _myStyle.font;
    });
    Public.__defineSetter__("fontSize", function (theFontSize) {
        _myStyle.fontSize = theFontSize;
        Protected.render();
    });
    Public.__defineGetter__("fontSize", function () {
        return _myStyle.fontSize;
    });
    Public.__defineSetter__("textColor", function (theColorString) {
        _myStyle.textColor = theColorString;
        Protected.render();
    });
    Public.__defineGetter__("textColor", function () {
        return _myStyle.textColor;
    });
};
