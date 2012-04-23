/*jslint nomen:false*/
/*globals spark, window, Vector4f, Modelling*/

spark.Rectangle = spark.ComponentClass("Rectangle");

spark.Rectangle.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.ResizableRectangle);

    /////////////////////
    // Private Members //
    /////////////////////

    var _myColor;
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Base.realize = Public.realize;
    Public.realize = function (theMaterialOrShape) {
        var myMaterial = null;
        if (theMaterialOrShape) {
            if (theMaterialOrShape.nodeName === "shape") {
                var _myShape = theMaterialOrShape;
                var myMateriaId = _myShape.
                                  childNode("primitives").
                                  childNode("elements").material;
                myMaterial = _myShape.getElementById(myMateriaId);
            } else if (theMaterialOrShape.nodeName === "material") {
                myMaterial = theMaterialOrShape;
            }
        } else {
            _myColor = Protected.getVector3f("color", new Vector3f(1,1,1));
            myMaterial = Modelling.createColorMaterial(window.scene, _myColor.rgb1);
            myMaterial.transparent = true;
        }
        Base.realize(myMaterial);
    };
    
    Public.__defineGetter__("color", function () {
        return _myColor;
    });

    Public.__defineSetter__("color", function (theColor) {
        _myColor = theColor;
        Protected.material.properties.surfacecolor = new Vector4f(_myColor.x, _myColor.y, _myColor.z, Public.alpha);
    });
};
