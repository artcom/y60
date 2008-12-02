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
//    $RCSfile: Line.h,v $
//
//     $Author: david $
//
//   $Revision: 1.14 $
//
// Description: Line class
//
//=============================================================================

#ifndef _included_asl_Line_
#define _included_asl_Line_

#include "Vector234.h"
#include "Point234.h"

#include <asl/base/Ptr.h>
#include <iostream>

namespace asl {	

    /*! @addtogroup aslmath */
    /* @{ */
    
    // a line is of infinite length and has no start or end point
    template<class Number>
	class Line {
	public:
		Line() {};
		Line(const Line<Number> & l) 
            : origin(l.origin), direction(l.direction) {}
        Line(const Point3<Number> & theOrigin, const Vector3<Number> & theDirection)
            : origin(theOrigin), direction(theDirection) {}
		Line(const Point3<Number> & a, const Point3<Number> & b)
            : origin(a), direction(normalized(b-a)) {}
        void setOrigin(const Point3<Number> & theOrigin) {
            origin = theOrigin;
        }
        void setDirection(const Vector3<Number> & theDirection) {
            direction = theDirection;
        }
        void normalize() {
            direction = normalized(direction);
        }
        Point3<Number> getPoint(Number t) const {
            return origin + direction * t;
        }
	public:
		Point3<Number> origin; // a point on the line
		Vector3<Number> direction; // the direction of the line
	};

    // a ray is a half line of infinite length and starts at <origin>
    template<class Number>
	class Ray {
	public:
		Ray() {};
		Ray(const Ray<Number> & l) 
            : origin(l.origin), direction(l.direction) {}
        Ray(const Point3<Number> & theOrigin, const Vector3<Number> & theDirection)
            : origin(theOrigin), direction(theDirection) {}
		Ray(const Point3<Number> & a, const Point3<Number> & b)
            : origin(a), direction(normalized(b-a)) {}
        void setOrigin(const Point3<Number> & theOrigin) {
            origin = theOrigin;
        }
        void setDirection(const Vector3<Number> & theDirection) {
            direction = theDirection;
        }
        void normalize() {
            direction = normalized(direction);
        }
        Point3<Number> getPoint(Number t) const {
            return origin + direction * t;
        }
	public:
		Point3<Number> origin; // a point on the line
		Vector3<Number> direction; // the direction of the line
	};

    // line and ray have the same implementation, but are something different
    // especially when making intersection test
    template <class Number>
    const Line<Number> & asLine(const Ray<Number> & theRay) {
        return reinterpret_cast<const Line<Number> &>(theRay);
    }

    // a finite length line
    template<class Number>
	class LineSegment {
	public:
		LineSegment() {};
		LineSegment(const LineSegment<Number> & l) 
            : origin(l.origin), end(l.end) {}
        LineSegment(const Point3<Number> & theOrigin, const Point3<Number> & theEnd)
            : origin(theOrigin), end(theEnd) {}
        void setOrigin(const Point3<Number> & theOrigin) {
            origin = theOrigin;
        }
        void setEnd(const Vector3<Number> & theEnd) {
            end = theEnd;
        }
        Point3<Number> getPoint(Number t) const {
            return origin + getDirection() * t;
        }
        Vector3<Number> getDirection() const {
            return end-origin;
        }
	public:
		Point3<Number> origin; // origin off the segment
		Point3<Number> end; // the end of the segment
	};
    template <class Number>
    LineSegment<Number> * asLineSegment(Point3<Number> * theTwoPoints) {
        return reinterpret_cast<LineSegment<Number>*>(theTwoPoints);
    }
    template <class Number>
    const LineSegment<Number> * asLineSegment(const Point3<Number> * theTwoPoints) {
        return reinterpret_cast<LineSegment<Number>*>(theTwoPoints);
    }
    
    template <class Number>
    LineSegment<Number> & asLineSegment(Point3<Number> & theTwoPoints) {
        return reinterpret_cast<LineSegment<Number>&>(theTwoPoints);
    }
    template <class Number>
    const LineSegment<Number> & asLineSegment(const Point3<Number> & theTwoPoints) {
        return reinterpret_cast<const LineSegment<Number>&>(theTwoPoints);
    }

    //==============================================================================
    // stream operator stuff
    //==============================================================================
    template <class VECTOR_A, class VECTOR_B>
    std::ostream & printTwoVectors(std::ostream & os, const VECTOR_A & myVectorA, const VECTOR_B & myVectorB,
            bool oneElementPerLine = false,
            const char theStartToken = '[',
            const char theEndToken = ']',
            const char theDelimiter = ',')

    {
            os << theStartToken;
            asl::printVector(os, myVectorA, oneElementPerLine, theStartToken, theEndToken, theDelimiter);
            os << theDelimiter;
            asl::printVector(os, myVectorB, oneElementPerLine, theStartToken, theEndToken, theDelimiter);
            os << theEndToken;
            return os;
    }
    template <class VECTOR_A, class VECTOR_B>
    std::ostream & printTwoVectorsFormatted(std::ostream & os, const VECTOR_A & myVectorA, const VECTOR_B & myVectorB) {
        if (os.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return printTwoVectors(os, myVectorA, myVectorB,
                    0 != os.iword(FixedVectorStreamFormatter::ourOneElementPerLineFlagIndex),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
        } else {
            return printTwoVectors(os, myVectorA, myVectorB);
        }
    }

    template <class VECTOR_A, class VECTOR_B>
    std::istream & parseTwoVectors(std::istream & is, VECTOR_A & myVectorA, VECTOR_B & myVectorB,
                           const char theStartToken = '[',
                           const char theEndToken = ']',
                           const char theDelimiter = ',')
    {
        char myChar;
        if ((is >> myChar)) {
            if (myChar == theStartToken) {
                if (asl::parseVector(is, myVectorA, theStartToken, theEndToken, theDelimiter)) {
                    if ((is >> myChar)) {
                        if (myChar == theDelimiter) {
                            if (asl::parseVector(is, myVectorB, theStartToken, theEndToken, theDelimiter)) {
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

    template <class VECTOR_A, class VECTOR_B>
    std::istream & parseTwoVectorsFormatted(std::istream & is, VECTOR_A & myVectorA, VECTOR_B & myVectorB) {
        if (is.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return parseTwoVectors(is, myVectorA, myVectorB,
                static_cast<char>(is.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                static_cast<char>(is.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                static_cast<char>(is.iword(FixedVectorStreamFormatter::ourDelimiterIndex)));
        } else {
            return parseTwoVectors(is, myVectorA, myVectorB);
        }
    }
    template <class Number>
    std::ostream & operator << (std::ostream & os, const Line<Number> & theLine) {
        return printTwoVectorsFormatted(os, theLine.origin, theLine.direction);
    }
    template <class Number>
    std::istream & operator>>(std::istream & is, Line<Number> & theLine) {
        return parseTwoVectorsFormatted(is, theLine.origin, theLine.direction);
    }
    template <class Number>
    std::ostream & operator << (std::ostream & os, const Ray<Number> & theRay) {
        return printTwoVectorsFormatted(os, theRay.origin, theRay.direction);
    }
    template <class Number>
    std::istream & operator>>(std::istream & is, Ray<Number> & theRay) {
        return parseTwoVectorsFormatted(is, theRay.origin, theRay.direction);
    }
    template <class Number>
    std::ostream & operator << (std::ostream & os, const LineSegment<Number> & theLineSegment) {
        return printTwoVectorsFormatted(os, theLineSegment.origin, theLineSegment.end);
    }
    template <class Number>
    std::istream & operator>>(std::istream & is, LineSegment<Number> & theLineSegment) {
        return parseTwoVectorsFormatted(is, theLineSegment.origin, theLineSegment.end);
    }

    /* @} */

    typedef Ptr< LineSegment<float> > LineSegment3fPtr;


}

    
#endif

