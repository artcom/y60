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

//own header
#include "RankedFeature.h"

#include <asl/base/Block.h>
#include <asl/dom/Value.h>

#include <string>
#include <vector>

using namespace asl;

namespace y60 {

    std::ostream &
    operator << (std::ostream & theStream, const RankedFeature & theRankedFeature) {
        theStream << theRankedFeature._myRanking;
        theStream << "[";
        for (unsigned i = 0; i < theRankedFeature._myFeature.size(); ++i) {
            theStream << theRankedFeature._myFeature[i];
            if (i < theRankedFeature._myFeature.size() - 1) {
                theStream << ",";
            }
        }
        theStream << "]";
        return theStream;
    }

    std::istream &
    operator >> (std::istream & theStream, RankedFeature & theRankedFeature) {
            theStream >> theRankedFeature._myRanking;

            if (theStream.fail()) {
                return theStream;
            }

            char myChar;
            theStream >> myChar;

            if (myChar != '[') {
                theStream.setstate(std::ios::failbit);
                return theStream;
            }

            std::string myElement;
            do {
                theStream >> myChar;
                if ((myChar == ',') || (myChar == ']')) {
                    theRankedFeature._myFeature.push_back(myElement);
                    myElement.clear();
                    if (myChar == ']') {
                        break;
                    }
                } else {
                    myElement += myChar;
                }
            } while ( ! theStream.eof());

            if (myChar != ']') {
                theStream.setstate(std::ios::failbit);
            }

            return theStream;
    }
}

namespace dom {
    void binarize(const y60::RankedFeature & myValue, asl::WriteableStream & theDest) {
        theDest.appendFloat32(myValue._myRanking);
        dom::binarize(myValue._myFeature,theDest);
    };

    asl::AC_SIZE_TYPE debinarize(y60::RankedFeature & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
        thePos = theSource.readFloat32(myValue._myRanking,thePos);
        return dom::debinarize(myValue._myFeature,theSource,thePos);
    };
}
