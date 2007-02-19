//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
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

#include "iostream_functions.h"

#include <iostream>
#include <string>

namespace asl {

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

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfBool & theVector) {
        return printSimpleVector(theStream, theVector);

    }

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfUnsignedInt & theVector) {
        return printSimpleVector(theStream, theVector);
    }

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfSignedInt & theVector) {
        return printSimpleVector(theStream, theVector);
    }

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfFloat & theVector) {
        return printSimpleVector(theStream, theVector);
    }

    std::ostream &
    operator << (std::ostream & theStream, const std::vector<std::string> & theStringVector) {
        theStream << "[";
        for (unsigned i = 0; i < theStringVector.size(); ++i) {
            const std::string & myString = theStringVector[i];
            for (int j = 0; j < myString.size();++j) {
                const char & c = myString[j];
                if (c == '[' || c==']' || c==',' || c=='\\') {
                    theStream << '\\';
                }
                theStream << c;
            }
            if (i < theStringVector.size() - 1) {
                theStream << ",";
            }
        }
        theStream << "]";
        return theStream;
    }

    /**
     * @error Bugzilla 91
     */
    std::istream &
    operator >> (std::istream & theStream, std::vector<std::string> & theStringVector) {
            char myChar;
            theStream >> myChar;

            if (myChar != '[') {
                theStream.setstate(std::ios::failbit);
                return theStream;
            }

            std::string myElement;
            do {
                theStream >> myChar;
                if (myChar =='\\') {
                    theStream >> myChar;
                    myElement+=myChar;
                } else {
                    if (myChar == ']' && theStringVector.empty() && myElement.empty()) {
                        //empty vector of strings
                        break;
                    }
                    if ((myChar == ',') || (myChar == ']')) {
                        theStringVector.push_back(myElement);
                        myElement.clear();
                        if (myChar == ']') {
                            break;
                        }
                    } else {
                        myElement += myChar;
                    }
                }
            } while ( ! theStream.eof());

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
            theStream << theVector.getElement(i)->getString();
        }
        theStream << "]";
        return theStream;
    }
}

