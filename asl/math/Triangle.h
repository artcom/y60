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

