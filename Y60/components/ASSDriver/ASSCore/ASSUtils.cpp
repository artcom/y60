//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "ASSUtils.h"

#include <iostream>

using namespace std;

namespace y60 {

dom::NodePtr
getASSSettings(dom::NodePtr theSettings) {
    dom::NodePtr mySettings(0);
    if ( theSettings->nodeType() == dom::Node::DOCUMENT_NODE) {
        if (theSettings->childNode(0)->nodeName() == "settings") {
            mySettings = theSettings->childNode(0)->childNode("ASSDriver", 0);
        }
    } else if ( theSettings->nodeName() == "settings") {
        mySettings = theSettings->childNode("ASSDriver", 0);
    } else if ( theSettings->nodeName() == "ASSDriver" ) {
        mySettings = theSettings;
    }
    
    if ( ! mySettings ) {
        throw ASSException(
            std::string("Could not find ASSDriver node in settings: ") +
            as_string( * theSettings), PLUS_FILE_LINE );
    }
    return mySettings;
}

void dumpBuffer(std::vector<unsigned char> & theBuffer) {
    if ( ! theBuffer.empty()) {
        cerr << "=========== buffer: " << endl;
        unsigned myMax = asl::minimum( unsigned(theBuffer.size()), unsigned(80));
        cerr.setf( std::ios::hex, std::ios::basefield );
        cerr.setf( std::ios::showbase );
        for (unsigned i = 0; i < /*myMax*/ theBuffer.size(); ++i) {
            if (theBuffer[i] == 0xff) {
                cerr << endl;
            }
            cerr << int(theBuffer[i]) << " ";
        }
        cerr.unsetf( std::ios::hex );
        cerr.unsetf( std::ios::showbase );
        cerr << endl << "==================" << endl;
    }
}

void fillBufferWithString(std::vector<unsigned char> & theBuffer, const char * theString) {
    string myString( theString );
    for (unsigned i = 0; i < myString.size(); ++i) {
        theBuffer.push_back( myString[i] );
    }
    AC_PRINT << "================";
    dumpBuffer( theBuffer );
}

}
