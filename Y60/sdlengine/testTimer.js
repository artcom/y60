/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testTimer.js,v $
//
//   $Revision: 1.3 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

var ourMin = 0;
var ourMax = 10;
var ourFactor = 10;
//var ourFactor = 4294967295;
//var ourFactor =   3000000000;

function testOverflow() {
    var myStep = (ourMax - ourMin) / 10;
    for (var i = ourMin; i <= ourMax; i += myStep) {
        var myMilliseconds = debugmillisec(i, ourFactor);
        var mySeconds = myMilliseconds / (1000 * ourFactor);
        var myError   = Math.abs(myMilliseconds - i * ourFactor * 1000);
        print("  JS milliseconds: " + myMilliseconds);
        print("  JS counter       " + i);
        print("  JS seconds:      " + mySeconds);
        print("  error:           " + myError);
        if (myError > 0.001) {
            if (myError > 10) {
                print("+++++++++++++++++++++++  OVERFLOW  +++++++++++++++++++++++");
            } else {
                print("+++++++++++++++++++++++  PRECISION < 1 microsec.  +++++++++++++++++++++++");
            }
            ourMin = i - myStep;
            ourMax = i;
            return true;
        }
    }

    return false;
}

while(!testOverflow()) {
    ourMax *= 10;
}

while((ourMax - ourMin) > 1) {
    testOverflow()
}

exit(0);