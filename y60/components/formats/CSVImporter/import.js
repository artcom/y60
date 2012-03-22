plug("CSV");

var printArray = function (theArray) {
    for (var i = 0, l = theArray.length; i < l; ++i) {
        var myRow = "";
        for (var j = 0, lj = theArray[i].length; j < lj; ++j) {
            myRow += theArray[i][j] + " | ";
        }
        print(myRow + "\n");
    }
};

var _myArray = CSV.Importer.csv2array("simple.csv");
printArray(_myArray);
print("-------------");

_myArray = CSV.Importer.csv2array("other_delimiter.csv",'~');
printArray(_myArray);


