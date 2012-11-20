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
//    $RCSfile: XmlHelper.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description:
//
// (CVS log at the bottom of this file)
//

#include "XmlHelper.h"

#include <asl/base/string_functions.h>

#include <stdio.h>
#include <stdexcept>

using namespace std;

namespace TexGen {

int getIntXMLParam (const dom::Node & theNode,
                                      const string & theField)
{
    string s = getRequiredXMLField (theNode, theField);
    int anIntValue;
    int numConverted = sscanf (s.c_str(), "%d", &anIntValue);
    if (numConverted != 1) {
        string s;
        s = "In node " + asl::as_string (theNode) + ":\n" +
            "Field " + asl::as_string (theField) +
            " should be integer.";
        throw (invalid_argument(s.c_str()));
    }

    return anIntValue;
}

float getFloatXMLParam (const dom::Node & theNode,
                                          const std::string & theField)
{
    string s = getRequiredXMLField (theNode, theField);
    float aFloatValue;
    int numConverted = sscanf (s.c_str(), "%f", &aFloatValue);
    if (numConverted != 1) {
        string s;
        s = "In node " + asl::as_string (theNode) + ":\n" +
            "Field " + asl::as_string (theField) +
            " should be float.";
        throw (invalid_argument(s.c_str()));
    }

    return aFloatValue;
}

const string & getRequiredXMLField(const dom::Node & theNode,
                                                 const std::string & theFieldName)
{
    using namespace dom;
    const Node& theField = theNode[theFieldName];
    if (!theField) {
        string s;
        s = "In node " + asl::as_string (theNode) + ":\n" +
            "Field " + theFieldName + " missing.";
        throw (invalid_argument(s.c_str()));
    }
    return theField.nodeValue();
}

int getDefaultedIntXMLParam (const dom::Node & theNode,
                                               const std::string & theField,
                                               int theDefault)
{
    int anIntValue;
    try {
        anIntValue = getIntXMLParam (theNode, theField);
    } catch (invalid_argument) {
        anIntValue = theDefault;
    }

    return anIntValue;
}

PLPixel32 getXMLColorNode (const dom::Node & theNode) {
    int r;
    int g;
    int b;
    int numConverted = sscanf (theNode("#text").nodeValue().c_str(), "%d,%d,%d", &r, &g, &b);
    if (numConverted != 3) {
        const string s = "Field " + asl::as_string (theNode) + " should contain a color value.\n";
        throw invalid_argument(s.c_str());
    }
    return PLPixel32 ( static_cast<PLBYTE>(r)
                     , static_cast<PLBYTE>(g)
                     , static_cast<PLBYTE>(b), 0 );
}


}
