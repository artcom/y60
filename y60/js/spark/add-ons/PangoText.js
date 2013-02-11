/*jslint nomen:false*/
/*globals spark, Logger, Vector2i, Modelling, window*/

/* This is a text rendering facility for spark using pango with cairo component
 * It supports autosizing: the quad has the appropriate size after
 * setting the 'text' property.
 */

plug("Pango");


spark.PangoText = spark.ComponentClass("PangoText");

spark.PangoText.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.ResizableRectangle);

    /////////////////////
    // Private Members //
    /////////////////////

    var _myText  = "";
    var _myImage = null;

    var _myMaxWidth = null;
    var _myMaxHeight = null;

    var _myStyle = {};
    var _myPangoLayout = null;

    var RENDER_AREA_SIZE = 700; //TODO: maybe this should be maxwidth/maxheight later

    /////////////////////
    // Private Methods //
    /////////////////////
    
   
    ///////////////////////
    // Protected Methods //
    ///////////////////////
    Protected.__defineGetter__("image", function () {
        return _myImage;
    });
    
    Protected.render = function () {
        applyImageFilter(_myImage, "resizebox", [RENDER_AREA_SIZE, RENDER_AREA_SIZE]);
        var dimensions = _myPangoLayout.setText(_myText);
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
        if (_myMaxWidth > -1) {
            _myPangoLayout.setWidth(_myMaxWidth);
        }
        if (_myMaxHeight > -1) {
            _myPangoLayout.setHeight(_myMaxHeight);
        }
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
        Public.text = _myText;
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
    };
};
