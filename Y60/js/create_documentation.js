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

const VALIDATE_HTML = true;

if (!("0" in arguments)) {
    print("Usage: create_documentation.js outputdir");
    exit(1);
}
var ourDirectory = arguments[0];

main();

function main() {
    try {
        if (fileExists(ourDirectory) && !isDirectory(ourDirectory)) {
            print("### ERROR: " + ourDirectory + " is a file, not a directory.");
            exit(1);
        }

        generateJSLibDocumentation(ourDirectory);

        print("Successfully created documentation in " + ourDirectory);
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
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
    for (var myModule in theModules) {
        if (myLineCount > 10) {
            myString += "</td><td class='IndexTable'>";
            myLineCount = 0;
        }
        myString += "<p><b>" + myModule + "</b></p>\n";
        for (myClass in theModules[myModule]) {
            if (myClass != "") {
                myString += "<a href='" + myModule + "/" + myClass + ".html'>" + myClass + "</a><br/>\n";
                myLineCount++;
            } else {
                var theFunctions = theModules[myModule][myClass].functions;
                for (var i = 0; i < theFunctions.length; ++i) {
                    var myName = theFunctions[i].name;
                    myString += "<a href='" + myModule + "/" + myName + ".html'>" + myName + "</a><br/>\n";
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
        writeHTML(theModuleName + "/" + theClassName + "_" + myName + ".html", documentFunction(theClassName, theFunctions[i]));
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
    var myTitle = theFunction.name;
    var myClass = theClassName ? theClassName + "::" : "";
    myString += "<h2>" + myClass + theFunction.name + "</h2>";
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
    return myString;
}

function documentFunctions(theClassName, theFunctionsTitle, theFunctions) {
    var myString = "";
    myString += "<h3>" + theFunctionsTitle + "</h3>\n";
    myString += "<ul>\n";
    for (var i = 0; i < theFunctions.length; ++i) {
        myString += "<li>";
        myString += documentFunction(theClassName, theFunctions[i]);
        myString += "</li><br/>\n";
    }
    myString += "</ul>\n";
    return myString;
}

