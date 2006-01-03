

use("BSpline.js");

use("UnitTest.js");

function MyBSplineTest() {
    this.Constructor(this, "MyBSplineTest");
};


MyBSplineTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {

        var mySpline = new BSpline();
        var myPoints = [new Vector3f(0,0,0), new Vector3f(1,1,0),
            new Vector3f(3,1,0), new Vector3f(4,0,0)];

        mySpline.setupFromPoints(myPoints, 2);
        obj.myArcLength = mySpline.getArcLength();

        obj.mySumDistance = 0;
        var n = 999;
        for(var i = 0;i < n-1; ++i) {
            var myP1 = mySpline.evaluate(i/n);
            var myP2 = mySpline.evaluate((i+1)/n);
            obj.mySumDistance += distance(myP1,myP2);
        }
        print(obj.mySumDistance + " vs " + obj.myArcLength);
        ENSURE('Math.abs(obj.mySumDistance - obj.myArcLength) < 0.01');
    }
};

var myTestName = "testBSpline.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new MyBSplineTest());
mySuite.run();

exit(mySuite.returnStatus());
