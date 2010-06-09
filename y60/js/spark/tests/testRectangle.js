/*jslint nomen: false*/
/*global print, use, UnitTest, UnitTestSuite, __FILE__, exit, Namespace,
         ENSURE, ENSURE_EQUAL, spark, Vector2f, Vector3f, Node*/

use("UnitTest.js");
this.useSparkWidgets = true;
use("spark.js");

// Widget

function RectangleUnitTest() {
    this.Constructor(this, "RectangleUnitTest");
}

RectangleUnitTest.prototype.Constructor = function (obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);
    
    function testRectangleBasics() {
        obj.App = spark.loadFile("tests/fixtures/simpleRectangleFixture.spark");
        ENSURE_EQUAL(1, obj.App.children.length, "has one child");
        var myRectangle = obj.App.getChildByName("testRectangle");
        ENSURE_EQUAL("testRectangle", myRectangle.name, "Rectangle has correct name");
        ENSURE_EQUAL("[1,0.5,0,1]", myRectangle.color.toString(), "Rectangle has correct correctColor");
        ENSURE_EQUAL(50, myRectangle.x, "correct x");
        ENSURE_EQUAL(40, myRectangle.y, "correct y");
        ENSURE_EQUAL(0, myRectangle.z, "correct z");
        ENSURE_EQUAL(300, myRectangle.width, "correct width");
        ENSURE_EQUAL(200, myRectangle.height, "correct height");
        ENSURE_EQUAL(true, myRectangle instanceof spark.Rectangle, "Constructor is properly set and type of instance can be determined");
        ENSURE_EQUAL("Rectangle", myRectangle._className_, "_className_ is set according to spark conventions");
        ENSURE_EQUAL(spark.ResizableRectangle, myRectangle._classes_.ResizableRectangle, "is derived from ResizableRectangle");
        ENSURE_EQUAL(spark.Body, myRectangle._classes_.Body, "is derived from Body");
        ENSURE_EQUAL(spark.Widget, myRectangle._classes_.Widget, "is derived from Widget");
        ENSURE_EQUAL(spark.Container, myRectangle._classes_.Container, "is derived from Container");
        ENSURE_EQUAL(spark.Component, myRectangle._classes_.Component, "is derived from Component");
        ENSURE_EQUAL(spark.EventDispatcher, myRectangle._classes_.EventDispatcher, "is derived from EventDispatcher");
        // sadly the actual Inheritance chain cannot be seen directly as Inherit() only fills an object
        ENSURE_EQUAL("[300,200]", myRectangle.size.toString(), "has size");
        ENSURE_EQUAL(true, myRectangle.size instanceof Vector2f, "and size is Vector2f");
        ENSURE_EQUAL(obj.App, myRectangle.parent, "app is parent");
        ENSURE_EQUAL(obj.App, myRectangle.root, "and root");
        //alpha
        ENSURE_EQUAL(obj.App.actualAlpha, myRectangle.parentAlpha, "parent alpha is alpha of app");
        ENSURE_EQUAL(1.0, myRectangle.actualAlpha, "actualAlpha is correctly set");
        ENSURE_EQUAL(1.0, myRectangle.alpha, "also known as alpha (as Property)");
        
        //position
        ENSURE_EQUAL("[50,40,0]", myRectangle.position.toString(), "has position");
        ENSURE_EQUAL(true, myRectangle.position instanceof Vector3f, "and position is Vector3f");
        ENSURE_EQUAL(true, myRectangle.position instanceof Vector3f, "and position is Vector3f");
        
        // rotation (X/Y/Z)
        ENSURE_EQUAL("[0,0,0]", myRectangle.rotation.toString(), "proper rotation (as Property)");
        ENSURE_EQUAL(0.0, myRectangle.rotationX, "rotationX is correct");
        ENSURE_EQUAL(0.0, myRectangle.rotationY, "rotationY is correct");
        ENSURE_EQUAL(0.0, myRectangle.rotationZ, "rotationZ is correct");
        
        //origin (X/Y/Z)
        ENSURE_EQUAL("[0,0,0]", myRectangle.origin.toString(), "proper origin (as Property)");
        ENSURE_EQUAL(0.0, myRectangle.originX, "originX is correct");
        ENSURE_EQUAL(0.0, myRectangle.originY, "originY is correct");
        ENSURE_EQUAL(0.0, myRectangle.originZ, "originZ is correct");
        
        // pivot (X/Y/Z)
        ENSURE_EQUAL("[0,0,0]", myRectangle.pivot.toString(), "proper pivot (as Property)");
        ENSURE_EQUAL(0.0, myRectangle.pivotX, "pivotX is correct");
        ENSURE_EQUAL(0.0, myRectangle.pivotY, "pivotY is correct");
        ENSURE_EQUAL(0.0, myRectangle.pivotZ, "pivotZ is correct");

        //scale (X/Y/Z)
        ENSURE_EQUAL("[1,1,1]", myRectangle.scale.toString(), "proper scale (as Property)");
        ENSURE_EQUAL(1.0, myRectangle.scaleX, "scaleX is correct");
        ENSURE_EQUAL(1.0, myRectangle.scaleY, "scaleY is correct");
        ENSURE_EQUAL(1.0, myRectangle.scaleZ, "scaleZ is correct");

        //node
        ENSURE_EQUAL(true, myRectangle.node instanceof Node, "node is a Node");
        // could make more assertions here but that would basically just test Node

        //visible, sensible, vocation
        ENSURE_EQUAL(true, myRectangle.visible, "correct visibility");
        ENSURE_EQUAL(true, myRectangle.sensible, "correct sensibility");
        ENSURE_EQUAL("named testRectangle", myRectangle.vocation, "correct vocation");
        
        //stage
        
        // worldScale, worldPosition, worldRotation, worldBounds, worldWidth
        // worldHeight, worldDepth
    }
    
    function testRectangleTransformations() {
        obj.App = spark.loadFile("tests/fixtures/simpleRectangleFixture.spark");
        var myRectangle = obj.App.getChildByName("testRectangle");
        
        // rotation
        myRectangle.rotationX = 1.5;
        myRectangle.rotationY = 0.75;
        myRectangle.rotationZ = -0.5;
        ENSURE_EQUAL(1.5, myRectangle.rotationX, "rotationX is correct");
        ENSURE_EQUAL(0.75, myRectangle.rotationY, "rotationY is correct");
        ENSURE_EQUAL(-0.5, myRectangle.rotationZ, "rotationZ is correct");
        ENSURE_EQUAL("[1.5,0.75,-0.5]", myRectangle.rotation.toString(), "rotation Vector is correctly transformed");
        
        // scale
        myRectangle.scaleX = 2.5;
        myRectangle.scaleY = 1.75;
        myRectangle.scaleZ = -1.5;
        ENSURE_EQUAL(2.5,  myRectangle.scaleX, "scaleX is correct");
        ENSURE_EQUAL(1.75, myRectangle.scaleY, "scaleY is correct");
        ENSURE_EQUAL(-1.5, myRectangle.scaleZ, "scaleZ is correct");
        ENSURE_EQUAL("[2.5,1.75,-1.5]", myRectangle.scale.toString(), "scale Vector is correctly transformed");
        
        // position
        myRectangle.position = new Vector3f(0.25, 0.4, 0.33);
        ENSURE_EQUAL("[0.25,0.4,0.33]", myRectangle.position.toString(), "position Vector is correctly transformed");
        myRectangle.x = -0.33;
        myRectangle.y = 0.44;
        myRectangle.z = 3.1415;
        ENSURE_EQUAL(-0.33,  myRectangle.x, "x is correct");
        ENSURE_EQUAL(0.44, myRectangle.y, "y is correct");
        ENSURE_EQUAL(3.1415, myRectangle.z, "z is correct");
        ENSURE_EQUAL("[-0.33,0.44,3.1415]", myRectangle.position.toString(), "position Vector is correctly transformed");
        
        // pivot
        myRectangle.pivotX = 1.5;
        myRectangle.pivotY = 2.75;
        myRectangle.pivotZ = -4.5;
        ENSURE_EQUAL(1.5,  myRectangle.pivotX, "pivotX is correct");
        ENSURE_EQUAL(2.75, myRectangle.pivotY, "pivotY is correct");
        ENSURE_EQUAL(-4.5, myRectangle.pivotZ, "pivotZ is correct");
        ENSURE_EQUAL("[1.5,2.75,-4.5]", myRectangle.pivot.toString(), "pivot Vector is correctly transformed");
        
        // origin
        myRectangle.originX = 0.5;
        myRectangle.originY = 0.75;
        myRectangle.originZ = -0.5;
        ENSURE_EQUAL(0.5,  myRectangle.originX, "originX is correct");
        ENSURE_EQUAL(0.75, myRectangle.originY, "originY is correct");
        ENSURE_EQUAL(-0.5, myRectangle.originZ, "originZ is correct");
        ENSURE_EQUAL("[0.5,0.75,-0.5]", myRectangle.origin.toString(), "origin Vector is correctly transformed");
    }
    
    obj.run = function () { 
        testRectangleBasics();
        testRectangleTransformations();
    };
};

// testSuite

try {
    // if (operatingSystem() === 'OSX') {}
    var myScriptPathSegments = __FILE__().split("/");
    var mySuiteName = myScriptPathSegments[myScriptPathSegments.length - 1];
    var mySuite = new UnitTestSuite(mySuiteName);
    
    mySuite.addTest(new RectangleUnitTest());
    mySuite.run();
    
    print(">> Finished test suite '" + mySuiteName + "', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}