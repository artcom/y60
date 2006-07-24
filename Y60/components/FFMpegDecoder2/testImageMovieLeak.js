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


use("Overlay.js");
use("Exception.js");
use("UnitTest.js");

var _myOverlay    = null;

plug("ProcFunctions");

const START_FRAMES = 999;
const END_FRAMES   = 100;
const IMAGE_CTOR_COUNT = 100;
const MOVIE_CTOR_COUNT = 100;

var window     = new RenderWindow();

function ImageMovieLeakUnitTest() {
    this.Constructor(this, "ImageMovieLeakUnitTest");
};

ImageMovieLeakUnitTest.prototype.Constructor = function(obj, theName) {
    var _myToggle         = false;
    var _myShowFrame      = -1;
    var _myImageShowCount = 0;
    var _myMovieShowCount = 0;
    var _myFrameCount = 0;
    var _myStartMemory = 0;
    
    UnitTest.prototype.Constructor(obj, theName);
    obj.run = function() {    
        window.onFrame = function(theTime) {
            if (_myFrameCount == START_FRAMES) {
                _myStartMemory = getProcessMemoryUsage();
            } else if (_myFrameCount > START_FRAMES &&
                  _myFrameCount < START_FRAMES +((IMAGE_CTOR_COUNT + MOVIE_CTOR_COUNT)*5) +1) {
                  if (_myFrameCount % 5 == 0) {
                    if (_myImageShowCount < IMAGE_CTOR_COUNT) {
                        setup(true);
                        _myImageShowCount++;            
                    } else if (_myMovieShowCount < MOVIE_CTOR_COUNT) {
                        setup(false);
                        _myMovieShowCount++            
                    }
                    _myShowFrame = _myFrameCount;
                } 
                if (_myFrameCount == _myShowFrame + 4) {
                    teardown();
                    gc();
                }        
            } else if (_myFrameCount == START_FRAMES+ ((IMAGE_CTOR_COUNT + MOVIE_CTOR_COUNT) *5) + END_FRAMES) {
                gc();
                var myUsedMemory = getProcessMemoryUsage();
                obj.myMemoryDifff =  myUsedMemory - _myStartMemory;
                print("Memory at starttime: " + _myStartMemory);
                print("Memory at app end  : " + myUsedMemory);
                print("Difference         : " + obj.myMemoryDifff);

                obj.ImageBaseMemoryUsage =  1600000;
                obj.VideoBaseMemoryUsage =  1600000;
                obj.VideoCopyMemory      = 40000000;
                obj.AllowedMemoryUsage = obj.ImageBaseMemoryUsage + obj.VideoBaseMemoryUsage + obj.VideoCopyMemory;
                print("allowed difference : " + obj.AllowedMemoryUsage);
                print("(due to some memory allocation things during playback and display)");
                // Sorry, one image and one movie still resides in memory till shutdown, tolerable (vs)
                ENSURE('obj.myMemoryDifff < obj.AllowedMemoryUsage');
                
                window.stop();
            }
            _myFrameCount++;
        }
    
        window.go();
    }
    function setup(theImageFlag) {
        if (theImageFlag) {
            if (_myToggle) {
                _myOverlay = new ImageOverlay(window.scene, "../../shader/shadertex/ac_logo.png");
            } else {
                _myOverlay = new ImageOverlay(window.scene, "../../shader/shadertex/hatch.jpg");
            }
            //print("setup image: "+_myOverlay.image.src);
        } else {
            if (_myToggle) {
                _myOverlay = new MovieOverlay(window.scene, "../../video/testmovies/mpeg2_160x120_25_1_noaudio.mpg");
            } else {
                _myOverlay = new MovieOverlay(window.scene, "../../video/testmovies/xvid_160x120_25_1_noaudio.mov");
            }
            //print("setup movie: "+_myOverlay.movie.src);
        }
        _myToggle = !_myToggle;
    }
    
    function teardown() {
        _myOverlay.removeFromScene();
        _myOverlay = null;
        //print("teardown");
    }
    
}
    
var myTestName = "testImageMovieLeak.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new ImageMovieLeakUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());

