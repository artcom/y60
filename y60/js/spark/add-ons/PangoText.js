/*jslint nomen:false*/
/*globals spark, Logger, Vector2i, Modelling, window*/

/* This is a text rendering facility for spark using pango with cairo component
 * It supports autosizing: the quad has the appropriate size after
 * setting the 'text' property.
 */

spark.PangoText = spark.ComponentClass("PangoText");

spark.PangoText.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.ResizableRectangle);

    plug("Pango");

    /////////////////////
    // Private Members //
    /////////////////////

    var _myText  = "";
    var _myTextId = null;
    var _myTextItem = null;

    var _myImage = null;

    var _myMaxWidth = null;
    var _myMaxHeight = null;

    var _myStyle = {};
    var _myPangoLayout = null;

    var RENDER_AREA_SIZE = 700; //TODO: maybe this should be maxwidth/maxheight later

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
        //TODO: font style merging needed? (done by widget/Text)
        Public.text = _myTextItem.text;
    }
   
    ///////////////////////
    // Protected Methods //
    ///////////////////////
    Protected.__defineGetter__("image", function () {
        return _myImage;
    });
    
    Protected.render = function (debug) {
        applyImageFilter(_myImage, "resizebox", [RENDER_AREA_SIZE, RENDER_AREA_SIZE]);
        var dimensions = _myPangoLayout.setText(_myText);
        //for some fonts small parts of the letters are drawn outside the box given by dimension
        if (debug) {
            _myPangoLayout.saveToPNG("PangoText_debug.png");
        }

        //TODO: make this configurable?
        dimensions[0] += 10;
        dimensions[1] += 10;
        if (_myMaxWidth > -1) {
            dimensions[0] = _myMaxWidth;
        }
        if (_myMaxHeight > -1) {
            dimensions[1] = _myMaxHeight;
        }
        applyImageFilter(_myImage, "crop", [0,0,dimensions[0],dimensions[1]]);
        Public.width = dimensions[0];
        Public.height = dimensions[1];
    };
    
    ////////////////////
    // Public Methods //
    ////////////////////

    Public.__defineGetter__("text", function () {
        return _myText;
    });

    Public.__defineSetter__("text", function (theValue) {
        _myText = theValue;
        Protected.render();
    });

    Base.realize = Public.realize;
    Public.realize = function () {

        _myText = Protected.getString("text", "");
        // handle internationalization
        _myTextId = Protected.getString("textId", "");

        _myStyle.fontName = Protected.getString("fontName", "Arial");
        _myStyle.fontSize = Protected.getString("fontSize", 23);
        _myStyle.textColor = asColor(Protected.getString("textColor", "777777ff"));
        _myStyle.backgroundColor = Protected.getString("backgroundColor", null);
        if (_myStyle.backgroundColor) {
            _myStyle.backgroundColor = asColor(_myStyle.backgroundColor);
        }
        _myMaxWidth = Protected.getNumber("maxWidth", -1);
        _myMaxHeight = Protected.getNumber("maxHeight", -1);
        _myStyle.lineSpacing = Protected.getString("lineSpacing", null);
        _myStyle.indent = Protected.getString("indent", null);
        _myStyle.hAlign = Protected.getString("hAlign", "left");

        _myImage = Modelling.createImage(window.scene, RENDER_AREA_SIZE, RENDER_AREA_SIZE, "BGRA");
        var myTexture  = Modelling.createTexture(window.scene, _myImage);
        myTexture.name = Public.name + "-texture";
        myTexture.wrapmode = "clamp_to_edge";
        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene,
                myTexture, Public.name + "-material", true);

        Base.realize(myMaterial);
        _myPangoLayout = new Pango.Layout(_myImage);
        var description = _myStyle.fontName + " " + _myStyle.fontSize;
        var myFontDesc = new Pango.FontDescription(description);
        _myPangoLayout.font_description = myFontDesc;
        _myPangoLayout.setWidth(_myMaxWidth);
        _myPangoLayout.setHeight(_myMaxHeight);
        _myPangoLayout.setColor(new Vector4f(_myStyle.textColor[0],
                                             _myStyle.textColor[1],
                                             _myStyle.textColor[2],
                                             _myStyle.textColor[3]));
        if (_myStyle.backgroundColor) {
            _myPangoLayout.setBackground(new Vector4f(_myStyle.backgroundColor[0],
                                             _myStyle.backgroundColor[1],
                                             _myStyle.backgroundColor[2],
                                             _myStyle.backgroundColor[3]));
        }
        if (_myStyle.lineSpacing) {
            _myPangoLayout.setSpacing(_myStyle.lineSpacing);
        }
        if (_myStyle.indent) {
            _myPangoLayout.setIndent(_myStyle.indent);
        }
        if (_myStyle.hAlign) {
            _myPangoLayout.setAlignment(_myStyle.hAlign);
        }
        Public.text = _myText;
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        if (_myTextId) {
            attachToI18nItem(_myTextId);
        }
        Base.postRealize();
    };

    Public.debug = function() {
        Protected.render(true);
    };

    //////////////////////////////////////////////////Getter/Setter
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
    Public.__defineGetter__("maxWidth", function () {
        return _myMaxWidth;
    });
    Public.__defineSetter__("maxWidth", function (w) {
        _myMaxWidth = w;
        _myPangoLayout.setWidth(_myMaxWidth);
        Protected.render();
    });
    Public.__defineGetter__("maxHeight", function () {
        return _myMaxHeight;
    });
    Public.__defineSetter__("maxHeight", function (h) { 
        _myMaxHeight = h; 
        _myPangoLayout.setHeight(_myMaxHeight);
        Protected.render();
    });
    Public.__defineSetter__("textColor", function (theColorString) {
        _myStyle.textColor = asColor(theColorString);
        _myPangoLayout.setColor(new Vector4f(_myStyle.textColor[0],
                                             _myStyle.textColor[1],
                                             _myStyle.textColor[2],
                                             _myStyle.textColor[3]));
        Protected.render();
    });
    Public.__defineGetter__("textColor", function () {
        return _myStyle.textColor;
    });
};
