/*jslint nomen: false*/
/*global print, use, UnitTest, UnitTestSuite, __FILE__, exit, Namespace,
         ENSURE, ENSURE_EQUAL, spark*/

use("UnitTest.js");
use("spark.js");

// Loader

function LoaderUnitTest() {
    this.Constructor(this, "LoaderUnitTest");
}

LoaderUnitTest.prototype.Constructor = function (obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);
    
    function testLoadString() {
        spark.myComponentClass = spark.ComponentClass("myComponentClass");
        spark.myComponentClass.Constructor = function (Protected) {
            var Public = this;
            this.Inherit(spark.Component); // the simplest ComponentClass has to inherit from Component
        };
        
        var myString = "<myComponentClass foo='bar'><a></a></myComponentClass>";
        obj.myComponentInstance = spark.loadString(myString);
        ENSURE_EQUAL("[myComponentClass without a name]", obj.myComponentInstance.toString(), "ensure default string repr without name being set");
        obj.myComponentInstance.name = "myRoot";
        ENSURE_EQUAL("[myComponentClass named myRoot]", obj.myComponentInstance.toString(), "ensure default string repr with name being set");
        ENSURE_EQUAL(false, ("foo" in obj.myComponentInstance), "xml attributes are not automatically converted to members");
        
        spark.myComponentClass.Constructor = function (Protected) {
            var Public = this;
            var Base = {};
            this.Inherit(spark.Component); // the simplest ComponentClass has to inherit from Component
            
            Base.realize = Public.realize;
            Public.realize = function () {
                Base.realize();
                this.foo = Protected.getString("foo", null);
            };
        };
        obj.myComponentInstance = spark.loadString(myString);
        ENSURE_EQUAL('bar', obj.myComponentInstance.foo, "specified xml attr was realized");
    }
    
    function testLoadFromFile() {
        spark.myComponentClass = spark.ComponentClass("myComponentClass");
        spark.myComponentClass.Constructor = function (Protected) {
            var Public = this;
            var Base = {};
            this.Inherit(spark.Component); // the simplest ComponentClass has to inherit from Component
            
            Base.realize = Public.realize;
            Public.realize = function () {
                Base.realize();
                this.foo = Protected.getString("foo", null);
            };
        };
        obj.myComponentInstance = spark.loadFile("tests/fixtures/myLoaderFixture_1.xml");
        ENSURE_EQUAL('hallo', obj.myComponentInstance.foo, "specified xml attr was realized");
    }
    
    function testContainerFromFile() {
        spark.myComponentClass = spark.ComponentClass("myComponentClass");
        spark.myComponentClass.Constructor = function (Protected) {
            var Public = this;
            var Base = {};
            this.Inherit(spark.Container);
            
            Base.realize = Public.realize;
            Public.realize = function () {
                Base.realize();
                this.foo = Protected.getString("foo", null);
            };
        };
        obj.myComponentInstance = spark.loadFile("tests/fixtures/myLoaderFixture_2.xml");
        ENSURE_EQUAL('hallo', obj.myComponentInstance.foo, "specified xml attr was realized");
        ENSURE_EQUAL(2, obj.myComponentInstance.children.length, "has one child");
        
        ENSURE_EQUAL('grube', obj.myComponentInstance.children[0].foo, "child has xml attr realized properly");
        ENSURE_EQUAL(null, obj.myComponentInstance.getChildByName("unknown"), "unknown child-name gets null")
        
        var myTmpChild = obj.myComponentInstance.getChildByName("tief");
        ENSURE_EQUAL(1, myTmpChild.children.length, "Child of root has one child");
        var myTmpChildChild = myTmpChild.getChildByName("hoch");
        ENSURE_EQUAL('haufen', myTmpChildChild.foo, "specified xml attr was realized");
        ENSURE_EQUAL(true, (myTmpChildChild.root === obj.myComponentInstance), "root of grandchild is correctly resolved");
        myTmpChild.removeChild(myTmpChildChild);
        ENSURE_EQUAL(0, myTmpChild.children.length, "remove specific child by reference");
        obj.myComponentInstance.removeAllChildren();
        ENSURE_EQUAL(0, obj.myComponentInstance.children.length, "has no children after removeAllChildren");
        
        obj.myComponentInstance.addChild(myTmpChildChild);
        ENSURE_EQUAL(1, obj.myComponentInstance.children.length, "has no children after removeAllChildren");
        ENSURE_EQUAL('haufen', obj.myComponentInstance.children[0].foo, "specified xml attr was realized");
        
        // potentially test adding the same instance-reference multiple times as child - is this a real-world use-case?
    }
    
    obj.run = function () {
        testLoadString();
        testLoadFromFile();
        testContainerFromFile();
    };
};

// testSuite

try {
    // if (operatingSystem() === 'OSX') {}
    var myScriptPathSegments = __FILE__().split("/");
    var mySuiteName = myScriptPathSegments[myScriptPathSegments.length - 1];
    var mySuite = new UnitTestSuite(mySuiteName);
    
    mySuite.addTest(new LoaderUnitTest());
    mySuite.run();
    
    print(">> Finished test suite '" + mySuiteName + "', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}