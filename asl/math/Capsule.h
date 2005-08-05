//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: Capsule.h,v $
//
//     $Author: david $
//
//   $Revision: 1.7 $
//
// Description: Line class
//
//=============================================================================

#ifndef _included_asl_Capsule_
#define _included_asl_Capsule_

#include "Vector234.h"
#include "Line.h"

#include <iostream>

namespace asl {	
  
    /*! @addtogroup aslmath */
    /*  @{ */

    // a finite length line
    template<class Number>
	class Capsule {
	public:
		Capsule() {};
		Capsule(const Capsule<Number> & l) : segment(l.segment), radius(l.radius) {}
        Capsule(const Point3<Number> & theOrigin, const Point3<Number> & theEnd, Number theRadius)
            : segment(theOrigin, theEnd), radius(theRadius) {}
        void setSegment(const Point3<Number> & theSegment) {
            segment = theSegment;
        }
        void setOrigin(const Point3<Number> & theOrigin) {
            segment.origin = theOrigin;
        }
        void setEnd(const Vector3<Number> & theEnd) {
            segment.end = theEnd;
        }
        void setRadius(const Vector3<Number> & theEnd) {
            segment.end = theEnd;
        }
        Vector3<Number> getDirection() const {
            return segment.getDirection();
        }
        LineSegment<Number> getAxisSegment() {
            Vector3<Number> myCap = normalized(getDirection()) * radius;
            return LineSegment<Number>(segment.origin - myCap, segment.end + myCap);
        }
	public:
		LineSegment<Number> segment;
		Number radius;
	};
   
    //==============================================================================
    // stream operator stuff
    //==============================================================================
    template <class Number>
    std::ostream & printCapsule(std::ostream & os, const LineSegment<Number> & theSegment, Number theRadius,
            bool oneElementPerLine = false,
            const char theStartToken = '[',
            const char theEndToken = ']',
            const char theDelimiter = ',')

    {
            os << theStartToken;
            os << theSegment;
            os << theDelimiter;
            os << theRadius;
            os << theEndToken;
            return os;
    }
    template <class Number>
    std::ostream & printCapsuleFormatted(std::ostream & os, const LineSegment<Number> & theSegment, Number theRadius) {
        if (os.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return printCapsule(os, theSegment, theRadius,
                    os.iword(FixedVectorStreamFormatter::ourOneElementPerLineFlagIndex),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
        } else {
            return printCapsule(os, theSegment, theRadius);
        }
    }

    template <class Number>
    std::istream & parseCapsule(std::istream & is, const LineSegment<Number> & theSegment, Number & theRadius,
                           const char theStartToken = '[',
                           const char theEndToken = ']',
                           const char theDelimiter = ',')
    {
        char myChar;
        if ((is >> myChar)) {
            if (myChar == theStartToken) {
                if (is >> theSegment) {
                    if ((is >> myChar)) {
                        if (myChar == theDelimiter) {
                            if (is >> theRadius) {
                                if (is >> myChar) {
                                    if (myChar == theEndToken) {
                                        return is;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        is.setstate(std::ios::failbit);
        return is;
    }

    template <class Number>
    std::istream & parseCapsuleFormatted(std::istream & is, const LineSegment<Number> & theSegment, Number & theRadius) {
        if (is.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return parseCapsule(is, theSegment, theRadius,
                static_cast<char>(is.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                static_cast<char>(is.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                static_cast<char>(is.iword(FixedVectorStreamFormatter::ourDelimiterIndex)));
        } else {
            return parseCapsule(is, theSegment, theRadius);
        }
    }
    template <class Number>
    std::ostream & operator << (std::ostream & os, const Capsule<Number> & theCapsule) {
        return printCapsuleFormatted(os, theCapsule.segment, theCapsule.radius);
    }
    template <class Number>
    std::istream & operator>>(std::istream & is, Capsule<Number> & theCapsule) {
        return parseCapsuleFormatted(is, theCapsule.segment, theCapsule.radius);
    }

    /* @} */

}
#endif

