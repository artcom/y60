/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

/*jslint white:false, nomen: false, plusplus:false*/
/*globals arrayToString, Node, Logger*/

// XXX Are these used anywhere?
/*const ELEMENT_TYPE_POINTS         = "points";
const ELEMENT_TYPE_LINES          = "lines";
const ELEMENT_TYPE_LINE_STRIP     = "linestrip";
const ELEMENT_TYPE_LINE_LOOP      = "lineloop";
const ELEMENT_TYPE_TRIANGLES      = "triangles";
const ELEMENT_TYPE_TRIANGLE_STRIP = "trianglestrip";
const ELEMENT_TYPE_TRIANGLE_FAN   = "trianglefan";
const ELEMENT_TYPE_QUADS          = "quads";
const ELEMENT_TYPE_QUAD_STRIP     = "quadstrip";
const ELEMENT_TYPE_POLYGON        = "polygon";
*/
function Element(theType, theMaterialId) {
    this.Constructor(this, theType, theMaterialId);
}

Element.prototype.Constructor = function(self, theType, theMaterialId) {
    self.type = theType;
    self.material = theMaterialId;

    // position,normal,etc. indices
    self.positions = [];
    self.normals   = [];
    self.colors    = [];
    self.texcoords = [[]];
};

function ShapeBuilder() {
    this.Constructor(this);
}

ShapeBuilder.prototype.Constructor = function(obj) {

    /////////////////////
    // Private Members //
    /////////////////////

    var _myElements  = [];
    var _myPositions = [];
    var _myNormals   = [];
    var _myColors    = [];
    var _myUVCoords  = [[]];

    /////////////////////
    // Private Methods //
    /////////////////////

    // this is a utility function for appendQuadWithCustomTexCoordsAndPositions && appendQuadWithCustomTexCoords
    function appendRemainingVertexData(theElement, theUVOrigin, theUVSize, theGenSingleVertexColorFlag) {
        // a single normal pointing z for all vertices
        var myNormalIndex = _myNormals.length;
        _myNormals.push([0, 0, 1]);
        theElement.normals.push(myNormalIndex,myNormalIndex,myNormalIndex,myNormalIndex);

        // uv coordintaes for the 4 vertices
        var myTexIndex = _myUVCoords[0].length;

        _myUVCoords[0].push([theUVOrigin[0], theUVOrigin[1] + theUVSize[1]]);
        _myUVCoords[0].push([theUVOrigin[0] + theUVSize[0], theUVOrigin[1] + theUVSize[1]]);
        _myUVCoords[0].push([theUVOrigin[0] + theUVSize[0], theUVOrigin[1]]);
        _myUVCoords[0].push([theUVOrigin[0], theUVOrigin[1]]);

        theElement.texcoords[0].push(myTexIndex,myTexIndex+1,myTexIndex+2,myTexIndex+3);

        var myColorIndex = _myColors.length;
        if (theGenSingleVertexColorFlag || theGenSingleVertexColorFlag === undefined) {
            _myColors.push([1,1,1,1]);
            theElement.colors.push(myColorIndex, myColorIndex, myColorIndex, myColorIndex);
        } else {
            _myColors.push([1,1,1,1]);
            _myColors.push([1,1,1,1]);
            _myColors.push([1,1,1,1]);
            _myColors.push([1,1,1,1]);
            theElement.colors.push(myColorIndex, myColorIndex+1, myColorIndex+2, myColorIndex+3);
        }
    }

    ////////////////////
    // Public Methods //
    ////////////////////

    /// Create <shape> node from the data previously passed in.
    obj.buildNode = function(theName) {
        theName = theName || "ShapeBuilder";

        // vertex data
        var myShapeString =
            '<shape name="' + theName + '" renderstyle="[frontfacing]">\n' +
            ' <vertexdata>\n' +
            '  <vectorofvector3f name="position">' + arrayToString(_myPositions) + '</vectorofvector3f>\n';
        if (_myNormals.length > 0) {
            myShapeString += '  <vectorofvector3f name="normal">' +  arrayToString(_myNormals) + '</vectorofvector3f>\n';
        }
        if (_myColors.length > 0) {
            myShapeString += '  <vectorofvector4f name="color">' + arrayToString(_myColors) + '</vectorofvector4f>\n';
        }
        for (var myUVCoordsIndex = 0; myUVCoordsIndex < _myUVCoords.length; myUVCoordsIndex++) {
            if (_myUVCoords[myUVCoordsIndex].length >0) {
                myShapeString += '  <vectorofvector2f name="map' + (myUVCoordsIndex+1) + '">' + arrayToString(_myUVCoords[myUVCoordsIndex]) + '</vectorofvector2f>\n';
            }
        }
        myShapeString += ' </vertexdata>\n<primitives>\n';

        // elements and indices
        for (var i = 0; i < _myElements.length; ++i) {
            var myElement = _myElements[i];
            myShapeString +=
                '  <elements type="' + myElement.type + '" material="' + myElement.material + '">\n' +
                '   <indices vertexdata="position" role="position">' + arrayToString(myElement.positions) + '</indices>\n';
            if (myElement.normals.length > 0) {
                myShapeString += '   <indices vertexdata="normal" role="normal">' + arrayToString(myElement.normals) + '</indices>\n';
            }
            if (myElement.colors.length > 0) {
                myShapeString +='   <indices vertexdata="color" role="color">' + arrayToString(myElement.colors) + '</indices>\n';
            }
            for (var myUVCoordIndexListIndex = 0; myUVCoordIndexListIndex < myElement.texcoords.length; myUVCoordIndexListIndex++) {
                if (myElement.texcoords[myUVCoordIndexListIndex].length > 0) {
                    myShapeString +='   <indices vertexdata="map' + (myUVCoordIndexListIndex + 1) +
                                     '"role="texcoord' + myUVCoordIndexListIndex + '">' + arrayToString(myElement.texcoords[myUVCoordIndexListIndex]) + '</indices>\n';
                }
            }
            myShapeString +='  </elements>\n';
        }
        myShapeString += '</primitives>\n</shape>\n';

        var myShapeDoc = new Node(myShapeString);
        return myShapeDoc.firstChild;
    };

    /// Append a new shape element of the type using the material.
    obj.appendElement = function(theType, theMaterialId) {
        var e = new Element(theType, theMaterialId);
        _myElements.push(e);
        return e;
    };

    obj.appendVertex = function(theElement, theVertex) {
        var myIndex = _myPositions.length;
        _myPositions.push(theVertex);
        theElement.positions.push(myIndex);
        return myIndex;
    };

    obj.appendNormal = function(theElement, theNormal) {
        var myIndex = _myNormals.length;
        _myNormals.push(theNormal);
        theElement.normals.push(myIndex);
        return myIndex;
    };

    obj.appendColor = function(theElement, theColor) {
        var myIndex = _myColors.length;
        _myColors.push(theColor);
        theElement.colors.push(myIndex);
        return myIndex;
    };

    obj.appendTexCoord = function(theElement, theTexCoord, theTextureSlot) {
        if (theTextureSlot === undefined) {
            theTextureSlot = 0;
        } else {
            if (_myUVCoords.length < theTextureSlot+1) {
                _myUVCoords.push([]);
            }
            if (theElement.texcoords.length < theTextureSlot+1) {
                theElement.texcoords.push([]);
            }
        }

        var myIndex = _myUVCoords[theTextureSlot].length;
        _myUVCoords[theTextureSlot].push(theTexCoord);
        theElement.texcoords[theTextureSlot].push(myIndex);
        return myIndex;
    };

    /// Append a line to the given element.
    obj.appendLineElement = function (theElement, theLineBegin, theLineEnd){
        _myPositions.push(theLineBegin);
        theElement.positions.push(_myPositions.length-1);
        _myPositions.push(theLineEnd);
        theElement.positions.push(_myPositions.length-1);
        theElement.normals.push(0);
        theElement.normals.push(0);
        theElement.colors.push(_myColors.length-1);
        theElement.colors.push(_myColors.length-1);
    };

    /// Append a point to a linestrip element.
    obj.appendLineStripElement = function (theElement, thePosition) {
        Logger.warning("Consider using separate appendVertex");
        theElement.positions.push(obj.appendVertex(thePosition));
        theElement.normals.push(0);
        theElement.colors.push(0);
    };

    obj.appendQuad = function (theElement, thePosition, theSize, theDepth) {
        theDepth = theDepth || 0;

        // add positions to shape's vertex data
        var myHalfWidth  = theSize[0] / 2;
        var myHalfHeight = theSize[1] / 2;
        var myPosIndex = _myPositions.length;
        _myPositions.push([thePosition[0] - myHalfWidth, thePosition[1] - myHalfHeight, theDepth]);
        _myPositions.push([thePosition[0] + myHalfWidth, thePosition[1] - myHalfHeight, theDepth]);
        _myPositions.push([thePosition[0] + myHalfWidth, thePosition[1] + myHalfHeight, theDepth]);
        _myPositions.push([thePosition[0] - myHalfWidth, thePosition[1] + myHalfHeight, theDepth]);
        // set element's position indices
        theElement.positions.push(myPosIndex,myPosIndex+1,myPosIndex+2,myPosIndex+3);

        // a single normal pointing z for all vertices
        var myNormalIndex = _myNormals.length;
        _myNormals.push([0, 0, 1]);
        theElement.normals.push(myNormalIndex,myNormalIndex,myNormalIndex,myNormalIndex);

        // uv coordintaes for the 4 vertices
        var myTexIndex = _myUVCoords[0].length;
        _myUVCoords[0].push([0,1]);
        _myUVCoords[0].push([1,1]);
        _myUVCoords[0].push([1,0]);
        _myUVCoords[0].push([0,0]);
        theElement.texcoords[0].push(myTexIndex,myTexIndex+1,myTexIndex+2,myTexIndex+3);
    };

    obj.appendQuad2 = function (theElement, p1, p2, p3, p4, theUVcoords) {
        // add positions to shape's vertex data
        var myPosIndex = _myPositions.length;

        _myPositions.push(p1);
        _myPositions.push(p2);
        _myPositions.push(p3);
        _myPositions.push(p4);

        // set element's position indices
        theElement.positions.push(myPosIndex,myPosIndex+1,myPosIndex+2,myPosIndex+3);

        // a single normal pointing z for all vertices
        var myNormalIndex = _myNormals.length;
        _myNormals.push([0, 0, 1]);
        theElement.normals.push(myNormalIndex,myNormalIndex,myNormalIndex,myNormalIndex);

        // uv coordintaes for the 4 vertices
        var myTexIndex = _myUVCoords[0].length;
        if (theUVcoords === undefined) {
            _myUVCoords[0].push([0,1]);
            _myUVCoords[0].push([1,1]);
            _myUVCoords[0].push([1,0]);
            _myUVCoords[0].push([0,0]);
        } else {
            _myUVCoords[0].push(theUVcoords[0]);
            _myUVCoords[0].push(theUVcoords[1]);
            _myUVCoords[0].push(theUVcoords[2]);
            _myUVCoords[0].push(theUVcoords[3]);
        }
        theElement.texcoords[0].push(myTexIndex,myTexIndex+1,myTexIndex+2,myTexIndex+3);
    };

    obj.appendQuadWithCustomTexCoords = function (theElement, thePosition, theSize, theUVOrigin, theUVSize, theGenSingleVertexColorFlag) {
        // add positions to shape's vertex data
        var myHalfWidth  = theSize[0] / 2;
        var myHalfHeight = theSize[1] / 2;
        var myPosIndex = _myPositions.length;
        _myPositions.push([thePosition[0] - myHalfWidth, thePosition[1] - myHalfHeight, 0]);
        _myPositions.push([thePosition[0] + myHalfWidth, thePosition[1] - myHalfHeight, 0]);
        _myPositions.push([thePosition[0] + myHalfWidth, thePosition[1] + myHalfHeight, 0]);
        _myPositions.push([thePosition[0] - myHalfWidth, thePosition[1] + myHalfHeight, 0]);
        // set element's position indices
        theElement.positions.push(myPosIndex,myPosIndex+1,myPosIndex+2,myPosIndex+3);
        // append remaining stuff
        appendRemainingVertexData(theElement, theUVOrigin, theUVSize, theGenSingleVertexColorFlag);
    };
    
    obj.appendQuadWithCustomTexCoordsAndPositions = function (theElement, thePositions, theUVOrigin, theUVSize, theGenSingleVertexColorFlag) {
        // add positions to shape's vertex data
        var myPosIndex = _myPositions.length;
        _myPositions.push(thePositions[0]);
        _myPositions.push(thePositions[1]);
        _myPositions.push(thePositions[2]);
        _myPositions.push(thePositions[3]);
        // set element's position indices
        theElement.positions.push(myPosIndex,myPosIndex+1,myPosIndex+2,myPosIndex+3);
        // append remaining stuff
        appendRemainingVertexData(theElement, theUVOrigin, theUVSize, theGenSingleVertexColorFlag);
    };
};