//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

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
        _myBody = window.scene.createBody(_myShape);
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

        _myBody = window.scene.createBody(_myShape);
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
        _myBody = window.scene.createBody(_myShape);
        _myBody.name = "Body_" + theSvgNode.id;
        window.scene.update(Scene.SHAPES | Scene.MATERIALS);
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
        for(var myPointIndex = 0; myPointIndex < myPoints.length; myPointIndex++) {
            var myPoint = trim(myPoints[myPointIndex]);
            if (myPoint.length >0 ) {
                var myCoordinates = myPoint.split(",");
                var myVertex = new Vector3f(Number(myCoordinates[0]) + theOffset.x, Number(myCoordinates[1]) + theOffset.y, 0);
                push(myShapeBuilder, myShapeElement, myVertex);
                if ( myPointIndex == 0) {
                    myFirstVertex = myVertex;
                }
            }
        }
        // create shape, body
        _myShape = myShapeBuilder.buildNode();
        _myShape.name = "Shape_" + theSvgNode.id;
        window.scene.shapes.appendChild(_myShape);

        _myBody = window.scene.createBody(_myShape);
        _myBody.name = "Body_" + theSvgNode.id;
        window.scene.update(Scene.SHAPES | Scene.MATERIALS);
        
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
