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
//    $RCSfile: testEnum.tst.js,v $
//
//   $Revision: 1.2 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

function TestEnum() {
    this.Constructor(this, "TestEnum");
};

TestEnum.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myEnum = BlendEquation.max;
        ENSURE('obj.myEnum == BlendEquation.max');

        switch (obj.myEnum) {
            case BlendEquation.max:
                SUCCESS("switch");
                break;
            default:
                FAILURE("switch");
        }

        ENSURE('obj.myEnum == "max"');
        obj.myScene = new Scene(null);
        obj.myScene.setup();
        obj.myMaterial = Modelling.createUnlitTexturedMaterial(obj.myScene, "", "Foo", false);
        ENSURE("obj.myMaterial.properties.blendequation == BlendEquation.add");
        obj.myMaterial.properties.blendequation = BlendEquation.subtract;
        ENSURE("obj.myMaterial.properties.blendequation == BlendEquation.subtract");
        ENSURE("obj.myMaterial.properties.blendequation == 'subtract'");
        switch (obj.myMaterial.properties.blendequation) {
            case BlendEquation.subtract:
                SUCCESS("switch");
                break;
            default:
                FAILURE("switch");
        }

    }
};

function TestBitset() {
    this.Constructor(this, "TestBitset");
};
TestBitset.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myBitset = new RenderStyles();
        ENSURE(' ! obj.myBitset.frontfacing');
        ENSURE(' ! obj.myBitset.backfacing');
        ENSURE(' ! obj.myBitset["backfacing"]');
        obj.myBitset.backfacing = true;
        obj.myBitset.frontfacing = true;
        ENSURE('obj.myBitset.backfacing');
        ENSURE('obj.myBitset["backfacing"]');
        ENSURE_EXCEPTION('obj.myBitset.topfacing = true');

        obj.myBitset.setValue( RenderStyle.frontfacing, RenderStyle.backfacing, RenderStyle.ignore_depth);
        obj.myBitset.setValue( 0xff );
        var myMask = RenderStyles.asMask( RenderStyle.frontfacing, RenderStyle.backfacing, RenderStyle.ignore_depth);
        obj.myBitset.setValue( myMask );
    }
};
var mySuite = new UnitTestSuite("UnitTest");

try {
    mySuite.addTest(new TestBitset);
    mySuite.addTest(new TestEnum);
    mySuite.run();
} catch (e) {
    print("## An unknown exception occured during execution." + e + "");
    mySuite.incrementFailedCount();
}

print(">> Finished test suite '"+mySuite.getMyName() +"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
