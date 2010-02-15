
use("UnitTest.js");
use("Hoccer.js");
use("GoodParts.js");

function HoccerUnitTest() {
    this.Constructor(this, "HoccerUnitTest");
};

HoccerUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function testDefaults() {
        DTITLE("testing default settings");

        var testStation = Object.beget(Hoccer.station);
        ENSURE_EQUAL(80, testStation.accuracy, "accuracy");
    }

    function testThrowing() {
        DTITLE("test throwing");

        var testStation = Object.beget(Hoccer.station);
        testStation.latitude += 1;
        var isDone = false;
        testStation.buildPeerGroup({
            onDone : function(){
                isDone = true;
            }});
        ENSURE_WITHIN_TIMEOUT(function(){
                testStation.update();
                return isDone;
            }, 2000, "creating peer group");

        var isError = false;
        testStation.buildPeerGroup({
            onError : function(){
                isError = true;
            }});
        ENSURE_WITHIN_TIMEOUT(function(){
                testStation.update();
                return isError;
            }, 2000, "creating peer group");


    }

    obj.run = function() {
        testDefaults();
        testThrowing();
    }
}

var myTestName = "testHoccer.js";
var mySuite = new UnitTestSuite( myTestName );

mySuite.addTest( new HoccerUnitTest() );
mySuite.run();

print( ">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "" );
exit( mySuite.returnStatus() );

