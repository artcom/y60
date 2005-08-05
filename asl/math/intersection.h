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
//    $RCSfile: intersection.h,v $
//
//     $Author: danielk $
//
//   $Revision: 1.5 $
//
// Description: intersections between point, vector, line, plane and other classes
//
//=============================================================================

#ifndef _included_asl_intersection_
#define _included_asl_intersection_

#include "Vector234.h"
#include "linearAlgebra.h"
#include "Matrix4.h"
#include "Point234.h"
#include "Box.h"
#include "Line.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Capsule.h"

namespace asl {

    // see Graphics GEMS 1 Page 297 for some of the following functions

    // Line/Sphere intersection
    template<class Number>
	bool intersection(const Sphere<Number> & s, const Line<Number> & l, Point3<Number> & P1, Point3<Number> & P2) {
		Vector3<Number> G = l.origin - s.center;
		Number a = dot(l.direction,l.direction);
		Number b = 2*dot(l.direction,G);
		Number c = dot(G,G) - s.radius*s.radius;
		Number d = b*b - 4*a*c;
		if (d < 0) {
			return false;
		}
		Number wd = static_cast<Number>(sqrt(d));
		P1 = l.origin + l.direction * (-b + wd)/Number(2)*a;
		P2 = l.origin + l.direction * (-b - wd)/Number(2)*a;
		return true;
	}

    // Line/Line intersection (G.Gems pp 304)
    template<class Number>
	bool intersection(const Line<Number> & l1,const Line<Number> & l2, Point3<Number> & result, 
                        Number & t, Number & s) 
    {
        const Point3<Number> & p1 = l1.origin;
        const Point3<Number> & p2 = l2.origin;
        const Vector3<Number> & v1 = l1.direction;
        const Vector3<Number> & v2 = l2.direction;
        Vector3<Number> cv = cross(v1,v2);
        Number cv2 = dot(cv,cv);
        
        if (almostEqual(cv2,0)) {
            //lines are parallel
            return false;                
        }
        Vector3<Number> p21 = difference(p2,p1);
        t = det3x3(p21,v2,cv) / cv2;
        s = det3x3(p21,v1,cv) / cv2;    

        if (almostEqual(p1 + t*v1,p2 + s*v2)) {
            //lines intersect
            result = p1 + t*v1;
            return true;
        }
        
        //lines are skew
		return false;
    }
    template<class Number>
	bool intersection(const Line<Number> & l1,const Line<Number> & l2, Point3<Number> & result)
    {
        Number t,s;
        return intersection(l1,l2,result,t,s);
    }

    template<class Number>
	bool intersection(const LineSegment<Number> & ls1,const LineSegment<Number> & ls2, 
                        Point3<Number> & result, Number & t, Number & s) 
    {
        Line<Number> l1(ls1.origin, ls1.getDirection());
        Line<Number> l2(ls2.origin, ls2.getDirection());
        bool mayIntersect = intersection(l1, l2, result, t, s);        
        //here we rely on ls1.getDirection() returning l1.end - l1.origin
        return mayIntersect && t >= 0 && t <= 1 && s >= 0 && s <= 1;
    }
    template<class Number>
	bool intersection(const LineSegment<Number> & ls1,const LineSegment<Number> & ls2, Point3<Number> & result)
    {
        Number t,s;
        return intersection(ls1,ls2,result,t,s);
    }
    
    template<class Number>
	bool intersection(const LineSegment<Number> & ls1,const Line<Number> & l2, 
                        Point3<Number> & result, Number & t, Number & s) 
    {
        Line<Number> l1(ls1.origin, ls1.getDirection());
        bool mayIntersect = intersection(l1, l2, result, t, s);        
        //here we rely on ls1.getDirection() returning l1.end - l1.origin
        return mayIntersect && t >= 0 && t <= 1;
    }
    template<class Number>
	bool intersection(const LineSegment<Number> & ls1,const Line<Number> & l2, Point3<Number> & result)
    {
        Number t,s;
        return intersection(ls1,l2,result,t,s);
    }
    
    template<class Number>
	bool intersection(const Line<Number> & l1,const LineSegment<Number> & ls2, 
                        Point3<Number> & result, Number & t, Number & s) 
    {
        return intersection(ls2,l1,result,s,t);
    }
    template<class Number>
	bool intersection(const Line<Number> & l1,const LineSegment<Number> & ls2, Point3<Number> & result)
    {
        Number t,s;
        return intersection(l1,ls2,result,t,s);
    }


    //line segment - ray intersection
    template<class Number>
	bool intersection(const LineSegment<Number> & ls1,const Ray<Number> & r2, 
                        Point3<Number> & result, Number & t, Number & s) 
    {
        Line<Number> l1(ls1.origin, ls1.getDirection());
        const Line<Number> & l2 = asLine(r2);
        bool mayIntersect = intersection(l1, l2, result, t, s);        
        //here we rely on ls1.getDirection() returning l1.end - l1.origin
        return mayIntersect && t >= 0 && t <= 1 && s >= 0;
    }
    template<class Number>
	bool intersection(const LineSegment<Number> & ls1,const Ray<Number> & r2, Point3<Number> & result)
    {
        Number t,s;
        return intersection(ls1,r2,result,t,s);
    }
    
    template<class Number>
	bool intersection(const Ray<Number> & r1,const LineSegment<Number> & ls2, 
                        Point3<Number> & result, Number & t, Number & s) 
    {
        return intersection(ls2,r1,result,s,t);
    }
    template<class Number>
	bool intersection(const Ray<Number> & r1,const LineSegment<Number> & ls2, Point3<Number> & result)
    {
        Number t,s;
        return intersection(r1,ls2,result,t,s);
    }

    //line - ray intersection
    template<class Number>
	bool intersection(const Line<Number> & l1,const Ray<Number> & r2, 
                        Point3<Number> & result, Number & t, Number & s) 
    {
        const Line<Number> & l2 = asLine(r2);
        bool mayIntersect = intersection(l1, l2, result, t, s);        
        //here we rely on ls1.getDirection() returning l1.end - l1.origin
        return mayIntersect && s >= 0;
    }
    template<class Number>
	bool intersection(const Line<Number> & l1,const Ray<Number> & r2, Point3<Number> & result)
    {
        Number t,s;
        return intersection(l1,r2,result,t,s);
    }
    
    template<class Number>
	bool intersection(const Ray<Number> & r1,const Line<Number> & l2, 
                        Point3<Number> & result, Number & t, Number & s) 
    {
        return intersection(l2,r1,result,s,t);
    }
    template<class Number>
	bool intersection(const Ray<Number> & r1,const Line<Number> & l2, Point3<Number> & result)
    {
        Number t,s;
        return intersection(r1,l2,result,t,s);
    }
    


    
    // Line/Plane intersection
    template<class Number>
	bool intersection(const Line<Number> & l,const Plane<Number> & j, Point3<Number> & result, Number & t) {
		Number d = dot(l.direction,j.normal);
        if (almostEqual(d,0)) {
			return false;
        }
		t = -(j.offset + dot(asVector(l.origin),j.normal)) / d;
        result = l.origin + l.direction * t;
		return true;
	}

    // Line/Plane intersection
    template<class Number>
	bool intersection(const Line<Number> & l,const Plane<Number> & j, Point3<Number> & result) {
		Number t;
        return intersection(l,j,result,t);
	}
    template<class Number>
	bool intersection(const Plane<Number> & j, const Line<Number> & l,Point3<Number> & result) {
		Number t;
        return intersection(l,j,result,t);
	}
    // Ray/Plane intersection
    template<class Number>
	bool intersection(const Ray<Number> & l,const Plane<Number> & j, Point3<Number> & result) {
		Number t;
        if (intersection(asLine(l),j,result,t)) {
            return t >= 0;
        }
        return false;
	}

    template<class Number>
	bool intersection(const Plane<Number> & j, const Ray<Number> & l, Point3<Number> & result) {
        return intersection(l,j,result);
    }

	//nearest point to p on a line l
    template<class Number>
	Point3<Number> nearest(const Point3<Number> & p, const Line<Number> & l) {
		Point3<Number> result;
		if (intersection(l, Plane<Number>(l.direction,-dot(asVector(p),l.direction)), result)) {
			return result;
		}
		return p;
	}
    template<class Number>
	Point3<Number> nearest(const Line<Number> & l, const Point3<Number> & p) {
        return nearest(p, l);
    }

    template<class Number>
	Point3<Number> nearest(const Point3<Number> & p, const LineSegment<Number> & l, float * factor=0) {
        Vector3<Number> direction = l.getDirection();
        Number distance = magnitude(l.getDirection());
        Number f = dot(p-l.origin, normalized(direction))/distance;
        if (factor) {
            *factor = f;
        }
        //Number dist = clamp(dot(direction, p-l.origin),0.0f,1.0f);
        return l.origin + direction * clamp(f, 0.0f, 1.0f); 
	}
    
    template<class Number>
	Point3<Number> nearest(const LineSegment<Number> & l, const Point3<Number> & p) {
        return nearest(p,l);
    }

    // point/line distance
    template<class Number>
	Number distance(const Point3<Number> & p, const Line<Number> & l) {
		return distance(p, nearest(p,l));
	}
    template<class Number>
	Number distance(const Line<Number> & l, const Point3<Number> & p) {
		return distance(p, l);
	}

    template<class Number>
	Number distance(const Point3<Number> & p, const LineSegment<Number> & l) {
		return distance(p, nearest(p,l));
	}
    template<class Number>
	Number distance(const LineSegment<Number> & l, const Point3<Number> & p) {
		return distance(p, l);
	}

    // LineSegment/Plane intersection
    template<class Number>
	bool intersection(const LineSegment<Number> & l,const Plane<Number> & j, Point3<Number> & result) {
		Number t;
        Line<Number> myLine(l.origin,l.end);
        if (intersection(myLine,j,result,t)) {
            return (t >= 0) && (t <= distance(l.origin,l.end));
        }
        return false;
	}
    template<class Number>
	bool intersection(const Plane<Number> & j, const LineSegment<Number> & l, Point3<Number> & result) {
        return intersection(l,j,result);
    }

    // Box3/Plane intersection
    // see Tomas Moeller - Real-Time Rendering PG 311
    // returns true if the box intersects the plane, false otherwise.
    // if the result is false theHalfSpace indicates on which side of the plane the box is
    template <class Number>
    bool intersection(const Box3<Number> & theBox, const Plane<Number> & thePlane, int & theHalfSpace) {
        Vector3<Number> myVmin;
        Vector3<Number> myVmax;
        for (unsigned i = 0; i < 3; ++i) {
            if (thePlane.normal[i] >= 0) {
                myVmin[i] = theBox[Box3<Number>::MIN][i];
                myVmax[i] = theBox[Box3<Number>::MAX][i];
            } else {
                myVmin[i] = theBox[Box3<Number>::MAX][i];
                myVmax[i] = theBox[Box3<Number>::MIN][i];
            }
        }
        if ((dot(thePlane.normal, myVmin) + thePlane.offset > 0)) {
            theHalfSpace = 1;
            return false;
        } else if (dot(thePlane.normal, myVmax) + thePlane.offset >= 0) {
            theHalfSpace = 0;
            return true;
        }
        theHalfSpace = -1;
        return false;
    }


    // three plane intersection
    template<class Number>
	bool intersection(const Plane<Number> & p1,
                      const Plane<Number> & p2,
                      const Plane<Number> & p3,
                      Point3<Number> & theResult)
    {
        Number myDet = det3x3(p1.normal,p2.normal,p3.normal);
        if (almostEqual(myDet,0)) {
            return false;
        }
        Vector3<Number> pop1 = - p1.normal * p1.offset;
        Vector3<Number> pop2 = - p2.normal * p2.offset;
        Vector3<Number> pop3 = - p3.normal * p3.offset;
        theResult = (dot(pop1, p1.normal) * cross(p2.normal, p3.normal) +
                     dot(pop2, p2.normal) * cross(p3.normal, p1.normal) +
                     dot(pop3, p3.normal) * cross(p1.normal, p2.normal)) / myDet;
        return true;
    }

    // return index of largest element
    template<class Number>
    unsigned maxindex(const Point3<Number> & v) {
        Number   myMax      = 0;
        unsigned myMaxIndex = 0;
        for (unsigned i = 0; i < v.size(); ++i) {
            Number myValue = fabs(v[i]);
            if (myValue > myMax) {
                myMax = myValue;
                myMaxIndex = i;
            }
        }

        return myMaxIndex;
        //return std::max_element(v.begin(), v.end()) - v.begin();
    }

    // see Graphics GEMS 1 Pg. 393
    template<class Number>
    bool inside(const Triangle<Number> & tri,
                const Vector3<Number> & theNormal,
                const Point3<Number> & P,
                Number & alpha,
                Number & beta)
    {
        unsigned i0 =  maxindex<Number>(theNormal);
        int i1, i2;
        switch (i0) {
            case 0: i1 = 1; i2 = 2; break;
            case 1: i1 = 0; i2 = 2; break;
            case 2: i1 = 0; i2 = 1; break;
        };
        Number u0 = P[i1] - tri[0][i1];
        Number v0 = P[i2] - tri[0][i2];
        Number u1 = tri[1][i1] - tri[0][i1];
        Number u2 = tri[2][i1] - tri[0][i1];
        Number v1 = tri[1][i2] - tri[0][i2];
        Number v2 = tri[2][i2] - tri[0][i2];
        bool hit = false;
        if  (almostEqual(u1,Number(0))) {
            beta = u0/u2;
            if ((beta >= 0) && (beta <= 1)) {
                alpha = (v0 - beta * v2)/v1;
                hit = ((alpha >= 0) && ((alpha+beta) <= 1));
            }
        } else {
            beta = (v0 * u1 - u0 * v1)/(v2 * u1 - u2 * v1);
            if ((beta >= 0) && (beta <= 1)) {
                alpha = (u0 - beta * u2)/u1;
                hit = ((alpha >= 0) && ((alpha+beta) <= 1));
            }
        }
        return hit;
    }

    // see GEMS1 Pg. 393
    //============== Triangle/Line intersection ===================
    //
    // return true and intersection point in "theResult" when they intersects
    template<class Number>
	bool intersection(const Triangle<Number> & theTriangle,
                      const Line<Number> & theLine,
                      Point3<Number> & theResult)
    {
        Plane<Number> myPlane = theTriangle.plane();
        if (intersection(theLine,myPlane,theResult)) {
            Number alpha, beta;
            return inside(theTriangle, myPlane.normal, theResult, alpha, beta);
        }
        return false;
    }
    template<class Number>
	bool intersection(const Line<Number> & theLine,
                      const Triangle<Number> & theTriangle,
                      Point3<Number> & theResult)
    {
        return intersection(theTriangle, theLine, theResult);
    }
    // same as above, but returns also the computed plane normal to avoid recomputation
    template<class Number>
	bool intersection(const Triangle<Number> & theTriangle,
                      const Line<Number> & theLine,
                      Point3<Number> & theResult,
                      Vector3<Number> & theResultNormal)
    {
        Plane<Number> myPlane = theTriangle.plane();
        if (intersection(theLine,myPlane,theResult)) {
            Number alpha, beta;
            if (inside(theTriangle, myPlane.normal, theResult, alpha, beta)) {
                theResultNormal = myPlane.normal;
            }
        }
        return false;
    }
    // same as above but returns interpolated normal from supplied normals
    template<class Number>
	bool intersection(const Triangle<Number> & theTriangle,
                      const Vector3<Vector3<Number> > & theNormals,
                      const Line<Number> & theLine,
                      Point3<Number> & theResult,
                      Vector3<Number> & theResultNormal)
    {
        Plane<Number> myPlane = theTriangle.plane();
        if (intersection(theLine,myPlane,theResult)) {
            Number alpha, beta;
            if (inside(theTriangle, myPlane.normal, theResult, alpha, beta)) {
                theResultNormal = theNormals[0] * (1.0 - (alpha+beta)) +
                                  theNormals[1] * alpha +
                                  theNormals[2] * beta;
                return true;
            }
        }
        return false;
    }


    //============== Triangle/Ray intersection ===================
    //
    // return true and intersection point in "theResult" when they intersects
    template<class Number>
	bool intersection(const Triangle<Number> & theTriangle,
                      const Ray<Number> & theRay,
                      Point3<Number> & theResult)
    {
        Plane<Number> myPlane = theTriangle.plane();
        if (intersection(theRay,myPlane,theResult)) {
            Number alpha, beta;
            return inside(theTriangle, myPlane.normal, theResult, alpha, beta);
        }
        return false;
    }
    template<class Number>
	bool intersection(const Ray<Number> & theRay,
                      const Triangle<Number> & theTriangle,
                      Point3<Number> & theResult)
    {
        return intersection(theTriangle, theRay, theResult);
    }
    // same as above, but returns also the computed plane normal to avoid recomputation
    template<class Number>
	bool intersection(const Triangle<Number> & theTriangle,
                      const Ray<Number> & theRay,
                      Point3<Number> & theResult,
                      Vector3<Number> & theResultNormal)
    {
        Plane<Number> myPlane = theTriangle.plane();
        if (intersection(theRay,myPlane,theResult)) {
            Number alpha, beta;
            if (inside(theTriangle, myPlane.normal, theResult, alpha, beta)) {
                theResultNormal = myPlane.normal;
            }
        }
        return false;
    }
    // same as above but returns interpolated normal from supplied normals
    template<class Number>
	bool intersection(const Triangle<Number> & theTriangle,
                      const Vector3<Vector3<Number> > & theNormals,
                      const Ray<Number> & theRay,
                      Point3<Number> & theResult,
                      Vector3<Number> & theResultNormal)
    {
        Plane<Number> myPlane = theTriangle.plane();
        if (intersection(theRay,myPlane,theResult)) {
            Number alpha, beta;
            if (inside(theTriangle, myPlane.normal, theResult, alpha, beta)) {
                theResultNormal = theNormals[0] * (1.0 - (alpha+beta)) +
                                  theNormals[1] * alpha +
                                  theNormals[2] * beta;
                return true;
            }
        }
        return false;
    }


    //============== Triangle/LineSegment intersection ===================
    //
    // return true and intersection point in "theResult" when they intersects
    template<class Number>
	bool intersection(const Triangle<Number> & theTriangle,
                      const LineSegment<Number> & theLine,
                      Point3<Number> & theResult)
    {
        Plane<Number> myPlane = theTriangle.plane();
        if (intersection(theLine,myPlane,theResult)) {
            Number alpha, beta;
            return inside(theTriangle, myPlane.normal, theResult, alpha, beta);
        }
        return false;
    }
    template<class Number>
	bool intersection(const LineSegment<Number> & theLine,
                      const Triangle<Number> & theTriangle,
                      Point3<Number> & theResult)
    {
        return intersection(theTriangle, theLine, theResult);
    }

   // same as above, but returns also the computed plane normal to avoid recomputation
    template<class Number>
	bool intersection(const Triangle<Number> & theTriangle,
                      const LineSegment<Number> & theLine,
                      Point3<Number> & theResult,
                      Vector3<Number> & theResultNormal)
    {
        Plane<Number> myPlane = theTriangle.plane();
        if (intersection(theLine,myPlane,theResult)) {
            Number alpha, beta;
            if (inside(theTriangle, myPlane.normal, theResult, alpha, beta)) {
                theResultNormal = myPlane.normal;
            }
        }
        return false;
    }
    template<class Number>
	bool intersection(const Triangle<Number> & theTriangle,
                      const Vector3<Vector3<Number> > & theNormals,
                      const LineSegment<Number> & theLine,
                      Point3<Number> & theResult,
                      Vector3<Number> & theResultNormal)
    {
        Plane<Number> myPlane = theTriangle.plane();
        if (intersection(theLine,myPlane,theResult)) {
            Number alpha, beta;
            if (inside(theTriangle, myPlane.normal, theResult, alpha, beta)) {
                theResultNormal = theNormals[0] * (1.0 - (alpha+beta)) +
                                  theNormals[1] * alpha +
                                  theNormals[2] * beta;
                return true;
            }
        }
        return false;
    }

 
    // line/box intersection see http://www.cs.utah.edu/~awilliam/box/
    template<class Number>
    bool intersection(const Box3<Number> & b, const Line<Number> & r, Number & tmin, Number & tmax)
    {
        enum { X = 0, Y = 1, Z = 2 };

        Vector3<Number> invDirection(1,1,1);
        invDirection /= r.direction;

        Vector3<int> sign;
        sign[X] = (invDirection[X] < 0);
        sign[Y] = (invDirection[Y] < 0);
        sign[Z] = (invDirection[Z] < 0);

               tmin  = (b[    sign[X]][X] - r.origin[X]) * invDirection[X];
               tmax  = (b[1 - sign[X]][X] - r.origin[X]) * invDirection[X];
        Number tymin = (b[    sign[Y]][Y] - r.origin[Y]) * invDirection[Y];
        Number tymax = (b[1 - sign[Y]][Y] - r.origin[Y]) * invDirection[Y];

        if ( (tmin > tymax) || (tymin > tmax) ) {
            return false;
        }
        if (tymin > tmin) {
            tmin = tymin;
        }
        if (tymax < tmax) {
            tmax = tymax;
        }
        Number tzmin = (b[    sign[Z]][Z] - r.origin[Z]) * invDirection[Z];
        Number tzmax = (b[1 - sign[Z]][Z] - r.origin[Z]) * invDirection[Z];

        if ( (tmin > tzmax) || (tzmin > tmax) ) {
            return false;
        }
        if (tzmin > tmin) {
            tmin = tzmin;
        }
        if (tzmax < tmax) {
            tmax = tzmax;
        }
        return true;
    }
    template<class Number>
    bool intersection(const Line<Number> & r, const Box3<Number> & b, Number & tmin, Number & tmax) {
        return intersection(b, r, tmin, tmax);
    }
    template<class Number>
    bool intersection(const Box3<Number> & b, const Line<Number> & r)
    {
        Number tmin;
        Number tmax;
        return intersection(b,r,tmin,tmax);
    }
    template<class Number>
    bool intersection(const Line<Number> & r, const Box3<Number> & b) {
        return intersection(b, r);
    }

    // ======== Ray/Box ========
    //
    template<class Number>
    bool intersection(const Box3<Number> & b, const Ray<Number> & r, Number & tmin, Number & tmax)
    {
        if (intersection(b,asLine(r),tmin,tmax)) {
            return tmax > 0;
        }
        return false;
    }
    template<class Number>
    bool intersection(const Ray<Number> & r, const Box3<Number> & b, Number & tmin, Number & tmax) {
        return intersection(b, r, tmin, tmax);
    }
    template<class Number>
    bool intersection(const Box3<Number> & b, const Ray<Number> & r)
    {
        Number tmin;
        Number tmax;
        return intersection(b,r,tmin,tmax);
    }
    template<class Number>
    bool intersection(const Ray<Number> & r, const Box3<Number> & b) {
        return intersection(b, r);
    }

    // ======== LineSegment/Box ========
    //
    template<class Number>
    bool intersection(const Box3<Number> & b, const LineSegment<Number> & r, Number & tmin, Number & tmax)
    {
        Line<Number> myLine(r.origin,r.end);
        if (intersection(b,myLine,tmin,tmax)) {
            return tmax > 0 && tmin < distance(r.origin,r.end);
        }
        return false;
    }
    template<class Number>
    bool intersection(const LineSegment<Number> & r, const Box3<Number> & b, Number & tmin, Number & tmax) {
        return intersection(b, r, tmin, tmax);
    }
    template<class Number>
    bool intersection(const Box3<Number> & b, const LineSegment<Number> & r)
    {
        Number tmin;
        Number tmax;
        return intersection(b,r,tmin,tmax);
    }
    template<class Number>
    bool intersection(const LineSegment<Number> & r, const Box3<Number> & b) {
        return intersection(b, r);
    }
    template<class Number>
    bool visible(const Box3<Number> & theObjectBounds,
                 const Matrix4<Number> & theProjection,
                 const Box3<Number> & theScreen)
    {
        Box3<Number> myScreenSpaceBounds = theObjectBounds * theProjection;
        return myScreenSpaceBounds.intersects(theScreen);
    }

} // asl

#endif
