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
//    $RCSfile: Sphere.h,v $
//
//     $Author: david $
//
//   $Revision: 1.11 $
//
// Description: Sphere class
//
//=============================================================================

#ifndef _included_asl_Sphere_
#define _included_asl_Sphere_

#include "Vector234.h"
#include "Point234.h"
#include "numeric_functions.h"
#include <math.h>

namespace asl { 

    /*! @addtogroup aslmath */
    /* @{ */

    template<class Number>
    class Sphere {
    public:
        Sphere() {};
        Sphere(const Sphere<Number> & s) {
            center = s.center;
            radius = s.radius;
        }
        Sphere(const Point3<Number> & theCenter,const Number & theRadius) {
            center = theCenter;
            radius = theRadius;
        }
        Sphere(const Point3<Number> & p1,const Point3<Number> & p2) {
            radius = distance(p1,p2)/2;
            center = asPoint((asVector(p1) + asVector(p2))/2);
        }
        void setCenter(const Point3<Number> & theCenter) {
            center = theCenter;
        }
        void setRadius(Number theRadius) {
            radius = theRadius;
        }

        // Returns the center of a sphere
        const Point3<Number> & getCenter() const {
            return center;
        }
        // Returns the radius of a sphere
        Number getRadius() const {
            return radius;
        }

        // Extends Sphere if necessary to contain given 3D point
        void extendBy(const Point3<Number> & pt) {
            // TODO: check if there is a faster way
            if (!hasPosition()) {
                center = pt;
                radius = 0;
                return;
            }
            Number myDistance = distance(center,pt);
            if (radius >= myDistance) {
                // already contained
                return;
            }
            // new radius to contain both is half of largest diameter
            Number myNewRadius = Number((myDistance + radius)/Number(2) * Number(1.00001));
            Number myRadiusIncrease = myNewRadius - radius;
            // move center by radius increase towards other sphere's center
            center += (pt - center)/myDistance * myRadiusIncrease;
            radius = myNewRadius;
            // alternative:
            // center = (pt - center) * ((1 - (myDistance * radius))/4);
            // radius = (myDistance + radius)/2;
        }

        // Extends Sphere if necessary to contain given sphere
        void extendBy(const Sphere<Number> & s) {
            // TODO: check if there is a faster way
            if (!hasPosition()) {
                // move sphere
                center = s.center;
                radius = s.radius;
                return;
            }
            Number myDistance = distance(center,s.center);
            Number myMaxDistance = myDistance + s.radius;
            if (radius >= myMaxDistance) {
                // already contained
                return;
            }
            if (s.radius >= myDistance + radius) {
                // other sphere contains us
                center = s.center;
                radius = s.radius;
                return;
            }
            // new radius to contain both is half of largest diameter
            Number myNewRadius = (myMaxDistance + radius)/Number(2) * Number(1.00001);
            Number myRadiusIncrease = myNewRadius - radius;
            // move center by radius increase towards other sphere's center
            center += (s.center - center)/myDistance * myRadiusIncrease;
            radius = myNewRadius;
        }

        void makeEmpty() {
            radius = -1;
        }
        bool isEmpty() const {
            return radius <= 0;
        }
        // 0D
        bool hasPosition() const {
            return radius >= 0;
        }
        // 1D
        bool hasSize() const {
            return radius > 0;
        }
        // 2D
        bool hasArea() const {
            return radius > 0;
        }
        // 3D
        bool hasVolume() const {
            return radius > 0;
        }
        Number getSize() const {
            return Number(2) * radius;
        }
        Number getArea() const {
            return Number(4) * Number(PI) * radius * radius;
        }
        Number getVolume() const {
            return Number(4)/Number(3) * Number(PI) * radius * radius * radius;
        }

        // point is inside or on surface
        bool contains(const Point3<Number> & pt) const {
            return distance(pt,center) <= radius; 
        }      
        // point is inside or on surface
        bool touches(const Point3<Number> & pt) const {
            return contains(pt); 
        }
        // point is inside
        bool envelopes(const Point3<Number> & pt) const {
            return distance(pt,center) < radius; 
        }
        // other sphere is inside or same
        bool contains(const Sphere<Number> & s) const {
            return !s.isEmpty() && distance(s.center,center) + s.radius <= radius; 
        } 
        // other sphere is inside
        bool envelopes(const Sphere<Number> & s) const {
            return !s.isEmpty() && distance(s.center,center)+s.radius < radius; 
        } 
        // intersection is non-empty
        bool intersects(const Sphere<Number> & s) const {
            return !s.isEmpty() && !isEmpty() &&
                distance(s.center,center) < s.radius + radius; 
        } 
        // intersect or no distance between closest points
        bool touches(const Sphere<Number> & s) const {
            return !s.isEmpty() && !isEmpty() &&
                distance(s.center,center) <= s.radius + radius; 
        } 
    public:
        Point3<Number> center;
        Number radius;
    };

   
    template<class Number>
    Point3<Number> nearest(const Sphere<Number> & s,const Point3<Number> & P) {
        return s.center + s.radius * normalized(P-s.center);
    }
    template<class Number>
    Point3<Number> nearest(const Point3<Number> & P, const Sphere<Number> & s) {
        return nearest(s, P);
    }

        //==============================================================================
    // stream operator stuff
    //==============================================================================
    template <class Number>
    std::ostream & printSphere (std::ostream & os, const Sphere<Number> & mySphere,
            bool oneElementPerLine = false,
            const char theStartToken = '[',
            const char theEndToken = ']',
            const char theDelimiter = ',')

    {
            os << theStartToken;
            asl::printVector(os, mySphere.center, oneElementPerLine, theStartToken, theEndToken, theDelimiter);
            os << theDelimiter;
            os << mySphere.radius;
            os << theEndToken;
            return os;
    }

    template <class Number>
    std::ostream & operator << (std::ostream & os, const Sphere<Number> & theSphere) {
        if (os.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return printSphere(os, theSphere,
                    0 != os.iword(FixedVectorStreamFormatter::ourOneElementPerLineFlagIndex),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
        } else {
            return printSphere(os, theSphere);
        }
    }

    template <class Number>
    std::istream & parseSphere(std::istream & is, Sphere<Number> & p,
                           const char theStartToken = '[',
                           const char theEndToken = ']',
                           const char theDelimiter = ',')
    {
        char myChar;
        if ((is >> myChar)) {
            if (myChar == theStartToken) {
                if (asl::parseVector(is, p.center, theStartToken, theEndToken, theDelimiter)) {
                    if ((is >> myChar)) {
                        if (myChar == theDelimiter) {
                            if (is >> p.radius) {
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

    template <class T>
    std::istream & operator>>(std::istream & is, Sphere<T> & theSphere) {
        if (is.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return parseSphere(is, theSphere, static_cast<char>(is.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                                        static_cast<char>(is.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                                        static_cast<char>(is.iword(FixedVectorStreamFormatter::ourDelimiterIndex)));
        } else {
            return parseSphere(is, theSphere);
        }
    }

    /* @} */
}
#endif
