/*jslint nomen:false*/
/*globals spark, use, Vector4f, Vector2f, Vector2i ShapeStretcher*/

use("spark/add-ons/ShapeStretcher.js");

spark.StretchyAtlasImage = spark.ComponentClass("StretchyAtlasImage");

spark.StretchyAtlasImage.Constructor = function (Protected) {
    var Public = this;
    var Base   = {};
    Public.Inherit(spark.AtlasImage);
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myShapeStretcher = null;
    var _myImageSize      = null;
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function _addEdgeProperty(theAcessorName, theEdgeName) {
        Public.__defineGetter__(theAcessorName, function () {
            return _myShapeStretcher.edges[theEdgeName];
        });

        Public.__defineSetter__(theAcessorName, function (theValue) {
            _myShapeStretcher.edges[theEdgeName] = theValue;
            _myShapeStretcher.updateGeometry(Public.size, false, Public.origin);
        });
    }
    
    function _addCropProperty(theAcessorName, theEdgeName) {
        Public.__defineGetter__(theAcessorName, function () {
            return _myShapeStretcher.crop[theEdgeName];
        });

        Public.__defineSetter__(theAcessorName, function (theValue) {
            _myShapeStretcher.crop[theEdgeName] = theValue;
            _reset();
        });
    }
    
    function _reset() {
        _myShapeStretcher.setupGeometry(Protected.originalImageSize, Public.origin);
        Protected.storeOriginalUVCoords();
        Protected.applyAtlasTextureInformation();
        _myShapeStretcher.updateGeometry(Public.size, false, Public.origin);
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    _addEdgeProperty('edgeTop',    'top');
    _addEdgeProperty('edgeBottom', 'bottom');
    _addEdgeProperty('edgeLeft',   'left');
    _addEdgeProperty('edgeRight',  'right');

    _addCropProperty('cropTop',    'top');
    _addCropProperty('cropBottom', 'bottom');
    _addCropProperty('cropLeft',   'left');
    _addCropProperty('cropRight',  'right');

    Public.__defineGetter__("edges", function () {
        return [_myShapeStretcher.edges.left,
                _myShapeStretcher.edges.bottom,
                _myShapeStretcher.edges.right,
                _myShapeStretcher.edges.top];
    });
    
    Public.__defineSetter__("edges", function (theEdges) {
        _myShapeStretcher.edges.left   = theEdges[0];
        _myShapeStretcher.edges.bottom = theEdges[1];
        _myShapeStretcher.edges.right  = theEdges[2];
        _myShapeStretcher.edges.top    = theEdges[3];
        _myShapeStretcher.updateGeometry(Public.size, false, Public.origin);
    });
    
    Public.__defineGetter__("crop", function () {
        return [_myShapeStretcher.crop.left,
                _myShapeStretcher.crop.bottom,
                _myShapeStretcher.crop.right,
                _myShapeStretcher.crop.top];
    });
    
    Public.__defineSetter__("crop", function (theEdges) {
        _myShapeStretcher.crop.left   = theEdges[0];
        _myShapeStretcher.crop.bottom = theEdges[1];
        _myShapeStretcher.crop.right  = theEdges[2];
        _myShapeStretcher.crop.top    = theEdges[3];
        _reset();
    });
    
    Protected.__defineGetter__("shapeStretcher", function () {
            return _myShapeStretcher;
    });

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        var myShapeStretcherCtor = spark.ShapeStretcher.Factory.getShapeStretcherFromAttribute(
                Protected.getString("shapeStretcher", "default"));
        _myShapeStretcher = new myShapeStretcherCtor(Protected.shape);
        
        Base.imageSetter = Public.__lookupSetter__("image");
        Public.__defineSetter__("image", function (theImage) {
            Base.imageSetter(theImage);
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

        _myShapeStretcher.initialize(Public.node);
        _reset();
    };
    
    Base.setTexture = Public.setTexture;
    Public.setTexture = function (theTextureName, theAtlasPath) {
        Base.setTexture(theTextureName, theAtlasPath);
        /*_myShapeStretcher.setupGeometry(Protected.originalImageSize, Public.origin);
        Protected.storeOriginalUVCoords();
        Protected.applyAtlasTextureInformation();*/
        _reset();
    };
};
