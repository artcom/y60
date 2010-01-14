/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
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
//    $RCSfile: Triangle.h,v $
//
//     $Author: danielk $
//
//   $Revision: 1.13 $
//
// Description: Line class
//
//=============================================================================

#ifndef _included_asl_Triangle_
#define _included_asl_Triangle_

#include "asl_math_settings.h"

#include "Vector234.h"
#include "Point234.h"
#include "Plane.h"

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    template<class Number>
    class Triangle : public TripleOf<Point3<Number> > {
	public:
        typedef TripleOf<Point3<Number> > Base;
		Triangle() {}
		Triangle(const Point3<Number> & p1, const Point3<Number> & p2, const Point3<Number> & p3)
            : Base(p1,p2,p3)
        {}
        Vector3<Number> normal() const {
            return normalized(cross(this->val[1]-this->val[0],this->val[2]-this->val[0]));
        }
        Vector3<Number> backnormal() const {
            return normalized(cross(this->val[2]-this->val[0],this->val[1]-this->val[0]));
        }
        Plane<Number> plane() const {
            Vector3<Number> myNormal = normal();
            return Plane<Number>(myNormal,dot(-this->val[0],myNormal));
        }
        Point3<Number> & edgeOrigin(int theEdge) {
            return this->val[edgeOriginIndex(theEdge)];
        }
        Point3<Number> & edgeEnd(int theEdge) {
            return this->val[edgeEndIndex(theEdge)];
        }
        Number area() const {
            return length(cross(this->val[1]-this->val[0],this->val[2]-this->val[0])) / 2;
        }
        Point3<Number> centroid() const {
            return (this->val[0] + this->val[1] + this->val[2]) / 3;
        }

        const Point3<Number> & edgeOrigin(int theEdge) const {
            return this->val[edgeOriginIndex(theEdge)];
        }
        const Point3<Number> & edgeEnd(int theEdge) const {
            return this->val[edgeEndIndex(theEdge)];
        }
        Vector3<Number> edgeVector(int theEdge) const {
            return edgeEnd(theEdge) - edgeOrigin(theEdge);
        }
        static
        int edgeOriginIndex(int theEdge) {
            return theEdge;
        }
        static
        int edgeEndIndex(int theEdge) {
            return (theEdge+1) % 3;
        }
    };

    template <class Number>
    Triangle<Number> * asTriangle(Point3<Number> * theThreePoints) {
        return reinterpret_cast<Triangle<Number>*>(theThreePoints);
    }
    template <class Number>
    const Triangle<Number> * asTriangle(const Point3<Number> * theThreePoints) {
        return reinterpret_cast<const Triangle<Number>*>(theThreePoints);
    }

    template <class Number>
    Triangle<Number> & asTriangle(Point3<Number> & theThreePoints) {
        return reinterpret_cast<Triangle<Number>&>(theThreePoints);
    }
    template <class Number>
    const Triangle<Number> & asTriangle(const Point3<Number> & theThreePoints) {
        return reinterpret_cast<const Triangle<Number>&>(theThreePoints);
    }
    /* @} */
}
#endif

