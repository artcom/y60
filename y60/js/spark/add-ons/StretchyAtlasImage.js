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
    var _myImageSize      = null;
    
    // Public Methods //
    
    Base.realize = Public.realize;
    Public.realize = function () {
        
        Base.realize();
        // ...now we have the shape
        _myShapeStretcher = new ShapeStretcher(Protected.shape, {
            edges : {'top'    : Protected.getNumber("edgeTop",    0),
                     'left'   : Protected.getNumber("edgeLeft",   0),
                     'bottom' : Protected.getNumber("edgeBottom", 0),
                     'right'  : Protected.getNumber("edgeRight",  0)},
            crop  : {'top'    : Protected.getNumber("cropTop",    0),
                     'left'   : Protected.getNumber("cropLeft",   0),
                     'bottom' : Protected.getNumber("cropBottom", 0),
                     'right'  : Protected.getNumber("cropRight",  0)},
            quadsPerSide : new Vector2i(Protected.getNumber("quadsPerSideX", 3),
                                        Protected.getNumber("quadsPerSideY", 3)),
            uvTransformationMatrix: Protected.uvTransformationMatrix
        });
        
        //_myImageSize  = getImageSize(Public.image);
        Base.imageSetter = Public.__lookupSetter__("image");
        Public.__defineSetter__("image", function (theImage) {
            Base.imageSetter(theImage);
            //_myImageSize = getImageSize(theImage);
            _myShapeStretcher.setupGeometry(Public.size, Public.origin);
        });

        Base.widthSetter = Public.__lookupSetter__("width");
        Public.__defineSetter__("width", function (theWidth) {
            Base.widthSetter(theWidth);
            _myShapeStretcher.updateGeometry(new Vector2f(theWidth, Public.height), false, Public.origin);
        });

        Base.heightSetter = Public.__lookupSetter__("height");
        Public.__defineSetter__("height", function (theHeight) {
            Base.heightSetter(theHeight);
            _myShapeStretcher.updateGeometry(new Vector2f(Public.width, theHeight), false, Public.origin);
        });

        _myShapeStretcher.initialize();
        _myShapeStretcher.setupGeometry (Protected.originalImageSize, Public.origin);
        _myShapeStretcher.updateGeometry(Public.size, false, Public.origin);
    };
    
};
