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

use("PathAlign.js"); // for ALIGNMENT consts
use("BuildUtils.js");

/**
 */
function PathStrip(theSceneViewer, theMaterial) {
    this.Constructor(this, theSceneViewer, theMaterial);
}

PathStrip.prototype.Constructor = function(self, theSceneViewer, theMaterial) {

    self.getMaterial = function() {
        return _myMaterial;
    }

    self.getShape = function() {
        return _myShape;
    }

    self.getBody = function() {
        return _myBody;
    }

    self.getPositions = function() {
        var myPositions = _myShape.find(".//*[@name = 'position']");
        myPositions = myPositions.childNode('#text')[1];
        return myPositions;
    }

    self.getColors = function() {
        var myColors = _myShape.find(".//*[@name = 'color']");
        myColors = myColors.childNode('#text')[1];
        return myColors;
    }

    self.getTexCoords = function() {
        var myTexCoords = _myShape.find(".//*[@name = 'uvmap']");
        myTexCoords = myTexCoords.childNode('#text')[1];
        return myTexCoords;
    }

    self.setAlignment = function(theAlignment) {
        _myAlignment = theAlignment;
    }

    self.align = function(thePath, theLength, thePos, theWidth) {

        if (theLength == null) {
            theLength = thePath.length;
        }
        if (theWidth == null) {
            theWidth = 1.0;
        }

        // shape building
        var myShapeBuilder = new ShapeBuilder();
        var myShapeElement = null;

        // current position
        var myLastPos = null;
        var mySegment = 0;
        var myFirstSegmentOffset = 0;
        if (thePos != null) {
            var myNearest = thePath.nearest(thePos);
            mySegment = myNearest.element;
            myLastPos = myNearest.point;
        } else {
            mySegment = 0;
            myLastPos = thePath.getElement(mySegment).origin;
        }
        var myLastElement = null;

        // last
        var myLastLeftVector = null, myLastRightVector = null;
        var myLastLeftLine = null, myLastRightLine = null;

        var myLength = theLength;
        var myU = 0.0;
        var myLengthSum = 0;
        while (myLength > 0.01) {
            // next element
            var myElement = thePath.getElement(mySegment);
            //print("segment=" + mySegment, "element=" + myElement + " / " + thePath.getNumElements());
            mySegment = (mySegment + 1) % thePath.getNumElements();

            var myForward = difference(myElement.end, myElement.origin);
            var myRealForward = distance(myElement.end, myLastPos);
            // last piece
            if (myLength < myRealForward) {
                myRealForward = myLength;
            }
            myLengthSum += myRealForward;
            if (magnitude(myForward) < 0.005) {
                // skip zero-length segment
                //print("skip zero-length");
                continue;
            }

            // path tex coord
            var myUDelta = myRealForward / theLength;
            var myLeftUV = new Vector2f(myU, 0.0);
            var myRightUV = new Vector2f(myU, 1.0);
            myForward = normalized(myForward);
            //print("fwd=" + myForward);
            // left-vector
            var myNormalVector = normalized(cross(UP_VECTOR, myForward));
            var myLeftVector = null;
            var myRightVector = null;
            switch (_myAlignment) {
                case TOP_ALIGNMENT:
                    myLeftVector = new Vector3f(0,0,0);
                    myRightVector = product(myNormalVector, theWidth * -1.0);
                    break;
                case BOTTOM_ALIGNMENT:
                    myLeftVector = product(myNormalVector, theWidth);
                    myRightVector = new Vector3f(0,0,0);
                    break;
                case CENTER_ALIGNMENT:
                case BASELINE_ALIGNMENT:
                    myLeftVector = product(myNormalVector, theWidth * 0.5);
                    myRightVector = product(myNormalVector, theWidth * -0.5);
                    break;
            }

            // begin new shape element when start/end positions don't match
            if (myShapeElement == null ||
                (myLastElement && !almostEqual(myLastElement.end, myElement.origin))) {
                if (myShapeElement) {
                    // finish previous strip
                    //print("finish previous");
                    push(myShapeBuilder, myShapeElement, sum(myLastPos, myLastLeftVector), myLeftUV);
                    push(myShapeBuilder, myShapeElement, sum(myLastPos, myLastRightVector), myRightUV);
                }
                if (myLastElement && !almostEqual(myLastElement.end, myElement.origin)) {
                    myLastPos = myElement.origin;
                }

                // begin new strip
                //print("begin");
                myShapeElement = myShapeBuilder.appendElement("quadstrip", _myMaterial.id);
                push(myShapeBuilder, myShapeElement, sum(myLastPos, myLeftVector), myLeftUV);
                push(myShapeBuilder, myShapeElement, sum(myLastPos, myRightVector), myRightUV);
                myLastLeftLine = null;
                myLastRightLine = null;
            }

            // lines
            var myL0 = sum(myElement.origin, myLeftVector);
            var myLeftLine = new Line(myL0, myForward);

            var myR0 = sum(myElement.origin, myRightVector);
            var myRightLine = new Line(myR0, myForward);

            if (myLastLeftLine && myLastRightLine) {
                // intersect lines
                var myLeftIsect = intersection(myLastLeftLine, myLeftLine);
                var myRightIsect = intersection(myLastRightLine, myRightLine);
                //print("isect", myLastRightLine, myRightLine);

                if (!myLeftIsect) {
                    myLeftIsect = sum(myElement.end, myLeftVector);
                }
                push(myShapeBuilder, myShapeElement, myLeftIsect, myLeftUV);

                if (!myRightIsect) {
                    myRightIsect = sum(myElement.end, myRightVector);
                }
                push(myShapeBuilder, myShapeElement, myRightIsect, myRightUV);
            }

            // remaining vector of this segment
            var myRemain = difference(myElement.end, myLastPos);
            var myRemainLength = magnitude(myRemain);

            // length fits this segment
            if (myLength < myRemainLength) {
                //print("fits");
                myLastPos = sum(myLastPos, product(myForward, myLength));
                myLastLeftVector = myLeftVector;
                myLastRightVector = myRightVector;
                break;
            }

            // next segment
            myLength -= myRemainLength;
            myLastElement = myElement;
            myLastPos = myElement.end;
            myLastLeftVector = myLeftVector;
            myLastRightVector = myRightVector;
            myLastLeftLine = myLeftLine;
            myLastRightLine = myRightLine;
            myU += myUDelta;
        }

        // finish
        if (myLastLeftVector) {
            //print("last");
            myU = 1.0;
            push(myShapeBuilder, myShapeElement, sum(myLastPos, myLastLeftVector), new Vector2f(myU,0));
            push(myShapeBuilder, myShapeElement, sum(myLastPos, myLastRightVector), new Vector2f(myU,1));
        }

        // delete body, shape
        if (_myBody) {
            theSceneViewer.getScene().world.removeChild(_myBody);
        }
        if (_myShape) {
            theSceneViewer.getScene().shapes.removeChild(_myShape);
        }

        // create shape, body
        _myShape = myShapeBuilder.buildNode();
        _myShape.name = "PathStrip_"+_myMaterial.id;
        window.scene.shapes.appendChild(_myShape);
        _myBody = Modelling.createBody(theSceneViewer.getScene(), _myShape.id);
        theSceneViewer.getScene().update(Scene.SHAPES | Scene.MATERIALS);

    }

    /**********************************************************************
     *
     * Private
     *
     **********************************************************************/

    var _myMaterial = null;
    var _myColor = null;

    var _myShape = null;
    var _myBody = null;
    var _myAlignment = CENTER_ALIGNMENT;

    function push(theShapeBuilder, theElement, theVertex, theTexCoord) {
        //print("push", theVertex);
        theShapeBuilder.appendVertex(theElement, theVertex);
        theShapeBuilder.appendColor(theElement, _myColor);
        if (theTexCoord) {
            //print("texcoord=" + theTexCoord);
            theShapeBuilder.appendTexCoord(theElement, theTexCoord);
        }
    }

    function setup() {
        if (theMaterial == undefined) {
            // material w. vertex color
            _myMaterial = Modelling.createUnlitTexturedMaterial(theSceneViewer.getScene());
            _myMaterial.transparent = 1;
            addMaterialRequirement(_myMaterial, "vertexparams", "[10[color]]");
        } else {
            _myMaterial = theMaterial;
        }
        _myColor = new Vector4f(1,1,1,1); // boring ol' white
    }

    setup();
}

/*
 * test
 */
function test_PathStrip() {

    use("SceneViewer.js");
    var mySceneViewer = new SceneViewer(null);
    mySceneViewer.setup(800, 600, false, "SvgPath");

    // path
    var myPath;
    var myWidth = 1.0;
    if (0) {
        myPath = new SvgPath();
        myPath.setCurveSegmentLength(1);
        myPath.move(new Vector3f(0,0,0));
        myPath.vline(5, true);
        myPath.vline(5, true);
        myPath.line(new Vector3f(20,10,0), true);
        myPath.line(new Vector3f(-10,10,0), true);
        myPath.move(new Vector3f(-10,0,0));
        myPath.vline(15, true);
        //myPath.cbezier(new Vector3f(20,10,0), new Vector3f(0,-10,0), new Vector3f(0,0,0));
        //myPath.close();

        window.camera.position = new Vector3f(0,0,30);
    } else {
        var mySvgFile = readFileAsString(expandEnvironment("${PRO}/testmodels/curves.svg")); //svg-logo-001.svg");
        var mySvgNode = new Node(mySvgFile);
        var myPaths = mySvgNode.findAll(".//path");

        myPath = new SvgPath(myPaths[6].d, 100);
        myWidth = 20.0;

        window.camera.position = new Vector3f(700, 1800, 1300);
    }

    var myStrip = new PathStrip(mySceneViewer);
    myStrip.setAlignment(CENTER_ALIGNMENT);
    //myStrip.align(myPath, 35.0, new Vector3f(1200,2200,0), myWidth);

    var myValue = 30.0;
    var myRealign = true;

    var myBaseOnKey = mySceneViewer.onKey;
    mySceneViewer.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theCtrlFlag) {
            myBaseOnKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        } else if (theState) {
            if (theKey == "up") {
                myValue += (theShiftFlag ? 1.0 : 0.25);
                myRealign = true;
            } else if (theKey == "down") {
                myValue -= (theShiftFlag ? 1.0 : 0.25);
                myRealign = true;
            }
        }
    }

    mySceneViewer.onFrame = function(theTime) {
        if (myRealign) {
            myStrip.align(myPath, myValue, new Vector3f(1200,2150,0), myWidth);
            myRealign = false;
        }
    }

    mySceneViewer.onPostRender = function() {
        window.getRenderer().draw(myPath);
    }

    mySceneViewer.go();
}
//test_PathStrip();
