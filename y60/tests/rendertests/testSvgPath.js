//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");

try {
    var ourShow = new SceneTester(arguments);

    var Base = {};
    var mySvgPath = [];

    function addPathFromString(thePathList, theDescription, theColor, theMatrix) {

        //print("d=" + theDescription);
        var mySvgPath = new SvgPath(theDescription);
        var myItem = { path:mySvgPath, color:theColor, matrix:theMatrix };
        thePathList.push(myItem);
    }

    function addPathFromFile(thePathList, theFilename, theColor, theMatrix) {
        var mySvgFile = readFileAsString("tex/svg-logo-001.svg");
        var mySvgNode = new Node(mySvgFile);
        var myPathList = mySvgNode.findAll(".//path");
        for (var i = 0; i < myPathList.length; ++i) {
            if ('d' in myPathList[i]) {
                addPathFromString(thePathList, myPathList[i].d, theColor, theMatrix);
            }
        }
    }


    Base.setup = ourShow.setup;
    ourShow.setup = function() {
        Base.setup();

        // setup matrix so we can see something
        var myColor = new Vector4f(1,0,0,1);
        var myMatrix = new Matrix4f();
        myMatrix.scale(new Vector3f(0.0000017, -0.0000017, 0));
        myMatrix.translate(new Vector3f(-0.6, 0.4, -1));
        addPathFromFile(mySvgPath, "tex/svg-logo-001.svg", myColor, myMatrix);

        // fixed path
        const SVGPATH = [
            'M 0 0 l 0 50 l 10 50 l -10 50 l -50 0 z',
            'M 0 0 h 100 v 100 Z',
            'M 50 40 l 50 50 l 30 -20 Z',
            'M 50 60 Q 100 10 150 60 T 250 60 T 350 60',
            'M 50 90 L 150 35 Q 200 110 250 35 L 300 90 Z',
            'M 350 35 V 75 C 300 30 250 110 200 85 S 150 30 100 50 V 35 Z',
            'M 50 90 V 75 C 60 30 200 110 250 35 L 300 90 Z',
            'M 365 35 V 95 H 35 V 75 A 40 40 0 0 0 75 35 Z M 355 45 V 85 H 45 A 50 50 0 0 0 85 45 Z',
            ];

        myColor = new Vector4f(1,1,0,1);
        myMatrix = new Matrix4f();
        myMatrix.scale(new Vector3f(0.002, 0.002, 0));
        myMatrix.translate(new Vector3f(-0.35, -0.2, -1));

        for (var i = 0; i < SVGPATH.length; ++i) {
            addPathFromString(mySvgPath, SVGPATH[i], myColor, myMatrix);
        }
    }

    Base.onPostRender = ourShow.onPostRender;
    ourShow.onPostRender = function() {
        var myRenderer = window.getRenderer();
        for (var i = 0; i < mySvgPath.length; ++i) {
            var myItem = mySvgPath[i];
            myRenderer.draw(myItem.path, myItem.color, myItem.matrix);
        }
        Base.onPostRender();
    }

    ourShow.setSplashScreen(false);
    ourShow.setTestDurationInFrames(10);
    ourShow.setup();
    ourShow.go();

} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
