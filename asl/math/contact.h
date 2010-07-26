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
//    $RCSfile: contact.h,v $
//
//     $Author: martin $
//
//   $Revision: 1.7 $
//
// Description: point, vector, line, plane and matrix classes and function
//
//=============================================================================


#ifndef _included_asl_contact_h_
#define _included_asl_contact_h_

#include "asl_math_settings.h"

#include "Vector234.h"
#include "Point234.h"
#include "Line.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "intersection.h"
#include "numeric_functions.h"

namespace asl {

    /////////////////////////////////////////////////////////////////////////////////////////
    // calculate first at last contact positions of a swept sphere and an edge
    // x0: relative distance the sphere origin moves to first contact, 0 = origin, 1 = end
    //     position of center at contact point = theSphere.origin + theMotionVector * x0
    // f0: relative distance of the first contact point on the edge
    //     position of contact point = theEdge.origin + (theEdge.end - theEdge.origin) * f0
    // x1: relative distance the sphere origin moves to last contact, 0 = origin, 1 = end
    //     position of center at contact point = theSphere.origin + theMotionVector * x1
    // f1: relative distance of the last contact point on the edge
    //     position of contact point = theEdge.origin + (theEdge.end - theEdge.origin) * f1
    // all return value will be in the range [0..1]
    // returns number of contacts :
    //     0 = no valid return value,
    //     1 = x0, f0 valid
    //     2 = x0, f0, x1, f1 valid
    template <class Number>
    int contacts(const Sphere<Number> & theSphere,
                const Vector3<Number> & theMotionVector,
                const LineSegment<Number> & theEdge,
                Number & x0, // relative distance the sphere origin moved, 0 = origin, 1 = end
                Number & x1, // relative distance the sphere origin moved, 0 = origin, 1 = end
                Number & f0, // distance of contact point from Edge origin, 0 = origin, 1 = end
                Number & f1) // distance of contact point from Edge origin, 0 = origin, 1 = end
    {
        Vector3<Number> myOriginToPoint = theEdge.origin - theSphere.center;
        Vector3<Number> myEdgeVector = theEdge.end - theEdge.origin;

        Number myEdgeSquared = dot(myEdgeVector, myEdgeVector);
        Number dotEdgeAxisVector = dot(myEdgeVector, theMotionVector);
        Number a = myEdgeSquared * -dot(theMotionVector, theMotionVector) + dotEdgeAxisVector*dotEdgeAxisVector;
        Number dotEdgeOriginToPoint = dot(myEdgeVector, myOriginToPoint);

        if (almostEqual(a,Number(0))) { // segment and MotionVector are parallel
            if (distance(theSphere.center, Line<Number>(theEdge.origin, theEdge.end)) > theSphere.radius) {
                return 0;
            }
            x0 = 0;
            x1 = 1;
        } else {
            Number b = myEdgeSquared * Number(2) * dot(theMotionVector, myOriginToPoint) -
                        Number(2) * dotEdgeAxisVector * dotEdgeOriginToPoint;
            Number c = myEdgeSquared * (theSphere.radius * theSphere.radius - dot(myOriginToPoint, myOriginToPoint))
                        + dotEdgeOriginToPoint * dotEdgeOriginToPoint;

            int mySolutions = solveQuadratic(a, b, c, x0, x1);
            if (mySolutions==0) {
                return 0;
            }
            if (x0 > x1) {
                std::swap(x0, x1);
            }
        }

        int mySolutions = 0;
        f0 = (dotEdgeAxisVector * x0 - dotEdgeOriginToPoint) / myEdgeSquared;
        if ((f0 >= 0) && (f0 <= 1)) {
            // first solution is in bounds
            ++mySolutions;
        }
        f1 = (dotEdgeAxisVector * x1 - dotEdgeOriginToPoint) / myEdgeSquared;
        if ((f1 >= 0) && (f1 <= 1)) {
            // second solution is in bounds
            ++mySolutions;
            if (mySolutions == 1) {
                // and first solution was out-of-bounds
                x0 = x1;
                f0 = f1;
            }
        } else {
            if (mySolutions == 1) {
                // first solution was in-bounds and second out-of-bounds
                x1 = x0;
                f1 = f0;
            }
        }

        return mySolutions;
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    // calculate first at last contact positions of a swept sphere and a vertex
    // theMin.t: relative distance the sphere origin moves to first contact
    //     position of center at contact point = theSphere.origin + theMotionVector * theMin.t
    // theMax.t: relative distance the sphere origin moves to first contact
    //     position of center at contact point = theSphere.origin + theMotionVector * theMax.t
    // returns:
    //      0 = no contact, theMin.t and theMax.t are invalid
    //      1 = theMin.t and theMax.t are the same
    //      2 = theMin.t and theMax.t are different
    template <class Number>
    int contacts(const Sphere<Number> & theSphere,
                 const Vector3<Number> & theMotionVector,
                 const Point3<Number> & theVertex,
                 Number & theMin_t, // relative distance the sphere origin moved, 0 = origin, 1 = end, <0= before first sphere, >1 beyond last sphere
                 Number & theMax_t) // relative distance the sphere origin moved, 0 = origin, 1 = end, <0= before first sphere, >1 beyond last sphere
   {
        Number a = dot(theMotionVector,theMotionVector);
        Vector3<Number> myVertexToOrigin = theSphere.center - theVertex;
        Number b = Number(2) * dot(theMotionVector, myVertexToOrigin);
        Number c = dot(myVertexToOrigin, myVertexToOrigin) - theSphere.radius * theSphere.radius;
        int mySolutions = solveQuadratic(a, b, c, theMin_t, theMax_t);
        if (!mySolutions) {
            return false;
        }

        if (theMin_t > theMax_t) {
            std::swap(theMin_t, theMax_t);
        }
        return mySolutions;
    }

    template <class Number>
    int clipContacts(Number & theMin_t, Number & theMax_t, Number theMaxRange = Number(1)) {

        if (theMin_t < 0 && theMax_t < 0) {
            return 0;
        }
        if (theMin_t > theMaxRange && theMax_t > theMaxRange) {
            return 0;
        }

        // clamp to [0,theMaxRange]
        theMin_t = maximum(Number(0), theMin_t);

        theMax_t = minimum(theMaxRange, theMax_t);

        if (almostEqual(theMin_t, theMax_t)) {
            return 1;
        }

        return 2;
    }

    enum ContactType { NO_CONTACT = 0, EMBEDDED, SURFACE_CONTACT, EDGE_CONTACT, VERTEX_CONTACT };


    // do not need to call directly; called from a loop to find min and max
    // a SURFACE_CONTACT will always be the first or last contact, edge and
    // vertex contact positions are always between primitive plane contact
    // positions
    template<class Number>
	void sortContacts(int i, int theSolutions, ContactType newContact, Number newT0, Number newT1,
                      ContactType & theMin_contactType, ContactType & theMax_contactType,
                      Number & theMin_t,  Number & theMax_t,
                      int & theMinIndex, int & theMaxIndex)
    {
        if (theSolutions > 0) {
            if (theMin_contactType != SURFACE_CONTACT) {
                if (newT0 < theMin_t || theMin_contactType == NO_CONTACT) {
                    theMin_t = newT0;
                    theMin_contactType = newContact;
                    theMinIndex = i;
                }
            }
            if (theMax_contactType != SURFACE_CONTACT) {
                if (newT0 > theMax_t || theMax_contactType == NO_CONTACT) {
                    theMax_t = newT0;
                    theMax_contactType = newContact;
                    theMaxIndex = i;
                }
            }
            if (theSolutions > 1) {
                if (theMax_contactType != SURFACE_CONTACT) {
                    if (newT1 > theMax_t || theMax_contactType == NO_CONTACT) {
                        theMax_t = newT1;
                        theMax_contactType = newContact;
                        theMaxIndex = i;
                    }
                }
            }
        }
    }

    template<class Number>
    struct SweptSphereContact {
        Number t; // distance of sphere center from Sphere origin, 0 = origin, 1 = end
        Point3<Number> contactPoint;
        Vector3<Number> contactNormal;
        Vector3<Number> contactSphereCenter;
        Vector3<Number> contactSphereNormal;
        ContactType contactType;
        SweptSphereContact() : t(0), contactPoint(), contactNormal(), contactSphereCenter(), contactSphereNormal(), contactType(NO_CONTACT) {}
    };

    // find the first and last contact with a triangle
    // see Kasper Fauerby, Improved Collision Detection and Response, http://www.peroxide.dk
    template<class Number>
	int contacts(const Sphere<Number> & theSphere,
	             const Vector3<Number> & theMotionVector,
                 const Triangle<Number> & theTriangle,
                 const Vector3<Vector3<Number> > * theNormals,
                 SweptSphereContact<Number> & theMin,
                 SweptSphereContact<Number> & theMax)
    {
        theMin.contactType = NO_CONTACT;
        theMax.contactType = NO_CONTACT;
        Plane<Number> myTrianglePlane = theTriangle.plane();
        Number myOriginPlaneDistance = signedDistance(theSphere.center, myTrianglePlane);
        Number dotPlaneNormalAxis = dot(myTrianglePlane.normal, theMotionVector);

        if (dotPlaneNormalAxis != 0) {
            theMin.t = (theSphere.radius - myOriginPlaneDistance) / dotPlaneNormalAxis;
            theMax.t = (-theSphere.radius - myOriginPlaneDistance) / dotPlaneNormalAxis;
            if ((theMin.t < 0) && (theMin.t > 1) && (theMax.t < 0) && (theMax.t > 1)) {
                // whole Sphere in front or behind triangle plane
                return 0;
            }
            if (theMin.t > theMax.t) {
                std::swap(theMin.t, theMax.t);
            }
            // theMin.t = clamp(theMin.t, Number(0), Number(1));
            // theMax.t = clamp(theMax.t, Number(0), Number(1));
        } else {
            // triangle-plane and sweep-vector are parallel
            if (fabs(myOriginPlaneDistance) <= theSphere.radius) {
                theMin.t = 0;
                theMax.t = 1;
            } else {
                // too far away
                return 0;
            }
        }

        {
            if (theMin.t == 0) {
                theMin.contactPoint = nearest(theSphere.center, myTrianglePlane);
            } else {
                theMin.contactPoint = theSphere.center - (myTrianglePlane.normal * theSphere.radius) + (theMotionVector * theMin.t);
            }
            Number alpha0, beta0;
            if (inside(theTriangle, myTrianglePlane.normal, theMin.contactPoint, alpha0, beta0)) {
                if (theNormals) {
                    theMin.contactNormal = (*theNormals)[0] * (1.0 - (alpha0+beta0)) +
                                                (*theNormals)[1] * alpha0 +
                                                (*theNormals)[2] * beta0;
                } else {
                    theMin.contactNormal = myTrianglePlane.normal;
                }

                theMin.contactSphereNormal = -theMin.contactNormal;
                theMin.contactSphereCenter = theSphere.center+(theMotionVector * theMin.t);
                if (theMin.t == 0) {
                    theMin.contactType = EMBEDDED;
                } else {
                    theMin.contactType = SURFACE_CONTACT;
                }
            }
        }
        {
            if (theMax.t == 1) {
                theMax.contactPoint = nearest(theSphere.center+theMotionVector, myTrianglePlane);
            } else {
                theMax.contactPoint = theSphere.center - (myTrianglePlane.normal * theSphere.radius) + (theMotionVector * theMax.t);
            }
            Number alpha1, beta1;
            if (inside(theTriangle, myTrianglePlane.normal, theMax.contactPoint, alpha1, beta1)) {
                if (theNormals) {
                    theMax.contactNormal = (*theNormals)[0] * (1.0 - (alpha1+beta1)) +
                                                (*theNormals)[1] * alpha1 +
                                                (*theNormals)[2] * beta1;
                } else {
                    theMax.contactNormal = myTrianglePlane.normal;
                }
                theMax.contactSphereNormal = -theMax.contactNormal;
                theMax.contactSphereCenter = theSphere.center+(theMotionVector * theMax.t);
                if (theMax.t == 1) {
                    theMax.contactType = EMBEDDED;
                } else {
                    theMax.contactType = SURFACE_CONTACT;
                }
            }
        }
        if (theMin.contactType != NO_CONTACT && theMax.contactType != NO_CONTACT ) {
            // both contact points determined inside the triangle
            return 2;
        }

        // 0 or 1 contacts found, look for more contacts

        // look for vertex contact first
        int myMinIndex = -1;
        int myMaxIndex = -1;
        for (int i = 0; i < 3; ++i) {
            Number newT0, newT1;
            int mySolutions = contacts(theSphere, theMotionVector, theTriangle[i], newT0, newT1);
            if (mySolutions) {
                mySolutions = clipContacts(newT0, newT1);
    	        sortContacts(i, mySolutions, VERTEX_CONTACT, newT0, newT1, theMin.contactType, theMax.contactType, theMin.t, theMax.t, myMinIndex, myMaxIndex);
            }
        }

        // now check for edge contact
        Number x0, x1; // distance of sphere center from Sphere origin, 0 = origin, 1 = end
        Number f0[3], f1[3]; // distance of contact point edge origin, 0 = origin, 1 = end

        for (int i = 0; i < 3; ++i) {
            LineSegment<Number> myEdge(theTriangle.edgeOrigin(i), theTriangle.edgeEnd(i));
            int myContacts = contacts(theSphere, theMotionVector, myEdge, x0, x1, f0[i], f1[i]);
            if (myContacts) {
                myContacts = clipContacts(x0, x1);
    	        sortContacts(0, myContacts, EDGE_CONTACT, x0, x1, theMin.contactType, theMax.contactType, theMin.t, theMax.t, myMinIndex, myMaxIndex);
            }
        }

        if (theMin.contactType == EDGE_CONTACT || theMin.contactType == VERTEX_CONTACT) {
            theMin.contactSphereCenter = theSphere.center + theMotionVector * theMin.t;
            if (theMin.contactType == EDGE_CONTACT) {
                theMin.contactPoint = theTriangle.edgeOrigin(myMinIndex) + theTriangle.edgeVector(myMinIndex) * f0[myMinIndex];
                if (theNormals) {
                    theMin.contactNormal = (*theNormals)[Triangle<Number>::edgeOriginIndex(myMinIndex)] * (1.0 - f0[myMinIndex]) +
                                           (*theNormals)[Triangle<Number>::edgeEndIndex(myMinIndex)]    *        f0[myMinIndex];
                } else {
                    theMin.contactNormal = myTrianglePlane.normal;
                }
            } else {
                // theMin.contactType == VERTEX_CONTACT
                theMin.contactPoint = theTriangle[myMinIndex];
                if (theNormals) {
                    theMin.contactNormal = (*theNormals)[myMinIndex];
                } else {
                    theMin.contactNormal = myTrianglePlane.normal;
                }
            }
        }
        if (theMax.contactType == EDGE_CONTACT || theMax.contactType == VERTEX_CONTACT) {
            theMax.contactSphereCenter = theSphere.center + theMotionVector * theMax.t;
            if (theMax.contactType == EDGE_CONTACT) {
                theMax.contactPoint = theTriangle.edgeOrigin(myMaxIndex) + theTriangle.edgeVector(myMaxIndex) * f1[myMaxIndex];
                if (theNormals) {
                    theMax.contactNormal = (*theNormals)[Triangle<Number>::edgeOriginIndex(myMaxIndex)] * (1.0 - f1[myMaxIndex]) +
                                           (*theNormals)[Triangle<Number>::edgeEndIndex(myMaxIndex)]    *        f1[myMaxIndex];
                } else {
                    theMax.contactNormal = myTrianglePlane.normal;
                }
            } else {
                // theMax.contactType == VERTEX_CONTACT
                theMax.contactPoint = theTriangle[myMaxIndex];
                if (theNormals) {
                    theMax.contactNormal = (*theNormals)[myMaxIndex];
                } else {
                    theMax.contactNormal = myTrianglePlane.normal;
                }
            }
        }

        if (theMin.contactType == NO_CONTACT && theMax.contactType == NO_CONTACT){
            return 0;
        }
        if (almostEqual(theMotionVector, Vector3<Number>(0,0,0))) {
            theMax = theMin;
            return 1;
        }
        if (theMin.contactType != NO_CONTACT && theMax.contactType != NO_CONTACT) {
            if (!almostEqual(theMin.t,theMax.t)) {
                return 2;
            }
        }
        return 1;
    }

    // find the first contact point smaller than theCurrentContact.
    // see Kasper Fauerby, Improved Collision Detection and Response, http://www.peroxide.dk
    template<class Number>
	int firstContact(const Sphere<Number> & theSphere,
	             const Vector3<Number> & theMotionVector,
                 const Triangle<Number> & theTriangle,
                 const Vector3<Vector3<Number> > * theNormals,
                 const SweptSphereContact<Number> & theCurrentContact,
                 SweptSphereContact<Number> & theContact)
    {
        theContact.contactType = NO_CONTACT;

        // check triangle plane
        Number theLastContact = 0;
        Plane<Number> myTrianglePlane = theTriangle.plane();
        Number myOriginPlaneDistance = signedDistance(theSphere.center, myTrianglePlane);
        Number dotPlaneNormalAxis = dot(myTrianglePlane.normal, theMotionVector);

        if (dotPlaneNormalAxis != 0) {
            theContact.t = (theSphere.radius - myOriginPlaneDistance) / dotPlaneNormalAxis;
            theLastContact = (-theSphere.radius - myOriginPlaneDistance) / dotPlaneNormalAxis;
            if (theContact.t > theLastContact) {
                std::swap(theContact.t, theLastContact);
            }

            if ((theContact.t > 1) || (theLastContact < 0)) {
                // whole Sphere in front or behind triangle plane
                return false;
            }
            theContact.t = clamp(theContact.t, Number(0), Number(1));
            theLastContact = clamp(theLastContact, Number(0), Number(1));
        } else {
            // triangle-plane and sweep-vector are parallel
            if (fabs(myOriginPlaneDistance) <= theSphere.radius) {
                theContact.t = 0;
                theLastContact = 1;
            } else {
                // too far away
                return false;
            }
        }
        if (theCurrentContact.contactType != NO_CONTACT) {
            if (theContact.t > theCurrentContact.t) {
                // plane is farther away than the current contact
                return false;
            }
        }
        {
            if (theContact.t == 0) {
                theContact.contactPoint = nearest(theSphere.center, myTrianglePlane);
            } else {
                theContact.contactPoint = theSphere.center - (myTrianglePlane.normal * theSphere.radius) + (theMotionVector * theContact.t);
            }
            Number alpha0, beta0;
            if (inside(theTriangle, myTrianglePlane.normal, theContact.contactPoint, alpha0, beta0)) {
                if (theNormals) {
                    theContact.contactNormal = (*theNormals)[0] * (1.0 - (alpha0+beta0)) +
                                                (*theNormals)[1] * alpha0 +
                                                (*theNormals)[2] * beta0;
                } else {
                    theContact.contactNormal = myTrianglePlane.normal;
                }

                theContact.contactSphereNormal = -theContact.contactNormal;
                theContact.contactSphereCenter = theSphere.center+(theMotionVector * theContact.t);
                if (theContact.t == 0) {
                    theContact.contactType = EMBEDDED;
                } else {
                    theContact.contactType = SURFACE_CONTACT;
                }
            }
        }

        if (theContact.contactType == NO_CONTACT) {
            int myMinIndex = -1;
            Number myBestSolution = 1;
            if (theCurrentContact.contactType != NO_CONTACT) {
                myBestSolution = theCurrentContact.t;
            }

            // look for vertex contact first
            for (int i = 0; i < 3; ++i) {
                Number newT0, newT1;
                int mySolutions = contacts(theSphere, theMotionVector, theTriangle[i], newT0, newT1);
                if (mySolutions) {
                    mySolutions = clipContacts(newT0, newT1, myBestSolution);
                    if (mySolutions) {
                        myBestSolution = newT0;
                        theContact.contactType = VERTEX_CONTACT;
                        theContact.t = newT0;
                        myMinIndex = i;
                    }
                }
            }

            // now check for edge contact
            Number x0, x1; // distance of sphere center from Sphere origin, 0 = origin, 1 = end
            Number f0[3], f1[3]; // distance of contact point edge origin, 0 = origin, 1 = end

            for (int i = 0; i < 3; ++i) {
                LineSegment<Number> myEdge(theTriangle.edgeOrigin(i), theTriangle.edgeEnd(i));
                int mySolutions = contacts(theSphere, theMotionVector, myEdge, x0, x1, f0[i], f1[i]);
                if (mySolutions) {
                    mySolutions = clipContacts(x0, x1, myBestSolution);
                    if (mySolutions) {
                        myBestSolution = x0;
                        theContact.contactType = EDGE_CONTACT;
                        theContact.t = x0;
                        myMinIndex = i;
                    }
                }
            }

            if (theContact.contactType == EDGE_CONTACT || theContact.contactType == VERTEX_CONTACT) {
                theContact.contactSphereCenter = theSphere.center + theMotionVector * theContact.t;
                if (theContact.contactType == EDGE_CONTACT) {
                    theContact.contactPoint = theTriangle.edgeOrigin(myMinIndex) + theTriangle.edgeVector(myMinIndex) * f0[myMinIndex];
                    if (theNormals) {
                        theContact.contactNormal = (*theNormals)[Triangle<Number>::edgeOriginIndex(myMinIndex)] * (1.0 - f0[myMinIndex]) +
                                               (*theNormals)[Triangle<Number>::edgeEndIndex(myMinIndex)]    *        f0[myMinIndex];
                    } else {
                        theContact.contactNormal = myTrianglePlane.normal;
                    }
                } else {
                    // theContact.contactType == VERTEX_CONTACT
                    theContact.contactPoint = theTriangle[myMinIndex];
                    if (theNormals) {
                        theContact.contactNormal = (*theNormals)[myMinIndex];
                    } else {
                        theContact.contactNormal = myTrianglePlane.normal;
                    }
                }
            }
        }
        return (theContact.contactType != NO_CONTACT);
    }



} // asl


#endif
