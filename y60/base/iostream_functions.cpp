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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $Id: iostream_functions.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: iostream_functions.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//
//=============================================================================

//own header
#include "iostream_functions.h"


#include <iostream>
#include <string>

using namespace asl;

namespace y60 {

    template <class T>
    std::ostream &
    printSimpleVector(std::ostream & theStream, const T & theVector) {
        theStream << "[";
        for (unsigned i = 0; i < theVector.size(); ++i) {
            theStream << theVector[i];
            if (i < theVector.size() - 1) {
                theStream << ",";

                if (i && ((i % 24) == 0)) {
                    theStream << std::endl;
                }
            }
        }
        theStream << "]";
        return theStream;
    }
}
namespace std {

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfBool & theVector) {
        return y60::printSimpleVector(theStream, theVector);

    }

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfUnsignedInt & theVector) {
        return y60::printSimpleVector(theStream, theVector);
    }

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfSignedInt & theVector) {
        return y60::printSimpleVector(theStream, theVector);
    }

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfFloat & theVector) {
        return y60::printSimpleVector(theStream, theVector);
    }

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfString & theStringVector) {
        theStream << "[";
        for (unsigned i = 0; i < theStringVector.size(); ++i) {
            const std::string & myString = theStringVector[i];
            theStream << VECTOR_OF_STRING_DELIMITER;
            for (std::string::size_type j = 0; j < myString.size();++j) {
                const char & c = myString[j];
                if (c == VECTOR_OF_STRING_DELIMITER || c == '[' || c == ']' || c == ',' || c == '\\') {
                    theStream << '\\';
                }
                theStream << c;
            }
            theStream << VECTOR_OF_STRING_DELIMITER;
            if (i < theStringVector.size() - 1) {
                theStream << ",";
            }
        }
        theStream << "]";
        return theStream;
    }

    std::ostream &
    operator << (std::ostream & theStream, const std::vector<const char *> & theStringVector) {
        theStream << "[";
        for (unsigned i = 0; i < theStringVector.size(); ++i) {
            if (theStringVector[i]) {
                std::string myString(theStringVector[i]);
                theStream << VECTOR_OF_STRING_DELIMITER;
                for (std::string::size_type j = 0; j < myString.size();++j) {
                    const char & c = myString[j];
                    if (c == VECTOR_OF_STRING_DELIMITER || c == '[' || c == ']' || c == ',' || c == '\\') {
                        theStream << '\\';
                    }
                    theStream << c;
                }
                theStream << VECTOR_OF_STRING_DELIMITER;
                if (i < theStringVector.size() - 1) {
                    theStream << ",";
                }
            }
        }
        theStream << "]";
        return theStream;
    }

    std::istream &
    operator >> (std::istream & theStream, y60::VectorOfString & theStringVector) {
        char myChar;
        theStream >> myChar;

        if (myChar != '[') {
            theStream.setstate(std::ios::failbit);
            return theStream;
        }

        std::string myElement;
        bool myStartFlag   = false;
        bool myElementFlag = false;
        do {
            theStream >> myChar;
            if ((myChar == ']' || myChar == ',') && !myStartFlag) {
                if (myElementFlag) {
                    theStringVector.push_back(myElement);
                    myElement.clear();
                }
                if (myChar == ']') {
                    break;
                }
            } else if (myChar != VECTOR_OF_STRING_DELIMITER && myStartFlag) {
                if (myChar == '\\') {
                    theStream >> myChar;
                }
                myElement += myChar;
            } else if (myChar == VECTOR_OF_STRING_DELIMITER) {
                myStartFlag = !myStartFlag;
                if (myStartFlag) {
                    myElementFlag = true;
                }
            } else {
                theStream.setstate(std::ios::failbit);
                return theStream;
            }
        } while (!theStream.eof());

        if (myChar != ']') {
            theStream.setstate(std::ios::failbit);
        }

        return theStream;
    }
    
    std::ostream &
    operator << (std::ostream & theStream, const dom::ResizeableVector & theVector) {
        theStream << "[";
        for (unsigned i = 0; i < theVector.length(); ++i) {
            if (i) {
                theStream << ",";
            }
            theStream << theVector.getItem(i)->getString();
        }
        theStream << "]";
        return theStream;
    }
}

