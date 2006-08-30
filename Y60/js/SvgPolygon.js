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


function SvgPolygon(theSvgNode,  theMaterial, theShapeMode, theOffset) {
    this.Constructor(this, theSvgNode, theMaterial, theShapeMode, theOffset);
}

SvgPolygon.prototype.Constructor = function(self, theSvgNode, theMaterial, theShapeMode, theOffset) {

    self.getMaterial = function() {
        return _myMaterial;
    }

    self.getShape = function() {
        return _myShape;
    }
    self.getBody = function() {
        return _myBody;
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

    function createLinestripShape() {
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
                var myVertex = new Vector3f(myCoordinates[0], myCoordinates[1], 0);
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

    function createPolygonShape(theOffset) {
        if (theOffset == undefined) {
            theOffset = new Vector2f(0,0);
        }
        var myPointString = theSvgNode.points;
        var myPoints = myPointString.split(" ");
        
        var myVector2fList = [];
        var myMin = new Vector2f(1E+20, 1E+20);
        var myMax = new Vector2f(-1E+20, -1E+20);
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
    
    function setup() {
        if (theMaterial == undefined) {
            _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene);
            _myMaterial.transparent = 1;
            addMaterialRequirement(_myMaterial, "vertexparams", "[10[color]]");
        } else {
            _myMaterial = theMaterial;
            //print(_myMaterial);
        }
        _myColor = new Vector4f(1,1,1,1); // boring ol' white

        
        if( theShapeMode == "polygon") {
            createPolygonShape(theOffset);
        } else if (theShapeMode == "linestrip"){
            createLinestripShape();
        }
        
    }

    var _myMaterial = null;
    var _myShape = null;
    var _myBody = null;
    var _myColor = null;
    setup();


}
