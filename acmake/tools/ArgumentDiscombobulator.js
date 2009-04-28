// This dirty little script takes the arguments to the current WSH
// and discombobulates them in a manner that allows passing strings
// containing spaces through NSIS.
//
// This is done by means of %-separation.
//

var arguments = [];
var currentArgument = "";
for(var i = 0; i < WScript.Arguments.Count(); i++) {
    var a = WScript.Arguments.Item(i);
    if(a == "%") {
        if(currentArgument != "") {
            arguments.push(currentArgument);
            currentArgument = "";
        }
    } else {
        var s = " ";
        if(currentArgument == "") {
            s = "";
        }
        currentArgument = currentArgument + s + a;
    }
}
if(currentArgument != "") {
    arguments.push(currentArgument);
}
