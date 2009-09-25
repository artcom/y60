
spark.Rectangle = spark.ComponentClass("Rectangle");

spark.Rectangle.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.ResizableRectangle);

    var _myColor;
    Base.realize = Public.realize;
    Public.realize = function(theMaterialOrShape) {

        _myColor = Protected.getVector3f("color", [1,1,1]);

        var myMaterial = Modelling.createColorMaterial(window.scene, new Vector4f(_myColor[0], _myColor[1], _myColor[2], Public.alpha));
        myMaterial.transparent = true;

        Base.realize(myMaterial);        
    };

    Public.color getter = function() {
        var c = Protected.material.properties.surfacecolor;
        return new Vector3f(c.r, c.g, c.b);
    };

    Public.color setter = function(theColor) {
        Protected.material.properties.surfacecolor = theColor;
    };
}
