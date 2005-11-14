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
//   $RCSfile: Y60JSSL.js,v $
//   $Author: jens $
//   $Revision: 1.39 $
//   $Date: 2005/04/26 15:15:13 $
//
//
//=============================================================================

//==============================================================================
// Y60 JSSL - Y60 JavaScript Standard Library
// Content:
//     - JavaScript helper functions
//==============================================================================

var PI_2 = Math.PI / 2;
var PI_4 = Math.PI / 4;
var PI_180 = Math.PI / 180;
var TWO_PI = Math.PI * 2.0;

function clone(theObject) {
    var myNewObject = [];
    for (i in theObject) {
        if (typeof theObject[i] == "object") {
            myNewObject[i] = clone(theObject[i]);
        } else {
            myNewObject[i] = theObject[i];
        }
    }
    return myNewObject;
}

// WTF?
function getFocalLength(theHfov) {
    return (35 / (2 * Math.tan(theHfov * Math.PI / 360)));
}

function getHPR(theMatrix) {
    // From "3D Engine Design", page 19
    var myH = Math.asin(theMatrix[2][0]);
    var myP = 0;
    var myR = 0;
    if (myH < PI_2) {
        if (myH > - PI_2) {
            myP = Math.atan2(-theMatrix[2][1], theMatrix[2][2]);
            myR = Math.atan2(-theMatrix[1][0], theMatrix[0][0]);
        } else {
            // not a unique solution
            myP = -Math.atan2(theMatrix[0][1], theMatrix[1][1]);
            myR = 0;
        }
    } else {
        // not a unique solution
        myP = Math.atan2(theMatrix[0][1], theMatrix[1][1]);
        myR = 0;
    }

    return new Vector3f(myH, myP, myR);
}

// get normal from three points
function calcNormal(thePoint1, thePoint2, thePoint3) {
    var myVector1 = difference(thePoint1, thePoint2);
    var myVector2 = difference(thePoint2, thePoint3);

    return normalized(cross(myVector1, myVector2));
}

function fmod(a, b) {
    return (a - (Math.floor(a / b) * b));
}

function degFromRad(theRadiant) {
    return theRadiant * (180.0 / Math.PI);
}

function radFromDeg(theDegree) {
    return theDegree * (Math.PI / 180.0);
}

function clamp(theValue, theMin, theMax) {
    if (theValue < theMin) {
        return theMin;
    }
    if (theValue > theMax) {
        return theMax;
    }
    return theValue;
}

function tanh(x) {
    var myExp = Math.exp(x);
    var myNegExp = Math.exp(-x)
    return (myExp-myNegExp) / (myExp+myNegExp);
}

function stringToArray(s) {
    return s.substring(1, s.length - 1).split(",");
}

function stringToByteArray(theString) {
    var myResult = [];
    for (var i = 0; i < theString.length; ++i) {
        myResult.push(theString.charCodeAt(i));
    }
    return myResult;
}

// Expects theDateString to be in the format "2000-1-31"
function parseDate(theDateString) {
    var myDate = theDateString.split("-");
    return new Date(myDate[0], myDate[1] - 1, myDate[2]);
}

// use like this: asColor("00BFA3", 1);
function asColor(theHexString, theAlpha) {
    if (theAlpha == undefined) {
        theAlpha = 1;
    }
    var myRed   = eval("0x" + theHexString[0] + theHexString[1]);
    var myGreen = eval("0x" + theHexString[2] + theHexString[3]);
    var myBlue  = eval("0x" + theHexString[4] + theHexString[5]);
    return new Vector4f(myRed / 255, myGreen / 255, myBlue / 255, theAlpha);
}

// returns a string represenation of an (nested) array
function arrayToString(a) {
    var myString = "[";
    for (var i = 0; i < a.length; ++i) {
        var myElement = a[i];
        if (a[i] && a[i].constructor && a[i].constructor == Array) {
            myString += arrayToString(a[i]);
        } else {
            myString += a[i];
        }

        if (i < a.length - 1) {
            myString += ",";
        }
    }
    myString += "]";
    return myString;
}

// returns a precision formated string from a Vector3f list
function formatVector3f(theVector, thePrecision) {

    var myString = "";
    for (var i = 0; i < theVector.length; ++i) {

        if (i != 0) {
            myString += ",";
        }

        myString += "[" + theVector[i].x.toFixed(thePrecision) + ",";
        myString += theVector[i].y.toFixed(thePrecision) + ",";
        myString += theVector[i].z.toFixed(thePrecision) + "]";
    }

    return myString;
}

// returns a precision formated string from a Vector2f list
function formatVector2f(theVector, thePrecision) {

    var myString = "";
    for (var i = 0; i < theVector.length; ++i) {

        if (i != 0) {
            myString += ",";
        }

        myString += "[" + theVector[i].x.toFixed(thePrecision) + ",";
        myString += theVector[i].y.toFixed(thePrecision) + "]";
    }

    return myString;
}

// NOTE: normalized vectors required
function getOrientationFromDirection(theViewVector, theUpVector) {
    var myMatrix = new Matrix4f();
    var myUpVector = normalized(projection(theUpVector, new Planef(theViewVector, [0,0,0])));

    var myRightVector = cross(theViewVector, myUpVector);
    myMatrix.setRow(0, new Vector4f(myRightVector[0], myRightVector[1], myRightVector[2], 0));
    myMatrix.setRow(1, new Vector4f(myUpVector[0], myUpVector[1], myUpVector[2], 0));
    myMatrix.setRow(2, new Vector4f(-theViewVector[0], -theViewVector[1], -theViewVector[2], 0));

    return myMatrix.decompose().orientation;
}

// Returns a string containing all the attributes of element E
function listAttributes(E) {
    var myResult = ' ';
    for (var i = 0; i < E.attributes.length; ++i) {
        myResult+=E.attributes[i].nodeName+'="'+E.attributes[i].nodeValue+'" ';
    }
    return myResult;
}

// Dumps element E to console
function dumpElementShallow(E) {
    dumpElement(E,'',0);
}

// Dumps element E and maxDepth levels of children to console
function dumpElementLevels(E,maxDepth) {
    dumpElement(E,'',maxDepth);
}

// Dumps element E with theMaxDepth levels of children using theSpace as base indent
function dumpElement(E,theSpace,theMaxDepth) {
    if (E) {
        if (E.nodeType==Node.ELEMENT_NODE) {
            var myId = E.getAttribute('id');
            print(theSpace+'<'+E.nodeName+listAttributes(E)+'>');
            if (theMaxDepth!=0) {
                for (var i=0; i < E.childNodes.length; ++i) {
                    dumpElement(E.childNodes[i], theSpace+'    ',theMaxDepth-1);
                    //dumpElement(E.childNodes.item(i), theSpace+'    ',theMaxDepth-1);
                }
            }
            print(theSpace+'<'+E.nodeName+'/>');
        } else if (E.nodeType==E.TEXT_NODE) {
            print(E.nodeValue);
        }
    }
}

// Adjust Node.id (and it's descendants) to be unique
function adjustNodeId(theNode, theDeepAdjustFlag) {
    theNode.id = createUniqueId();
    if (theDeepAdjustFlag == undefined) {
        theDeepAdjustFlag = false;
    }
    if (theDeepAdjustFlag) {
        for (var i = 0; i < theNode.childNodes.length; ++i) {
            adjustNodeId(theNode.childNodes[i], theDeepAdjustFlag);
        }
    }
}

//searches for a descendant of theNode (must be in DOM below theNode)
function getDescendantById(theNode, theId, doDeepSearch) {
    return getDescendantByAttribute(theNode, "id", theId, doDeepSearch);
}

function getDescendantByName(theNode, theName, doDeepSearch) {
    return getDescendantByAttribute(theNode, "name", theName, doDeepSearch);
}

// Recursivly searches theNode for the first element that has theAttribute.
// Can search deep or shallow depending on the value of doDeepSearch
function getDescendantByAttributeName(theNode, theAttribute, doDeepSearch) {
    try {
        if (!theAttribute || !theNode) {
            return null;
        }
        if (theNode.nodeType==Node.ELEMENT_NODE) {
            if (theAttribute in theNode) {
                return theNode;
            }
        }
        var myChildren = theNode.childNodes;
        for (var i = 0; i < myChildren.length; ++i) {
            if (myChildren[i].nodeType == Node.ELEMENT_NODE) {
                if (doDeepSearch) {
                    var myNode = getDescendantByAttributeName(myChildren[i], theAttribute, doDeepSearch);
                    if (myNode) {
                        return myNode;
                    }
                } else if (theAttribute in myChildren[i]) {
                    return myChildren[i];
                }

            }
        }
    } catch (ex) {
        print('Exception in getDescendantByAttributeName():\n'+ex);
    }
    return null;
}

// Recursivly searches theNode for the first element that has theAttribute with value theValue.
// Can search deep or shallow depending on the value of doDeepSearch
function getDescendantByAttribute(theNode, theAttribute, theValue, doDeepSearch) {
    try {
        if (!theValue || !theNode) {
            return null;
        }
        if (theNode.nodeType==Node.ELEMENT_NODE) {
            if (theNode.getAttribute(theAttribute) == theValue) {
                return theNode;
            }
        }
        var myChildren = theNode.childNodes;
        for (var n = 0; n < myChildren.length; ++n) {
            if (myChildren[n].nodeType == Node.ELEMENT_NODE) {

                if (doDeepSearch) {
                    var myNode = getDescendantByAttribute(myChildren[n], theAttribute, theValue, doDeepSearch);
                    if (myNode) {
                        return myNode;
                    }
                } else if (myChildren[n].getAttribute(theAttribute) == theValue) {
                    return myChildren[n];
                }

            }
        }
    } catch (ex) {
        print('Exception in getDescendantByAttribute():\n'+ex);
    }
    return null;
}

// Recursivly searches theNode for all elements that have theAttribute with value theValue.
// Can search deep or shallow depending on the value of doDeepSearch
function getDescendantsByAttribute(theNode, theAttribute, theValue, doDeepSearch) {
    var myResult = new Array();
    try {
        if (!theValue || !theNode) {
            return null;
        }
        var myNumberOfChildren = theNode.childNodes.length;
        for (var i = 0; i < myNumberOfChildren; ++i) {
            var myChild = theNode.childNodes[i];
            if (myChild.nodeType == Node.ELEMENT_NODE) {
                if (myChild.getAttribute(theAttribute) == theValue) {
                    myResult = myResult.concat(myChild);
                }

                if (doDeepSearch) {
                    myResult = myResult.concat(getDescendantByAttribute(myChild, theAttribute, theValue, doDeepSearch));
                }
            }
        }
    } catch (ex) {
        print('Exception in getDescendantsByAttribute():\n'+ex);
    }
    return myResult;
}

function getDescendantsByAttributeName(theNode, theAttribute, doDeepSearch) {
    var myResult = new Array();
    try {
        if (!theNode) {
            return null;
        }
        var myNumberOfChildren = theNode.childNodes.length;
        for (var i = 0; i < myNumberOfChildren; ++i) {
            var myChild = theNode.childNodes[i];
            if (myChild.nodeType == Node.ELEMENT_NODE) {
                if (myChild.getAttribute(theAttribute)) {
                    myResult = myResult.concat(myChild);
                }

                if (doDeepSearch) {
                    myResult = myResult.concat(getDescendantByAttributeName(myChild, theAttribute, doDeepSearch));
                }
            }
        }
    } catch (ex) {
        print('Exception in getDescendantsByAttributeName():\n'+ex);
    }
    return myResult;
}

// Recursivly search for the first element by tagname
function getDescendantByTagName(theNode, theTagName, doDeepSearch, caseInsensitive) {
    if (caseInsensitive == undefined) {
        caseInsensitive = false;
    }
    try {
        if (!theTagName || !theNode) {
            return null;
        }
        if (caseInsensitive) {
            theTagName = String(theTagName).toLowerCase();
        }

        for (var n = 0; n < theNode.childNodes.length; n++) {
            var myChildNode = theNode.childNodes[n];
            if (caseInsensitive) {
                var myChildNodeName = String(myChildNode.nodeName).toLowerCase();
                if (myChildNodeName == theTagName) {
                    return myChildNode;
                }
            } else if (myChildNode.nodeName == theTagName) {
                return myChildNode;
            }

            if (doDeepSearch) {
                var myResult = getDescendantByTagName(myChildNode, theTagName, doDeepSearch, caseInsensitive);

                if (myResult) {
                    return myResult;
                }
            }
        }
    } catch (ex) {
        print('Exception in getDescendantByTagName():\n' + ex);
    }
    return null;
}

// Recursivly search for all elements by tagname
function getDescendantsByTagName(theNode, theTagName, doDeepSearch, caseInsensitive) {
    if (caseInsensitive == undefined) {
        caseInsensitive = false;
    }
    var myResults = new Array();
    try {
        if (!theTagName || !theNode) {
            return null;
        }
        if (caseInsensitive) {
            theTagName = String(theTagName).toLowerCase();
        }

        for (var n = 0; n < theNode.childNodes.length; n++) {
            var myChildNode = theNode.childNodes[n];
            if (caseInsensitive) {
                var myChildNodeName = String(myChildNode.nodeName).toLowerCase();
                if (myChildNodeName == theTagName) {
                    myResults = myResults.concat(myChildNode);
                }
            } else if (myChildNode.nodeName == theTagName) {
                myResults = myResults.concat(myChildNode);
            }

            if (doDeepSearch) {
                var myChildResults = getDescendantsByTagName(myChildNode, theTagName, doDeepSearch, caseInsensitive);
                if (myChildResults) {
                    myResults = myResults.concat(myChildResults);
                }
            }
        }
    } catch (ex) {
        print('Exception in getDescendantsByTagName():\n' + ex);
    }
    return myResults;
}

function getChildElementNodes(theNode, theFilterOperation, theFilterNodeName) {
    var myChildElementNodes = [];
    var i;

    try {
        if (theNode) {
            for (var n = 0; n < theNode.childNodes.length; n++) {
                if (theNode.childNodes[n].nodeType==Node.ELEMENT_NODE) {
                    switch (theFilterOperation) {
                    case 'ignore':
                        if (theFilterNodeName != theNode.childNodes[n].nodeName) {
                            myChildElementNodes.push(theNode.childNodes[n]);
                        }
                        break;
                    case 'match':
                        if (theFilterNodeName == theNode.childNodes[n].nodeName) {
                            myChildElementNodes.push(theNode.childNodes[n])
                                }
                        break;
                    case 'ignore-list':
                        for (i = 0; i < theFilterNodeName.length; i++) {
                            if (theFilterNodeName[i] != theNode.childNodes[n].nodeName) {
                                myChildElementNodes.push(theNode.childNodes[n]);
                            }
                        }
                        break;
                    case 'match-list':
                        for (i = 0; i < theFilterNodeName.length; i++) {
                            if (theFilterNodeName[i] == theNode.childNodes[n].nodeName) {
                                myChildElementNodes.push(theNode.childNodes[n]);
                            }
                        }
                        break;
                    case '':
                    default:
                        myChildElementNodes.push(theNode.childNodes[n]);
                    }
                }
            }
        }
    } catch (ex) {
        print('Exception in getChildElementNodes('+theNode+', '+theFilterOperation+', '+theFilterNodeName+'):\n'+ex);
    }

    return myChildElementNodes;
}

function getDescendantByNameChecked(theNode, theName, theDeepFlag) {
    var myResult = getDescendantByName(theNode, theName, true);
    if (!myResult) {
        print("### ERROR: node name '"+theName+"' not found");
    }
    return myResult;
}

function getPropertyValue() {
    var myNode = getDescendantByName(thePropertiesNode, theProperty);
    if (!myNode) {
        return null;
    }
    if (myNode.nodeName != theDataType) {
        Logger.error("Property "+theProperty+" is not a "+theDataType);
        return null;
    }
    return myNode.childNode('#text').nodeValue;
}

function setPropertyValue(thePropertiesNode, theDataType, theProperty, theValue) {
    var myNode = getDescendantByName(thePropertiesNode, theProperty);
    if (!myNode) {
        myNode = Node.createElement(theDataType);
        thePropertiesNode.appendChild(myNode);
        myNode.name = theProperty;
        myNode.appendChild(Node.createTextNode(theValue));
    } else {
        if (myNode.nodeName != theDataType) {
            Logger.error("Property "+theProperty+" is not a "+theDataType);
            return null;
        }
        myNode.childNode('#text').nodeValue = theValue;
    }
}

function getTimestamp() {
    var myDate  = new Date();
    var myYear  = myDate.getFullYear();
    var myMonth = myDate.getMonth()+1;
    var myDay   = myDate.getDate();
    var myHour  = myDate.getHours();
    var myMin   = myDate.getMinutes();
    var mySec   = myDate.getSeconds();

    return myYear + "_" +
        ((myMonth < 10) ? "0" : "") + myMonth + "_" +
        ((myDay   < 10) ? "0" : "") + myDay   + "-" +
        ((myHour  < 10) ? "0" : "") + myHour  + "_" +
        ((myMin   < 10) ? "0" : "") + myMin   + "_" +
        ((mySec   < 10) ? "0" : "") + mySec;
}

function padStringFront(theString, thePaddingChar, theLength) {
    while (theString.length < theLength) {
        theString = thePaddingChar + theString;
    }
    return theString;
}

function padStringBack(theString, thePaddingChar, theLength) {
    while (theString.length < theLength) {
        theString = theString + thePaddingChar;
    }
    return theString;
}

function hexToNum(theHex) {
    return parseInt(theHex, 16);
}

function numToHex(theNum) {
    return theNum.toString(16);
}

function countNodes(theNode) {
    var myCount = 1;
    for (var i = 0; i < theNode.childNodes.length; ++i) {
        myCount += countNodes(theNode.childNode(i));
    }
    return myCount;
}

function asMemoryString(theBytes) {
    if (theBytes < 1024) {
        return theBytes;
    } else if (theBytes < 1048576) {
        return (theBytes / 1024).toFixed(1) + " K";
    } else if (theBytes < 1073741824) {
        return (theBytes / 1048576).toFixed(1) + " M";
    } else if (theBytes < 1073741824 * 1024) {
        return (theBytes / (1048576 * 1024)).toFixed(1) + " G";
    }
}

// Removes space, newline and tab characters from front and back of a string
function trim(theString) {
    var outString;
    var frontIndex = 0;
    var backIndex = theString.length - 1;
    while (theString.charAt(frontIndex) == " " || theString.charAt(frontIndex) == "\t" ||
           theString.charAt(frontIndex) == "\n" || theString.charAt(frontIndex) == "\r")
    {
        frontIndex++;
    }
    while (theString.charAt(backIndex) == " " || theString.charAt(backIndex) == "\t" ||
           theString.charAt(backIndex) == "\n" || theString.charAt(backIndex) == "\r" )
    {
        backIndex--;
    }
    return frontIndex >= backIndex ? "" : theString.substring(frontIndex, (backIndex + 1));
}
function nextPowerOfTwo(n) {
    var myPowerOfTwo = 1;
    while (myPowerOfTwo < n) {
        myPowerOfTwo <<= 1;
    }
    return myPowerOfTwo;
}


function parseXML(theFilename) {
    if (!fileExists(theFilename)) {
        print("### ERROR: parseXML(): File " + theFilename + " does not exist.");
        return null;
    }
    var myDocument = new Node();
    myDocument.parseFile(theFilename);
    return myDocument.firstChild;
}

function formatTimecode(theTimestamp) {
    var myHours   = 0;
    var myMinutes = 0;
    var mySeconds = 0;

    if (theTimestamp >= 3600) {
        myHours = (theTimestamp - theTimestamp % 3600) / 3600;
        theTimestamp = theTimestamp % 3600;
    }

    if (theTimestamp >= 60) {
        myMinutes = (theTimestamp - theTimestamp % 60) / 60;
        theTimestamp = theTimestamp % 60;
    }

    mySeconds = theTimestamp.toFixed(0);

    var myTC  = (myHours < 10 ? "0" + myHours : myHours) + ":";
    myTC += (myMinutes < 10 ? "0" + myMinutes : myMinutes) + ":";
    myTC += (mySeconds < 10 ? "0" + mySeconds : mySeconds);

    return myTC;
}

function randomBetween(theMin, theMax) {
    return Math.random() * (theMax - theMin) + theMin;
}

function removeElement(theArray, theIndex) {
    return theArray.slice(0, theIndex).concat(theArray.slice(theIndex + 1, theArray.length));
}
