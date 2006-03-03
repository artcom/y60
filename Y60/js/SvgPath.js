//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("BSpline.js");

/// Alignment for Text and PathStrip.
const TOP_ALIGNMENT    = 0;
const BOTTOM_ALIGNMENT = 1;
const CENTER_ALIGNMENT = 2;
const BASELINE_ALIGNMENT = 3;

/// Global up vector.
const UP_VECTOR = new Vector3f(0,0,1);

/**
 * SvgPath parses and manages a path description from a svg '<path>' node.
 * The constructor takes either a string containing '<path d="..."/>' or
 * a suitable XML node.
 */
function SvgPath(theDescription) {
    this.Constructor(this, theDescription);
}

SvgPath.prototype.Constructor = function(self, theDescription, theCurveSegmentLength) {

    /// Move to position.
    self.move = function(thePoint, theRelativeFlag) {
        if (theRelativeFlag == null) {
            theRelativeFlag = false;
        }
        var myEnd = thePoint;
        if (theRelativeFlag) {
            myEnd = sum(_myLastPos, myEnd);
        }
        _myOrigin = myEnd;
        _myLastPos = myEnd;
    }

    /// Line to position.
    self.line = function(thePoint, theRelativeFlag) {
        if (theRelativeFlag == null) {
            theRelativeFlag = false;
        }
        var myEnd = thePoint;
        if (theRelativeFlag) {
            myEnd = sum(_myLastPos, myEnd);
        }
        push(new LineSegment(_myLastPos, myEnd));
        _myLastPos = myEnd;
    }

    /// Horizontal line to position.
    self.hline = function(theX, theRelativeFlag) {
        if (theRelativeFlag == null) {
            theRelativeFlag = false;
        }
        var myEnd = null;
        if (theRelativeFlag) {
            myEnd = new Vector3f(_myLastPos[0] + theX, _myLastPos[1], _myLastPos[2]);
        } else {
            myEnd = new Vector3f(theX, _myLastPos[1], _myLastPos[2]);
        }
        push(new LineSegment(_myLastPos, myEnd));
        _myLastPos = myEnd;
    }

    /// Vertical line to position.
    self.vline = function(theY, theRelativeFlag) {
        if (theRelativeFlag == null) {
            theRelativeFlag = false;
        }
        var myEnd = null;
        if (theRelativeFlag) {
            myEnd = new Vector3f(_myLastPos[0], _myLastPos[1] + theY, _myLastPos[2]);
        } else {
            myEnd = new Vector3f(_myLastPos[0], theY, _myLastPos[2]);
        }
        push(new LineSegment(_myLastPos, myEnd));
        _myLastPos = myEnd;
    }

    /// Cubic bezier spline.
    self.cbezier = function(theStartHandle, theEndHandle, theEnd, theRelativeFlag) {
        if (theRelativeFlag == null) {
            theRelativeFlag = false;
        }
        var myStartHandle = theStartHandle;
        var myEndHandle = theEndHandle;
        var myEnd = theEnd;
        if (theRelativeFlag) {
            myStartHandle = sum(_myLastPos, myStartHandle);
            myEndHandle = sum(_myLastPos, myEndHandle);
            myEnd = sum(_myLastPos, myEnd);
        }
        var mySpline = new BSpline(_myLastPos, myStartHandle, myEnd, myEndHandle);
        //push(mySpline); // XXX what we ultimately want
        segmentBSpline(mySpline, myEnd);
        _myLastPos = myEnd;
    }

    /// Close path to origin.
    self.close = function() {
        var myEnd = _myOrigin;
        push(new LineSegment(_myLastPos, myEnd));
        _myLastPos = myEnd;
    }

    /// Return length of path.
    self.length getter = function() {
        return _myLength;
    }

    /// Get number of elements.
    self.getNumElements = function() {
        return _myElements.length;
    }

    /// Get element.
    self.getElement = function(theIndex) {
        if (theIndex >= 0 && theIndex < _myElements.length) {
            return _myElements[theIndex];
        }
        return null;
    }

    /// Revert the path.
    self.revert = function() {
        _myElements.reverse();
        for (var i = 0; i < _myElements.length; ++i) {
            var myElement    = _myElements[i];
            var myEnd        = myElement.end;
            myElement.end    = myElement.origin;
            myElement.origin = myEnd;
        }            
    }

    /**
     * Return point on path by percentage of path length.
     */
    self.pointOnPath = function(theValue) {

        if (theValue <= 0.0) {
            return _myElements[0].origin;
        } else if (theValue >= 1.0) {
            return _myElements[_myElements.length-1].end;
        }

        var myPathLength = theValue * _myLength;
        var mySegment = 0;
        var myAccumLength = 0.0;

        for (var i = 0; i < _myElements.length; ++i) {

            var myElement = _myElements[i];
            var myElementVector = difference(myElement.end, myElement.origin);
            var myElementLength = magnitude(myElementVector);

            if ((myAccumLength + myElementLength) >= myPathLength) {
                var myRemainLength = myPathLength - myAccumLength;
                var myPoint = sum(myElement.origin, product(normalized(myElementVector), myRemainLength));
                myPoint.segment = i;
                return myPoint;
            }
            myAccumLength += myElementLength;
        }

        return null;
    }

    /**
     * Find closest point on path.
     * Returns {point:Point3f, segment:int} or null.
     */
    self.nearest = function(thePoint) {

        if (_myElements.length == 0) {
            return null;
        }

        var myNearestPoint = null;
        var myNearestElement = -1;
        var myNearestDistance = -1;

        for (var i = 0; i < _myElements.length; ++i) {

            var myElement = _myElements[i];
            var myNearest = nearest(myElement, thePoint);
            var myDistance = magnitude(difference(thePoint, myNearest));
            if (myNearestDistance == -1 || myDistance < myNearestDistance) {
                myNearestPoint = myNearest;
                myNearestElement = i;
                myNearestDistance = myDistance;
            }
        }

        return {point:myNearestPoint, segment:myNearestElement};
    }

    /**
     * Intersect path with given element.
     * Returns {points:[Point3f], segments:[int]} or null.
     */
    self.intersect = function(theElement) {

        if (_myElements.length == 0) {
            return null;
        }

        var myIntersectPoints = [];
        var myIntersectSegments = [];

        for (var i = 0; i < _myElements.length; ++i) {

            var myIntersection = intersection(theElement, _myElements[i]);
            //print("isect type=" + typeof(myIntersection));
            if (myIntersection == null) {
                continue;
            }
            for (var j = 0; j < myIntersection.length; ++j) {
                myIntersectPoints.push(myIntersection[j]);
                myIntersectSegments.push(i);
            }
        }

        if (myIntersectPoints.length == 0) {
            return null;
        }
        return { points:myIntersectPoints, segments:myIntersectSegments };
    }

    /**
     * Return normal vector at the given point.
     * The optional up-vector defines the plane the new path is constructed in.
     * Returns {nearest:Point3f, normal:Vector3f} or null.
     */
    self.normal = function(thePoint, theUpVector) {

        if (theUpVector == null) {
            theUpVector = UP_VECTOR;
        }

        var myNearest = self.nearest(thePoint);
        if (myNearest == null) {
            return null;
        }

        var myElement = self.getElement(myNearest.segment);
        var myForwardVector = difference(myElement.end, myElement.origin);
        var myLeftVector = normalized(cross(theUpVector, myForwardVector));

        return { nearest:myNearest.point, normal:myLeftVector };
    }

    /**
     * Create a perpendicular path of the given length
     * that intersects this path at the given point.
     */
    self.createPerpendicularPath = function(thePoint, theLength, theUpVector) {

        var myNormal = self.normal(thePoint, theUpVector);
        var myLeftVector = product(myNormal.normal, theLength);

        // setup path
        var myPath = new SvgPath();
        myPath.move(myNormal.nearest);
        myPath.line(myLeftVector, true);

        return myPath;
    }

    /**
     * Create a subpath from between the both given positions.
     */
    function createShortestSubPath(theStart, theEnd, theMaxLength) {

        var myLastPos = null;
        var mySegment = theStart.segment;

        var myPath = new SvgPath();
        myPath.move(theStart.point);
        var myLooped = false;

        while (1) {

            var myElement = _myElements[mySegment];
            if (myLastPos && !almostEqual(myElement.origin, myLastPos)) {
                // discontinuous
                myPath.move(myElement.origin);
            }

            if (mySegment == theEnd.segment) {
                if (mySegment == theStart.segment) {
                    if (myLooped == true) {
                        // been there, done that ...
                        break;
                    } else {
                        /*
                         * if start/end are on the same segment, determine if
                         * end point is 'behind' start point.
                         */
                        var myStartLen = magnitude(difference(theStart.point, myElement.origin));
                        var myEndLen = magnitude(difference(theEnd.point, myElement.origin));
                        if (myEndLen >= myStartLen) {
                            break;
                        } else {
                            myLooped = true;
                        }
                    }
                } else {
                    break;
                }
            }
 
            // draw to end of element
            myLastPos = myElement.end;
            myPath.line(myLastPos);
            mySegment = (mySegment + 1) % _myElements.length;

            if (myPath.length > theMaxLength) {
                return null;
            }
        }

        myPath.line(theEnd.point);

        return myPath;
    }

    self.createSubPath = function(theStartPoint, theEndPoint, theShortestPathFlag) {

        if (theShortestPathFlag == null) {
            theShortestPathFlag = false;
        }

        var myStartNearest = self.nearest(theStartPoint);
        var myEndNearest = self.nearest(theEndPoint);
        if (myStartNearest == null || myEndNearest == null) {
            Logger.error("Unable to find start/end point on path");
            return null;
        }

        var myMaxLength = _myLength * 0.5;
        var myPath = createShortestSubPath(myStartNearest, myEndNearest, myMaxLength);
        if (myPath == null) {
            myPath = createShortestSubPath(myEndNearest, myStartNearest, myMaxLength);
            if (myPath == null) {
                Logger.error("createSubPath unable to create shortest path");
            } else {
                print("Reverting path");
                myPath.revert();
            }
        }

        return myPath;
    }

    /// Set segment length for curves.
    self.setCurveSegmentLength = function(theLength) {
        _myCurveSegmentLength = theLength;
    }

    /// Draw path.
    self.draw = function(theColor, theMatrix, theWidth) {

        if (theColor == null) {
            theColor = new Vector4f(1,1,0,1);
        }
        if (theMatrix == null) {
            theMatrix = new Matrix4f();;
        }
        if (theWidth == null) {
            theWidth = 2;
        }

        // draw callback
        var myRenderFunc = function(theElement) {
            window.getRenderer().draw(theElement, theColor, theMatrix, theWidth);
        }
        render(myRenderFunc);
    }

    /// Print path.
    self.print = function() {

        // print callback
        var myPrintFunc = function(theElement) {
            print(theElement.origin + " -> " + theElement.end);
        }
        render(myPrintFunc);
    }

    /**********************************************************************
     * 
     * Private
     *
     **********************************************************************/

    /**
     * Supported path commands.
     */
    const MOVE_TO = 0;
    const LINE_TO = 1;
    const VERTICAL_LINE = 2;
    const HORIZONTAL_LINE = 3;
    const SHORTHAND_QUADRATIC_BEZIER = 4;
    const QUADRATIC_BEZIER = 5;
    const SHORTHAND_CUBIC_BEZIER = 6;
    const CUBIC_BEZIER = 7;
    const ARC = 8;
    const RETURN_TO_ORIGIN = 9;

    /**
     * Path command definitions.
     * Token, lower-case char, number of arguments
     */
    const PATH_COMMANDS = [
        [ MOVE_TO, 'm', 2 ],                    // xe,ye
        [ LINE_TO, 'l', 2 ],                    // xe,ye
        [ VERTICAL_LINE, 'v', 1 ],              // ye
        [ HORIZONTAL_LINE, 'h', 1 ],            // xe
        [ SHORTHAND_QUADRATIC_BEZIER, 't', 2 ], // xe,ye
        [ QUADRATIC_BEZIER, 'q', 4 ],           // x_cp,y_cp, xe,ye
        [ SHORTHAND_CUBIC_BEZIER, 's', 4 ],     // x_cp1,y_cp1, xe,ye
        [ CUBIC_BEZIER, 'c', 6 ],               // x_cp0,y_cp0, x_cp1,y_cp1, xe,ye
        [ ARC, 'a', 7 ],
        [ RETURN_TO_ORIGIN, 'z', 0 ]
    ];

    // state
    var _myOrigin = new Vector3f(0,0,0);
    var _myLastPos = new Vector3f(0,0,0);

    var _myCp0 = null;
    var _myCp1 = null;

    // list of elements. For now BSplines etc. are split into LineSegments
    var _myElements = [];
    var _myLength = 0;

    // parameters for BSpline segmentation
    var _myCurveSegmentLength = 16;
    var _myMinSegments = 1;
    var _myMaxSegments = 32;

    // push element, update path length
    function push(theElement) {
        _myElements.push(theElement);
        _myLength += magnitude(difference(theElement.end, theElement.origin));
    }

    // render elements
    function render(theRenderFunc) {
        for (var j = 0; j < _myElements.length; ++j) {
            theRenderFunc(_myElements[j]);
        }
    }

    // render BSpline into LineSegments
    function segmentBSpline(theSpline, theEnd) {

        // number of line segments
        var myNumSegments = Math.floor(theSpline.getArcLength() / _myCurveSegmentLength);
        if (myNumSegments < _myMinSegments) {
            myNumSegments = _myMinSegments;
        } else if (myNumSegments > _myMaxSegments) {
            myNumSegments = _myMaxSegments;
        }

        var myResult = theSpline.calculate(myNumSegments);
        for (var i = 1; i < myResult.length; ++i) {
            push(new LineSegment(myResult[i-1], myResult[i]));
        }
    }

    // render path part
    function renderPart(thePart) {

        var myRelativeFlag = thePart.relative;
        var myEnd = null;

        switch (thePart.cmd[0]) {
        case MOVE_TO:
            self.move(new Vector3f(thePart.args[0], thePart.args[1], 0), myRelativeFlag);
            break;
        case LINE_TO:
            self.line(new Vector3f(thePart.args[0], thePart.args[1], 0), myRelativeFlag);
            break;
        case VERTICAL_LINE:
            self.vline(thePart.args[0], myRelativeFlag);
            break;
        case HORIZONTAL_LINE:
            self.hline(thePart.args[0], myRelativeFlag);
            break;
        case SHORTHAND_QUADRATIC_BEZIER:
            // XXX this is wrong. Our BSpline is cubic.
            // mirror the last Cp1
            _myCp1 = sum(_myLastPos, product(difference(_myCp1, _myLastPos), -1.0));
            myEnd = new Vector3f(thePart.args[0], thePart.args[1], 0);
            self.cbezier(_myCp1, _myCp1, myEnd, myRelativeFlag);
            break;
        case QUADRATIC_BEZIER:
            // XXX this is wrong. Our BSpline is cubic.
            _myCp1 = new Vector3f(thePart.args[0], thePart.args[1], 0);
            myEnd = new Vector3f(thePart.args[2], thePart.args[3], 0);
            self.cbezier(_myCp1, _myCp1, myEnd, myRelativeFlag);
            break;
        case SHORTHAND_CUBIC_BEZIER:
            // mirror the last Cp1
            _myCp0 = sum(_myLastPos, product(difference(_myCp1, _myLastPos), -1.0));
            _myCp1 = new Vector3f(thePart.args[0], thePart.args[1], 0);
            myEnd = new Vector3f(thePart.args[2], thePart.args[3], 0);
            self.cbezier(_myCp0, _myCp1, myEnd, myRelativeFlag);
            break;
        case CUBIC_BEZIER:
            _myCp0 = new Vector3f(thePart.args[0], thePart.args[1], 0);
            _myCp1 = new Vector3f(thePart.args[2], thePart.args[3], 0);
            myEnd = new Vector3f(thePart.args[4], thePart.args[5], 0);
            self.cbezier(_myCp0, _myCp1, myEnd, myRelativeFlag);
            break;
        case ARC:
            // XXX
            myEnd = new Vector3f(thePart.args[5], thePart.args[6], 0);
            //self.line(myEnd, myRelativeFlag);
            _myLastPos = myEnd;
            break;
        case RETURN_TO_ORIGIN:
            self.close();
            break;
        }
    }

    /// 'true' if theChar is a supported command.
    function isCommand(theChar) {
        theChar = theChar.toLowerCase();
        switch (theChar) {
        case 'm':
        case 'l':
        case 'h':
        case 'v':
        case 't':
        case 'q':
        case 's':
        case 'c':
        case 'a':
        case 'z':
            return true;
        }
        return false;
    }

    /// 'true' if theChar is a number prefix. (Prefixes act as separators.)
    function isPrefix(theChar) {
        switch (theChar) {
        case '+':
        case '-':
            return true;
        }
        return false;
    }

    /// 'true' if theChar is a whitespace.
    function isWhiteSpace(theChar) {
        switch (theChar) {
        case ' ':
        case ',':
        case '\r':
        case '\n':
        case '\t':
            return true;
        }
        return false;
    }

    /// 'true' if theChar is any kind of separator.
    function isSeparator(theChar) {
        return isPrefix(theChar) || isWhiteSpace(theChar) || isCommand(theChar);
    }

    /// Split the path definition into a list of parts (commands and arguments).
    function splitPathDefinition(thePathDefinition) {
        var myParts = [];
        var myPart = null;
        for (var i = 0; i < thePathDefinition.length; ++i) {

            var myChar = thePathDefinition[i];
            if (isSeparator(myChar)) {
                if (myPart != null) {
                    myParts.push(myPart);
                    myPart = null;
                }
                if (isCommand(myChar)) {
                    myParts.push(myChar);
                    continue;
                } else if (isWhiteSpace(myChar)) {
                    continue;
                }
            }

            if (myPart == null) {
                myPart = "";
            }
            myPart += myChar;
        }

        // push last
        if (myPart != null) {
            myParts.push(myPart);
        }

        return myParts;
    }

    /// Lookup path command definition for the given part.
    function getCommandDefinition(theCommand) {

        theCommand = theCommand.toLowerCase();
        for (var i = 0; i < PATH_COMMANDS.length; ++i) {
            if (PATH_COMMANDS[i][1] == theCommand) {
                return PATH_COMMANDS[i];
            }
        }
        return null;
    }

    /// Parse parts into path command and its arguments.
    function parsePathParts(theParts) {
        var myPath = [];
        var myLastCommand = null;
        var myRelativeFlag = false;

        for (var i = 0; i < theParts.length; ++i) {

            var myCommandChar = theParts[i];
            var myCommand = getCommandDefinition(myCommandChar);
            if (myCommand == null) {
                if (myLastCommand == null) {
                    Logger.error("Part '" + myCommandChar + "' is not a command");
                    continue;
                }
 
                // repeat command
                myCommand = myLastCommand;
                print("Repeat command " + myCommand);
                --i;
            } else {
                myRelativeFlag = (myCommandChar >= 'a' && myCommandChar <= 'z');
            }

            var myArgs = [];
            //print("CHAR=" + myCommandChar, "CMD=" + myCommand, "REL=" + myRelativeFlag);
            for (var j = 0; j < myCommand[2]; ++j) {
                //print("ARGS " + theParts[i+1]);
                myArgs.push(Number(theParts[++i]));
            }
            myPath.push({cmd:myCommand, args:myArgs, relative:myRelativeFlag});
            myLastCommand = myCommand;
        }
        return myPath;
    }

    function setup() {

        if (theDescription == null) {
            return;
        }
        if (theCurveSegmentLength != null) {
            self.setCurveSegmentLength(theCurveSegmentLength);
        }

        var myNode = null;
        if (typeof(theDescription) == "string") {
            // convert to node
            theDescription = new Node(theDescription);
            if (!theDescription) {
                Logger.error("Description not valid XML:" + theDescription);
                return;
            }
            myNode = theDescription.firstChild;
        } else {
            myNode = theDescription;
        }

        // sanity checks
        if (myNode == null || myNode.nodeName != "path") {
            Logger.error("Node is not a '<path>':" + myNode);
            return;
        }
        if (!('d' in myNode)) {
            Logger.error("Node has no 'd' attribute:" + myNode);
            return;
        }
 
        var myParts = splitPathDefinition(myNode.d);
        var myPathParts = parsePathParts(myParts);

        // render path parts into elements
        for (var j = 0; j < myPathParts.length; ++j) {
            renderPart(myPathParts[j]);
        }
    }

    setup();
}

/*
 * test
 */
function test_SvgPath() {
    var i;

    var p = [];
    var myColor = new Vector4f(1,1,0,1);
    var myMatrix = new Matrix4f();

    if (1) {
        var SVG_FILE;
        if (0) {
            SVG_FILE = "../CONFIG/svg-logo-001.svg";
            myMatrix.scale(new Vector3f(0.0000015, -0.0000015, 0));
            myMatrix.translate(new Vector3f(-0.5, 0.4, -1));
        } else {
            SVG_FILE = "../CONFIG/curves.svg";
            myMatrix.scale(new Vector3f(0.00025, 0.00025, 0));
            myMatrix.translate(new Vector3f(-0.15, -0.25, -1));
        }

        print("Loading '" + SVG_FILE + "'");
        var mySvgFile = readFileAsString(SVG_FILE);
        var mySvgNode = new Node(mySvgFile);

        var myPaths = getDescendantsByTagName(mySvgNode, "path", true);
        for (i = 0; i < myPaths.length; ++i) {
            p.push(new SvgPath(myPaths[i]));
        }
    } else {
        // fixed path
        const __SVGPATH_TEST = [
            '<path d="M 0 0 l 0 50 l 10 50 l -10 50 l -50 0 z" />',
            '<path d="M 0 0 h 100 v 100 Z"/>',
            '<path d="M 50 40 l 50 50 l 30 -20 Z" />',
            '<path d="M 50 60 Q 100 10 150 60 T 250 60 T 350 60" />',
            '<path d="M 50 90 L 150 35 Q 200 110 250 35 L 300 90 Z" />',
            '<path d="M 350 35 V 75 C 300 30 250 110 200 85 S 150 30 100 50 V 35 Z" />',
            '<path d="M 50 90 V 75 C 60 30 200 110 250 35 L 300 90 Z" />',
            '<path d="M 365 35 V 95 H 35 V 75 A 40 40 0 0 0 75 35 Z M 355 45 V 85 H 45 A 50 50 0 0 0 85 45 Z" />',
            ];

        for (i = 0; i < __SVGPATH_TEST.length; ++i) {
            p.push(new SvgPath(__SVGPATH_TEST[i]));
        }
        myMatrix.scale(new Vector3f(0.002, 0.002, 0));
        myMatrix.translate(new Vector3f(-0.35, -0.2, -1));
    }

    if (0) {
        // dynamic path
        var myDynamicPath = new SvgPath();
        p.push(myDynamicPath);
        myDynamicPath.vline(100);
        myDynamicPath.hline(100);
        myDynamicPath.vline(-50);
        myDynamicPath.close();
    }

    if (0) {
        // perpendicular path
        var myPPath = p[0].createPerpendicularPath(new Vector3f(25,5,0), 100);
        p.push(myPPath);

        // intersect
        var myIntersections = p[0].intersect(new Sphere(new Vector3f(717,2319,0), 50));
        if (myIntersections) {
            print("intersections=" + myIntersections.points);
        }
    }

    var mySubPath = null;
    if (0) {
        // sub-path
        //mySubPath = p[6].createSubPath([612.944,2155.85,0], [867.678,2155.85,0], true); // top, revert, swap
        //mySubPath = p[6].createSubPath([867.678,2155.85,0], [612.944,2155.85,0], true); // top, OK, XXX
        //mySubPath = p[6].createSubPath([612.944,155.85,0], [867.678,155.85,0], true); // bottom, OK
        //mySubPath = p[6].createSubPath([1000, 100, 0], [1000, 1000, 0], true); // right, OK
        //mySubPath = p[6].createSubPath([1000, 1000, 0], [1000, 100, 0], true); // right, revert, YYY
        //mySubPath = p[6].createSubPath([1000, 100, 0], [100, 1000, 0], true); // left, revert, YYY
        //mySubPath = p[6].createSubPath([100, 1000, 0], [100, 100, 0], true); // left, OK
        mySubPath = p[6].createSubPath([378.002,2155.28,0], [487.287,2155.85,0], true); // left, OK
        p.push(mySubPath);
    }

    var myIndex = 0;
    var myLastTime = null;

    use("SceneViewer.js");
    var mySceneViewer = new SceneViewer(null);
    mySceneViewer.setup(800, 600, false, "SvgPath");

    mySceneViewer.onKey = function(theKey, theState) {
        if (theKey == "space" && theState) {
            myIndex = (myIndex + 1) % p.length;
            print("index=" + myIndex);
        }
    }

    mySceneViewer.onPostRender = function() {
        p[myIndex].draw(myColor, myMatrix);
        if (mySubPath) {
            if (1) {
                for (var i = 0; i < 1.0; i += 0.1) {
                    var myPoint = mySubPath.pointOnPath(i);
                    window.getRenderer().draw(new Sphere(myPoint, 5), [1,0,0,1], myMatrix, 1);
                }
            } else {
                myPoint = mySubPath.pointOnPath(myIndex * 0.1);
                window.getRenderer().draw(new Sphere(myPoint, 5), [1,0,0,1], myMatrix, 1);
            }
        }
    }
    mySceneViewer.go();
}
//test_SvgPath();
