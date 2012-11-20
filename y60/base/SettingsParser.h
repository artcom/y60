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

#ifndef __SETTINGS_PARSER_UTILS_INCLUDED__
#define __SETTINGS_PARSER_UTILS_INCLUDED__

#include "y60_base_settings.h"

namespace y60 {

template <class T>
bool
getConfigSetting(dom::NodePtr theSettings, const std::string & theName, T & theValue,
                 const T & theDefault)
{
    dom::NodePtr myNode = theSettings->childNode( theName );
    if ( ! myNode ) {
        AC_WARNING << "No node named '" << theName << "' found in configuration. "
                   << "Adding default value '" << theDefault << "'";
        myNode = theSettings->appendChild( dom::NodePtr( new dom::Element( theName )));
        dom::NodePtr myTextNode = myNode->appendChild( dom::NodePtr( new dom::Text() ));
        myTextNode->nodeValue( asl::as_string( theDefault ));
    }

    if ( myNode->childNodesLength() != 1 ) {
        throw asl::Exception(std::string("Configuration node '") + theName +
            "' must have exactly one child.", PLUS_FILE_LINE);
    }
    if ( myNode->childNode("#text") ) {
        T myNewValue = asl::as<T>( myNode->childNode("#text")->nodeValue());
        if (myNewValue != theValue) {
            theValue =  myNewValue;
            return true;
        } else {
            return false;
        }
    } else {
        throw asl::Exception(std::string("Node '") + myNode->nodeName() +
                "' does not have a text child." , PLUS_FILE_LINE);
    }
}

template <class Enum>
bool
getConfigSetting(dom::NodePtr theSettings, const std::string & theName, Enum & theValue,
                 typename Enum::Native theDefault)
{
    return getConfigSetting( theSettings, theName, theValue, Enum( theDefault ));
}

inline
bool
getConfigSettingString(dom::NodePtr theSettings, const std::string & theName, std::string & theValue, const std::string & theDefault)
{
    return getConfigSetting( theSettings, theName, theValue, theDefault);
}

}



#endif // __SETTINGS_PARSER_UTILS_INCLUDED__
