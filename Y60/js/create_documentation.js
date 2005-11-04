//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: create_documentation.js,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/28 17:12:57 $
//
//
//=============================================================================

use("Exception.js");
use("SyntaxHighlighter.js");

const VALIDATE_HTML = false;

if (!("0" in arguments)) {
    print("Usage: create_documentation.js outputdir");
    exit(1);
}

var ourDirectory = arguments[0];
var ourTutorialIndex     = [];
var ourSyntaxHighlighter = new SyntaxHighlighter();

main();

function main() {
    try {
        if (fileExists(ourDirectory) && !isDirectory(ourDirectory)) {
            print("### ERROR: " + ourDirectory + " is a file, not a directory.");
            exit(1);
        }

        createTutorials();
        generateJSLibDocumentation(ourDirectory);
        createStyleSheet();
        print("Successfully created documentation in " + ourDirectory);
    } catch (ex) {
        reportException(ex);
        exit(1);
    }
}

// This function makes sure, some well-known modules come first
function getModuleNames() {
    var myModuleNames = ["Global", "Math", "GlobalFunctions", "MathFunctions"];
    var myModuleNames2 = getDocumentedModules();
    for (var i = 0; i < myModuleNames2.length; ++i) {
        var myNewModuleFlag = true;
        for (var j = 0; j < myModuleNames.length; ++j) {
            if (myModuleNames2[i] == myModuleNames[j]) {
                myNewModuleFlag = false;
            }
        }
        if (myNewModuleFlag) {
            myModuleNames.push(myModuleNames2[i]);
        }
    }
    return myModuleNames;
}

function generateJSLibDocumentation() {
    var myModules = [];
    var myModuleNames = getModuleNames();
    for (var i = 0; i < myModuleNames.length; ++i) {
        myModules[myModuleNames[i]] = getDocumentation(myModuleNames[i]);
    }

    createIndex(myModules);

    for (var myModuleName in myModules) {
        for (var myClassName in myModules[myModuleName]) {
            if (myClassName == "") {
                // Global functions
                var myFunctions = myModules[myModuleName][""].functions;
                createFunctionDocumentation(myModuleName, null, myFunctions);
            } else {
                // Classes
                createClassDocumentation(myModules, myModuleName, myClassName);
            }
        }
    }
}

function createIndex(theModules) {
    var myString = "<table class='IndexTable'><tr><td class='IndexTable'>";
    var myLineCount = 0;
    for (var myModuleName in theModules) {
        if (myLineCount > 10) {
            myString += "</td><td class='IndexTable'>";
            myLineCount = 0;
        }
        myString += "<p><b>" + myModuleName + "</b></p>\n";
        for (myClassName in theModules[myModuleName]) {
            if (myClassName != "") {
                myString += "<a href='" + myModuleName + "/" + myClassName + ".html'>" + myClassName + "</a><br/>\n";
                var myClass = theModules[myModuleName][myClassName];
                myLineCount++;
            } else {
                var theFunctions = theModules[myModuleName][myClassName].functions;
                for (var i = 0; i < theFunctions.length; ++i) {
                    var myName = theFunctions[i].name;
                    myString += "<a href='" + myModuleName + "/" + myName + ".html'>" + myName + "</a><br/>\n";
                    myLineCount++;
                }
            }
        }
    }

    myString += "</td></tr></table>";

    writeHTML("index.html", myString);
}

function createFunctionDocumentation(theModuleName, theClassName, theFunctions) {
    for (var i = 0; i < theFunctions.length; ++i) {
        var myName = theFunctions[i].name;
        var myPath = theModuleName + "/";
        if (theClassName) {
             myPath += theClassName + "_";
        }
        myPath += myName + ".html";
        writeHTML(myPath, documentFunction(theClassName, theFunctions[i]));
    }
}

function createClassDocumentation(theModules, theModuleName, theClassName) {
    print("Documenting class: " + theClassName);
    var myClass = theModules[theModuleName][theClassName];

    createFunctionDocumentation(theModuleName, theClassName, myClass.constructors);
    createFunctionDocumentation(theModuleName, theClassName, myClass.functions);
    createFunctionDocumentation(theModuleName, theClassName, myClass.static_functions);
    createClassSummary(theModules, theModuleName, theClassName);
}

function createClassSummary(theModules, theModuleName, theClassName) {
    var myString = "<h2>" + theClassName;
    var myClass = theModules[theModuleName][theClassName];

    if (myClass.base_class) {
        var myBaseClass = myClass.base_class;
        while (myBaseClass != "") {
            myString += " [<a href='" + myBaseClass + ".html'>"
                + myBaseClass + "</a>]";
            myBaseClass = theModules[theModuleName][myBaseClass].base_class;
        }
    }
    myString += '</h2>\n';
    myString += "<table class='SummaryTable'>\n";

    myString += summarizeFunctions(theClassName, "Constructors", myClass.constructors);
    myString += summarizeFunctions(theClassName, "Functions", myClass.functions);
    myString += summarizeFunctions(theClassName, "Static Functions", myClass.static_functions);

    myString += summarizeProperties("Properties", myClass.properties);
    myString += summarizeProperties("Static Properties", myClass.static_properties);
    myString += summarizeProperties("Constants", myClass.constants);

    myString += "</table>\n";

    writeHTML(theModuleName + "/" + theClassName + ".html", myString);
}

function summarizeFunctions(theClassName, theFunctionsTitle, theFunctions) {
    var myString = "";
    if (theFunctions.length) {
        myString += "<tr><td colspan='2' class='SummaryTableCell'><div class='SummeryHeader'>" + theFunctionsTitle + "</div></td></tr>\n";
        for (var i = 0; i < theFunctions.length; ++i) {
            var myFunction = theFunctions[i];
            myString += "<tr><td class='SummaryTableCell'><a href='" + theClassName + "_" + myFunction.name + ".html'>" + myFunction.name + "</a></td>\n";
            if (myFunction.description == "") {
                myFunction.description = "not yet documented";
            }
            myString += "<td class='SummaryTableCell'>" + myFunction.description + '</td></tr>\n';
        }
    }
    return myString;
}

function summarizeProperties(thePropertyTitle, theProperties) {
    var myString = "";
    if (theProperties.length) {
        myString += "<tr><td colspan='2' class='SummaryTableCell'><div class='SummeryHeader'>" + thePropertyTitle + "</div></td></tr>\n";
        for (var i = 0; i < theProperties.length; ++i) {
            myString += "<tr><td class='SummaryTableCell'>" + theProperties[i] + "</td>\n";
            myString += "<td class='SummaryTableCell'></td></tr>\n";
        }
    }
    return myString;
}

function writeHTML(theFileName, theString) {
    var myString = "<html><head>\n";
    myString += "<title>Y60 JavaScript Class Reference</title>\n";
    if (theFileName == "index.html") {
        myString += "    <link rel='StyleSheet' href='jsdoc.css' type='text/css'/>\n";
    } else {
        myString += "    <link rel='StyleSheet' href='../jsdoc.css' type='text/css'/>\n";
    }
    myString += "</head>\n"
    myString += "<body>\n";
    myString += theString;
    myString += "<div class='TimeStamp'>Created " + new Date() + " on " + hostname() + "</div>";
    myString += "</body></html>";

    if (VALIDATE_HTML) {
        var myXmlDoc = new Node(myString);
        if (!myXmlDoc.firstChild) {
            exit(1);
        }
    }

    var myPath = ourDirectory + "/" + theFileName;
    var myDir  = dirname(myPath);

    if (!isDirectory(myDir)) {
        makeDir(myDir);
    }

    if (!putWholeFile(myPath, myString)) {
        throw new Exception("Could not write file: " + myPath, fileline());
    }
}

function documentSignature(theName, theParameters, theReturnType) {
    var myString = "<div class='SectionHeader'>Syntax:</div>";
    myString += "<div class='Indent'>";

    var myReturnType = theReturnType ? theReturnType : "void";
    var myParameterOption = [];
    var myAltParameters   = [];
    var myJStart          = 0;
    for (var i = 0; i < theParameters.length; ++i) {
        if (theParameters[i].type == '__intern__') {
            myParameterOption = theParameters.slice(myJStart, i);
            myJStart = i + 1;
            myAltParameters.push(myParameterOption);
        }
    }
    myParameterOption = theParameters.slice(myJStart,i);
    myAltParameters.push(myParameterOption);

    for (i = 0; i < myAltParameters.length; ++i) {
        myString += "<span class='ParameterType'>" + myReturnType + "</span> <b>" + theName + "</b>(";
        for (var j = 0; j < myAltParameters[i].length; ++j) {
            var myParameter = myAltParameters[i][j];
            myString += "<span class='ParameterType'>" + myParameter.type + "</span> " + myParameter.name;
            if (myParameter.default_value) {
                myString += " = " + myParameter.default_value;
            }
            if (j < myAltParameters[i].length-1) {
                myString += ", ";
            }
        }
        myString += ")<br/>";
    }
    myString += "</div>\n";
    return myString;
}

function documentFunction(theClassName, theFunction) {
    var myString = "";

    // Description
    var myClass = theClassName ? theClassName + "::" : "";
    var myName  = myClass + theFunction.name;
    myString += "<h2>" + myName + "</h2>";

    if (theFunction.description == "") {
        theFunction.description = "Not yet documented.";
    }
    myString += "<div class='SectionHeader'>Description:</div>";
    myString += "<div class='FunctionDescription'>" + theFunction.description + '</div>\n';

    // Signature
    myString += documentSignature(theFunction.name, theFunction.parameters, theFunction.return_type);

    // Parameters
    if (theFunction.parameters.length) {
        myString += "<div class='SectionHeader'>Parameters:</div>";
        myString += "<div class='Indent'><table class='ParameterTable'>";
        for (var j = 0; j < theFunction.parameters.length; ++j) {
            var myParameter = theFunction.parameters[j];
            if (String(myParameter.type) != "__intern__") {
                if (myParameter.description == "") {
                    myParameter.description = "Not yet documented.";
                }
                myString += "<tr><td class='ParameterTableCell'>" + myParameter.name;
                myString += "</td><td class='ParameterTableCell'>" + myParameter.description;
                myString += "</td></tr>";
            }
        }
        myString += "</table></div>"
    }

    // Return value
    if (theFunction.return_value) {
        myString += "<div class='SectionHeader'>Return Value:</div>";
        myString += "<div class='Indent'>" + theFunction.return_value + "</div>";
    }

    // Tutorials
    if (myName in ourTutorialIndex) {
        var myPro = expandEnvironment("${PRO}");
        var myTutorials = ourTutorialIndex[myName];
        myString += "<div class='SectionHeader'>Tutorials:</div><div class='Indent'>";
        for (var i = 0;  i < myTutorials.length; ++i) {
            myString += "<a href='../tutorials/" + myTutorials[i].file + "'>" + myTutorials[i].title + "</a><br/>";
        }
        myString += "</div>";
    }
    return myString;
}

function createStyleSheet() {
    var myString = "";
    myString += "body { padding-left: 20px; }";
    myString += "a { text-decoration:none; color:darkblue; }";
    myString += "a:hover { color:red; text-decoration:underline; }";
    myString += "h2 { padding-top:30px; }";
    myString += ".IndexTable { vertical-align:top; padding-right:30px; }";
    myString += ".SectionHeader { padding-top:30px; font-style:italic; padding-bottom:10px; }";
    myString += ".Indent { padding-left:50px; }";
    myString += ".ParameterTable { border-collapse:collapse; }";
    myString += ".ParameterTableCell { vertical-align:top; border: solid 1px #000000; padding-top: 2px; padding-bottom: 2px; padding-left: 5px; padding-right: 10px; }";
    myString += ".ParameterType { font-family:Courier; }";
    myString += ".TimeStamp { font-style:italic; padding-top:60px; font-size:10pt; }";
    myString += ".SummaryTable { border-collapse:collapse; }";
    myString += ".SummeryHeader { font-weight:bold; }";
    myString += ".SummaryTableCell { vertical-align:top; border: solid 1px #000000; padding-top: 2px; padding-bottom: 2px; padding-left: 5px; padding-right: 10px; }";
    myString += ".FunctionDescription {  padding-left:50px; }";
    myString += ".TutorialCode { font-family:Courier; }";
    myString += ".TutorialText { color:black; }";
    myString += ".comment { color: green; }";
    myString += ".string { color: grey; }";
    myString += ".keyword { color: blue; }";
    myString += ".vars { color: #d00; }";

    var myPath = ourDirectory + "/jsdoc.css";
    if (!putWholeFile(myPath, myString)) {
        throw new Exception("Could not write file: " + myPath, fileline());
    }
}

function createTutorials() {
    var myTutorials = getDirList("${PRO}/tutorials");
    includePath("${PRO}/tutorials");
    for (var i = 0; i < myTutorials.length; ++i) {
        var myTutorial = myTutorials[i];
        var myDotIndex = myTutorial.lastIndexOf(".");
        if (myDotIndex != -1 && myTutorial.substring(myDotIndex, myTutorial.length) == ".js") {
            var myHtmlFileName = myTutorial.substr(0, myDotIndex) + ".html";
            var myFile = getWholeFile(myTutorial);
            var myTitle = myTutorial;
            var myHTMLString = "";
            var myLines = myFile.split("\n");
            var myHeader = false;
            for (var j = 0; j < myLines.length; ++j) {
                var myLine = myLines[j];

                if (myLine.search(/\/\*\*/) != -1) {
                    myHeader = true;
                    myHTMLString += "<div class='TutorialText'>";
                }

                if (myHeader) {
                    if (myLine.search(/@title (.*)/) != -1) {
                        myTitle = RegExp.$1;
                    } else if (myLine.search(/@link (.*)/) != -1) {
                        var myLink = RegExp.$1;
                        if (!(myLink in ourTutorialIndex)) {
                            ourTutorialIndex[myLink] = [];
                        }
                        ourTutorialIndex[myLink].push({title: myTitle, file: myHtmlFileName});
                    } else {
                        if (myLine.search(/^ \* /) != -1) {
                            myHTMLString += myLine.substr(3) + "<br/>";
                        }
                    }
                } else {
                    myHTMLString += ourSyntaxHighlighter.highlight(myLine) + "</br>";
                }

                if (myLine.search(/\*\//) != -1) {
                    if (myHeader) {
                        myHTMLString += "</div><div class='TutorialCode'>";
                    }
                    myHeader = false;
                }
            }
            myHTMLString = "<h1>" + myTitle + "</h1>" + myHTMLString + "</div>";
            writeHTML("tutorials/" + myHtmlFileName, myHTMLString);
        }
    }
}