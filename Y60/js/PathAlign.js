//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/// Alignment for Text and PathStrip.
const TOP_ALIGNMENT    = 0;
const BOTTOM_ALIGNMENT = 1;
const CENTER_ALIGNMENT = 2;
const BASELINE_ALIGNMENT = 3;

/// Up-vector for determination of path left-vector.
const UP_VECTOR = new Vector3f(0,0,1);

function PathAlign(thePath) {
    this.Constructor(this, thePath);
}

PathAlign.prototype.Constructor = function(self, thePath) {

    /// Get path this objects aligns to.
    self.getPath = function() {
        return _myPath;
    }

    /// Set start point for path alignment.
    self.setCurrentPosition = function(thePoint) {

        var myNearest = _myPath.nearest(thePoint);
        if (myNearest == null) {
            return false;
        }
        _myCurrentPosition = myNearest.point;
        _myCurrentSegment = myNearest.element;
        return true;
    }

    self.getCurrentPosition = function() {
        return _myCurrentPosition;
    }

    self.fitsAdvancement = function(theLength, theSegment, theCurrentPosition) {
        if (theSegment == undefined) {
            theSegment = _myCurrentSegment;
        }        
        if (theCurrentPosition == undefined) {
            theCurrentPosition = _myCurrentPosition;
        }        
        var myElement = _myPath.getElement(theSegment);
        var myRemainSegment = difference(myElement.end, theCurrentPosition);
        var myRemainLength = magnitude(myRemainSegment);
        if (myRemainLength >= theLength) {
            return true;
        } else {
            if (theSegment == _myPath.getNumElements()-1) {
                return false;
            } else {
                return self.fitsAdvancement(theLength-myRemainLength ,theSegment+1, myElement.end);
            }
        }
    }
    
    /**
     * Advance on path for the given length.
     * NOTE: 'theStartPosition' must not be used by external applications!
     */
    self.advance = function(theLength, theStartPosition) {

        if (theStartPosition == null) {
            theStartPosition = _myCurrentPosition;
        }
        var myResult = { start:theStartPosition, end:null };

        if (theLength == 0) {
            myResult.end = theStartPosition;
            return myResult;
        }

        var myLastPos = theStartPosition;
        var mySegment = _myCurrentSegment;
        var myForwardTraversal = (theLength > 0.0 ? true : false);
        while (Math.abs(theLength) > 0.0) {

            var myElement = _myPath.getElement(mySegment);
            if (myElement == null) {
                Logger.error("Path (" + _myPath.getNumElements() + ") has no element " + mySegment);
                return null;
            }

            // remaining segment
            var myRemainSegment;
            if (myForwardTraversal) {
                myRemainSegment = difference(myElement.end, myLastPos);
            } else {
                myRemainSegment = difference(myLastPos, myElement.origin);
            }
            var myRemainLength = magnitude(myRemainSegment);
            var myRemainDirection = normalized(myRemainSegment);
            if (Math.abs(theLength) > myRemainLength) {
                // still room
                if (myForwardTraversal) {
                    theLength -= myRemainLength;
                    myLastPos = myElement.end;
                } else {
                    theLength += myRemainLength;
                    myLastPos = myElement.origin;
                }
            } else {
                // fits
                myLastPos = sum(myLastPos, product(myRemainDirection, theLength));
                break;
            }

            // next/prev segment
            if (_myPath.getNumElements() == 1) {
                //_myPath.print();
                break;
            }
            if (myForwardTraversal) {
                // next segment
                mySegment = (mySegment + 1) % _myPath.getNumElements();
            } else {
                // previous segment
                mySegment -= 1;
                if (mySegment < 0) {
                    mySegment += _myPath.getNumElements();
                }
            }
        }

        _myCurrentPosition = myLastPos;
        _myCurrentSegment = mySegment;
        myResult.end = myLastPos;

        return myResult;
    }

    /**********************************************************************
     *
     * Private
     *
     **********************************************************************/

    var _myPath = thePath;
    var _myCurrentSegment = 0;
    var _myCurrentPosition = null;

    function setup() {
        _myCurrentPosition = _myPath.getElement(_myCurrentSegment).origin;    
    }

    setup();
}

function test_PathAlign() {

    use("Y60JSSL.js");

    var myMatrix = new Matrix4f();
    var myPath;
    if (1) {
        var SVG_FILE = expandEnvironment("${PRO}/testmodels/curves.svg");
        myMatrix.scale(new Vector3f(0.00025, 0.00025, 0));
        myMatrix.translate(new Vector3f(-0.15, -0.25, -1));

        print("Loading '" + SVG_FILE + "'");
        var mySvgFile = readFileAsString(SVG_FILE);
        var mySvgNode = new Node(mySvgFile);

        var myPaths = getDescendantsByTagName(mySvgNode, "path", true);
        
        myPath = new SvgPath(myPaths[6].d);
    } else {
        const __PATHALIGN_TESTS = [
            'M 0 0 l 0 50 l 10 50 l -10 50 l -50 0 z',
            'M 50 40 l 50 50 l 30 -20 Z',
            'M 50 60 Q 100 10 150 60 T 250 60 T 350 60',
            'M 50 90 L 150 35 Q 200 110 250 35 L 300 90 Z',
            'M 350 35 V 75 C 300 30 250 110 200 85 S 150 30 100 50 V 35 Z',
            'M 50 90 V 75 C 60 30 200 110 250 35 L 300 90 Z',
            'M 365 35 V 95 H 35 V 75 A 40 40 0 0 0 75 35 Z M 355 45 V 85 H 45 A 50 50 0 0 0 85 45 Z',
            ];

        myPath = new SvgPath(__PATHALIGN_TESTS[0]);
        myMatrix.translate(new Vector3f(0,0,-10));
    }
    var myPathAlign = new PathAlign(myPath);

    var next = true;
    var myLineSegment = null;

    use("SceneViewer.js");
    var mySceneViewer = new SceneViewer(null);
    mySceneViewer.setup(800, 600, false, "PathAlign");

    mySceneViewer.onKey = function(theKey, theState) {
        if (theKey == "space" && theState) {
            myIndex = (myIndex + 1) % p.length;
            print("index=" + myIndex);
        }
    }

    mySceneViewer.onKey = function(theKey, theState) {
        if (theState) {
            next = true;
        }
    }

    mySceneViewer.onPostRender = function() {
        window.getRenderer().draw(myPath, [1,1,0,1], myMatrix);
        if (next) {
            var mySegment = myPathAlign.advance(-45);
            myLineSegment = new LineSegment(mySegment.start, mySegment.end);
            next = false;
        }
        window.getRenderer().draw(myLineSegment, [1,0,0,1], myMatrix, 4);
    }

    mySceneViewer.go();
}
//test_PathAlign();
