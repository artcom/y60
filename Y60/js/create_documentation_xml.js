//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
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

use("Y60JSSL.js");
use("Exception.js");
use("SyntaxHighlighter.js");

const VALIDATE_HTML = false;

if (!("0" in arguments)) {
    print("Usage: create_documentation.js outputfile");
    exit(1);
}

var ourResultFile        = arguments[0];
var ourTutorialIndex     = [];
var ourModuleIndex       = [];
var ourSyntaxHighlighter = new SyntaxHighlighter();

main();

function main() {
    try {
        includePath("${PRO}/lib");
        plugComponentsForDocumentation();
        var myModules = [];
        var myModuleNames = getModuleNames();
        for (var i = 0; i < myModuleNames.length; ++i) {
            myModules[myModuleNames[i]] = getDocumentation(myModuleNames[i]);
        }
        createIndex(myModules);
        createTutorials();
        generateJSLibDocumentation(myModules);
        print("Successfully created documentation " + ourResultFile);
    } catch (ex) {
        reportException(ex);
        exit(1);
    }

    exit(0);
}

function plugComponentsForDocumentation() {
    var myComponents = ["jsnet", "y60JSSound", "ProcFunctions"];
    if (operatingSystem() == "Win32") {
        myComponents = myComponents.concat(["TaskManager","y60WMPPlayer"]);
    } else if (operatingSystem() == "Linux") {
        myComponents = myComponents.concat([]);
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

function createIndex(theModules) {
    for (var myModuleName in theModules) {
        for (myClassName in theModules[myModuleName]) {
            if (myClassName == "") {
                // Global functions
                var theFunctions = theModules[myModuleName][myClassName].functions;
                for (var i = 0; i < theFunctions.length; ++i) {
                    var myName = theFunctions[i].name;
                    ourModuleIndex[myName] = myModuleName;
                }
            } else {
                // Classes
                ourModuleIndex[myClassName] = myModuleName;
            }
        }
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

function generateJSLibDocumentation(theModules) {
    var myDom = new Node("<jslib/>").firstChild;
    for (var myModuleName in theModules) {
        var myNode = myDom.appendChild(new Node("<module/>").firstChild);
        myNode.name = myModuleName;
        for (var myClassName in theModules[myModuleName]) {
            if (myClassName == "") {
                // Global functions
                var myFunctions = theModules[myModuleName][""].functions;
                createFunctionDocumentation(myFunctions, myNode);
            } else {
                // Classes
                var myClass = theModules[myModuleName][myClassName];
                myClass.name = myClassName;
                createClassDocumentation(myClass, myNode);
            }
        }
    }
    myDom.saveFile(ourResultFile);
}

function createFunctionDocumentation(theFunctions, theResultNode) {
    for (var i = 0; i < theFunctions.length; ++i) {
        var myNode = theResultNode.appendChild(new Node("<function/>").firstChild);
        myNode.name = theFunctions[i].name;
        documentFunction(theFunctions[i], myNode);
    }
}

function createClassDocumentation(theClass, theResultNode) {
    print("Documenting class: " + theClass.name);
    var myClassNode = theResultNode.appendChild(new Node("<class/>").firstChild);
    myClassNode.name = theClass.name;

    if (theClass.constructors.length) {
        var myNode = myClassNode.appendChild(new Node("<constructor/>").firstChild);
        createFunctionDocumentation(theClass.constructors, myNode);
    }
    if (theClass.functions.length) {
        myNode = myClassNode.appendChild(new Node("<functions/>").firstChild);
        createFunctionDocumentation(theClass.functions, myNode);
    }
    if (theClass.static_functions.length) {
        myNode = myClassNode.appendChild(new Node("<static_functions/>").firstChild);
        createFunctionDocumentation(theClass.static_functions, myNode);
    }

    summarizeProperties("properties", theClass.properties, myClassNode);
    summarizeProperties("static_properties", theClass.static_properties, myClassNode);
    summarizeProperties("constants", theClass.constants, myClassNode);
}

function summarizeProperties(thePropertyTitle, theProperties, theResultNode) {
    if (theProperties.length) {
        var myProperitiesNode = theResultNode.appendChild(new Node("<" + thePropertyTitle + "/>").firstChild);
        for (var i = 0; i < theProperties.length; ++i) {
            var myNode = myProperitiesNode.appendChild(new Node("<property/>").firstChild);
            myNode.name = theProperties[i];
        }
    }
}

function documentFunction(theFunction, theResultNode) {
    // Description
    if (theFunction.description != "") {
        theResultNode.description = theFunction.description;
    } else {
        theResultNode.description = "Not yet documented.";
    }

    // Parameters
    if (theFunction.parameters.length) {
        for (var j = 0; j < theFunction.parameters.length; ++j) {
            var myParameter = theFunction.parameters[j];
            if (String(myParameter.type) != "__intern__") {
                var myParamterNode = theResultNode.appendChild(new Node("<parameter/>").firstChild);
                myParamterNode.name = myParameter.name;
                myParamterNode.description = myParameter.description;
                myParamterNode.type = myParameter.type;
                if (myParameter.default_value) {
                    myParamterNode.default_value = myParameter.default_value;
                }
            } else {
                theResultNode.appendChild(new Node("<spacer/>").firstChild);
            }
        }
    }

    // Return value
    if (theFunction.return_value) {
        theResultNode.return_value = theFunction.return_value;
    }

    theResultNode.return_type = theFunction.return_type ? theFunction.return_type : "void";

/*
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
*/
}

function getLink(theString) {
    var myResult   = {};
    var mySplit    = theString.split("::");
    var myKey      = ""
    if (mySplit.length > 1) {
        myResult.classname = mySplit[0];
        myResult.functionname = mySplit[1];
        myKey = myResult.classname;
    } else {
        myResult.classname = "";
        myResult.functionname = mySplit[0];
        myKey = myResult.functionname;
    }

    if (myKey in ourModuleIndex) {
        myResult.module = ourModuleIndex[myKey];
        return myResult;
    } else if (myKey == "DOM") {
        return null; //"http://himmel/doc60/schema/noNamespace/element/" + mySplit[1].toLowerCase() + ".html";
    } else {
        return null;
    }
}

function createTutorials() {
    var myTutorials = getDirectoryEntries("${PRO}/tutorials");
    myTutorials.sort();
    includePath("${PRO}/tutorials");

    var myY60Keywords = "";
    for (var myWord in ourModuleIndex) {
        if (myY60Keywords) {
            myY60Keywords += " ";
        }
        myY60Keywords += myWord;
    }
    ourSyntaxHighlighter.addKeywords(myY60Keywords, "y60class");
/*
    var myDir = ourDirectory + "/tutorials"
    if (!isDirectory(ourDirectory)) {
        createDirectory(ourDirectory);
    }
*/
    var myDom = new Node("<tutorials/>").firstChild;
    for (var i = 0; i < myTutorials.length; ++i) {
        var myTutorial = myTutorials[i];
        var myDotIndex = myTutorial.lastIndexOf(".");
        if (myDotIndex != -1 && myTutorial.substring(myDotIndex, myTutorial.length) == ".js") {
            var myNode = myDom.appendChild(new Node("<tutorial/>").firstChild);
            var myTutorialLinks = myNode.appendChild(new Node("<links/>").firstChild);

            var myHtmlFileName = myTutorial.substr(0, myDotIndex) + ".html";
            var myFile = readFileAsString(myTutorial);
            var myTitle = myTutorial;
            var myTutorialExplanation = "";
            var myTutorialCode = "";

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
                        var myLinkString = RegExp.$1;
                        if (!(myLinkString in ourTutorialIndex)) {
                            ourTutorialIndex[myLinkString] = [];
                        }
                        ourTutorialIndex[myLinkString].push({title: myTitle, file: myHtmlFileName});
                        var myLink = getLink(myLinkString);
                        if (myLink) {
                            var myLinkNode = myTutorialLinks.appendChild(new Node("<link/>").firstChild);
                            myLinkNode.module = myLink.module;
                            myLinkNode.classname = myLink.classname;
                            myLinkNode.functionname = myLink.functionname;
                        }
                    } else {
                        if (myLine.search(/^ \*/) != -1) {
                            myTutorialExplanation += myLine.substr(3) + "<br/>\n";
                        }
                    }
                } else {
                    myTutorialCode += ourSyntaxHighlighter.highlight(myLine) + "<br/>\n";
                }

                if (myLine.search(/\*\//) != -1) {
                    myHeader = false;
                }
            }

            myNode.screenshot = myTutorial.substr(0, myDotIndex) + ".png";
            myNode.title = myTitle;
            myNode.appendChild(new Node("<explanation><![CDATA[\n" + myTutorialExplanation + "]]></explanation>").firstChild);
            myNode.appendChild(new Node("<tutorialcode><![CDATA[\n" + myTutorialCode + "]]></tutorialcode>").firstChild);

            //exec("bash -c 'cp $PRO/tutorials/BaselineImages/*.png " + ourDirectory + "/tutorials/'");
        }
    }
    print("Create tutorial xml");
    myDom.saveFile("tutorials.xml");
}
