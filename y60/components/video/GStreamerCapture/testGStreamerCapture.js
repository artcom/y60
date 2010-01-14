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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// libraries
use("SceneViewer.js");
plug("GStreamerCapture");
//plug("DC1394");

var ourShow  = null;

const WINDOW_WIDTH = 640;
const WINDOW_HEIGHT = 480;

function TestGStreamerCapture(theArguments, theWidth, theHeight, theTitle) {
    var Base = {};
    var Public = this;

    var _myCameraInput = null;

    SceneViewer.prototype.Constructor(Public, theArguments);

    //////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////

    Base.setup = Public.setup;
    Public.setup = function() {
        Logger.info("Setting up...");

        // configure window
        // (before Base.setup because of SDL window initialization)
        window              = new RenderWindow();
        window.position     = [0, 0];

        Base.setup(WINDOW_WIDTH, WINDOW_HEIGHT, false, theTitle);

        _myCameraInput = createGrabber( "video://video_mode=640x480_YUV422,fps=30", [0,0] );
        //_myCameraInput = Modelling.createImage( window.scene, "test.png" );

        var myOverlay = new ImageOverlay( window.scene, _myCameraInput );


        //window.camera.position = new Vector3f( 0.0, 0.0, 500.0 );


    }

    function createGrabber(thePath, thePosition){
        var myCaptureNode = Node.createElement("capture");
        myCaptureNode.resize = "none";
        myCaptureNode.playmode = "play";
        myCaptureNode.width=640;
        myCaptureNode.height=480;
        myCaptureNode.src = thePath;
        window.scene.images.appendChild(myCaptureNode);
        window.scene.loadCaptureFrame(myCaptureNode);
        return myCaptureNode;
    }

    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);
    }


    //////////////////////////////////////////////////////////////////
    // Private
    //////////////////////////////////////////////////////////////////

}

try {
    print(arguments);
    ourShow = new TestGStreamerCapture(arguments, WINDOW_WIDTH, WINDOW_HEIGHT, "TestGStreamerCapture");
    ourShow.setup();
    ourShow.go();
} catch (exception) {
    reportException(exception);
}
