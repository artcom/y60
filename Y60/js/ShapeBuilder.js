//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: ShapeBuilder.js,v $
//   $Author: ulrich $
//   $Revision: 1.10 $
//   $Date: 2004/11/16 18:35:02 $
//
//=============================================================================

use("BuildUtils.js");

function Element(theType, theMaterial) {
    this.type      = theType;
    this.material  = theMaterial;
    this.positions = [];
    this.normals   = [];
    this.colors    = [];
    this.texcoords = [];
}

function ShapeBuilder(theShapeId) {
    this.Constructor(this, theShapeId);
}

ShapeBuilder.prototype.Constructor = function(obj, theShapeId) {


    var _myShapeId   = theShapeId;
    var _myElements  = [];
    var _myPositions = [];
    var _myNormals   = [];
    var _myUVCoords  = [];
    var _myColors    = [];

    obj.buildNode = function() {
        var myShapeString =
            '<shape id="' + _myShapeId + '">\n' +
            '    <vertexdata>\n' +
            '        <vectorofvector3f name="position">' + arrayToString(_myPositions) + '</vectorofvector3f>\n' +
            '        <vectorofvector3f name="normal">' +  arrayToString(_myNormals) + '</vectorofvector3f>\n';
            if (_myColors.length >0) {
                myShapeString += '        <vectorofvector4f name="color">' + arrayToString(_myColors) + '</vectorofvector4f>\n';
            }
            if (_myUVCoords.length >0) {
                myShapeString += '        <vectorofvector2f name="map1">' + arrayToString(_myUVCoords) + '</vectorofvector2f>\n';
            }
            myShapeString += '    </vertexdata>\n' +
            '    <primitives>\n';
        for (var i = 0; i < _myElements.length; ++i) {
            var myElement = _myElements[i];
            myShapeString +=
                '    <elements type="' + myElement.type + '" material="' + myElement.material + '">\n' +
                '        <indices vertexdata="position" role="position">' + arrayToString(myElement.positions) + '</indices>\n' +
                '        <indices vertexdata="normal" role="normal">' + arrayToString(myElement.normals) + '</indices>\n';
                if (myElement.colors.length >0) {
                   myShapeString +='        <indices vertexdata="color" role="color">' + arrayToString(myElement.colors) + '</indices>\n';
                }
                if (myElement.texcoords.length >0) {
                   myShapeString +='        <indices vertexdata="map1" role="texcoord0">' + arrayToString(myElement.texcoords) + '</indices>\n';
                }
                myShapeString +='    </elements>\n';
        }
        myShapeString += '    </primitives>\n';
        myShapeString += '</shape>\n';

        var myShapeDoc = new Node(myShapeString);
        return myShapeDoc.firstChild;
    }

    obj.appendElement = function(theType, theMaterial) {
        var e = new Element(theType, theMaterial);
        _myElements.push(e);
        return e;
    }

    obj.appendNormal = function (theNormal) {
        var myIndex = _myNormals.length;
        _myNormals.push(theNormal);
        return myIndex;
    }

    obj.appendColor = function (theColor) {
        var myIndex = _myColors.length;
        _myColors.push(theColor);
        return myIndex;
    }

    obj.appendLineElement = function (theElement, theLineBegin, theLineEnd){
        _myPositions.push(theLineBegin);
        theElement.positions.push(_myPositions.length-1);
        _myPositions.push(theLineEnd);
        theElement.positions.push(_myPositions.length-1);
        theElement.normals.push(0);
        theElement.normals.push(0);
        theElement.colors.push(_myColors.length-1);
        theElement.colors.push(_myColors.length-1);
    }

    obj.appendLineStripElement = function (theElement, thePosition) {
        // add positions to shape's vertex data
        _myPositions.push(thePosition);
        theElement.positions.push(_myPositions.length-1);
        theElement.normals.push(0);
        theElement.colors.push(0);
    }

    obj.appendQuad = function (theElement, thePosition, theSize, theDepth, theOffset) {
        var myDepth = 0;
        if (theDepth != undefined) {
            myDepth = theDepth;
        }
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
        var myNormalIndex = _myNormals.length
        _myNormals.push([0, 0, 1]);
        theElement.normals.push(myNormalIndex,myNormalIndex,myNormalIndex,myNormalIndex);

        // uv coordintaes for the 4 vertices
        var myTexIndex = _myUVCoords.length;
        _myUVCoords.push([0,1]);
        _myUVCoords.push([1,1]);
        _myUVCoords.push([1,0]);
        _myUVCoords.push([0,0]);
        theElement.texcoords.push(myTexIndex,myTexIndex+1,myTexIndex+2,myTexIndex+3);
    }

    obj.appendQuad2 = function (theElement, p1, p2, p3, p4) {
        // add positions to shape's vertex data
        var myPosIndex = _myPositions.length

        _myPositions.push(p1);
        _myPositions.push(p2);
        _myPositions.push(p3);
        _myPositions.push(p4);

        // set element's position indices
        theElement.positions.push(myPosIndex,myPosIndex+1,myPosIndex+2,myPosIndex+3);

        // a single normal pointing z for all vertices
        var myNormalIndex = _myNormals.length
        _myNormals.push([0, 0, 1]);
        theElement.normals.push(myNormalIndex,myNormalIndex,myNormalIndex,myNormalIndex);

        // uv coordintaes for the 4 vertices
        var myTexIndex = _myUVCoords.length;
        _myUVCoords.push([0,1]);
        _myUVCoords.push([1,1]);
        _myUVCoords.push([1,0]);
        _myUVCoords.push([0,0]);
        theElement.texcoords.push(myTexIndex,myTexIndex+1,myTexIndex+2,myTexIndex+3);
    }
    
    obj.appendQuadWithCustumTexCoords = function (theElement, thePosition, theSize, theUVOrigin, theUVSize) {
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

        // a single normal pointing z for all vertices
        var myNormalIndex = _myNormals.length
        _myNormals.push([0, 0, 1]);
        theElement.normals.push(myNormalIndex,myNormalIndex,myNormalIndex,myNormalIndex);

        // uv coordintaes for the 4 vertices
        var myTexIndex = _myUVCoords.length;
        
        _myUVCoords.push([theUVOrigin[0], theUVOrigin[1] + theUVSize[1]]);
        _myUVCoords.push([theUVOrigin[0] + theUVSize[0], theUVOrigin[1] + theUVSize[1]]);
        _myUVCoords.push([theUVOrigin[0] + theUVSize[0], theUVOrigin[1]]);
        _myUVCoords.push([theUVOrigin[0], theUVOrigin[1]]);
        
        theElement.texcoords.push(myTexIndex,myTexIndex+1,myTexIndex+2,myTexIndex+3);
    
        var myColorIndex = _myColors.length;
        _myColors.push([1,1,1,1]);
        theElement.colors.push(myColorIndex, myColorIndex, myColorIndex, myColorIndex);
    }
}
