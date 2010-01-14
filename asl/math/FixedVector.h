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

#ifndef _asl_fixedVector_h_
#define _asl_fixedVector_h_

#include "asl_math_settings.h"

#include "VectorManipulator.h"
#include "numeric_functions.h"

#include <iostream>

#include <cmath> // TODO: replace abs with apropriate C++ function

namespace asl {

/*! @addtogroup aslmath */
/* @{ */

/** FixedVector style interface for POD types.
 * Use this class if you have a POD-Type like a struct or c-array with well known
 * layout. Use with great caution.
 */

template <int SIZE, class T>
struct FixedVectorPOD {
    typedef FixedVectorPOD<SIZE, T> base_type;
    typedef T value_type;
    typedef T FixedVector_type[SIZE];
    typedef T * iterator;
    typedef const T * const_iterator;
    typedef std::size_t size_type;
    static size_type size() { return SIZE;}
    enum {VECTOR_SIZE = SIZE};

    template <class ANYPOD>
    FixedVectorPOD(ANYPOD &) {}

    T& operator[](size_type i) {
        return reinterpret_cast<T*>(this)[i];
    }
    const T& operator[](size_type i) const {
        return reinterpret_cast<const T*>(this)[i];
    }

    T * begin() {
        return &(*this)[0];
    }
    T * end() {
        return &(*this)[SIZE];
    }
    const T * begin() const {
        return &(*this)[0];
    }
    const T * end() const {
        return &(*this)[SIZE];
    }
};


/** Replacement type for C-type arrays. Mostly used for numeric n-tuples.
 *
 * \f[ \vec{a} = \left( a_1, a_2, \ldots, a_{size} \right) @f]
 *
 * Design Note:
 * FixedVector must not be derived from FixedVectorPOD because some compilers will
 * then put an additional field into our class layout when we derive from a class
 * without data members.
 */
template <int THE_SIZE, class T>
struct FixedVector  {
    enum { SIZE = THE_SIZE };
    typedef FixedVector<SIZE, T> my_type;
    typedef T value_type;
    typedef T FixedVector_type[SIZE];
    typedef T * iterator;
    typedef const T * const_iterator;
    typedef std::size_t size_type;

    /** Default constructor. */
    FixedVector() {}
    /** Initialize from C style array.
     * @param t
     */
    explicit FixedVector(const T * t) {
        size_type i;
        for (i=0;i<SIZE;i++) {
            val[i] = t[i];
        }
    }

    /** Initialize from two T pointers/iterators.
     * It is not an error if the size between theBegin and theEnd is larger than
     * the size of the FixedVector. Only SIZE elements will be copied.
     * @param theBegin
     * @param theEnd
     */
    FixedVector(const T * theBegin, const T * theEnd) {
        unsigned mySize = theEnd - theBegin;
        if (mySize > SIZE) {
            mySize = SIZE;
        }
        for (size_type i=0;i<mySize;i++) {
            val[i] = theBegin[i];
        }
    }
    /** Copy constructor.
     * @param otherVector
     */
    FixedVector(const FixedVector & otherVector) {
        for (size_type i=0;i<SIZE;i++) {
            val[i] = otherVector[i];
        }
    }
    /** Assignment method.
     * @param s
     */
    void assign(const FixedVector & s) {
        for (size_type i=0;i<SIZE;i++) {
            val[i] = s[i];
        }
    }
    /** Assignment operator.
     * @param s
     */
    FixedVector & operator=(const FixedVector & s) {
        assign(s);
        return *this;
    }
    /*
    FixedVector(const T & v) {
        for (size_type i=0;i<SIZE;i++) {
            val[i] = v;
        }
    }
    */
    /** Subscription operator.
     * @param i
     */
    T & operator[](size_type i) {
        return val[i];
    }
    /** Subscription operator.
     * @param i
     */
    const T & operator[](size_type i) const {
        return val[i];
    };
    /**
     * @name STL Style Methods */
    // @{
    /** Returns the size of the vector. */
    static size_type size() { return SIZE;}
    /** Returns an iterator pointing to the first element of the vector. */
    T * begin() {
        return &val[0];
    }
    /** Returns a const_iterator pointing to the first element of the vector. */
    const T * begin() const {
        return &val[0];
    }
    /** Returns an iterator pointing to the end of the vector. */
    T * end() {
        return &val[SIZE];
    }
    /** Returns a const_iterator pointing to the end of the vector. */
    const T * end() const {
        return &val[SIZE];
    }
    // @}
    /**
     * @name Artithmetic Methods */
    // @{
    /** Component wise addition. */
    void add(const FixedVector & v) {
        for (size_type i = 0; i < SIZE; ++ i) {
            val[i]+=v.val[i];
        }
    }
    /** Component wise subtraction. */
    void sub(const FixedVector & v) {
        for (size_type i = 0; i < SIZE; ++ i) {
            val[i]-=v.val[i];
        }
    }
    /** Component wise multiplication. */
    void mult(const FixedVector & v) {
        for (size_type i = 0; i < SIZE; ++ i) {
            val[i]*=v.val[i];
        }
    }
    /** Component wise division. */
    void div(const FixedVector & v) {
        for (size_type i = 0; i < SIZE; ++ i) {
            val[i]/=v.val[i];
        }
    }
    /** Scalar addition. */
    void add(const T & n) {
        for (size_type i = 0; i < SIZE; ++ i) {
            val[i]+=n;
        }
    }
    /** Scalar subtraction. */
    void sub(const T & n) {
        for (size_type i = 0; i < SIZE; ++ i) {
            val[i]-=n;
        }
    }
    /** Scalar multiplication. */
    void mult(const T & n) {
        for (size_type i = 0; i < SIZE; ++ i) {
            val[i]*=n;
        }
    }
    /** Scalar division. */
    void div(const T & n) {
        for (size_type i = 0; i < SIZE; ++ i) {
            val[i]/=n;
        }
    }
    /** Negation. */
    void negate() {
        for (size_type i = 0; i < SIZE; ++ i) {
            val[i] = -val[i];
        }
    }
    // @}

    /**
     * @name Vector Specific Methods */
    // @{
    /** Get the sqaured length of a vector.
     *
     * Returns @f[ \sum_{i=0}^{SIZE-1} v^2_i @f] where @f$\vec{v} @f$ is the vector
     * the method is called on. Faster than getLength() because it avoids the square
     * root operation.
     */
    T getSquaredLength() {
        T myResult = 0;
        for (size_type i = 0; i < SIZE; ++i) {
            myResult += val[i] * val[i];
        }
        return myResult;
    }

    /** Get the length of a vector.
     *
     * Returns @f$ |\vec{v}| @f$ where @f$\vec{v} @f$ is the vector
     * the method is called on.
     */
    T getLength() {
        return T(sqrt(double(getSquaredLength())));
    }

    bool empty() const {
        return false;
    }
    // @}

protected:
    FixedVector_type val;
};
/*
template <class RealType, int SIZE, class Number>
FixedVector<SIZE, Number> operator*(const RealType & y, const FixedVector<SIZE, Number> & v) {
    FixedVector<SIZE, Number> result(v);
    result.mult(Number(y));
    return result;
}
*/

/** Returns true if @a a equals @a b, false otherwise;
 * @relates FixedVector
 */
template <int SIZE, class T>
bool equal(const FixedVector<SIZE, T> & a, const FixedVector<SIZE, T> & b) {
    for (typename FixedVector<SIZE, T>::size_type i = 0; i < SIZE;++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

/** Equal operator for FixedVector
 * @relates FixedVector
 *
 * Returns true if @a a equals @a b, false otherwise;
 */
template <int SIZE, class T>
bool operator==(const FixedVector<SIZE,T> & a, const FixedVector<SIZE,T> & b) {
    return equal(a, b);
}

/** Returns true if @a a and @a b are not equal, false otherwise;
 * @relates FixedVector
 */
template <int SIZE, class T>
bool unequal(const FixedVector<SIZE, T> & a, const FixedVector<SIZE, T> & b) {
    for (typename FixedVector<SIZE, T>::size_type i = 0; i < SIZE;++i) {
        if (a[i] != b[i]) return true;
    }
    return false;
}

/** Unequal operator for FixedVector
 * @relates FixedVector
 *
 * Returns true if @a a and @a b are not equal, false otherwise.
 */
template <int SIZE, class T>
bool operator!=(const FixedVector<SIZE,T> & a, const FixedVector<SIZE,T> & b) {
    return unequal(a, b);
}

/** almostEqual for FixedVector
 * @relates FixedVector
 *
 * Returns true if almostEqual(double a, double b, double thePrecision) returns true
 * for each @f$a_i@f$, @f$b_i@f$ where @f$ 0 \leq i < SIZE @f$
 */
template <int SIZE, class T>
bool almostEqual(const FixedVector<SIZE,T> & a, const FixedVector<SIZE,T> & b, double thePrecision = 1E-5) {
    const T* myAIterator = a.begin();
    const T* myBIterator = b.begin();
    for (; myAIterator != a.end(); ++myAIterator, ++myBIterator) {
        if (!almostEqual(*myAIterator, *myBIterator, thePrecision)) return false;
    }
    return true;
}

/** Compute the dot product for @a a and @a b
 * @relates FixedVector
 *
 * @f[ \vec{a} \cdot \vec{b} = \sum_{i=0}^{SIZE-1} a_i * b_i @f]
 */
template <int SIZE, class T>
T
dot(const FixedVector<SIZE, T> & a, const FixedVector<SIZE, T> & b) {
    T myResult = 0;
    for (typename FixedVector<SIZE, T>::size_type i = 0; i < SIZE; ++i) {
        myResult += a[i] * b[i];
    }
    return myResult;
}



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

template <class T>
std::ostream & printVector(std::ostream &os, const T & v,
        bool oneElementPerLine = false,
        const char theStartToken = '[',
        const char theEndToken = ']',
        const char theDelimiter = ',')
{
    int mySize = v.end() - v.begin();
    os << theStartToken;
    for (int i = 0; i < mySize; ++i) {
        if (oneElementPerLine) {
            os << '\n';
        }
        os << v[i];
        if (i < mySize - 1) {
            os << theDelimiter;
        }
    }
    return os << theEndToken;
}

template <class T>
std::istream & parseVector(std::istream & is, T & v,
                           const char theStartToken = '[',
                           const char theEndToken = ']',
                           const char theDelimiter = ',')
{

    char myChar;
    is >> myChar;
    if ( myChar != theStartToken) {
        is.setstate(std::ios::failbit);
        return is;
    }

    typename T::size_type mySize = v.size();
    for (typename T::size_type i = 0; i < mySize; ++i) {
        is >> v[i];

        if (!is) {
            is.setstate(std::ios::failbit);
            return is;
        }

        is >> myChar;
        if (((i < mySize - 1) &&  myChar != theDelimiter) ||
                ((i == mySize - 1) &&  myChar != theEndToken))
        {
            is.setstate(std::ios::failbit);
            return is;
        }
    }
    return is;
}

template <int SIZE, class T>
std::istream & operator>>(std::istream & is, asl::FixedVector<SIZE,T> & f) {
    if (is.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
        return parseVector(is, f,
                static_cast<char>(is.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                static_cast<char>(is.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                static_cast<char>(is.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
    } else {
        return parseVector(is, f);
    }
}

template <int SIZE, class T>
std::ostream & operator<<(std::ostream & os,
        const asl::FixedVector<SIZE,T> & f)
{
    if (os.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
        return printVector(os, f,
                0 != os.iword(FixedVectorStreamFormatter::ourOneElementPerLineFlagIndex),
                static_cast<char>(os.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                static_cast<char>(os.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                static_cast<char>(os.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
    } else {
        return printVector(os, f);
    }
}


template <int SIZE, class T>
std::istream & operator>>(std::istream & is, asl::FixedVectorPOD<SIZE,T> & f) {
    return parseVector(is, f);
}

template <int SIZE, class T>
std::ostream & operator<<(std::ostream & os,
        const asl::FixedVectorPOD<SIZE,T> & f)
{
    if (os.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
        return printVector(os, f,
                0 != os.iword(FixedVectorStreamFormatter::ourOneElementPerLineFlagIndex),
                static_cast<char>(os.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                static_cast<char>(os.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                static_cast<char>(os.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
    } else {
        return printVector(os, f);
    }
}

/* @} */

}; // end of namespace asl

#endif
