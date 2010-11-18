plug("CSVImporter");

var _myCSVImporter = new CSVImporter();
var _myArray = _myCSVImporter.csv2array("simple.csv");

for (var i = 0, l = _myArray.length; i < l; ++i) {
    var myRow = "";
    for (var j = 0, lj = _myArray[i].length; j < lj; ++j) {
        myRow += _myArray[i][j] + " | ";
    }
    print(myRow + "\n");
}

