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

use("../../../../video/testLeak.js");

const MOVIE_1 = "../../../../video/testmovies/1.1.mov";
const MOVIE_2 = "../../../../video/testmovies/1.4.mov"


var myTestName = "Quicktimedecoder Leaktest";
var myDecoderPlug = "y60QuicktimeDecoder";
var myVideoCount = 100;

var myMovieLeak =  myVideoCount * 8 * 1024; // each qt movie leaks 8kb per movie (not per frame, tolerable)

var myMovieBaseMemoryUsage =  5*1024*1024; // (due to some basic memory allocation, i.e. plugin-ctor code, SomImageFactory)
var myAllowedMemoryUsage = myMovieBaseMemoryUsage + myMovieLeak;



var mySuite = new UnitTestSuite(myTestName);
mySuite.addTest(new MovieLeakUnitTest(myTestName, [MOVIE_1, MOVIE_2], myDecoderPlug, myVideoCount, myAllowedMemoryUsage));
mySuite.run();
print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());

