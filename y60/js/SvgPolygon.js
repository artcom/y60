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

use("BuildUtils.js");


function SvgPolygon(theMaterial) {
    this.Constructor(this, theMaterial);
}

SvgPolygon.prototype.Constructor = function(self, theMaterial) {

    self.getMaterial = function() {
        return _myMaterial;
    }

    self.getShape = function() {
        return _myShape;
    }
    self.getBody = function() {
        return _myBody;
    }

    self.createPolygonShapeByVectorList = function(theId, theVector2fList) {
        _myShape = Modelling.createSurface2DFromContour(window.scene, theMaterial.id, theVector2fList, "Surface2d");
        //print(theVector2fList);
        _myBody = Modelling.createBody(window.scene, _myShape.id);
        _myBody.name = "Body_" + theId;
        window.scene.update(Scene.SHAPES | Scene.MATERIALS);
    }
   self.createLinestripShapeByVectorList = function(theId, theVector2fList) {
        // shape building
        var myShapeBuilder = new ShapeBuilder();
        var myShapeElement = null;
        var myShapeElement = myShapeBuilder.appendElement("linestrip", _myMaterial.id);
        for(var myPointIndex = 0; myPointIndex < theVector2fList.length; myPointIndex++) {
            var myVertex = new Vector3f(theVector2fList[myPointIndex].x, theVector2fList[myPointIndex].y, 0);
            push(myShapeBuilder, myShapeElement, myVertex);
        }
        // create shape, body
        _myShape = myShapeBuilder.buildNode();
        _myShape.name = "Shape_" + theId;
        window.scene.shapes.appendChild(_myShape);

       _myBody = Modelling..createBody(window.scene, _myShape.id);
        _myBody.name = "Body_" + theId;
        window.scene.update(Scene.SHAPES | Scene.MATERIALS);
    }

    self.createPolygonShapeByNode = function(theSvgNode, theOffset) {
        if (theOffset == undefined) {
            theOffset = new Vector2f(0,0);
        }
        var myPointString = theSvgNode.points;
        var myPoints = myPointString.split(" ");

        var myVector2fList = [];
        for(var myPointIndex = 0; myPointIndex < (myPoints.length); myPointIndex++) {

            var myPoint = trim(myPoints[myPointIndex]);
            if (myPoint.length >0 ) {
                var myCoordinates = myPoint.split(",");
                var myVertex = new Vector2f(Number(myCoordinates[0]) + theOffset.x, Number(myCoordinates[1]) + theOffset.y);
                myVector2fList.push(myVertex);
            }
        }
        _myShape = Modelling.createSurface2DFromContour(window.scene, theMaterial.id, myVector2fList, "Surface2d");
        _myBody = Modelling.createBody(window.scene, _myShape.id);
        _myBody.name = "Body_" + theSvgNode.id;
        window.scene.update(Scene.SHAPES | Scene.MATERIALS);
        return myVector2fList;
    }

   self.createLinestripShapeByNode = function(theSvgNode, theOffset) {
        if (theOffset == undefined) {
            theOffset = new Vector2f(0,0);
        }
        // shape building
        var myShapeBuilder = new ShapeBuilder();
        var myShapeElement = null;
        var myShapeElement = myShapeBuilder.appendElement("linestrip", _myMaterial.id);

        var myPointString = theSvgNode.points;
        var myPoints = myPointString.split(" ");
        var myFirstVertex = null;
        var myVector2fList = [];
        for(var myPointIndex = 0; myPointIndex < myPoints.length; myPointIndex++) {
            var myPoint = trim(myPoints[myPointIndex]);
            if (myPoint.length >0 ) {
                var myCoordinates = myPoint.split(",");
                var myVertex = new Vector3f(Number(myCoordinates[0]) + theOffset.x, Number(myCoordinates[1]) + theOffset.y, 0);
                push(myShapeBuilder, myShapeElement, myVertex);
                if ( myPointIndex == 0) {
                    myFirstVertex = myVertex;
                }
                myVector2fList.push(myVertex);
            }
        }
        // create shape, body
        _myShape = myShapeBuilder.buildNode();
        _myShape.name = "Shape_" + theSvgNode.id;
        window.scene.shapes.appendChild(_myShape);

       _myBody = Modelling.createBody(window.scene, _myShape.id);
        _myBody.name = "Body_" + theSvgNode.id;
        window.scene.update(Scene.SHAPES | Scene.MATERIALS);
        return myVector2fList;
        //window.scene.save("test.x60", false);
        //exit(1);
    }

    /**********************************************************************
     *
     * Private
     *
     **********************************************************************/


    function push(theShapeBuilder, theElement, theVertex, theTexCoord) {
        //print("push", theVertex);
        theShapeBuilder.appendVertex(theElement, theVertex);
        theShapeBuilder.appendColor(theElement, _myColor);
        if (theTexCoord) {
            theShapeBuilder.appendTexCoord(theElement, theTexCoord);
        }
    }

    function setup() {
        if (theMaterial == undefined) {
            _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene);
            _myMaterial.transparent = 1;
            addMaterialRequirement(_myMaterial, "vertexparams", "[10[color]]");
        } else {
            _myMaterial = theMaterial;
        }
        _myColor = new Vector4f(1,1,1,1); // boring ol' white
    }

    var _myMaterial = null;
    var _myShape = null;
    var _myBody = null;
    var _myColor = null;
    setup();


}
