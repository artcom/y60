/*jslint nomen: false*/
/*global print, use, UnitTest, UnitTestSuite, __FILE__, exit, Namespace,
         ENSURE, ENSURE_EQUAL, spark, Vector2f, Vector3f, Node*/

use("UnitTest.js");
use("spark/spark.js");

// Widget

function SliderUnitTest() {
    this.Constructor(this, "SliderUnitTest");
}

SliderUnitTest.prototype.Constructor = function (obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);

    function testSliderBasics() {
        obj.App = spark.loadFile("fixtures/simpleSliderFixture.spark");
        ENSURE_EQUAL(1, obj.App.children.length, "has one child");
        var mySlider = obj.App.getChildByName("simple-slider");
        ENSURE_EQUAL(true, mySlider.centered, "slider centered default: true");
        mySlider.centered = false;
        ENSURE_EQUAL(false, mySlider.centered, "slider centered set to false");
        ENSURE_EQUAL(true, mySlider.sticky, "slider sticky default: true");
        mySlider.sticky = false;
        ENSURE_EQUAL(false, mySlider.sticky, "slider sticky set to false");
        ENSURE(!!mySlider.activeCursor, "active cursor exists");
        ENSURE(!!mySlider.idleCursor, "idle cursor exists");
        ENSURE_EQUAL(false, mySlider.verticalLock, "vertical Lock default: false");
        mySlider.verticalLock = true;
        ENSURE_EQUAL(true, mySlider.verticalLock, "vertical Lock set to true");
        ENSURE_EQUAL(false, mySlider.horizontalLock, "horizontal Lock default: false");
        mySlider.horizontalLock = true;
        ENSURE_EQUAL(true, mySlider.horizontalLock, "horizontal Lock set to true");
    }

    function testSliderAttributes() {
        obj.App = spark.loadFile("fixtures/lockedSliderFixture.spark");
        ENSURE_EQUAL(1, obj.App.children.length, "has one child");
        var mySlider = obj.App.getChildByName("simple-slider");
        ENSURE_EQUAL(false, mySlider.centered, "slider centered  false");
        ENSURE_EQUAL(false, mySlider.sticky, "slider sticky false");
        ENSURE_EQUAL(true, mySlider.verticalLock, "vertical Lock true");
        ENSURE_EQUAL(true, mySlider.horizontalLock, "horizontal Lock  true");
    }

    function testSliderEvents() {
        obj.App = spark.loadFile("fixtures/sliderFixture.spark");
        ENSURE_EQUAL(3, obj.App.children.length, "has one child");
        var mySlider = obj.App.getChildByName("simple-slider");
        mySlider.eventTarget = obj.App;
        function assertEvent0(theEvent) {
            ENSURE_EQUAL(0, theEvent.sliderX, "relative slider x positon 0");
            ENSURE_EQUAL(1, theEvent.sliderY, "relative slider y positon 1");
        };
        obj.App.addEventListener(spark.SliderEvent.START, assertEvent0);
        var myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 0,0);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent0);

       function assertEvent1(theEvent) {
            ENSURE(true, "slider start event was triggered");
            ENSURE_EQUAL(1, theEvent.sliderX, "relative slider x positon 1");
            ENSURE_EQUAL(0, theEvent.sliderY, "relative slider y positon 0");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent1);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 900,600);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent1);


        function assertEvent2(theEvent) {
            ENSURE_EQUAL(0.5, theEvent.sliderX, "relative slider x positon 0.5");
            ENSURE_EQUAL(0.5, theEvent.sliderY, "relative slider y positon 0.5");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent2);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 225,450);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent2);

        function assertEvent3(theEvent) {
            ENSURE_EQUAL(1, theEvent.sliderX, "relative slider x positon 1");
            ENSURE_EQUAL(0.25, theEvent.sliderY, "relative slider y positon 0.25");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent3);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 500,512.5);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent3);

        function assertEvent4(theEvent) {
            ENSURE_EQUAL(0.25, theEvent.sliderX, "relative slider x positon 0.25");
            ENSURE_EQUAL(0.75, theEvent.sliderY, "relative slider y positon 0.75");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent4);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 125,387.5);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent4);

        function assertEventMove(theEvent) {
            ENSURE(true, "slider move event was triggered");
            ENSURE_EQUAL(0.25, theEvent.sliderX, "relative slider x positon 0.25");
            ENSURE_EQUAL(0.75, theEvent.sliderY, "relative slider y positon 0.75");
        };

        obj.App.addEventListener(spark.SliderEvent.MOVE, assertEventMove);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.MOVE, 125,387.5);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.MOVE, assertEventMove);

        function assertEventStop(theEvent) {
            ENSURE(true, "slider stop event was triggered");
            ENSURE_EQUAL(0.25, theEvent.sliderX, "relative slider x positon 0.25");
            ENSURE_EQUAL(0.75, theEvent.sliderY, "relative slider y positon 0.75");
        };

        obj.App.addEventListener(spark.SliderEvent.STOP, assertEventStop);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_UP, 125,387.5);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.STOP, assertEventStop);
    }

    function testHorizontalLockedSliderEvents() {
        obj.App = spark.loadFile("fixtures/sliderFixture.spark");
        ENSURE_EQUAL(3, obj.App.children.length, "has one child");
        var mySlider = obj.App.getChildByName("horizontal-locked-slider");
        mySlider.eventTarget = obj.App;
        function assertEvent0(theEvent) {
            ENSURE_EQUAL(0.5, theEvent.sliderX, "relative slider x positon 0.5");
            ENSURE_EQUAL(1, theEvent.sliderY, "relative slider y positon 1");
        };
        obj.App.addEventListener(spark.SliderEvent.START, assertEvent0);
        var myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 0,0);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent0);

       function assertEvent1(theEvent) {
            ENSURE_EQUAL(0.5, theEvent.sliderX, "relative slider x positon 0.5");
            ENSURE_EQUAL(0, theEvent.sliderY, "relative slider y positon 0");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent1);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 900,600);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent1);


        function assertEvent2(theEvent) {
            ENSURE_EQUAL(0.5, theEvent.sliderX, "relative slider x positon 0.5");
            ENSURE_EQUAL(0.5, theEvent.sliderY, "relative slider y positon 0.5");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent2);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 225,450);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent2);

        function assertEvent3(theEvent) {
            ENSURE_EQUAL(0.5, theEvent.sliderX, "relative slider x positon 0.5");
            ENSURE_EQUAL(0.25, theEvent.sliderY, "relative slider y positon 0.25");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent3);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 500,512.5);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent3);

        function assertEvent4(theEvent) {
            ENSURE_EQUAL(0.5, theEvent.sliderX, "relative slider x positon 0.5");
            ENSURE_EQUAL(0.75, theEvent.sliderY, "relative slider y positon 0.75");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent4);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 125,387.5);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent4);
    }

    function testVerticalLockedSliderEvents() {
        obj.App = spark.loadFile("fixtures/sliderFixture.spark");
        ENSURE_EQUAL(3, obj.App.children.length, "has one child");
        var mySlider = obj.App.getChildByName("vertical-locked-slider");
        mySlider.eventTarget = obj.App;
        function assertEvent0(theEvent) {
            ENSURE_EQUAL(0, theEvent.sliderX, "relative slider x positon 0");
            ENSURE_EQUAL(0.5, theEvent.sliderY, "relative slider y positon 0.5");
        };
        obj.App.addEventListener(spark.SliderEvent.START, assertEvent0);
        var myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 0,0);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent0);

       function assertEvent1(theEvent) {
            ENSURE(true, "slider start event was triggered");
            ENSURE_EQUAL(1, theEvent.sliderX, "relative slider x positon 1");
            ENSURE_EQUAL(0.5, theEvent.sliderY, "relative slider y positon 0.5");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent1);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 900,600);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent1);


        function assertEvent2(theEvent) {
            ENSURE_EQUAL(0.5, theEvent.sliderX, "relative slider x positon 0.5");
            ENSURE_EQUAL(0.5, theEvent.sliderY, "relative slider y positon 0.5");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent2);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 225,450);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent2);

        function assertEvent3(theEvent) {
            ENSURE_EQUAL(1, theEvent.sliderX, "relative slider x positon 1");
            ENSURE_EQUAL(0.5, theEvent.sliderY, "relative slider y positon 0.5");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent3);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 500,512.5);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent3);

        function assertEvent4(theEvent) {
            ENSURE_EQUAL(0.25, theEvent.sliderX, "relative slider x positon 0.25");
            ENSURE_EQUAL(0.5, theEvent.sliderY, "relative slider y positon 0.5");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEvent4);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 125,387.5);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEvent4);

        function assertEventMove(theEvent) {
            ENSURE(true, "slider move event was triggered");
            ENSURE_EQUAL(0.25, theEvent.sliderX, "relative slider x positon 0.25");
            ENSURE_EQUAL(0.5, theEvent.sliderY, "relative slider y positon 0.5");
        };

        obj.App.addEventListener(spark.SliderEvent.MOVE, assertEventMove);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.MOVE, 125,387.5);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.MOVE, assertEventMove);

        function assertEventStop(theEvent) {
            ENSURE(true, "slider stop event was triggered");
            ENSURE_EQUAL(0.25, theEvent.sliderX, "relative slider x positon 0.25");
            ENSURE_EQUAL(0.5, theEvent.sliderY, "relative slider y positon 0.5");
        };

        obj.App.addEventListener(spark.SliderEvent.STOP, assertEventStop);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_UP, 125,387.5);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.STOP, assertEventStop);
    }


    function testStickiness() {
        obj.App = spark.loadFile("fixtures/sliderFixture.spark");
        ENSURE_EQUAL(3, obj.App.children.length, "has one child");
        var mySlider = obj.App.getChildByName("simple-slider");
        mySlider.eventTarget = obj.App;
        var myIdleCursor = mySlider.getChildByName("background").getChildByName("idle-cursor");
        var myActiveCursor = mySlider.getChildByName("background").getChildByName("active-cursor");

        ENSURE_EQUAL(true, mySlider.sticky, "slider sticky");
        ENSURE_EQUAL(true, mySlider.centered, "slider centered");
        // slider sticky and centered
        function assertEventStart(theEvent) {
            ENSURE_EQUAL(400, myActiveCursor.position.x, "slider jumping to clicked position x ");
            ENSURE_EQUAL(225, myActiveCursor.position.y, "slider jumping to clicked position y ");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEventStart);
        var myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 700, 350);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEventStart);


        function assertEventStop(theEvent) {
            ENSURE_EQUAL(400, myActiveCursor.position.x, "slider stays there x");
            ENSURE_EQUAL(225, myActiveCursor.position.y, "slider stays there y");
        };

        obj.App.addEventListener(spark.SliderEvent.STOP, assertEventStop);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_UP, 700, 350);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.STOP, assertEventStop);


        // slider not sticky but centered
        mySlider.sticky = false;
        ENSURE_EQUAL(false, mySlider.sticky, "slider not sticky");
        ENSURE_EQUAL(true, mySlider.centered, "slider centered");

        function assertEventStart2(theEvent) {
            ENSURE_EQUAL(400, myActiveCursor.position.x, "slider jumping to clicked position x ");
            ENSURE_EQUAL(225, myActiveCursor.position.y, "slider jumping to clicked position y ");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEventStart2);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 700, 350);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEventStart2);


        function assertEventStop2(theEvent) {
            ENSURE_EQUAL(200, myIdleCursor.position.x, "slider jumping to center Position x ");
            ENSURE_EQUAL(125, myIdleCursor.position.y, "slider jumping to center Position y ");
        };

        obj.App.addEventListener(spark.SliderEvent.STOP, assertEventStop2);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_UP, 700, 350);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.STOP, assertEventStop2);

        // slider not sticky and not centered
        mySlider.sticky = false;
        mySlider.centered = false;
        ENSURE_EQUAL(false, mySlider.sticky, "slider not sticky");
        ENSURE_EQUAL(false, mySlider.centered, "slider not centered");

        function assertEventStart3(theEvent) {
            ENSURE_EQUAL(400, myActiveCursor.position.x, "slider jumping to clicked position x ");
            ENSURE_EQUAL(225, myActiveCursor.position.y, "slider jumping to clicked position y ");
        };

        obj.App.addEventListener(spark.SliderEvent.START, assertEventStart3);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, 700, 350);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.START, assertEventStart3);

        function assertEventStop3(theEvent) {
            ENSURE_EQUAL(0, myIdleCursor.position.x, "slider jumping to origin Position x ");
            ENSURE_EQUAL(0, myIdleCursor.position.y, "slider jumping to origin Position y ");
        };

        obj.App.addEventListener(spark.SliderEvent.STOP, assertEventStop3);
        myMouseEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_UP, 700, 350);
        mySlider.dispatchEvent(myMouseEvent);
        obj.App.removeEventListener(spark.SliderEvent.STOP, assertEventStop3);
    }

    obj.run = function () {
       testSliderBasics();
       testSliderAttributes();
       testSliderEvents();
       testHorizontalLockedSliderEvents();
       testVerticalLockedSliderEvents();
       testStickiness();
    };
};

// testSuite

try {
    // if (operatingSystem() === 'OSX') {}
    var myScriptPathSegments = __FILE__().split("/");
    var mySuiteName = myScriptPathSegments[myScriptPathSegments.length - 1];
    var mySuite = new UnitTestSuite(mySuiteName);

    mySuite.addTest(new SliderUnitTest());
    mySuite.run();

    print(">> Finished test suite '" + mySuiteName + "', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}
