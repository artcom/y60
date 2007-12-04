//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy Protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("UnitTest.js");

function TestLocale() {
    this.Constructor(this, "TestLocale");
};

TestLocale.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myLocale = new Locale("german");
        //obj.myLocale = new Locale("");
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
