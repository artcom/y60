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

/// Global up vector.
const UP_VECTOR = new Vector3f(0,0,1);

/*
 * test
 */
function test_SvgPath() {

    use("SceneViewer.js");
    use("Y60JSSL.js");

    var i;
    var p = [];
    var myColor = new Vector4f(1,1,0,1);
    var myMatrix = new Matrix4f();

    if (1) {
        var SVG_FILE;
        if (0) {
            SVG_FILE = "${PRO}/testmodels/svg-logo-001.svg";
            myMatrix.scale(new Vector3f(0.0000015, -0.0000015, 0));
            myMatrix.translate(new Vector3f(-0.5, 0.4, -1));
        } else {
            SVG_FILE = "${PRO}/testmodels/curves.svg";
            myMatrix.scale(new Vector3f(0.00025, 0.00025, 0));
            myMatrix.translate(new Vector3f(-0.15, -0.25, -1));
        }

        print("Loading '" + SVG_FILE + "'");
        var mySvgFile = readFileAsString(expandEnvironment(SVG_FILE));
        var mySvgNode = new Node(mySvgFile);

        var myPaths = getDescendantsByTagName(mySvgNode, "path", true);
        for (i = 0; i < myPaths.length; ++i) {
            p.push(new SvgPath(myPaths[i].d));
        }
        //p[0].revert();
    } else {
        // fixed path
        const __SVGPATH_TEST = [
            'M 0 0 l 0 50 l 10 50 l -10 50 l -50 0 z',
            'M 0 0 h 100 v 100 Z',
            'M 50 40 l 50 50 l 30 -20 Z',
            'M 50 60 Q 100 10 150 60 T 250 60 T 350 60',
            'M 50 90 L 150 35 Q 200 110 250 35 L 300 90 Z',
            'M 350 35 V 75 C 300 30 250 110 200 85 S 150 30 100 50 V 35 Z',
            'M 50 90 V 75 C 60 30 200 110 250 35 L 300 90 Z',
            'M 365 35 V 95 H 35 V 75 A 40 40 0 0 0 75 35 Z M 355 45 V 85 H 45 A 50 50 0 0 0 85 45 Z',
            ];

        for (i = 0; i < __SVGPATH_TEST.length; ++i) {
            p.push(new SvgPath(__SVGPATH_TEST[i]));
        }
        myMatrix.scale(new Vector3f(0.002, 0.002, 0));
        myMatrix.translate(new Vector3f(-0.35, -0.2, -1));
    }

    if (1) {
        // perpendicular path
        var myPPath = p[0].createPerpendicularPath(new Vector3f(25,5,0), 100);
        print("perpendicular", myPPath, "len=" + myPPath.length);
        p.push(myPPath);
    }

    if (1) {
        // dynamic path
        var myDynamicPath = new SvgPath();
        myDynamicPath.vline(100);
        myDynamicPath.hline(100);
        myDynamicPath.vline(-50);
        myDynamicPath.close();
        print("dynamic", myDynamicPath, "len=" + myDynamicPath.length, myDynamicPath.numelements);
        p.push(myDynamicPath);
    }

    var mySubPath = null;
    if (1) {
        var myP = (p.length > 6 ? p[6] : p[p.length-1]);
        // sub-path
        //mySubPath = myP.createSubPath([612.944,2155.85,0], [867.678,2155.85,0], true); // top, revert, swap
        //mySubPath = myP.createSubPath([867.678,2155.85,0], [612.944,2155.85,0], true); // top, OK, XXX
        //mySubPath = myP.createSubPath([612.944,155.85,0], [867.678,155.85,0], true); // bottom, OK
        //mySubPath = myP.createSubPath([1000, 100, 0], [1000, 1000, 0], true); // right, OK
        //mySubPath = myP.createSubPath([1000, 1000, 0], [1000, 100, 0], true); // right, revert, YYY
        //mySubPath = myP.createSubPath([1000, 100, 0], [100, 1000, 0], true); // left, revert, YYY
        //mySubPath = myP.createSubPath([100, 1000, 0], [100, 100, 0], true); // left, OK
        mySubPath = myP.createSubPath([378.002,2155.28,0], [487.287,2155.85,0], true); // left, OK
        if (mySubPath) {
            p.push(mySubPath);
        }
    }

    var mySceneViewer = new SceneViewer(null);
    mySceneViewer.setup(800, 600, false, "SvgPath");
    var myIndex = 0;

    mySceneViewer.onKey = function(theKey, theState) {
        if (theKey == "space" && theState) {
            myIndex = (myIndex + 1) % p.length;
            print("index=" + myIndex);
        }
    }

    mySceneViewer.onPostRender = function() {
        window.getRenderer().draw(p[myIndex], [1,0,0,1], myMatrix, 1);
        if (mySubPath) {
            if (1) {
                for (var i = 0; i < 1.0; i += 0.1) {
                    var myPoint = mySubPath.pointOnPath(i);
                    window.getRenderer().draw(new Sphere(myPoint.point, 5), [1,0,0,1], myMatrix);
                }
            } else {
                myPoint = mySubPath.pointOnPath(myIndex * 0.1);
                window.getRenderer().draw(new Sphere(myPoint.point, 5), [1,0,0,1], myMatrix);
            }
        }

        if (1) {
            // intersect
            var myIsect;
            //myIsect = p[myIndex].intersect(new Sphere(new Vector3f(717,2200,0), 150));
            //myIsect = p[myIndex].intersect(new LineSegment(new Vector3f(717,0,0), new Vector3f(717,2500,0)));
            myIsect = p[myIndex].intersect(new Line(new Vector3f(394.477,2109.85,0), new Vector3f(0.0456089,-0.998959,0)));
            for (var i = 0; i < myIsect.points.length; ++i) {
                window.getRenderer().draw(new Sphere(myIsect.points[i], 10), [1,1,0,1], myMatrix);
            }
        }
    }
    mySceneViewer.go();
}
//test_SvgPath();
