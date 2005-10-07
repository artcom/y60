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
//   $Id: RankedFeature.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: RankedFeature.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: This file contains all the wellknown names for the
//               SOM
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_y60_rankedfeature_h_
#define _ac_y60_rankedfeature_h_

#include <asl/Stream.h>

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

    //typedef std::vector<RankedFeature> VectorOfRankedFeature;
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

}

namespace asl {
    
    std::ostream & operator << (std::ostream & theStream, const y60::RankedFeature & theRankedFeature);
    std::istream & operator >> (std::istream & theStream, y60::RankedFeature & theRankedFeature);
}

namespace dom {
    void binarize(const y60::RankedFeature & myValue, asl::WriteableStream & theDest);
	asl::AC_SIZE_TYPE debinarize(y60::RankedFeature & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0);
}

#endif
