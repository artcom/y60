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
//    $RCSfile: Vector234.h,v $
//
//     $Author: david $
//
//   $Revision: 1.5 $
//
// Description: 2D, 3D and 4D vector classes and functions
//
//=============================================================================


#ifndef _included_asl_Vector_
#define _included_asl_Vector_

#include "FixedVector.h"
#include <asl/base/Exception.h>
#include <asl/base/string_functions.h>
#include <math.h>

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    /** A 2-tupel of arbitrary (nummeric) type. 
    *  @f[ \vec{a} = (a_1, a_2) @f]
    */
    template<class T>
    class PairOf : public FixedVector<2,T>{
    public:
        typedef FixedVector<2,T> Base;
        typedef typename FixedVector<2,T>::iterator iterator;
        /** Default constructor. */
        PairOf() {};
        /** Converts any FixedVector<2, T> into a PairOf<T>. */
        PairOf(const Base & t) : Base(t) {}
        /** Copy constructor. */
        PairOf(const PairOf & t) : Base(t) {}
        /** Construct from two pointers/iterators. */
        explicit PairOf(const T * theBegin, const T * theEnd) : Base(theBegin, theEnd) {}

        /** Assignment operator. */
        PairOf & operator=(const PairOf & t) {
            assign(t);
            return *this;
        }
        /** Construct from C style array. */
        PairOf(const T * t) : Base(t) {}
        /** Construct from two scalar values. */
        PairOf(T a, T b) {
            this->val[0]=a;this->val[1]=b;
        }
    };

    /** A Triple of arbitrary (nummeric) type.
    *  @f[ \vec{a} = (a_1, a_2, a_3) @f]
    */
    template<class T>
    class TripleOf : public FixedVector<3,T> {
    public:
        typedef FixedVector<3,T> Base;
        typedef typename FixedVector<3,T>::iterator iterator;
        /** Default constructor. */
        TripleOf() {};
        /** Copy constructor. */
        TripleOf(const TripleOf & t) : Base(t) {}
        /** Converts any FixedVector<3, T> into a TripleOf<T>. */
        TripleOf(const Base & t) : Base(t) {}
        /** Construct from C style array. */
        TripleOf(const T * t) : Base(t) {}
        /** Construct from two pointers/iterators. */
        TripleOf(const T * theBegin, const T * theEnd) : Base(theBegin, theEnd) {}
        /** Assignment operator. */
        TripleOf & operator=(const TripleOf & t) {
            assign(t);
            return *this;
        }
        /** Construct from three scalar values. */
        TripleOf(T a, T b, T c) {
            this->val[0]=a;this->val[1]=b;this->val[2]=c;
        }
    };

    /** A Quadruple of arbitrary (nummeric) type.
    *  @f[ \vec{a} = (a_1, a_2, a_3, a_4) @f]
    */
    template<class T>
    class QuadrupleOf : public FixedVector<4,T> {
    public:
        typedef FixedVector<4,T> Base;
        /** Default constructor. */
        QuadrupleOf() : FixedVector<4, T>() {}
        /** Copy constructor. */
        QuadrupleOf(const QuadrupleOf & t) : Base(t) {}
        /** Converts any FixedVector<4, T> into a QuadrupleOf<T>. */
        QuadrupleOf(const Base & t) : Base(t) {}
        /** Construct from C style array. */
        QuadrupleOf(const T* t) : Base(t) {}
        /** Construct from two pointers/iterators. */
        QuadrupleOf(const T * theBegin, const T * theEnd) : Base(theBegin, theEnd) {}
        /** Construct from four scalar values. */
        QuadrupleOf(T a, T b, T c, T d) {
            this->val[0]=a;this->val[1]=b;this->val[2]=c;this->val[3]=d;
        }
        /** Assignment operator. */
        QuadrupleOf & operator=(const QuadrupleOf & t) {
            assign(t);
            return *this;
        }
    };

    /** A two dimensional vector 
    * @f[ \vec{v} = (v_x, v_y) @f] 
    */
    template<class Number>
    class Vector2 : public PairOf<Number> {
    public:
        typedef PairOf<Number> Base;
        /** Default constructor. */
        Vector2() : Base() {};
        /** Copy constructor. */
        Vector2(const Vector2<Number> & p) : Base(p) {};
        /** Converts any FixedVector<2, T> into a Vector2. */
        Vector2(const FixedVector<2, Number> & p) : PairOf<Number>(p) {};
        /** Construct from two scalar values. */
        Vector2(Number a, Number b) : Base(a,b) { }
        /** Construct from C style array. */
        Vector2(const Number * t) : Base(t) {};
        /** Construct from two pointers/iterators. */
        explicit Vector2(const Number * theBegin, const Number * theEnd) : Base(theBegin, theEnd) {}
        /** Assignment operator. */
        Vector2 & operator=(const Vector2 & t) {
            assign(t);
            return *this;
        }
        /** @name Vector - Scalar Operations */
        /* @{ */
        /** Multiplicative assignment operator for scalar types */
        template <class otherReal>
        void operator*=(const otherReal& y) {
            mult(Number(y));
        }
        /** Dividing assignment operator for scalar types */
        template <class otherReal>
        void operator/=(const otherReal& y) {
            div(Number(y));
        }
        /** Multiply Vector2 with a float @a y and return the result. */
        Vector2 operator*(const float & y) const {
            Vector2 result(*this);
            result.mult(Number(y));
            return result;
        }
        /** Multiply Vector2 with a double @a y and return the result. */
        Vector2 operator*(const double & y) const {
            Vector2 result(*this);
            result.mult(Number(y));
            return result;
        }
        /** Multiply Vector2 with an int @a y and return the result. */
        Vector2 operator*(const int & y) const {
            Vector2 result(*this);
            result.mult(Number(y));
            return result;
        }

        /** Divide Vector2 by a float @a y and return the result. */
        Vector2 operator/(const float & y) const {
            Vector2 result(*this);
            result.div(Number(y));
            return result;
        }
        /** Divide Vector2 by a double @a y and return the result. */
        Vector2 operator/(const double & y) const {
            Vector2 result(*this);
            result.div(Number(y));
            return result;
        }
        /** Divide Vector2 by a int @a y and return the result. */
        Vector2 operator/(const int & y) const {
            Vector2 result(*this);
            result.div(Number(y));
            return result;
        }
        /** Addition method for scalar. */
        void add(Number y) {
            Base::add(y);
        }
        /** Subtraction method for scalar. */
        void sub(Number y) {
            Base::sub(y);
        }
        /** Multiplication method for scalar. */
        void mult(Number y) {
            Base::mult(y);
        }
        /** Division method for scalar. */
        void div(Number y) {
            Base::div(y);
        }
        /* @} */

        /** @name Vector - Vector Operations */
        /* @{ */
        /** Component wise additive assignment operator */
        void operator+=(const Vector2& y) {
            add(y);
        }
        /** Component wise subtractive assignment operator */
        void operator-=(const Vector2& y) {
            sub(y);
        }
        /** Component wise multiplicative assignment operator */
        void operator*=(const Vector2 & y) {
            mult(y);
        }
        /** Component wise dividing assignment operator */
        void operator/=(const Vector2 & y) {
            div(y);
        }

        /** Component wise multiplication operator */
        Vector2 operator*(const Vector2 & y) const {
            Vector2 myResult(*this);
            myResult.mult(y);
            return myResult;
        }
        /** Component wise division operator */
        Vector2 operator/(const Vector2 & y) const {
            Vector2 myResult(*this);
            myResult.div(y);
            return myResult;
        }

        /** Addition operator. */
        Vector2 operator+(const Vector2 & y) const {
            Vector2 result(*this);
            result.add(y);
            return result;
        }
        /** Subtraction operator. */
        Vector2 operator-(const Vector2 & y) const {
            Vector2 result(*this);
            result.sub(y);
            return result;
        }
        /** Addition method. */
        void add(const Vector2 & y) {
            Base::add(y);
        }
        /** Subtraction method. */
        void sub(const Vector2 & y) {
            Base::sub(y);
        }
        /** Multiplication method. */
        void mult(const Vector2 & y) {
            Base::mult(y);
        }
        /** Division method. */
        void div(const Vector2 & y) {
            Base::div(y);
        }

        /* @} */

        /** Unary minus operator. Returns a component wise negation. */
        Vector2 operator-() const {
            Vector2 result(*this);
            result.negate();
            return result;
        }
        Vector2 normalize() const {
            return normalized( *this);
        }
    };
    /** Reinterprets a pointer as a Vector2. Use with caution.
    * @relates Vector2
    */
    template <class Number>
    Vector2<Number> * asVector2(Number * the2Numbers) {
        return reinterpret_cast<Vector2<Number>*>(the2Numbers);
    }
    /** Reinterprets a pointer as a Vector2. Use with caution.
    * @relates Vector2
    */
    template <class Number>
    const Vector2<Number> * asVector2(const Number * the2Numbers) {
        return reinterpret_cast<const Vector2<Number>*>(the2Numbers);
    }

    /** Reinterprets a reference as a Vector2. Use with caution.
    * @relates Vector2
    */
    template <class Number>
    Vector2<Number> & asVector2(Number & the2Numbers) {
        return reinterpret_cast<Vector2<Number>&>(the2Numbers);
    }
    /** Reinterprets a reference as a Vector2. Use with caution.
    * @relates Vector2
    */
    template <class Number>
    const Vector2<Number> & asVector2(const Number & the2Numbers) {
        return reinterpret_cast<const Vector2<Number>&>(the2Numbers);
    }

    /** Multiplies a Vector2 with a scalar and returns the result.
    * @relates Vector2
    */
    template <class Number>
    Vector2<Number> product(const Vector2<Number> & v, Number y) {
        Vector2<Number> result(v);
        result.mult(y);
        return result;
    }

    /** Multiplies a float with a Vector2 and returns the result.
    * @relates Vector2
    */
    template<class Number>
    Vector2<Number> operator*(const float & y, const Vector2<Number> & v) {
        return product(v, typename Vector2<Number>::value_type(y));
    }
    /** Multiplies a double with a Vector2 and returns the result.
    * @relates Vector2
    */
    template<class Number>
    Vector2<Number> operator*(const double & y, const Vector2<Number> & v) {
        return product(v, typename Vector2<Number>::value_type(y));
    }
    /** Multiplies an int with a Vector2 and returns the result.
    * @relates Vector2
    */
    template<class Number>
    Vector2<Number> operator*(const int & y, const Vector2<Number> & v) {
        return product(v, typename Vector2<Number>::value_type(y));
    }

    /** A three dimensional vector 
    * @f[ \vec{v} = (v_x, v_y, v_z) @f] 
    */
    template<class Number>
    class Vector3 : public TripleOf<Number> {
    public:
        typedef TripleOf<Number> Base;
        Vector3() : Base() {};
        Vector3(const Vector3<Number> & p) : Base(TripleOf<Number>(p)) {};
        Vector3(const FixedVector<3, Number> & p) : TripleOf<Number>(p) {};
        Vector3(Number a, Number b, Number c) : Base(TripleOf<Number>(a,b,c)) { }
        Vector3(const Number* t) : Base(TripleOf<Number>(t)) {};
        Vector3(const Number * theBegin, const Number * theEnd) : Base(theBegin, theEnd) {}
        Vector3 & operator=(const Vector3 & t) {
            assign(t);
            return *this;
        }
        template <class otherReal>
        void operator*=(const otherReal & y) {
            mult(Number(y));
        }
        template <class otherReal>
        void operator/=(const otherReal & y) {
            div(Number(y));
        }
        void operator+=(const Vector3 & y) {
            add(y);
        }
        void operator-=(const Vector3 & y) {
            sub(y);
        }
        void operator*=(const Vector3 & y) {
            mult(y);
        }
        void operator/=(const Vector3 & y) {
            div(y);
        }

        Vector3 operator*(const float & y) const {
            Vector3 result(*this);
            result.mult(Number(y));
            return result;
        }
        Vector3 operator*(const double & y) const {
            Vector3 result(*this);
            result.mult(Number(y));
            return result;
        }
        Vector3 operator*(const int & y) const {
            Vector3 result(*this);
            result.mult(Number(y));
            return result;
        }

        Vector3 operator/(const float & y) const {
            Vector3 result(*this);
            result.div(Number(y));
            return result;
        }
        Vector3 operator/(const double & y) const {
            Vector3 result(*this);
            result.div(Number(y));
            return result;
        }
        Vector3 operator/(const int & y) const {
            Vector3 result(*this);
            result.div(Number(y));
            return result;
        }

        Vector3 operator-() const {
            Vector3 result(*this);
            result.negate();
            return result;
        }
        Vector3 operator+(const Vector3 & y) const {
            Vector3 result(*this);
            result.add(y);
            return result;
        }
        Vector3 operator-(const Vector3 & y) const {
            Vector3 result(*this);
            result.sub(y);
            return result;
        }
        void add(const Vector3 & y) {
            Base::add(y);
        }
        void sub(const Vector3 & y) {
            Base::sub(y);
        }
        void mult(const Vector3 & y) {
            Base::mult(y);
        }
        void div(const Vector3 & y) {
            Base::div(y);
        }
        void add(Number y) {
            Base::add(y);
        }
        void sub(Number y) {
            Base::sub(y);
        }
        void mult(Number y) {
            Base::mult(y);
        }
        void div(Number y) {
            Base::div(y);
        }
        Vector3 normalize() const {
            return normalized( *this);
        }
    };
    template <class Number>
    Vector3<Number> * asVector3(Number * the3Numbers) {
        return reinterpret_cast<Vector3<Number>*>(the3Numbers);
    }
    template <class Number>
    const Vector3<Number> * asVector3(const Number * the3Numbers) {
        return reinterpret_cast<const Vector3<Number>*>(the3Numbers);
    }

    template <class Number>
    Vector3<Number> & asVector3(Number & the3Numbers) {
        return reinterpret_cast<Vector3<Number>&>(the3Numbers);
    }
    template <class Number>
    const Vector3<Number> & asVector3(const Number & the3Numbers) {
        return reinterpret_cast<const Vector3<Number>&>(the3Numbers);
    }

    template <class Number>
    Vector3<Number> product(const Vector3<Number> & v, Number y) {
        Vector3<Number> result(v);
        result.mult(y);
        return result;
    }
    template<class Number>
    Vector3<Number> operator*(const float & y, const Vector3<Number> & v) {
        return product(v, typename Vector3<Number>::value_type(y));
    }
    template<class Number>
    Vector3<Number> operator*(const double & y, const Vector3<Number> & v) {
        return product(v, typename Vector3<Number>::value_type(y));
    }
    template<class Number>
    Vector3<Number> operator*(const int & y, const Vector3<Number> & v) {
        return product(v, typename Vector3<Number>::value_type(y));
    }

    /** A four dimensional vector 
    * @f[ \vec{v} = (v_x, v_y, v_z, v_w) @f] 
    */
    template<class Number>
    class Vector4 : public QuadrupleOf<Number> {
    public:
        typedef QuadrupleOf<Number> Base;
        Vector4() : Base() {};
        Vector4(const Vector4<Number> & p) : Base(QuadrupleOf<Number>(p)) {};
        Vector4(const FixedVector<4, Number> & p) : QuadrupleOf<Number>(p) {};
        Vector4(Number a, Number b, Number c, Number d) : Base(a,b,c,d) { }
        Vector4(const Number* t) : Base(QuadrupleOf<Number>(t)) {};
        Vector4(const Number * theBegin, const Number * theEnd) : Base(theBegin, theEnd) {}
        Vector4 & operator=(const Vector4 & t) {
            assign(t);
            return *this;
        }
        template <class otherReal>
        void operator*=(const otherReal& y) {
            mult(Number(y));
        }
        template <class otherReal>
        void operator/=(const otherReal& y) {
            div(Number(y));
        }
        void operator+=(const Vector4& y) {
            add(y);
        }
        void operator-=(const Vector4& y) {
            sub(y);
        }
        void operator*=(const Vector4 & y) {
            mult(y);
        }
        void operator/=(const Vector4 & y) {
            div(y);
        }

        Vector4 operator*(const float & y) const {
            Vector4 result(*this);
            result.mult(Number(y));
            return result;
        }
        Vector4 operator*(const double & y) const {
            Vector4 result(*this);
            result.mult(Number(y));
            return result;
        }
        Vector4 operator*(const int & y) const {
            Vector4 result(*this);
            result.mult(Number(y));
            return result;
        }

        Vector4 operator/(const float & y) const {
            Vector4 result(*this);
            result.div(Number(y));
            return result;
        }
        Vector4 operator/(const double & y) const {
            Vector4 result(*this);
            result.div(Number(y));
            return result;
        }
        Vector4 operator/(const int & y) const {
            Vector4 result(*this);
            result.div(Number(y));
            return result;
        }

        Vector4 operator-() const {
            Vector4 result(*this);
            result.negate();
            return result;
        }
        Vector4 operator+(const Vector4 & y) const {
            Vector4 result(*this);
            result.add(y);
            return result;
        }
        Vector4 operator-(const Vector4 & y) const {
            Vector4 result(*this);
            result.sub(y);
            return result;
        }
        void add(const Vector4 & y) {
            Base::add(y);
        }
        void sub(const Vector4 & y) {
            Base::sub(y);
        }
        void mult(const Vector4 & y) {
            Base::mult(y);
        }
        void div(const Vector4 & y) {
            Base::div(y);
        }
        void add(Number y) {
            Base::add(y);
        }
        void sub(Number y) {
            Base::sub(y);
        }
        void mult(Number y) {
            Base::mult(y);
        }
        void div(Number y) {
            Base::div(y);
        }
        Vector4 normalize() const {
            return normalized( *this);
        }
    };
    template <class Number>
    Vector4<Number> * asVector4(Number * the4Numbers) {
        return reinterpret_cast<Vector4<Number>*>(the4Numbers);
    }
    template <class Number>
    const Vector4<Number> * asVector4(const Number * the4Numbers) {
        return reinterpret_cast<const Vector4<Number>*>(the4Numbers);
    }

    template <class Number>
    Vector4<Number> & asVector4(Number & the4Numbers) {
        return reinterpret_cast<Vector4<Number>&>(the4Numbers);
    }
    template <class Number>
    const Vector4<Number> & asVector4(const Number & the4Numbers) {
        return reinterpret_cast<const Vector4<Number>&>(the4Numbers);
    }

    template <class Number>
    Vector4<Number> product(const Vector4<Number> & v, Number y) {
        Vector4<Number> result(v);
        result.mult(y);
        return result;
    }
    template<class Number>
    Vector4<Number> operator*(const float & y, const Vector4<Number> & v) {
        return product(v, typename Vector4<Number>::value_type(y));
    }
    template<class Number>
    Vector4<Number> operator*(const double & y, const Vector4<Number> & v) {
        return product(v, typename Vector4<Number>::value_type(y));
    }
    template<class Number>
    Vector4<Number> operator*(const int & y, const Vector4<Number> & v) {
        return product(v, typename Vector4<Number>::value_type(y));
    }

    template <class VECTOR>
    VECTOR sum(const VECTOR & a, const VECTOR & b) {
        VECTOR myResult = a;
        myResult.add(b);
        return myResult;
    }
    template <class VECTOR>
    VECTOR difference(const VECTOR & a, const VECTOR & b) {
        VECTOR myResult = a;
        myResult.sub(b);
        return myResult;
    }

    template <class VECTOR>
    VECTOR product(const VECTOR & a, const VECTOR & b) {
        VECTOR myResult = a;
        myResult.mult(b);
        return myResult;
    }

    template <class VECTOR>
    VECTOR quotient(const VECTOR & a, const VECTOR & b) {
        VECTOR myResult = a;
        myResult.div(b);
        return myResult;
    }

    typedef Vector2<int>  Vector2i;
    typedef Vector2<float>  Vector2f;
    typedef Vector2<double> Vector2d;

    typedef Vector3<int> Vector3i;
    typedef Vector3<float> Vector3f;
    typedef Vector3<double> Vector3d;

    typedef Vector4<int> Vector4i;
    typedef Vector4<float> Vector4f;
    typedef Vector4<double> Vector4d;


    template<class Number>
    Number dot(const Vector2<Number> & a, const Vector2<Number> & b) {
        return a[0] * b[0] + a[1] * b[1];
    }
    template<class Number>
    Number dot(const Vector3<Number> & a, const Vector3<Number> & b) {
        return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    }
    template<class Number>
    Number dot(const Vector4<Number> & a, const Vector4<Number> & b) {
        return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
    }


    template<class Number>
    Number length(const Vector2<Number> & a) {
        return static_cast<Number>(sqrt(dot(a, a)));
    }

    template<class Number>
    Number length(const Vector3<Number> & a) {
        return static_cast<Number>(sqrt(dot(a, a)));
    }
    template<class Number>
    Number length(const Vector4<Number> & a) {
        return static_cast<Number>(sqrt(dot(a, a)));
    }

    inline
        int length(const Vector2<int> & a) {
            return static_cast<int>(sqrt(double(dot(a, a))));
    }
    inline
        int length(const Vector3<int> & a) {
            return static_cast<int>(sqrt(double(dot(a, a))));
    }
    inline
        int length(const Vector4<int> & a) {
            return static_cast<int>(sqrt(double(dot(a, a))));
    }
    template <class Number, template <class> class VECTOR>
    Number magnitude(const VECTOR<Number> & v) {
        return length(v);
    }

    template <class Number>
    Number distance(const Vector2<Number> & a, const Vector2<Number> & b) {
        return length(b - a);
    }
    template <class Number>
    Number distance(const Vector3<Number> & a, const Vector3<Number> & b) {
        return length(b - a);
    }
    template <class Number>
    Number distance(const Vector4<Number> & a, const Vector4<Number> & b) {
        return length(b - a);
    }

    template<class Number>
    Number cosAngle(const Vector2<Number> & a, const Vector2<Number> & b) {
        return dot(a, b)/(length(a) * length(b));
    }
    template<class Number>
    Number cosAngle(const Vector3<Number> & a, const Vector3<Number> & b) {
        return dot(a, b)/(length(a) * length(b));
    }

    template<class Number>
    Vector2<Number> normalized(const Vector2<Number> & v) {
        Number l = length(v);
        if (l==0) return v;
        return v / l;
    }

    template<class Number>
    Vector3<Number> normalized(const Vector3<Number> & v) {
        Number l = length(v);
        if (l==0) return v;
        return v / l;
    }
    template<class Number>
    Vector4<Number> normalized(const Vector4<Number> & v) {
        Number l = length(v);
        if (l==0) return v;
        return v / l;
    }

    // Note: cross product for Vector2 and Vector4 this is not defined in a geometrically useful way
    // and is therefore omitted
    template<class Number>
    Vector3<Number> cross(const Vector3<Number> & a, const Vector3<Number> & b) {
        return Vector3<Number>( (a[1] * b[2] - a[2] * b[1]),
            (a[2] * b[0] - a[0] * b[2]),
            (a[0] * b[1] - a[1] * b[0]) );
    }

    template<class Number>
    Vector3<Number> normal(const Vector3<Number> & a, const Vector3<Number> & b) {
        return normalized(cross(a, b));
    }

    /*!
    *  Computes an orthonormal vector to a that lies in the plane that is spanned by a
    *  and b.
    *  T = b - dot(a, b) * a
    */
    template <class Number>
    Vector3<Number>
        orthonormal(const Vector3<Number> & a, const Vector3<Number> & b) {
            Vector3<Number> myAn = normalized(a);
            Number myDotProduct = dot(myAn, b);
            return normalized( b - myDotProduct * myAn);
    }

    template <class Number>
    Number det2x2(Number a, Number b, Number c, Number d) {
        return a * d - b * c;
    }

    template <class Number>
    Number det3x3(Number a1, Number a2, Number a3,
        Number b1, Number b2, Number b3,
        Number c1, Number c2, Number c3 )
    {
        return a1 * det2x2( b2, b3, c2, c3 )
            - b1 * det2x2( a2, a3, c2, c3 )
            + c1 * det2x2( a2, a3, b2, b3 );
    }

    template <class Number>
    Number det3x3(const Vector3<Number> & a,
        const Vector3<Number> & b,
        const Vector3<Number> & c)
    {
        return a[0] * det2x2( b[1], b[2], c[1], c[2] )
            - b[0] * det2x2( a[1], a[2], c[1], c[2] )
            + c[0] * det2x2( a[1], a[2], b[1], b[2] );
    }

    template<class Number>
    Vector3<Number> projection(const Vector3<Number> & v1, const Vector3<Number> & v2) {
        Vector3<Number> v3 = normalized(v2);
        return dot(v1, v3) * v3;
    }

    /* @} */

} // namespace asl
#endif
