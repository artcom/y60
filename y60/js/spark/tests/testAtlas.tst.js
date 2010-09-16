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

    function testConstructor() {
        // test load constructor
        obj.newAtlas = new TextureAtlas("fixtures/textureAtlas.xml");
        ENSURE('obj.newAtlas instanceof TextureAtlas');
        ENSURE('obj.newAtlas.imagePath === "fixtures/textureAtlas.png"');
        ENSURE('obj.newAtlas.findTextureSize("red").x == 5'); 
        ENSURE('obj.newAtlas.findTextureSize("red").y == 1'); 
        ENSURE_EXCEPTION("new TextureAtlas( 'fixtures/file-not-found')");

        // test generating constructor
        obj.newAtlas = new TextureAtlas( { 'red': 'fixtures/textures/testbild00.rgb' } );
        ENSURE('obj.newAtlas instanceof TextureAtlas');
        ENSURE_EXCEPTION("new TextureAtlas( { 'red': 'fixtures/textures/file-not-found' } )");

        ENSURE_EXCEPTION("new TextureAtlas()");
    }

    function testAtlas() {
        var newAtlas = new TextureAtlas( { 'red': 'fixtures/textures/testbild00.rgb' } );
        obj.App = spark.loadFile("fixtures/atlasTest.spark");
        // obj.App.go();
    }

    obj.run = function () {
        testConstructor();
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

