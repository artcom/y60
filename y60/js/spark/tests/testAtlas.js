/*jslint nomen: false*/
/*global print, use, UnitTest, UnitTestSuite, __FILE__, exit, Namespace,
         ENSURE, ENSURE_EQUAL, spark, Vector2f, Vector3f, Node*/

use("UnitTest.js");

// Other imports needed for the tests
use("spark/spark.js");

// YourTest

function SparkAtlasTest() {
    this.Constructor(this, "SparkAtlasTest");
}

SparkAtlasTest.prototype.Constructor = function (obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);

    function testAtlas() {
        obj.App = spark.loadFile("fixtures/atlasTest.spark");
        obj.App.go();
    }

    obj.run = function () {
        testAtlas();
    };
};

/*
Add more tests here if necessary
*/

// testSuite

try {
    var myScriptPathSegments = __FILE__().split("/");
    var mySuiteName = myScriptPathSegments[myScriptPathSegments.length - 1];
    var mySuite = new UnitTestSuite(mySuiteName);

    // may be more than one test here
    mySuite.addTest(new SparkAtlasTest()); 
    mySuite.run();

    print(">> Finished test suite '" + mySuiteName + "', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}

