//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

var ourStaticTextNode = new Node();
ourStaticTextNode.parseFile("CONFIG/statictext.xml");
ourStaticTextNode = ourStaticTextNode.firstChild;

function getInternationalizedText(theLanguage, theId) {
    var myNode = ourStaticTextNode.find("//*[@name = '" + theId + "']");
    if (myNode) {
        if (theLanguage in myNode) {
            return myNode[theLanguage];
        } else {
            Logger.error("Static text '" + theId + "' not found for language '" + theLanguage + "'.");
            return "";
        }
    }

    Logger.error("No static text entry for '" + theId + "' found.");
    return "";   
}
