/*jslint nomen:false*/
/*globals spark, use, Vector4f, ShapeStretcher*/

use("spark/add-ons/ShapeStretcher.js");

spark.StretchyAtlasImage = spark.ComponentClass("StretchyAtlasImage");

spark.StretchyAtlasImage.Constructor = function (Protected) {
    var Public = this;
    var Base   = {};
    Public.Inherit(spark.AtlasImage);
    
    // Private Members //
    
    var _myShapeStretcher = null;
    
    // Public Methods //
    
    Base.realize = Public.realize;
    Public.realize = function () {
        
        Base.realize();
        // ...now we have the shape
        _myShapeStretcher = new ShapeStretcher(Protected.shape, {
            edges : new Vector4f(Protected.getNumber("edgeTop",    0),
                                 Protected.getNumber("edgeLeft",   0),
                                 Protected.getNumber("edgeBottom", 0),
                                 Protected.getNumber("edgeRight",  0)),
            crop  : new Vector4f(Protected.getNumber("cropTop",    0),
                                 Protected.getNumber("cropLeft",   0),
                                 Protected.getNumber("cropBottom", 0),
                                 Protected.getNumber("cropRight",  0))
        });
    };
    
};