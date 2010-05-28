/* This is an experimental new text rendering facility for spark
 * It supports autosizing: the quad has the appropriate size after
 * setting the 'text' property.
 */
spark.NewText = spark.ComponentClass("NewText");

spark.NewText.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.ResizableRectangle);

    var _myStyle = null;
    var _myUpcase = false;

    var _myText  = "";
    var _myTextId = null;
    var _myTextItem = null;

    var _myImage = null;
    var _myMaxWidth;
    var _myMaxHeight;
    var _myMaxTextWidth = {};
    var _myLayoutHook;
    var _myTextChangedHook;
    var _myLineWidths = [];

    Public.text getter = function() { return _myText; };
    Public.text setter = function(theValue) {
        if(theValue && _myUpcase) {
            theValue = theValue.toUpperCase();
        }
        if(_myLayoutHook) {
            _myText = _myLayoutHook(theValue);
        } else {
            _myText = theValue;
        }
        Protected.render(Public.size);
        if (_myTextChangedHook) {
            _myTextChangedHook();
        }
    };

    Public.style getter = function() {
        return _myStyle;
    };

    Public.upcase getter = function() {
        return _myUpcase;
    };
    Public.maxTextWidth getter = function() {
        return _myMaxTextWidth;
    };

    Public.lineWidths getter = function() {
        return _myLineWidths;
    };

    Public.lineCount getter = function() {
        return _myLineWidths.length;
    };

    Protected.render = function(theSize) {
        var myWidth = {width:0};
        _myLineWidths = [];
        var mySize = spark.renderText(_myImage, _myText, _myStyle, new Vector2i(_myMaxWidth,_myMaxHeight), myWidth, _myLineWidths);        
        _myMaxTextWidth =  myWidth.width;
        Public.width = mySize.x;
        Public.height = mySize.y;
        return mySize;
    }

    Base.realize = Public.realize;
    Public.realize = function() {
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
        Public.text = _myText;

        // finally, render the text
        // redundant? text setter renders text; thus commented out.
        //Protected.render();
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        if (_myTextId) {
            attachToI18nItem(_myTextId);
        }
        Base.postRealize();
    }

    Public.maxWidth getter = function() { return _myMaxWidth; }
    Public.maxWidth setter = function(w) { 
        _myMaxWidth = w;
        Protected.render();
    };

    Public.maxHeight getter = function() { return _myMaxHeight; }
    Public.maxHeight setter = function(h) { 
        _myMaxHeight = h; 
        Protected.render();
    };

    Public.textId getter = function() { return _myTextId; }
    Public.textId setter = function(id) {
        _myTextId = id;
        attachToI18nItem(id);
    };

    Public.i18nItem getter = function() { return Public.textId; }
    Public.i18nItem setter = function(id) { Public.textId = id; }

    Public.layoutHook getter = function() { return _myLayoutHook; }
    Public.layoutHook setter = function(f) {
        _myLayoutHook = f;
        Public.text = _myText;
    }

    Public.onTextChanged getter = function() { return _myTextChangedHook; }
    Public.onTextChanged setter = function(f) { _myTextChangedHook = f; }

    function handleI18nLanguage(e) {
        Public.text = _myTextItem.text;
    }

    function attachToI18nItem(theItemId) {
        if(_myTextItem) {
            _myTextItem.removeEventListener(spark.I18nEvent.LANGUAGE,
                                            handleI18nLanguage);
            _myTextItem = null;
        }

        if (theItemId) {
            _myTextItem = Public.getI18nItemByName(theItemId);

            if(!_myTextItem) {
                Logger.fatal("no i18n item named " + theItemId);
            }
            _myTextItem.addEventListener(spark.I18nEvent.LANGUAGE,
                    handleI18nLanguage);
            Public.text = _myTextItem.text;
        } else {
            Public.text = "";
        }

    };
    Public.textColor setter = function(theColorString) {
        _myStyle.textColor = theColorString;
        Protected.render();
    }
};
