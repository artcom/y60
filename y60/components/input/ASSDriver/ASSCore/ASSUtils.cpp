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

#include "ASSUtils.h"

#include <iostream>

using namespace std;

namespace y60 {

dom::NodePtr
getASSSettings(dom::NodePtr theSettings) {
    dom::NodePtr mySettings;
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
        //unsigned myMax = asl::minimum( unsigned(theBuffer.size()), unsigned(80));
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
