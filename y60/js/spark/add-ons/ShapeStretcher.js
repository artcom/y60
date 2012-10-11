//=============================================================================
// Copyright (C) 2011, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/*jslint nomen:false plusplus:false*/
/*globals Vector2i, Exception, Vector2f, Logger, clamp*/

spark.ShapeStretcher = spark.AbstractClass("ShapeStretcher");

/////////////
// Statics //
/////////////

spark.ShapeStretcher.Factory = (function () {
        var _myFactory = {};
        var registerShapeStretcher = function (theStretcherName, theCtor) {
            if (theStretcherName in _myFactory) {
                Logger.warning("overwriting previously registered shapestretcher " + theStretcherName);
            }
            Logger.debug("registering shapestretcher with name '" + theStretcherName + "'");
            _myFactory[theStretcherName] = theCtor;
        };

        var getShapeStretcherFromAttribute = function (theStretcherName) {
            Logger.debug("lookup shapestretcher with name '" + theStretcherName + "'");
            if (theStretcherName in _myFactory) {
                return _myFactory[theStretcherName];
            } else {
                Logger.warning("shapestretcher " + theStretcherName + " not found in factory");
                return _myFactory['default'];
            }
        };
        return {"registerShapeStretcher"         : registerShapeStretcher,
                "getShapeStretcherFromAttribute" : getShapeStretcherFromAttribute};
}());

spark.ShapeStretcher.DEFAULT_QUADS_PER_SIDE = new Vector2i(1, 1);
spark.ShapeStretcher.APPLY_EDGE_FILTERING_OFFSET_FLAG = true;

spark.ShapeStretcher.applyEdgeFilteringOffset = function (theEdgeAmount) {
    return (!spark.ShapeStretcher.APPLY_EDGE_FILTERING_OFFSET_FLAG || theEdgeAmount < 0) ? 0 : 1;
};

spark.ShapeStretcher.Constructor = function (Protected, theShape) {
    
    var Public = this;
    var Base   = {};
    Public.Inherit(spark.Component);
    ////////////////
    // Validation //
    ////////////////
    
    if (!theShape) {
        throw new Exception("Shape must be provided", fileline());
    }
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myShape      = theShape;
    var _myVertices   = _myShape.find(".//*[@name='position']").firstChild.nodeValue;
    var _myUVCoords   = null;
    
    var _myCrop  = null;
    var _myEdges = null;
    var _myQuadsPerSide = null;
    var _myVerticesPerSide = null;
    var _myNumVertices     = null;
    var _myNumQuads        = null;
    
    ///////////////////////
    // Protected Methods //
    ///////////////////////
    
    Protected.getVertexData = function (theShape, theVertexAttribute) {
        var myXPath = ".//*[@name='" + theVertexAttribute + "']";
        return theShape.find(myXPath).firstChild.nodeValue;
    };

    Protected.getIndexData = function (theShape, theVertexAttribute) {
        var myXPath = ".//*[@vertexdata='" + theVertexAttribute + "']";
        return theShape.find(myXPath).firstChild.nodeValue;
    };
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Public.updateGeometry = function (theSize, theUVCoordFlag, theOrigin) {
        var myWidth  = theSize.x;
        var myHeight = theSize.y;
        var myOrigin = theOrigin;
        _myVertices[0] = [-myOrigin.x, -myOrigin.y, -myOrigin.z];
        _myVertices[1] = [myWidth - myOrigin.x, -myOrigin.y, -myOrigin.z];
        _myVertices[2] = [-myOrigin.x, myHeight - myOrigin.y, -myOrigin.z];
        _myVertices[3] = [myWidth - myOrigin.x, myHeight - myOrigin.y, -myOrigin.z];
    };
    
    Public.setupGeometry = function (theSize, theOrigin) {
        _myUVCoords.resize(_myNumVertices);
        _myVertices.resize(_myNumVertices);
        Public.updateGeometry(theSize, true, theOrigin);
        
        var myPIdx = Protected.getIndexData(_myShape, 'position');
        myPIdx.resize(_myNumQuads * 4);
        var myUVIdx = Protected.getIndexData(_myShape, 'uvset');
        myUVIdx.resize(_myNumQuads * 4);
        var v = 0;
        for (var currentY = 0; currentY < _myQuadsPerSide.y; ++currentY) {
            for (var currentX = 0; currentX < _myQuadsPerSide.x; ++currentX) {
                v = currentY * _myVerticesPerSide.x + currentX;
                var q = 4 * (currentY * _myQuadsPerSide.x + currentX);
                myUVIdx[q]     = myPIdx[q]     = v;
                myUVIdx[q + 1] = myPIdx[q + 1] = v + 1;
                myUVIdx[q + 2] = myPIdx[q + 2] = v + 1 + _myVerticesPerSide.x;
                myUVIdx[q + 3] = myPIdx[q + 3] = v + _myVerticesPerSide.x;
            }
        }
    };
    
    Base.initialize = Public.initialize;
    Public.initialize = function (theNode) {
        Base.initialize(theNode);
        var myEdges = Protected.getArray("edges", [0,0,0,0]); 
        var myCrops = Protected.getArray("crops", [0,0,0,0]); 
        _myEdges    = {'left'   : Protected.getNumber("edgeLeft",   parseInt(myEdges[0], 10)),
                       'bottom' : Protected.getNumber("edgeBottom", parseInt(myEdges[1], 10)),
                       'right'  : Protected.getNumber("edgeRight",  parseInt(myEdges[2], 10)),
                       'top'    : Protected.getNumber("edgeTop",    parseInt(myEdges[3], 10))};

        _myCrop     = {'left'   : Protected.getNumber("cropLeft",   parseInt(myCrops[0], 10)),
                       'bottom' : Protected.getNumber("cropBottom", parseInt(myCrops[1], 10)),
                       'right'  : Protected.getNumber("cropRight",  parseInt(myCrops[2], 10)),
                       'top'    : Protected.getNumber("cropTop",    parseInt(myCrops[3], 10))};

        // TODO check that quadsPerSide are odd. Even quadsPerSide is possible but
        // it is not entirely clear from which "direction" the stretching is applied
        _myQuadsPerSide    = new Vector2i(Protected.getNumber("quadsPerSideX", spark.ShapeStretcher.DEFAULT_QUADS_PER_SIDE.x),
                                          Protected.getNumber("quadsPerSideY", spark.ShapeStretcher.DEFAULT_QUADS_PER_SIDE.y));
        
        _myVerticesPerSide = new Vector2i(_myQuadsPerSide.x + 1,
                                          _myQuadsPerSide.y + 1);
        
        _myNumVertices     = _myVerticesPerSide.x * _myVerticesPerSide.y;
        _myNumQuads        = _myQuadsPerSide.x * _myQuadsPerSide.y;
        _myUVCoords        = Protected.getVertexData(_myShape, 'uvset');
        Logger.debug("ShapeStretcher::  quadsPerSide " + _myQuadsPerSide +
                     ", verticesPerSide " + _myVerticesPerSide +
                     ", numVertices " + _myNumVertices +
                     ", numQuads " + _myNumQuads);
    };
    
    // TODO potentially make edges simply public
    Public.__defineGetter__("edges", function () {
        return _myEdges;
    });
    
    Public.__defineGetter__("crop", function () {
        return _myCrop;
    });
    Protected.__defineGetter__("vertices", function () {
        return _myVertices;
    });
    
    Protected.__defineGetter__("verticesPerSide", function () {
        return _myVerticesPerSide;
    });
    Protected.__defineGetter__("uvcoords", function () {
        return _myUVCoords;
    });
};

spark.DefaultShapeStretcher = spark.Class("DefaultShapeStretcher");
spark.ShapeStretcher.Factory.registerShapeStretcher('default', spark.DefaultShapeStretcher);

spark.DefaultShapeStretcher.Constructor = function (Protected, theShape) {
    spark.ShapeStretcher.DEFAULT_QUADS_PER_SIDE = new Vector2i(3, 3);
    var Public = this;
    var Base = {};

    Public.Inherit(spark.ShapeStretcher, theShape);
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Public.updateGeometry = function (theSize, theUVCoordFlag, theOrigin) {
        var myWidth  = theSize.x;
        var myHeight = theSize.y;
        var myOrigin = theOrigin;
        var v = 0;
        
        for (var i = 0, l = Protected.verticesPerSide.y; i < l; ++i) {
            for (var j = 0, ll = Protected.verticesPerSide.x; j < ll; ++j) {
                v = i * Protected.verticesPerSide.x + j;
                var myX = -myOrigin.x;
                var myY = -myOrigin.y;
                var myCropX = 0;
                var myCropY = 0;
                if (j === 0) {
                    myCropX = Public.crop.left;
                } else if (j === Protected.verticesPerSide.x - 3) {
                    myX += clamp(Public.edges.left +
                          spark.ShapeStretcher.applyEdgeFilteringOffset(Public.edges.left),0,myWidth);
                    myCropX = Public.crop.left;
                } else if (j === Protected.verticesPerSide.x - 2) {
                    myX += clamp(myWidth - Public.edges.right -
                          spark.ShapeStretcher.applyEdgeFilteringOffset(Public.edges.right),Math.min(Public.edges.left,myWidth),myWidth);
                    myCropX = -Public.crop.right;
                } else {
                    myX += clamp(myWidth,Math.min(Public.edges.left,myWidth),myWidth);
                    myCropX = -Public.crop.right;
                }
                if (i === 0) {
                    myCropY = Public.crop.bottom;
                } else if (i === Protected.verticesPerSide.y - 3) {
                    myY += clamp(Public.edges.bottom +
                          spark.ShapeStretcher.applyEdgeFilteringOffset(Public.edges.bottom),0,myHeight);
                    myCropY = Public.crop.bottom;
                } else if (i === Protected.verticesPerSide.y - 2) {
                    myY += clamp(myHeight - Public.edges.top -
                          spark.ShapeStretcher.applyEdgeFilteringOffset(Public.edges.top),Math.min(Public.edges.bottom,myHeight),myHeight);
                    myCropY = -Public.crop.top;
                } else {
                    myY += clamp(myHeight,Math.min(Public.edges.bottom,myHeight),myHeight);
                    myCropY = -Public.crop.top;
                }
                Protected.vertices[v] = [myX, myY, 0];
                if (theUVCoordFlag) {
                    Protected.uvcoords[v] = [(myWidth > 0 ? (myX + myCropX + myOrigin.x) / myWidth : 0),
                                      (myHeight > 0 ? 1 - (myY + myCropY + myOrigin.y) / myHeight : 0)];
                }
            }
        }
    };
};

