/**
 * A labelesque text component.
 */
spark.Text = spark.ComponentClass("Text");

spark.Text.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.ResizableRectangle);

    var _myStyle = null;
    var _myText  = "";
    var _myTextId = null;
    var _myMaxTextWidth = 0;
    var _myLineWidths = [];
    var _myImage;
    var _myTexture;

    Public.text getter = function() { return _myText; };
    Public.text setter = function(theValue) {
        _myText = Protected.performLayout(theValue);
        Protected.render(Public.size);
    };

    Public.textId getter = function() { return _myTextId; };
    Public.textId setter = function(theValue) {
        // XXX: re-trigger i18n events? how?
        _myTextId = theValue;
    };
    Public.i18nItem getter = function() { return Public.textId; }
    Public.i18nItem setter = function(i) { Public.textId = i; }

    Public.style getter = function() {
        return _myStyle;
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


    Protected.performLayout = function(theText) {return theText;}

    Protected.render = function(theSize) {
        if (_myText) {
            var myWidth = {};
            _myLineWidths = [];
            var myTextSize = spark.renderText(_myImage, _myText, _myStyle, Public.size, myWidth, _myLineWidths);
            _myMaxTextWidth = myWidth.width;
            return myTextSize;
        }
    };

    Base.realize = Public.realize;
    Public.realize = function() {
        // retrieve text
        _myText = Protected.getString("text", "");

        // handle internationalization
        _myTextId = Protected.getString("textId", "");
        if(_myTextId != "") {
            var myI18nText = Protected.getI18nText(_myTextId);
            if(myI18nText) {
                _myText = myI18nText;
            }
        }

        // extract font style
        _myStyle = spark.fontStyleFromNode(Public.node);

        var myWidth  = Protected.getNumber("width");
        var myHeight = Protected.getNumber("height");
        _myImage = Modelling.createImage(window.scene, myWidth, myHeight, "BGRA");
        _myTexture  = Modelling.createTexture(window.scene, _myImage);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";
        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene,
                _myTexture, Public.name + "-material", true);

        Base.realize(myMaterial);

        _myText = Protected.performLayout(_myText);

        // finally, render the text
        Protected.render();
    };
    Protected.image getter = function() {return _myImage;}
};
