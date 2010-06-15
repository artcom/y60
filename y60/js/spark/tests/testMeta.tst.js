/*jslint nomen: false*/
/*global print, use, UnitTest, UnitTestSuite, __FILE__, exit, Namespace,
         ENSURE, ENSURE_EQUAL, spark*/

use("UnitTest.js");
use("spark/spark.js");

// ComponentClass

function ComponentClassUnitTest() {
    this.Constructor(this, "ComponentClassUnitTest");
}

ComponentClassUnitTest.prototype.Constructor = function (obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);
    
    function testBasics() {
        ENSURE_EQUAL(true, "componentClasses" in spark, "spark namespace has member called 'componentClasses'");
        ENSURE_EQUAL(true, "ComponentClass" in spark, "spark namespace has member called 'ComponentClass'");
        ENSURE_EQUAL(true, "LoadedClass" in spark, "spark namespace has member called 'LoadedClass'");
        
        spark.myComponentClass = spark.ComponentClass("myComponentClass");
        spark.myComponentClass.Constructor = function (Protected) {
            var Public = this;
            var Base = {};
            
            this.Inherit(spark.Component); // the simplest ComponentClass has to inherit from Component
            
            this.foo = "bar";
            
            Base.realize = Public.realize;
            Public.realize = function () {
                Base.realize();
                this.foo = Protected.getString("foo", this.foo);
            };
        };
        
        ENSURE_EQUAL(true, "myComponentClass" in spark, "new component class in spark namespace");
        ENSURE_EQUAL("myComponentClass", spark.myComponentClass._className_, "new component class' name is set properly");
        ENSURE_EQUAL(spark.componentClasses.myComponentClass, spark.myComponentClass, "component class is in registry");
        
        var myComponentClassInstance = spark.loadString("<myComponentClass foo='blubb'></myComponentClass>");
        ENSURE_EQUAL(null, myComponentClassInstance.parent, "parent is null");
        ENSURE_EQUAL(myComponentClassInstance, myComponentClassInstance.root, "instance is its own root");
        ENSURE_EQUAL("blubb", myComponentClassInstance.foo, "member is set via realize from xml attr");
    }
    
    obj.run = function () {
        testBasics();
    };
};

// testSuite

try {
    // if (operatingSystem() === 'OSX') {}
    var myScriptPathSegments = __FILE__().split("/");
    var mySuiteName = myScriptPathSegments[myScriptPathSegments.length - 1];
    var mySuite = new UnitTestSuite(mySuiteName);
    
    mySuite.addTest(new ComponentClassUnitTest());
    mySuite.run();
    
    print(">> Finished test suite '" + mySuiteName + "', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}