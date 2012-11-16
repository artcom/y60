/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $RCSfile: create_documentation.js,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/28 17:12:57 $
//
//
//=============================================================================

use("Y60JSSL.js");
use("Exception.js");
use("SyntaxHighlighter.js");

const VALIDATE_HTML = false;

if (!("0" in arguments)) {
    print("Usage: create_documentation.js outputdir");
    exit(1);
}

var ourDirectory         = arguments[0];
var ourTutorialIndex     = [];
var ourModuleIndex       = [];
var ourSyntaxHighlighter = new SyntaxHighlighter();

main();

function main() {
    try {
        if (fileExists(ourDirectory) && !isDirectory(ourDirectory)) {
            print("### ERROR: " + ourDirectory + " is a file, not a directory.");
            exit(1);
        }

        includePath("${PRO}/lib");
        plugComponentsForDocumentation();
        var myModules = [];
        var myModuleNames = getModuleNames();
        for (var i = 0; i < myModuleNames.length; ++i) {
            myModules[myModuleNames[i]] = getDocumentation(myModuleNames[i]);
        }
        createIndex(myModules);
        // XXX
        //createTutorials();
        generateJSLibDocumentation(myModules);
        createStyleSheet();
        print("Successfully created documentation in " + ourDirectory);
    } catch (ex) {
        reportException(ex);
        exit(1);
    }

    exit(0);
}

function plugComponentsForDocumentation() {
    var myComponents = ["Network", "Sound", "ProcessFunctions", "Cairo", "GPUParticles",
                        "EventLoop", "ASSEventSource", "DSADriver", "VideoProcessing",
                        "OscReceiver", "SynergyServer"];
    if (operatingSystem() == "WIN32") {
        myComponents = myComponents.concat(["TaskManager","WMPPlayer"]);
    } else if (operatingSystem() == "LINUX") {
        myComponents = myComponents.concat(["Powermate"]);
    }

    for (var i = 0;i < myComponents.length; ++i) {
        print("Plugging " + myComponents[i]);
        try {
            plug(myComponents[i]);
        } catch(e) {
            print ("### ERROR: plug failed " + myComponents[i]);
        }
    }
}

// This function makes sure, some well-known modules come first
function getModuleNames() {
    var myModuleNames = ["Global", "Math", "GlobalFunctions", "MathFunctions", "FileFunctions"];
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
            if (myClassName == "") {
                // Global functions
                var theFunctions = theModules[myModuleName][myClassName].functions;
                for (var i = 0; i < theFunctions.length; ++i) {
                    var myName = theFunctions[i].name;
                    ourModuleIndex[myName] = myModuleName;
                    myString += "<a href='" + myModuleName + "/" + myName + ".html" + "'>" + myName + "</a><br/>\n";
                    myLineCount++;
                }
            } else {
                // Classes
                myString += "<a href='" + myModuleName + "/" + myClassName + ".html'>" + myClassName + "</a><br/>\n";
                ourModuleIndex[myClassName] = myModuleName;
                myLineCount++;
            }
        }
    }

    myString += "</td></tr></table>";

    writeHTML("index.html", myString);
}

function generateJSLibDocumentation(theModules) {
    for (var myModuleName in theModules) {
        for (var myClassName in theModules[myModuleName]) {
            if (myClassName == "") {
                // Global functions
                var myFunctions = theModules[myModuleName][""].functions;
                createFunctionDocumentation(myModuleName, null, myFunctions);
            } else {
                // Classes
                createClassDocumentation(theModules, myModuleName, myClassName);
            }
        }
    }
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
    var myDir  = getDirectoryPart(myPath);

    if (!isDirectory(myDir)) {
        createDirectory(myDir);
    }

    if (!writeStringToFile(myPath, myString)) {
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
                /*if (myParameter.description == "") {
                    myParameter.description = "Not yet documented.";
                }*/
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
    myString += "body { padding: 20px; }\n";
    myString += "a { text-decoration:none; color:darkblue; }\n";
    myString += "a:hover { color:red; text-decoration:underline; }\n";
    myString += "h2 { padding-top:30px; }\n";
    myString += ".IndexTable { vertical-align:top; padding-right:30px; }\n";
    myString += ".SectionHeader { padding-top:30px; font-style:italic; padding-bottom:10px; }\n";
    myString += ".Indent { padding-left:50px; }\n";
    myString += ".ParameterTable { border-collapse:collapse; }\n";
    myString += ".ParameterTableCell { vertical-align:top; border: solid 1px #000000; padding-top: 2px; padding-bottom: 2px; padding-left: 5px; padding-right: 10px; }\n";
    myString += ".ParameterType { font-family:Courier; }\n";
    myString += ".TimeStamp { font-style:italic; padding-top:60px; font-size:10pt; }\n";
    myString += ".SummaryTable { border-collapse:collapse; }\n";
    myString += ".SummeryHeader { font-weight:bold; }\n";
    myString += ".SummaryTableCell { vertical-align:top; border: solid 1px #000000; padding-top: 2px; padding-bottom: 2px; padding-left: 5px; padding-right: 10px; }\n";
    myString += ".FunctionDescription {  padding-left:50px; }\n";
    myString += ".TutorialCode { font-family:Courier; background-color: #FFFFE0; padding-left: 20px; margin-top:20px }\n";
    myString += ".TutorialText { color:black; }\n"
    myString += ".TutorialTableCell { vertical-align:top; padding-right: 10px}\n";
    myString += ".TutorialLinks { padding-left:50px }\n";
    myString += ".comment { color: green; }\n";
    myString += ".string { color: grey; }\n";
    myString += ".keyword { color: blue; }\n";
    myString += ".vars { color: #d00; }\n";
    myString += ".number { color: red; }\n";
    myString += ".y60class { color: #FF8000 }\n"

    var myPath = ourDirectory + "/jsdoc.css";
    if (!writeStringToFile(myPath, myString)) {
        throw new Exception("Could not write file: " + myPath, fileline());
    }
}

function getLinkedFile(theLink) {
    var mySplit  = theLink.split("::");
    var myClass  = mySplit[0];
    var myMethod = "";
    if (mySplit.length > 1) {
        myMethod = "_" + mySplit[1];
    }
    if (myClass in ourModuleIndex) {
        var myModule = ourModuleIndex[myClass];
        return "../" + myModule + "/" + myClass + myMethod + ".html";
    } else if (myClass == "DOM") {
        return "http://himmel/doc60/schema/noNamespace/element/" + mySplit[1].toLowerCase() + ".html";
    } else {
        return null;
    }
}

function createTutorials() {
    var myTutorials = getDirectoryEntries("${PRO}/tutorials");
    myTutorials.sort();
    var myTutorialIndex = "<h1>Y60 Tutorials</h1><div class='Indent'>";
    includePath("${PRO}/tutorials");

    var myY60Keywords = "";
    for (var myWord in ourModuleIndex) {
        if (myY60Keywords) {
            myY60Keywords += " ";
        }
        myY60Keywords += myWord;
    }
    ourSyntaxHighlighter.addKeywords(myY60Keywords, "y60class");

    var myDir = ourDirectory + "/tutorials"
    if (!isDirectory(ourDirectory)) {
        createDirectory(ourDirectory);
    }

    for (var i = 0; i < myTutorials.length; ++i) {
        var myTutorial = myTutorials[i];
        var myDotIndex = myTutorial.lastIndexOf(".");
        if (myDotIndex != -1 && myTutorial.substring(myDotIndex, myTutorial.length) == ".js") {
            var myHtmlFileName = myTutorial.substr(0, myDotIndex) + ".html";
            var myFile = readFileAsString(myTutorial);
            var myTitle = myTutorial;
            var myTutorialExplanation = "";
            var myTutorialCode = "";
            var myTutorialLinks = "";

            var myLines = myFile.split("\n");
            var myHeader = false;
            for (var j = 0; j < myLines.length; ++j) {
                var myLine = myLines[j];

                if (myLine.search(/\/\*\*/) != -1) {
                    myHeader = true;
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
                        var myLinkedFile = getLinkedFile(myLink);
                        if (myLinkedFile) {
                            myTutorialLinks += "<a href='" + myLinkedFile + "'>" + myLink + "</a></br>";
                        }
                    } else {
                        if (myLine.search(/^ \*/) != -1) {
                            myTutorialExplanation += myLine.substr(3) + "<br/>";
                        }
                    }
                } else {
                    myTutorialCode += ourSyntaxHighlighter.highlight(myLine) + "</br>";
                }

                if (myLine.search(/\*\//) != -1) {
                    myHeader = false;
                }
            }

            var myScreenshotFileName = myTutorial.substr(0, myDotIndex) + ".png";

            var myHTMLString = "";
            myHTMLString += "<h1>" + myTitle + "</h1>";
            myHTMLString += "<table><tr><td class='TutorialTableCell'>";
            myHTMLString += "<div class='TutorialImage'><img src='" + myScreenshotFileName + "'></div>";
            myHTMLString += "</td><td class='TutorialTableCell'><div class='TutorialText'>" + myTutorialExplanation + "</div></td></tr></table>";
            myHTMLString += "<div class='TutorialCode'>" + myTutorialCode + "</div>";
            if (myTutorialLinks) {
                myHTMLString += "<div class='SectionHeader'>See also:</div><div class='TutorialLinks'>" + myTutorialLinks + "</div>";
            }

            print("Create tutorial: " + myHtmlFileName);
            writeHTML("tutorials/" + myHtmlFileName, myHTMLString);

            myTutorialIndex += "<a href='" + myHtmlFileName + "'>" + myTitle + "</a></br>";
            exec("bash -c 'cp $PRO/tutorials/BaselineImages/*.png " + ourDirectory + "/tutorials/'");
        }
    }
    myTutorialIndex += "</div>";
    print("Create tutorial index");
    writeHTML("tutorials/index.html", myTutorialIndex);
}
