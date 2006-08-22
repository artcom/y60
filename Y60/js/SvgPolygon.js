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

function SvgPolygon(theSceneViewer ,theSvgNode,  theMaterial) {
    this.Constructor(this, theSceneViewer, theSvgNode, theMaterial);
}

SvgPolygon.prototype.Constructor = function(self, theSceneViewer,theSvgNode, theMaterial) {

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

    function createShape() {
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
        if (myFirstVertex) {
            push(myShapeBuilder, myShapeElement, myFirstVertex);        
        }
        // create shape, body
        _myShape = myShapeBuilder.buildNode();
        _myShape.name = "Shape_" + theSvgNode.id;
        window.scene.shapes.appendChild(_myShape);

        _myBody = window.scene.createBody(_myShape);
        _myBody.name = "Body_" + theSvgNode.id;
        theSceneViewer.getScene().update(Scene.SHAPES | Scene.MATERIALS);
        
        //window.scene.save("test.x60", false);
        //exit(1);
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
        
        createShape();
    }

    var _myMaterial = null;
    var _myShape = null;
    var _myBody = null;
    var _myColor = null;
    setup();


}
