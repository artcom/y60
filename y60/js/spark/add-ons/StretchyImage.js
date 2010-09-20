//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/*jslint nomen:false plusplus:false*/
/*globals use, spark, Logger, Vector2f, getImageSize, ShapeStretcher, Vector2i*/

use("spark/add-ons/ShapeStretcher.js");

spark.StretchyImage = spark.ComponentClass("StretchyImage");
spark.StretchyImage.Constructor = function (Protected) {
    var Public = this;
    var Base   = {};
    Public.Inherit(spark.Image);

    /////////////////////
    // Private Members //
    /////////////////////

    var _myImageSize      = null;
    var _myShapeStretcher = null;
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Public.__defineGetter__("edgeTop", function () {
        return _myShapeStretcher.edges.top;
    });

    Public.__defineSetter__("edgeTop", function (theValue) {
        _myShapeStretcher.edges.top = theValue;
        _myShapeStretcher.updateGeometry(new Vector2f(Public.width, Public.height), false, Public.origin);
    });
    
    Public.__defineGetter__("edgeBottom", function () {
        return _myShapeStretcher.edges.bottom;
    });

    Public.__defineSetter__("edgeBottom", function (theValue) {
        _myShapeStretcher.edges.bottom = theValue;
        _myShapeStretcher.updateGeometry(new Vector2f(Public.width, Public.height), false, Public.origin);
    });
    
    Public.__defineGetter__("edgeLeft", function () {
        return _myShapeStretcher.edges.left;
    });

    Public.__defineSetter__("edgeLeft", function (theValue) {
        _myShapeStretcher.edges.left = theValue;
        _myShapeStretcher.updateGeometry(new Vector2f(Public.width, Public.height), false, Public.origin);
    });
    
    Public.__defineGetter__("edgeRight", function () {
        return _myShapeStretcher.edges.right;
    });

    Public.__defineSetter__("edgeRight", function (theValue) {
        _myShapeStretcher.edges.right = theValue;
        _myShapeStretcher.updateGeometry(new Vector2f(Public.width, Public.height), false, Public.origin);
    });

    // TODO add getter setter for crop settings

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
        _myShapeStretcher.updateGeometry(new Vector2f(Public.width, Public.height), false, Public.origin);
    });

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        
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
                                        Protected.getNumber("quadsPerSideY", 3))
        });
        
        _myImageSize  = getImageSize(Public.image);
        Base.imageSetter = Public.__lookupSetter__("image");
        Public.__defineSetter__("image", function (theImage) {
            Base.imageSetter(theImage);
            _myImageSize = getImageSize(theImage);
            _myShapeStretcher.setupGeometry(_myImageSize, Public.origin);
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
        _myShapeStretcher.setupGeometry(_myImageSize, Public.origin);
        _myShapeStretcher.updateGeometry(Public.size, false, Public.origin);
    };
    
    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
    };
};