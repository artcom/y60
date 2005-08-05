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

//use("TrackballMover.js");
//use("BSpline.js");
//use("DistanceMeasurement.js");

//plug("ProcFunctions");

var ourModuleDocumentation = {};


    if (!("0" in arguments)) {
        print("Usage: create_documentation.js outputfile");
        exit(1);
    }
    try {
        generateJSLibDocumentation(arguments[0]);

    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }

    print("Successfully created " + arguments[0]);


function createDocHeader() {
    var myString = '<html><head>';
    myString += '<link rel="StyleSheet" href="http://himmel/twiki/pub/TWiki/PatternSkin/style.css" type="text/css"/>';
    myString += '<link rel="StyleSheet" href="http://himmel/twiki/pub/TWiki/PatternSkin/layout.css" type="text/css"/>';
    myString += '</head><body>';
    myString += '<div class="twikiTopBar"><div class="twikiTopBarContents"><div class="twikiLeft">' +
        '<a href="http://himmel.artcom.de/"><img src="http://himmel/icons/ac_simple_trans.png" border="0" alt="Home"/></a>' +
        '</div></div></div>';
    myString += '<div class="twikiMiddleContainer"><div class="twikiLeftBar">';
    myString += '<div class="twikiWebIndicator"><b>Y60 JavaScript Class Reference</b></div><div class="twikiLeftBarContents">';

    return myString;
}

function documentFunctionCall(theName, theParameters) {

    var myString = "";
    var i,j;
    var myParameterOption, myAltParameters = [];
    var jstart = 0;
    for (j = 0;j < theParameters.length; ++j) {
        if (theParameters[j].type == '__intern__') {
            myParameterOption = theParameters.slice(jstart,j);
            jstart = j+1;
            myAltParameters.push(myParameterOption);
        }
    }
    myParameterOption = theParameters.slice(jstart,j);
    myAltParameters.push(myParameterOption);

    for (i = 0; i < myAltParameters.length; ++i) {
        myString += '<b>' + theName + '</b>(';
        for (j = 0; j < myAltParameters[i].length; ++j) {
            var myParameter = myAltParameters[i][j];
            myString += myParameter.type + ' <b>' + myParameter.name + '</b>';
            if (myParameter.default_value) {
                myString += ' = ' + myParameter.default_value;
            }
            if (j < myAltParameters[i].length-1) {
                myString += ", ";
            }
        }
        myString += ')<br/>';
    }
    //print(myString);
    return myString;
}

function documentFunctions(theFunctionsTitle, theFunctions) {
    var myString = "";
    myString += "<h3>" + theFunctionsTitle + "</h3>\n";
    myString += "<ul>\n";
    for (var i = 0; i < theFunctions.length; ++i) {

        var myFunc = theFunctions[i];
        myString += '<li>';
        myString += documentFunctionCall(myFunc.name, myFunc.parameters);

        if (myFunc.description) {
            myString += myFunc.description + '<br/>\n';
        }
        if (myFunc.return_value) {
            myString += "<i>returns</i> " + '<b>' + myFunc.return_type + '</b> '
                          + myFunc.return_value + "<br/>";
        }
        myString += "</li>";
    }
    myString += "</ul>\n";
    return myString;
}

function generateJSLibDocumentation(theDocFile) {

    var myDocumentation = {"global" : getDocumentation("global"),
        "math" : getDocumentation("math"),
        "functions"  : getDocumentation("GlobalFunctions"),
        "math functions"  : getDocumentation("MathFunctions"),
        "directory functions"  : getDocumentation("DirectoryFunctions"),
        "proc functions"  : getDocumentation("ProcFunctions"),
        "gtk"    : getDocumentation("gtk")
    };

    var myDocuString = createDocHeader();

    for (myModule in myDocumentation) {
        myDocuString += '<p><b><a href="#' + myModule + '">' + myModule + '</a></b></p>\n';
        for (myClass in myDocumentation[myModule]) {
            if (myClass != "") {
                myDocuString += '<a href="#' + myClass + '">' + myClass + '</a><br/>\n';
            } else {
                var myFuncs = myDocumentation[myModule][myClass].functions;
                for (f in myFuncs) {
                    myDocuString += '<a href="#' + myModule + '">' + myFuncs[f].name + "</a><br/>\n";
                }
            }
        }
    }
    myDocuString += '</div></div><div class="twikiMain"><div class="twikiRevInfo" style="text-align:right"><span class="twikiGrayText">';
    myDocuString += '<span class="twikiToolbarElem">' + new Date();
    myDocuString += '</span></span></div><div class="twikiTopic">';
    for (myModule in myDocumentation) {
        var myModuleDocuString = '<a name="' + myModule + '"><div/></a><h1><br/>'
                                             + myModule + '</h1>\n';
        for (myClass in myDocumentation[myModule]) {
            var myClassDocu = myDocumentation[myModule][myClass];
            var myClassDocuString = '<a name="' + myClass + '"><div/></a><h2>' + myClass
                if (myClassDocu.base_class) {
                    var myBaseClass = myClassDocu.base_class;
                    while (myBaseClass != "") {
                        myClassDocuString += '<a href="#' + myBaseClass + '">[ '
                            + myBaseClass + " ] </a>";
                        myBaseClass = myDocumentation[myModule][myBaseClass].base_class;
                    }
                }
            myClassDocuString += '</h2>\n';

            if (myClassDocu.constructors.length) {
                myClassDocuString += documentFunctions("Constructors", myClassDocu.constructors);
            }
            if (myClassDocu.functions.length) {
                myClassDocuString += documentFunctions("Functions", myClassDocu.functions);
            }

            var i;
            if (myClassDocu.properties.length) {
                myClassDocuString += "<h3>Properties</h3><ul>\n";
                for (i = 0; i < myClassDocu.properties.length; ++i) {
                    myClassDocuString += '<li>' + myClassDocu.properties[i] + '</li>\n';
                }
                myClassDocuString += "</ul>"
            }
            if (myClassDocu.static_functions.length) {
                myClassDocuString += documentFunctions("Static Functions", myClassDocu.static_functions);
            }
            if (myClassDocu.static_properties.length) {
                myClassDocuString += "<h3>Static properties</h3><ul>\n";
                for (i = 0; i < myClassDocu.static_properties.length; ++i) {
                    myClassDocuString += '<li>' + myClassDocu.static_properties[i] + '</li>\n';
                }
                myClassDocuString += "</ul>"
            }
            if (myClassDocu.constants.length) {
                myClassDocuString += "<h3>Constants</h3><ul>\n";
                for (i = 0; i < myClassDocu.constants.length; ++i) {
                    myClassDocuString += '<li>' + myClassDocu.constants[i] + '</li>\n';
                }
                myClassDocuString += "</ul>"
            }
            myModuleDocuString += myClassDocuString;
        }
        myDocuString += myModuleDocuString;
    }
    myDocuString += "</div></div></div></body></html>";
    var myXmlDoc = new Node(myDocuString);
    myXmlDoc.saveFile(theDocFile);
}


