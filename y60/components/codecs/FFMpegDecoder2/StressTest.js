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

var _myMovieMaterial;
var _myMovieImage;
var _myMovieQuad;
var _myMovieBody;

function setup(theViewer) {
    _myMovieImage = theViewer.getImageManager().createMovie("../FFMpegDecoder/testfiles/counter_short.mpg", "FFMpegDecoder2");
    _myMovieImage.playmode = "play";
    _myMovieImage.audio = false;
    _myMovieMaterial = Modelling.createUnlitTexturedMaterial(theViewer.getScene(), _myMovieImage);
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
