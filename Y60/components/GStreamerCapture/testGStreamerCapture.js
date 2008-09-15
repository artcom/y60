//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

// libraries
use("SceneViewer.js");
plug("GStreamerCapturePlugin");
//plug("y60DC1394");

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
