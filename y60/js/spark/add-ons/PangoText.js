/*jslint nomen:false*/
/*globals spark, Logger, Vector2i, Modelling, window*/

/* This is a text rendering facility for spark using pango with cairo component
 * It supports autosizing: the quad has the appropriate size after
 * setting the 'text' property.
 */

plug("Cairo");
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
    var _myFont = null;
    var _mySize = null;

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
        var mySurface = new Cairo.Surface(_myImage);
        var cairoContext = new Cairo.Context(mySurface);
        cairoContext.setAntialias(Cairo.ANTIALIAS_NONE);
        cairoContext.setSourceRGB(0,0,0);

        var myLayout = new Pango.Layout(cairoContext);
        var myFontDesc = new Pango.FontDescription(_myFont + " " + _mySize);
        myLayout.font_description = myFontDesc;
        myLayout.text = _myText;
        myLayout.update_from_cairo_context(cairoContext);
        myLayout.show_in_cairo_context(cairoContext);
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
        _myFont = "Atrissi-ATV";
        _mySize = 22;
        _myText = readFileAsString("text.txt");


        _myImage = Modelling.createImage(window.scene, 700, 700, "BGRA");
        var myTexture  = Modelling.createTexture(window.scene, _myImage);
        myTexture.wrapmode = "clamp_to_edge";
        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myTexture);


        Base.realize(myMaterial);

        //var myShape = Modelling.createQuad(window.scene, myMaterial.id, new Vector3f(50, 50, 1), new Vector3f(800, 800, 2));
        //var myNode = Modelling.createBody(window.scene.world, myShape.id);
        
        Public.text = _myText;
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
    };
};
