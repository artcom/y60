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
//    $RCSfile: testLocale.tst.js,v $
//
//   $Revision: 1.2 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

function TestLocale() {
    this.Constructor(this, "TestLocale");
};

TestLocale.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        //obj.myLocale = new Locale("german");
        obj.myLocale = new Locale("");
        DPRINT('obj.myLocale');
        DPRINT('obj.myLocale.formatFloat(2.5)');
        DPRINT('obj.myLocale.parseFloat("not-a-number")');
        ENSURE('!isFinite(obj.myLocale.parseFloat("not-a-number"))');
        ENSURE('obj.myLocale.parseFloat("2,5") == 2.5');
        ENSURE('obj.myLocale.formatFloat(2.5) == "2,5"');
    }
};

var mySuite = new UnitTestSuite("UnitTest");

try {
    mySuite.addTest(new TestLocale);
    mySuite.run();
} catch (e) {
    print("## An unknown exception occured during execution." + e + "");
    mySuite.incrementFailedCount();
}
