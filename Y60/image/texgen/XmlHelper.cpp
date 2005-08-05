//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include <asl/string_functions.h>

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
    int r,g,b; // Not styleguide conform! 
               // TODO: Start big discussion.
    string s = theNode("#text").nodeValue();
    int numConverted = sscanf (s.c_str(), "%d,%d,%d", &r, &g, &b);
    if (numConverted != 3) {
        string s;
        s = "Field " + asl::as_string (theNode) + " should contain a color value.\n";
        throw (invalid_argument (s.c_str()));
    }
    return PLPixel32 (r,g,b,0);
}


}
