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

const MOVIE_1 = "../../../../video/testmovies/1.1.mpg";
const MOVIE_2 = "../../../../video/testmovies/1.4.mpg"
//const MOVIE_2 = "../../../../video/testmovies/T2_pan_7.mpg";

var myTestName = "y60FFMpegDecoder1 Leaktest";
var myDecoderPlug = "y60FFMpegDecoder1";
var myVideoCount = 500;

var myMovieLeak =  myVideoCount * 26 * 1024; // each mpeg2 movie leaks 10kb per movie (not per frame, tolerable)

var myMovieBaseMemoryUsage =  6*1024*1024; // (due to some basic memory allocation, i.e. plugin-ctor code, lib init code, SomImageFactory)
var myAllowedMemoryUsage = myMovieBaseMemoryUsage + myMovieLeak;



var mySuite = new UnitTestSuite(myTestName);
mySuite.addTest(new MovieLeakUnitTest(myTestName, [MOVIE_1, MOVIE_2], myDecoderPlug, myVideoCount, myAllowedMemoryUsage));
mySuite.run();
print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());

