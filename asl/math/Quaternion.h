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
//   $RCSfile: Quaternion.h,v $
//
//   $Author: ulrich $
//
//   $Revision: 1.18 $
//
//   Description: Quaterinon support for linear algebra library
//
//=============================================================================

#ifndef _included_asl_Quaternion_
#define _included_asl_Quaternion_

#include "Vector234.h"
#include "Sphere.h"
#include <asl/base/Exception.h>

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    /** A quaternion class usefull to compute and maintain orientations.
     *
     * The quaternion is defined as
     * @f[ \hat{q} = (i, j, k, w) @f]
     * where @f$ i @f$, @f$ j @f$, @f$ k @f$ are the imaginary parts of the
     * quaternion and @f$ w @f$ is the real part.
     *
     * It is also written as
     * @f[ \hat{q} = (\vec{q}, q_w) @f]
     * where @f$ \vec{q} @f$ is the imaginary vector and @f$ q_w @f$ is the
     * real part
     */
    template<class Number>
    class Quaternion : public QuadrupleOf<Number> {
        public:
            typedef QuadrupleOf<Number> Base;
            /** Default constructor */
            Quaternion() : QuadrupleOf<Number>()
            {}

            /** Copy constructor
             */
            Quaternion(const Quaternion & theOther) :
                QuadrupleOf<Number>(theOther)
            {}

            /** Construct from explicit real and imaginary part.
             * @param i imaginary part @f$ i @f$
             * @param j imaginary part @f$ j @f$
             * @param k imaginary part @f$ k @f$
             * @param w real part @f$ w @f$
             */
            Quaternion(Number i, Number j, Number k, Number w) :
                 QuadrupleOf<Number>(i, j, k, w)
            {}

            /** Construct from any FixedVector with the correct size
             */
		    Quaternion(const FixedVector<4, Number> & t) : QuadrupleOf<Number>(t) {}
            /** Construct from C type array.
             */
    		Quaternion(const Number* t) : QuadrupleOf<Number>(t) {}
            /** Construct from two iterators/pointers.
             */
            Quaternion(const Number * theBegin, const Number * theEnd) : QuadrupleOf<Number>(theBegin, theEnd) {}
            /** Given an axis and myAngle, compute quaternion.
             */
            Quaternion(const Vector3<Number> & theAxis, Number theAngle) {
                Vector3<Number> myVector = normalized(theAxis);
                // myAngle is clockwise
                myVector = myVector * Number(sin(-theAngle * 0.5));

                this->val[0] = myVector[0];
                this->val[1] = myVector[1];
                this->val[2] = myVector[2];
                this->val[3] = Number(cos(theAngle * 0.5));
            }

            /**
             * Given two normalized Vectors, compute the quaternion between them
             */
            Quaternion(const Vector3<Number> & theSourceVector, const Vector3<Number> & theTargetVector) {
                Vector3<Number> myCross = cross(theSourceVector, theTargetVector);
                this->val[0] = myCross[0];
                this->val[1] = myCross[1];
                this->val[2] = myCross[2];
                this->val[3] = magnitude(theSourceVector)*magnitude(theTargetVector) + 
                    dot(theSourceVector, theTargetVector);
                this->normalize();
            }

            /** Given a XYZRotation, compute quaternion.
             */
            void assignFromEuler(const asl::Vector3<Number> & theEulerAngles) {
                Quaternion myRotateX(Vector3<Number>(1,0,0), theEulerAngles[0]);
                Quaternion myRotateY(Vector3<Number>(0,1,0), theEulerAngles[1]);
                Quaternion myRotateZ(Vector3<Number>(0,0,1), theEulerAngles[2]);

                *this = myRotateX * myRotateY * myRotateZ;
                // *this = myRotateZ * myRotateY * myRotateX;
            }

            /** Set Quaternion to identity.
             */
            void makeIdentity() {
                this->val[0] = this->val[1] = this->val[2] = 0;
                this->val[3] = 1;
            }

            /** Given a XYZRotation, compute quaternion.
             *  @param theRotation
             */
            static Quaternion createFromEuler(const Vector3<Number> & theRotation) {
                Quaternion myResult;
                myResult.assignFromEuler(theRotation);
                return myResult;
            }

            /** Return the imaginary part of the quaternion.
             */
            Vector3<Number> getImaginaryPart() const {
                return Vector3<Number>(this->val[0], this->val[1], this->val[2]);
            }

            /** Return the real part of the quaternion.
             */
            Number getRealPart() const {
                return this->val[3];
            }

            /** Extracts @p theAxis and @p theAngle from normalized quaternion. 
             * If angle is almost 0, the axis is arbitrary and we
             * return [1,0,0].
             * @returns false if not normalized, otherwise true.
             */
            bool getAxisAndAngle(Vector3<Number> & theAxis, Number & theAngle) const {
                if (this->val[3] > 1) {
                    return false; // not normalized
                }
                theAngle = 2 * acos(this->val[3]);
                Number s = sqrt(1- this->val[3]*this->val[3] );
                if (almostEqual(s, 0)) {
                    // angle close to 0, axis not important
                    theAxis[0] = Number(1.0);
                    theAxis[1] = Number(0.0);
                    theAxis[2] = Number(0.0);
                } else {
                    theAxis[0] = this->val[0]/s;
                    theAxis[1] = this->val[1]/s;
                    theAxis[2] = this->val[2]/s;
                }
                return true;
            }

            /** Set the imaginary part of the quaternion.
             */
            void setImaginaryPart(const Vector3<Number> & theImaginary) {
                this->val[0] = theImaginary[0];
                this->val[1] = theImaginary[1];
                this->val[2] = theImaginary[2];
            }

            /** Set the real part of the quaternion.
             */
            void setRealPart(Number theReal) {
                this->val[3] = theReal;
            }

            /** Normalize the quaternion to unit length.
             *
             * Unit Quaternions always obey:  @f$ i^2 + j^2 + k^2 + w^2 = 1.0 @f$
             *
             * If they don't add up to 1.0, dividing by their magnitued will
             * renormalize them.
             *
             * @see
             * - Shoemake, K., Animating rotation with quaternion curves, Computer
             *   Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
             * - Pletinckx, D., Quaternion calculus as a basic tool in computer
             *   graphics, The Visual Computer 5, 2-13, 1989.
             */
            void normalize() {
                Number myMagnitude = Base::getLength();

                for (unsigned i = 0; i < Base::SIZE; i++) {
                    this->val[i] /= myMagnitude;
                }
            }

            /** inverts the quaternion.
             *
             */
            void invert() {
                this->val[3] *= -1;
            }

            /** Conjugates the quaternion.
             * The resulting quaternion performs exactly the reverse rotation as
             * before.
             *
             * From Real Time Rendering - Tomas Möller - p.44
             * @f[ \hat{q}^\ast = (\vec{q}, q_w)^\ast = (- \vec{q}, q_w) @f]
             */
            void conjugate() {
                this->val[0] = - this->val[0];
                this->val[1] = - this->val[1];
                this->val[2] = - this->val[2];
            }

            /** @name Quaternion - Scalar Operations */
            /* @{ */
            /** Multiply with scalar */
            template <class otherReal>
            void operator *= (const otherReal & s) {
                mult(Number(s));
            }
            /** Divide by scalar */
            template <class otherReal>
            void operator /= (const otherReal & s) {
                div(Number(s));
            }
            /** Multiply with float */
            Quaternion operator*(const float & s) const {
                Quaternion myResult(*this);
                myResult.mult(Number(s));
                return myResult;
            }
            /** Multiply with double */
            Quaternion operator*(const double & s) const {
                Quaternion myResult(*this);
                myResult.mult(Number(s));
                return myResult;
            }
            /** Multiply with int */
            Quaternion operator*(const int & s) const {
                Quaternion myResult(*this);
                myResult.mult(Number(s));
                return myResult;
            }
            /** Divide by float */
            Quaternion operator/(const float & s) const {
                Quaternion myResult(*this);
                myResult.div(Number(s));
                return myResult;
            }
            /** Divide by double */
            Quaternion operator/(const double & s) const {
                Quaternion myResult(*this);
                myResult.div(Number(s));
                return myResult;
            }
            /** Divide by int */
            Quaternion operator/(const int & s) const {
                Quaternion myResult(*this);
                myResult.div(Number(s));
                return myResult;
            }
            /* XXX DS: this breaks the JSQuaternion wrapper. quat * scalar should be a
                   free product() function anyway, right?
            */
            void mult(const Number & s) {
                Base::mult(s);
            }
            /* @} */



            /** @name Quaternion - Quaternion Operations */
            /* @{ */
            /** Multiplication operator for quaternions.
            */
            Quaternion operator*( const Quaternion & q) const {
                return product(*this, q);
            }
            /** Addition operator for quaternions.
             */
            Quaternion operator +(const Quaternion & q) const {
                Quaternion myResult(*this);
                myResult.add(q);
                return myResult;
            }
            /** Subtraction operator for quaternions.
             */
            Quaternion operator -(const Quaternion & q) const {
                Quaternion myResult(*this);
                myResult.sub(q);
                return myResult;
            }

            /** Multiply with other quaternion
             */
            void mult(const Quaternion & theOther) {
                *this = product(*this, theOther);
            }

            /** Add quaternion */
            void operator+=(const Quaternion & q) {
                add(q);
            }
            /** Subtract quaternion */
            void operator-=(const Quaternion & q) {
                sub(q);
            }
            /** Multiply with quaternion */
            void operator*=(const Quaternion & q) {
                mult(q);
            }
            /* @} */

    };


    /** Multiplies two quaternions @f$\hat{a}@f$ and @f$\hat{b}@f$ and return the result.
     * @relates Quaternion
     */
    template <class NUMBER>
    Quaternion<NUMBER>
    product(const Quaternion<NUMBER> & a, const Quaternion<NUMBER> & b) {
        Quaternion<NUMBER> myResult;
        myResult[0] = (a[1] * b[2]) - (a[2] * b[1]) +
            b[3] * a[0] + a[3] * b[0];

        myResult[1] = (a[2] * b[0]) - (a[0] * b[2]) +
            b[3] * a[1] + a[3] * b[1];

        myResult[2] = (a[0] * b[1]) - (a[1] * b[0]) +
            b[3] * a[2] + a[3] * b[2];

        myResult[3] = a[3] * b[3] - (a[0] * b[0] +
                a[1] * b[1] +
                a[2] * b[2]);

        return myResult;
    }

    /** Multiplies a quaternion @f$\hat{q}@f$ and a scalar and returns the result.
     * @relates Quaternion
     */
    template <class NUMBER>
    Quaternion<NUMBER> product(const Quaternion<NUMBER> & q, const NUMBER & s) {
    	Quaternion<NUMBER> myResult(q);
        myResult.mult(s);
    	return myResult;
    }

    /** Multiplication operator for a float and a quaternion @f$\hat{q}@f$.
     * @relates Quaternion
     */
    template <class NUMBER>
    Quaternion<NUMBER> operator *(const float & s,
                                  const Quaternion<NUMBER> & q)
    {
        return product(q, typename Quaternion<NUMBER>::value_type(s));
    }

    /** Multiplication operator for a double and a quaternion @f$\hat{q}@f$.
     * @relates Quaternion
     */
    template <class NUMBER>
    Quaternion<NUMBER> operator *(const double & s,
                                  const Quaternion<NUMBER> & q)
    {
        return product(q, typename Quaternion<NUMBER>::value_type(s));
    }

    /** Multiplication operator for a int and a quaternion @f$\hat{q}@f$.
     * @relates Quaternion
     */
    template <class NUMBER>
    Quaternion<NUMBER> operator *(const int & s,
                                  const Quaternion<NUMBER> & q)
    {
        return product(q, typename Quaternion<NUMBER>::value_type(s));
    }

    /** Adds two quaternions @f$\hat{q}@f$ and @f$\hat{r}@f$ and returns the result.
     * @relates Quaternion
     */
    template <class NUMBER>
    Quaternion<NUMBER> sum(const Quaternion<NUMBER> & q,
                           const Quaternion<NUMBER> & r)
    {
    	Quaternion<NUMBER> myResult(q);
        myResult.add(r);
    	return myResult;
    }

    /** Subtracts two quaternions @f$\hat{q}@f$ and @f$\hat{r}@f$ and returns the result.
     * @relates Quaternion
     */
    template <class NUMBER>
    Quaternion<NUMBER> difference(const Quaternion<NUMBER> & q,
                                  const Quaternion<NUMBER> & r)
    {
    	Quaternion<NUMBER> myResult(q);
        myResult.sub(r);
    	return myResult;
    }
    /** Subtraction operator for two quaternions @f$\hat{q}@f$ and @f$\hat{r}@f$.
     * @relates Quaternion
     */
    template <class NUMBER>
    Quaternion<NUMBER> operator -(const Quaternion<NUMBER> & q,
                                  const Quaternion<NUMBER> & r)
    {
        return difference(q, r);
    }

    /** Divides a quaternion @f$\hat{q}@f$ by a scalar and returns the result.
     * @relates Quaternion
     */
    template <class NUMBER>
    Quaternion<NUMBER> quotient(const Quaternion<NUMBER> & q,
                                const NUMBER & theDividend)
    {
    	Quaternion<NUMBER> myResult(q);
        myResult.div(theDividend);
    	return myResult;
    }

    /** Division operator for a quaternion @f$\hat{q}@f$ and a scalar.
     * @relates Quaternion
     */
    template <class NUMBER>
    Quaternion<NUMBER> operator /(const Quaternion<NUMBER> & q, const NUMBER & theDividend)
    {
        return quotient(q, theDividend);
    }

    /** Returns normalized copy.
     * @relates Quaternion
     * @see Quaternion::normalize()
     */
    template <class NUMBER>
    Quaternion<NUMBER> normalized( const Quaternion<NUMBER> & q) {
        Quaternion<NUMBER> myResult(q);
        myResult.normalize();
        return myResult;
    }

    /** Returns conjugated copy.
     * @relates Quaternion
     * @see Quaternion::conjugate()
     */
    template <class NUMBER>
    Quaternion<NUMBER> conjugated( const Quaternion<NUMBER> & q) {
        Quaternion<NUMBER> myResult(q);
        myResult.conjugate();
        return myResult;
    }


    /** Linear interpolation.
     * Taken from Game Programming Gems I, p. 195ff
     * @param q quaternion @f$\hat{q}@f$
     * @param r quaternion @f$\hat{r}@f$
     * @param theT interpolation value @f$ t @f$
     */
    template <class NUMBER>
    Quaternion<NUMBER>
    lerp(const Quaternion<NUMBER> & q, const Quaternion<NUMBER> & r, const NUMBER & theT)
    {
        Quaternion<NUMBER> myResult;
    	myResult = q + theT * ( r - q );
    	myResult.normalize();
    	return myResult;
    }

    /** Spherical linear interpolation.
     * Taken from Game Programming Gems I, p. 195ff
     * @param q quaternion @f$\hat{q}@f$
     * @param r quaternion @f$\hat{r}@f$
     * @param theT interpolation value @f$ t @f$
     */
    template <class NUMBER>
    Quaternion<NUMBER>
        slerp(const Quaternion<NUMBER> & q, const Quaternion<NUMBER> & r, const NUMBER & theT) {
            Quaternion<NUMBER> myResult;
            NUMBER myDot;
            myDot = dot(q, r);

            /*
            myDot = cos(theta)
            if (myDot < 0), q and r are more than 90 degrees apart,
            so we can invert one to reduce spinning
            */
            if (myDot < 0) {
                myDot = -myDot;
                myResult = r * NUMBER(-1.0);
            } else {
                myResult = r;
            }
            if (myDot < 0.95f) {
                NUMBER myAngle = static_cast<float>(acos(myDot));
                NUMBER mySinA, mySinAt, mySinAomt;
                mySinA = static_cast<NUMBER>(sin(myAngle));
                mySinAt = static_cast<NUMBER>(sin(myAngle * theT));
                mySinAomt = static_cast<NUMBER>(sin(myAngle*(1-theT)));
                return (q * mySinAomt + myResult*mySinAt ) / mySinA;
            } 
            // if the myAngle is small, use linear interpolation
            return lerp( q, myResult, theT);
    }

    typedef Quaternion<float>  Quaternionf;
    typedef Quaternion<double> Quaterniond;

    /** Seems to be an oriented sphere
     * @bug nothing implemented
     */
    template<class Number>
    class Ball : public Sphere<Number> {
    public:
        Ball() : Sphere<Number>()
        {}

    private:
        Quaternion<Number> _myOrientation;
    };

    /* @} */
} // namespace asl

#endif
