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
//    $RCSfile: linearAlgebra.h,v $
//
//     $Author: jens $
//
//   $Revision: 1.64 $
//
// Description: point, vector, line, plane and matrix classes and function
//
//=============================================================================
/*
TODO list:
- Matrix projection
- projection constructor
- Matrix preMultiply
- Matrix trace (Spur)
- eigenVector & eigenvalue
- Matrix mirror
- Matrix2 Klasse
- Matrix3 Klasse
- coord class
- Matrix orthogonizer
*/

#ifndef _included_asl_linearAlgebra_
#define _included_asl_linearAlgebra_

#include "Vector234.h"
#include "Matrix4.h"
#include "Point234.h"
#include "Box.h"
#include "Line.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Capsule.h"

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    /** Transforms a Box3 with a Matrix4. Retrieves the corners of the box and
     * transforms them with the Matrix4. The transformed corners are added to
     * an empty box. The box containing these transformed corner points is
     * returned.
     * @relates Box3 */
    template <class Number>
    Box3<Number> product(const Box3<Number> & theBox, const Matrix4<Number> & theMatrix) {
        if (!theBox.hasPosition()) {
            return theBox;
        }

        Point3<Number> myCorners[8];
        theBox.getCorners(myCorners[0], myCorners[1], myCorners[2], myCorners[3],
                          myCorners[4], myCorners[5], myCorners[6], myCorners[7]);
        Box3<Number> myResult;
        myResult.makeEmpty();

        for (unsigned i = 0; i < 8; ++i) {
            myResult.extendBy(myCorners[i] * theMatrix);
        }
        return myResult;
    }

    /** Product operator for Box3 and Matrix4
     * @relates Box3
     */
    template<class Number>
    Box3<Number> operator*(const Box3<Number> & theBox, const Matrix4<Number> & theMatrix) {
        return product(theBox, theMatrix);
    }

    /** Transforms a Line with a Matrix4.
     * @relates Line */
    template <class Number>
    Line<Number> product(const Line<Number> & theLine, const Matrix4<Number> & theMatrix) {
        Line<Number> myResult(theLine.origin * theMatrix,
                              (theLine.origin + theLine.direction) * theMatrix);
        myResult.normalize();
        return myResult;
    }
    /** Product operator for Line and Matrix4
     * @relates Line */
    template<class Number>
    Line<Number> operator*(const Line<Number> & theLine, const Matrix4<Number> & theMatrix) {
        return product(theLine, theMatrix);
    }

    /** Transforms a Ray with a Matrix4.
     * @relates Ray */
    template <class Number>
    Ray<Number> product(const Ray<Number> & theRay, const Matrix4<Number> & theMatrix) {
        Ray<Number> myResult(theRay.origin * theMatrix,
            (theRay.origin + theRay.direction) * theMatrix);
        myResult.normalize();
        return myResult;
    }
    /** Product operator for Ray and Matrix4
     * @relates Ray */
    template<class Number>
    Ray<Number> operator*(const Ray<Number> & theRay, const Matrix4<Number> & theMatrix) {
        return product(theRay, theMatrix);
    }

    /** Transforms a LineSegment with a Matrix4.
     * @relates LineSegment */
    template <class Number>
    LineSegment<Number> product(const LineSegment<Number> & theLine, const Matrix4<Number> & theMatrix) {
        return LineSegment<Number>(theLine.origin * theMatrix,
                                   theLine.end * theMatrix);
    }
    /** Product operator for LineSegment and Matrix4
     * @relates LineSegment */
    template<class Number>
    LineSegment<Number> operator*(const LineSegment<Number> & theLine, const Matrix4<Number> & theMatrix) {
        return product(theLine, theMatrix);
    }

    /** Transforms a Plane with a Matrix4.
     * @relates Plane */
    template <class Number>
    Plane<Number> product(const Plane<Number> & thePlane, const Matrix4<Number> & theMatrix) {
        Plane<Number> myResult;
        Matrix4<Number> myInverseTranspose(theMatrix);
        myInverseTranspose.invert();
        myInverseTranspose.transpose();
        Vector4<Number> myCoefficentVector(thePlane.normal[0],
                                           thePlane.normal[1],
                                           thePlane.normal[2],
                                           thePlane.offset);
        Vector4<Number> myResultCoefficents = product(myCoefficentVector, myInverseTranspose);
        myResult.normal[0] = myResultCoefficents[0];
        myResult.normal[1] = myResultCoefficents[1];
        myResult.normal[2] = myResultCoefficents[2];
        myResult.offset    = myResultCoefficents[3] / magnitude(myResult.normal);

        myResult.normalize();
        return myResult;
    }
    /** Product operator for Plane and Matrix4
     * @relates Plane */
    template <class Number>
    Plane<Number> operator*(const Plane<Number> & thePlane, const Matrix4<Number> & theMatrix) {
        return product(thePlane, theMatrix);
    }

    /** Transforms a Triangle with a Matrix4.
     * @relates Triangle */
    template <class Number>
    Triangle<Number> product(const Triangle<Number> & theTriangle, const Matrix4<Number> & theMatrix) {
        return Triangle<Number> (
            theTriangle[0] * theMatrix,
            theTriangle[1] * theMatrix,
            theTriangle[2] * theMatrix);
    }
    /** Product operator for Triangle and Matrix4
     * @relates Triangle */
    template<class Number>
    Triangle<Number> operator*(const Triangle<Number> & theTriangle, const Matrix4<Number> & theMatrix) {
        return product(theTriangle, theMatrix);
    }

    // XXX: DS: What is that for? It also match sum(Quaternion, Vector4) for example!
    template <class POINT, class VECTOR>
    POINT sum(const POINT & a, const VECTOR & b) {
        POINT myResult = a;
        myResult.add(b);
        return myResult;
    }
    // XXX: DS: What is that for? It also match sum(Quaternion, Vector4) for example!
    template <class POINT, class VECTOR>
    POINT difference(const POINT & a, const VECTOR & b) {
        POINT myResult = a;
        myResult.sub(b);
        return myResult;
    }

    template <class NUMBER>
    Vector2<NUMBER> difference(const Point2<NUMBER> & a, const Point2<NUMBER> & b) {
        Vector2<NUMBER> myResult = asVector(a);
        myResult.sub(asVector(b));
        return myResult;
    }
    template <class NUMBER>
    Vector3<NUMBER> difference(const Point3<NUMBER> & a, const Point3<NUMBER> & b) {
        Vector3<NUMBER> myResult = asVector(a);
        myResult.sub(asVector(b));
        return myResult;
    }

    template <class NUMBER>
    Vector4<NUMBER> difference(const Point4<NUMBER> & a, const Point4<NUMBER> & b) {
        Vector4<NUMBER> myResult = asVector(a);
        myResult.sub(asVector(b));
        return myResult;
    }

    template <class NUMBER>
    Point2<NUMBER> product(const Point2<NUMBER> & a, const Vector2<NUMBER> & b) {
        Point2<NUMBER> myResult = a;
        myResult.mult(b);
        return myResult;
    }

    template <class NUMBER>
    Point3<NUMBER> product(const Point3<NUMBER> & a, const Vector3<NUMBER> & b) {
        Point3<NUMBER> myResult = a;
        myResult.mult(b);
        return myResult;
    }

    template <class NUMBER>
    Point4<NUMBER> product(const Point4<NUMBER> & a, const Vector4<NUMBER> & b) {
        Point4<NUMBER> myResult = a;
        myResult.mult(b);
        return myResult;
    }

    template <class POINT, class VECTOR>
    POINT quotient(const POINT & a, const VECTOR & b) {
        POINT myResult = a;
        myResult.div(b);
        return myResult;
    }

    //=================================================
    // Quaternion Operations
    //=================================================

    template <class NUMBER>
    asl::Vector4<NUMBER> product(const Quaternion<NUMBER> & theQuaternion,
                                 const asl::Vector4<NUMBER> & theVector)
    {
        // TODO: find out which product(Vector*Quaternion)  matches
        throw asl::Exception("Quaternion * Vector is illegal, use Vector * Quaternion instead", PLUS_FILE_LINE);
    }

    template <class NUMBER>
    asl::Vector4<NUMBER> product(const asl::Vector4<NUMBER> & theVector,
                                 const Quaternion<NUMBER> & theQuaternion)
    {
    	asl::Matrix4<NUMBER> myMatrix(theQuaternion);
    	return product(theVector, myMatrix );
    }

    template <class NUMBER>
    asl::Vector4<NUMBER> operator * (const asl::Vector4<NUMBER> & theVector,
                                     const Quaternion<NUMBER> & theQuaternion)
    {
        return product(theVector, theQuaternion);
    }

    template <class NUMBER>
    asl::Point3<NUMBER> product(const asl::Point3<NUMBER> & thePoint,
                                const Quaternion<NUMBER> & theQuaternion)
    {
    	asl::Matrix4<NUMBER> myMatrix(theQuaternion);
    	return product(thePoint, myMatrix );
    }

    template <class NUMBER>
    asl::Point3<NUMBER> operator * (const asl::Point3<NUMBER> & thePoint,
                                    const Quaternion<NUMBER> & theQuaternion)
    {
        return product(thePoint, theQuaternion);
    }


    // NOTE: normalized vectors required
    inline
    asl::Quaternionf
    getOrientationFromDirection(const asl::Vector3f & theViewVector, const asl::Vector3f & theUpVector) {
        asl::Vector3f myUpVector = asl::normalized(projection(theUpVector, asl::Planef(theViewVector, 0)));
        asl::Vector3f myRightVector = asl::cross(theViewVector, myUpVector);

        asl::Matrix4f myMatrix;
        myMatrix.assign(myRightVector[0], myRightVector[1],  myRightVector[2],  0,
                       myUpVector[0],     myUpVector[1],     myUpVector[2],     0,
                       -theViewVector[0], -theViewVector[1], -theViewVector[2], 0,
                       0, 0, 0, 1,
                       asl::ROTATING);
        asl::Quaternionf   myOrientation;
        asl::Vector3f myPosition;
        myMatrix.decompose(myUpVector, myRightVector, myOrientation, myPosition);
        return myOrientation;
    }

    /* @} */
} // asl

#endif
