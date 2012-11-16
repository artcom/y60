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

#ifndef _ac_y60_rankedfeature_h_
#define _ac_y60_rankedfeature_h_

#include "y60_base_settings.h"

#include <asl/base/Stream.h>

#include <string>
#include <vector>

#if 0
namespace asl {
    class WriteableStream;
    class ReadableStream;
};
#endif

namespace y60 {

    struct RankedFeature {
        RankedFeature() {}
        RankedFeature(float theRanking, const std::string & theFeatures)
            : _myRanking(theRanking), _myFeature(1, theFeatures) {}
        typedef void value_type;
        float _myRanking;
        std::vector<std::string> _myFeature;
    };

    inline
    bool operator==(const RankedFeature & l, const RankedFeature & r) {
        return (l._myRanking == r._myRanking) &&
            (l._myFeature.size() == r._myFeature.size()) &&
            std::equal(l._myFeature.begin(), l._myFeature.end(), r._myFeature.begin());
    }
    inline
    bool operator!=(const RankedFeature & l, const RankedFeature & r) {
        return !(l == r);
    }

    typedef std::vector<RankedFeature> VectorOfRankedFeature;
    /*
    class VectorOfRankedFeature : public std::vector<RankedFeature> {
        public:
        VectorOfRankedFeature() {}
        VectorOfRankedFeature(float theRanking, const std::string & theFeatures)
        {
            RankedFeature myFeature;
            myFeature._myRanking = theRanking;
            myFeature._myFeature = asl::splitString(theFeatures, ",");
            push_back(myFeature);
        }
    };
    */

    Y60_BASE_DECL std::ostream & operator << (
        std::ostream & theStream, const y60::RankedFeature & theRankedFeature
    );
    Y60_BASE_DECL std::istream & operator >> (
        std::istream & theStream, y60::RankedFeature & theRankedFeature
    );
}

namespace dom {
    Y60_BASE_DECL void binarize(
        const y60::RankedFeature & myValue, asl::WriteableStream & theDest
    );
    Y60_BASE_DECL asl::AC_SIZE_TYPE debinarize(
        y60::RankedFeature & myValue, 
        const asl::ReadableStream & theSource, 
        asl::AC_SIZE_TYPE thePos = 0
    );
}

#endif
