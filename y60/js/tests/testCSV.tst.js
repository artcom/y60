
use('UnitTest.js');
use('csv/CSV.js');

function CSVUnitTest() {
  this.Constructor(this, "CSVUnitTest");
}

CSVUnitTest.prototype.Constructor = function(obj, name) {

  UnitTest.prototype.Constructor(obj, name);

  function run_csv_parse_test(file, is_inavlid) {
    obj.csv_string = readFileAsString(file);
    if (is_inavlid) {
      ENSURE_EXCEPTION("CSV.parse(obj.csv_string)");
    } else {
      CSV.parse(obj.csv_string);
      SUCCESS("parse did not throw");
    }
  }

  obj.run = function() {
    var fixture_dir = getDirectoryPart(__FILE__()) + "/fixtures/csv";
    var fixtures = getDirectoryEntries(fixture_dir);
    for (var i = 0; i < fixtures.length; ++i) {
      if (fixtures[i].match(/\.csv$/)) {
        var is_inavlid = fixtures[i].match(/\.invalid\./) ? true : false;
        run_csv_parse_test(fixture_dir + "/" + fixtures[i], is_inavlid);
      }
    }

    obj.result = CSV.parse("foo;bar;baz\nnarf;barf;snarf\n");
    ENSURE("obj.result.length === 2")
    ENSURE("obj.result[0].length === 3")
    ENSURE("obj.result[0][0] === 'foo'")
    ENSURE("obj.result[0][1] === 'bar'")
    ENSURE("obj.result[0][2] === 'baz'")
    ENSURE("obj.result[1].length === 3")
    ENSURE("obj.result[1][0] === 'narf'")
    ENSURE("obj.result[1][1] === 'barf'")
    ENSURE("obj.result[1][2] === 'snarf'")
  }
}

var myTestName = "testCSV.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new CSVUnitTest());
mySuite.run();

print(">> Finished test suite '" + myTestName + "', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());

