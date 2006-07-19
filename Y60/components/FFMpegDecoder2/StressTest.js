//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: testText.js,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/03/18 11:48:31 $
//
//
//=============================================================================

use("SceneTester.js");
use("ImageManager.js");
use("OverlayManager.js");


var _myMovieMaterial;
var _myMovieImage;
var _myMovieQuad;
var _myMovieBody;

function setup(theViewer) {
    _myMovieImage = theViewer.getImageManager().createMovie("../FFMpegDecoder/testfiles/counter_short.mpg", "y60FFMpegDecoder2");
    _myMovieImage.playmode = "play";
    _myMovieImage.audio = false;
    _myMovieMaterial = theViewer.getScene().createTexturedMaterial(_myMovieImage);  
    _myMovieQuad = Modelling.createQuad(theViewer.getScene(), _myMovieMaterial.id, [-100,-100,0], [100,100,0]);
    _myMovieBody = Modelling.createBody(theViewer.getScene().world, _myMovieQuad.id);
    _myMovieBody.position.z = -100;
}

function teardown(theViewer) {
    _myMovieBody.parentNode.removeChild(_myMovieBody);
    _myMovieBody = null;
    _myMovieQuad.parentNode.removeChild(_myMovieQuad);
    _myMovieQuad = null;
    _myMovieMaterial.parentNode.removeChild(_myMovieMaterial);
    _myMovieMaterial = null;
    _myMovieImage.parentNode.removeChild(_myMovieImage);
    _myMovieImage = null;
}

try {
    var ourShow = new SceneViewer(arguments);

    ourShow.SceneTester = [];

    var myFrameCount = 0;
    ourShow.SceneTester.onFrame = ourShow.onFrame;
    ourShow.onFrame = function(theTime) {
        ourShow.SceneTester.onFrame(theTime);
        if (myFrameCount % 5 == 0) {
            teardown(ourShow);
        }
        if (myFrameCount % 5 == 1) {
            setup(ourShow);
        }
        myFrameCount++;
    }
        

    ourShow.setup(400, 400);
    setup(ourShow);

    ourShow.go();
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(-1);
}
