
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
//   $RCSfile: convert_x60_EulerToQuaternion.js,v $
//   $Author: danielk $
//   $Revision: 1.2 $
//   $Date: 2005/04/01 17:16:03 $
//
//
//=============================================================================
use("Y60JSSL.js");

function convertFile(theFilename) {
    print("convert file : " + theFilename);
    var myXmlDoc = Node.createDocument();
    myXmlDoc.parseFile(theFilename);
    //var myWorld = getDescendantByTagName(myXmlDoc.childNode(0), "world", true);
    var myWorld = myXmlDoc.childNode(0).getNodesByTagName( "world", true)[0];
    var myNodes = getDescendantsByAttributeName(myWorld, "orientation", true);
    print(myNodes.length);
    for (var myNodeIndex = 0; myNodeIndex < myNodes.length; myNodeIndex++) {
        var myOrientation = stringToArray(myNodes[myNodeIndex].orientation);
        print(myOrientation);
        var myOrientatonVec = new Vector3f(myOrientation); 
        var myNewQuaternion = Quaternionf.createFromEuler(myOrientatonVec);
        myNodes[myNodeIndex].orientation = myNewQuaternion;
        print("convert euler orien: " + myOrientatonVec +" -> " + myNewQuaternion);
    }
    myXmlDoc.saveFile(theFilename + "_quaternialized");
}

try {
    var myArguments = String(arguments).split(",");
    for (var myIndex = 0; myIndex < myArguments.length; myIndex++) {
        var myFileToConvert = myArguments[myIndex];
        convertFile(myFileToConvert);        
    }
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(1);
}

