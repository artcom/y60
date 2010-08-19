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
/*globals spark, Logger, Vector2f, getImageSize*/

spark.StretchyImage = spark.ComponentClass("StretchyImage");
spark.StretchyImage.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    
    this.Inherit(spark.Image);

    /////////////////////
    // Private Members //
    /////////////////////

    var _myUVCoords   = null;
    var _myImageSize  = null;
    var _myEdgeTop    = 0;
    var _myEdgeBottom = 0;
    var _myEdgeLeft   = 0;
    var _myEdgeRight  = 0;
    
    var _myQuadsPerSide    = new Vector2f(3, 3);
    var _myVerticesPerSide = new Vector2f(_myQuadsPerSide.x + 1, _myQuadsPerSide.y + 1);
    var _myNumVertices     = _myVerticesPerSide.x * _myVerticesPerSide.y;
    var _myNumQuads        = _myQuadsPerSide.x * _myQuadsPerSide.y;
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function getVertexData(theShape, theVertexAttribute) {
        var myXPath = ".//*[@name='" + theVertexAttribute + "']";
        return theShape.find(myXPath).firstChild.nodeValue;
    }
    
    function initMembers() {
        _myUVCoords = getVertexData(Protected.shape, 'uvset');
        _myVerticesPerSide = new Vector2f(_myQuadsPerSide.x + 1, _myQuadsPerSide.y + 1);
        _myNumVertices = _myVerticesPerSide.x * _myVerticesPerSide.y;
        _myNumQuads = _myQuadsPerSide.x * _myQuadsPerSide.y;
        Logger.debug(Public.name + " quadsPerSide " + _myQuadsPerSide + ", verticesPerSide " + _myVerticesPerSide + ", numVertices " + _myNumVertices + ", numQuads " + _myNumQuads);
    }

    function getIndexData(theShape, theVertexAttribute) {
        var myXPath = ".//*[@vertexdata='" + theVertexAttribute + "']";
        return theShape.find(myXPath).firstChild.nodeValue;
    }
    
    function updateGeometry(theSize, theUVCoordFlag) {
        var myWidth = theSize.x;
        var myHeight = theSize.y; 
        var o = Public.origin;
        var v = 0;
        for (var i = 0; i < _myVerticesPerSide.y; ++i) {
            for (var j = 0; j < _myVerticesPerSide.x; ++j) {
                v = i * _myVerticesPerSide.x + j;
                var myX = -o.x;
                var myY = -o.y;
                if (j === 0) {
                    myX = -o.x;
                } else if (j === _myVerticesPerSide.x - 3) {
                    myX = -o.x + _myEdgeLeft;
                } else if (j === _myVerticesPerSide.x - 2) {
                    myX = myWidth - o.x - _myEdgeRight;
                } else if (j === _myVerticesPerSide.x - 1) {
                    myX = myWidth - o.x;
                }
                if (i === 0) {
                    myY = -o.y;
                } else if (i === _myVerticesPerSide.y - 3) {
                    myY = -o.y + _myEdgeBottom;
                } else if (i === _myVerticesPerSide.y - 2) {
                    myY = myHeight - o.y - _myEdgeTop;
                } else if (i === _myVerticesPerSide.y - 1) {
                    myY = myHeight - o.y;
                }
                Protected.vertices[v] = [myX, myY, 0];
                if (theUVCoordFlag) {
                    _myUVCoords[v] = [(myX + o.x) / myWidth, 1 - (myY + o.y) / myHeight];
                }
            }
        }
    }

    function setupGeometry() {
        _myUVCoords.resize(_myNumVertices);
        Protected.vertices.resize(_myNumVertices);
        updateGeometry(_myImageSize, true);

        var myPIdx = getIndexData(Protected.shape, 'position');
        myPIdx.resize(_myNumQuads * 4);
        var myUVIdx = getIndexData(Protected.shape, 'uvset');
        myUVIdx.resize(_myNumQuads * 4);
        var v = 0;
        for (var i = 0; i < _myQuadsPerSide.y; ++i) {
            for (var j = 0; j < _myQuadsPerSide.x; ++j) {
                v = i * _myVerticesPerSide.x + j;
                var q = 4 * (i * _myQuadsPerSide.x + j);
                myUVIdx[q]     = myPIdx[q]     = v;
                myUVIdx[q + 1] = myPIdx[q + 1] = v + 1;
                myUVIdx[q + 2] = myPIdx[q + 2] = v + 1 + _myVerticesPerSide.x;
                myUVIdx[q + 3] = myPIdx[q + 3] = v + _myVerticesPerSide.x;
            }
        }
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Public.__defineGetter__("edgeTop", function () {
        return _myEdgeTop;
    });

    Public.__defineSetter__("edgeTop", function (theValue) {
        _myEdgeTop = theValue;
    });
    
    Public.__defineGetter__("edgeBottom", function () {
        return _myEdgeBottom;
    });

    Public.__defineSetter__("edgeBottom", function (theValue) {
        _myEdgeBottom = theValue;
    });
    
    Public.__defineGetter__("edgeLeft", function () {
        return _myEdgeLeft;
    });

    Public.__defineSetter__("edgeLeft", function (theValue) {
        _myEdgeLeft = theValue;
    });
    
    Public.__defineGetter__("edgeRight", function () {
        return _myEdgeRight;
    });

    Public.__defineSetter__("edgeRight", function (theValue) {
        _myEdgeRight = theValue;
    });

    Public.__defineGetter__("edges", function () {
        return [_myEdgeLeft, _myEdgeBottom, _myEdgeRight, _myEdgeTop];
    });
    
    Public.__defineSetter__("edges", function (theEdges) {
        _myEdgeLeft   = theEdges[0];
        _myEdgeBottom = theEdges[1];
        _myEdgeRight  = theEdges[2];
        _myEdgeTop    = theEdges[3];
    });

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        _myImageSize  = getImageSize(Public.image);
        _myEdgeTop    = Protected.getNumber("edgeTop", 0);
        _myEdgeBottom = Protected.getNumber("edgeBottom", 0);
        _myEdgeLeft   = Protected.getNumber("edgeLeft", 0);
        _myEdgeRight  = Protected.getNumber("edgeRight", 0);
        var myQuadsPerSideX = Protected.getNumber("quadsPerSideX", 3);
        var myQuadsPerSideY = Protected.getNumber("quadsPerSideY", 3);
        if (myQuadsPerSideX > 3 || myQuadsPerSideY > 3) {
            Logger.warning("StretchyImage doesn't support more than 3 quads per side yet");
        }
        _myQuadsPerSide = new Vector2f(myQuadsPerSideX, myQuadsPerSideY);

        //XXX: to avoid texture filtering artefacts 
        _myEdgeTop    = (_myEdgeTop > 0)    ? _myEdgeTop + 1 : 0;
        _myEdgeBottom = (_myEdgeBottom > 0) ? _myEdgeBottom + 1 : 0;
        _myEdgeLeft   = (_myEdgeLeft > 0)   ? _myEdgeLeft + 1 : 0;
        _myEdgeRight  = (_myEdgeRight > 0)  ? _myEdgeRight + 1 : 0;
        //Public.texture.min_filter = "nearest";
        //Public.texture.mag_filter = "nearest";

        Base.imageSetter = Public.__lookupSetter__("image");
        Public.__defineSetter__("image", function (theImage) {
            Base.imageSetter(theImage);
            _myImageSize = getImageSize(theImage);
            setupGeometry();
        });

        Base.widthSetter = Public.__lookupSetter__("width");
        Public.__defineSetter__("width", function (theWidth) {
            Base.widthSetter(theWidth);
            updateGeometry(new Vector2f(theWidth, Public.height));
        });
        
        Base.heightSetter = Public.__lookupSetter__("height");
        Public.__defineSetter__("height", function (theHeight) {
            Base.heightSetter(theHeight);
            updateGeometry(new Vector2f(Public.width, theHeight));
        });

        initMembers();
        setupGeometry();
        updateGeometry(Public.size);
    };
    
    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
    };
};