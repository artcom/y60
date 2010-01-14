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
//    $RCSfile: Point234.h,v $
//
//     $Author: david $
//
//   $Revision: 1.3 $
//
// Description: Point2 and Point3 class
//
//=============================================================================

#ifndef _included_asl_Point_
#define _included_asl_Point_

#include "asl_math_settings.h"

#include "Vector234.h"
#include <math.h>

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

   template<class Number>
	class Point2 : public PairOf<Number> {
	public:
        typedef PairOf<Number> Base;
        Point2() : Base() {};
		Point2(const Point2<Number> & p) : Base(p) {};
		Point2(const FixedVector<2, Number> & p) : PairOf<Number>(p) {};
		Point2(Number a, Number b) : Base(a,b) { }
		Point2(const Number * t) : Base(t) {};
        explicit Point2(const Number * theBegin, const Number * theEnd) : Base(theBegin, theEnd) {}
		Point2 & operator=(const Point2 & t) {
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
        void operator+=(const Vector2<Number> & y) {
            add(y);
        }
        void operator*=(const Vector2<Number> & y) {
            mult(y);
        }

        Point2 operator*(const float & y) const {
            Point2 result(*this);
            result.mult(Number(y));
            return result;
        }
        Point2 operator*(const double & y) const {
            Point2 result(*this);
            result.mult(Number(y));
            return result;
        }
        Point2 operator*(const int & y) const {
            Point2 result(*this);
            result.mult(Number(y));
            return result;
        }

        Point2 operator/(const float & y) const {
            Point2 result(*this);
            result.div(Number(y));
            return result;
        }
        Point2 operator/(const double & y) const {
            Point2 result(*this);
            result.div(Number(y));
            return result;
        }
        Point2 operator/(const int & y) const {
            Point2 result(*this);
            result.div(Number(y));
            return result;
        }

        Point2 operator-() const {
            Point2 result(*this);
            result.negate();
            return result;
        }
        Point2 operator+(const Vector2<Number> & y) const {
            Point2 result(*this);
            result.add(y);
            return result;
        }
        Vector2<Number> operator-(const Point2 & y) const {
            Vector2<Number> result(asVector(*this));
            result.sub(y);
            return result;
        }
        Point2<Number> operator-(const Vector2<Number> & y) const {
            Point2 result(*this);
            result.sub(y);
            return result;
        }
        void add(const Vector2<Number> & y) {
            Base::add(asPoint(y));
        }
        void sub(const Vector2<Number> & y) {
            Base::sub(asPoint(y));
        }
        void mult(const Vector2<Number> & y) {
            Base::mult(asPoint(y));
        }
        void div(const Vector2<Number> & y) {
            Base::div(asPoint(y));
        }
        void add(Number y) {
            Base::add(Number(y));
        }
        void sub(Number y) {
            Base::sub(Number(y));
        }
        void mult(Number y) {
            Base::mult(Number(y));
        }
        void div(Number y) {
            Base::div(Number(y));
        }
    };
    template <class Number>
    Point2<Number> product(const Point2<Number> & v, Number y) {
        Point2<Number> result(v);
        result.mult(y);
        return result;
    }

    template<class Number>
    Point2<Number> operator*(const float & y, const Point2<Number> & v) {
        return product(v, typename Point2<Number>::value_type(y));
    }
    template<class Number>
    Point2<Number> operator*(const double & y, const Point2<Number> & v) {
        return product(v, typename Point2<Number>::value_type(y));
    }
    template<class Number>
    Point2<Number> operator*(const int & y, const Point2<Number> & v) {
        return product(v, typename Point2<Number>::value_type(y));
    }

    template<class Number>
    class Point3 : public TripleOf<Number> {
	public:
        typedef TripleOf<Number> Base;
        Point3() : Base() {};
		Point3(const Point3<Number> & p) : Base(TripleOf<Number>(p)) {};
		Point3(const FixedVector<3, Number> & p) : TripleOf<Number>(p) {};
		Point3(Number a, Number b, Number c) : Base(TripleOf<Number>(a,b,c)) { }
		Point3(const Number* t) : Base(TripleOf<Number>(t)) {};
        Point3(const Number * theBegin, const Number * theEnd) : Base(theBegin, theEnd) {}
		Point3 & operator=(const Point3 & t) {
            assign(t);
            return *this;
        }
        template <class otherReal>
        void operator*=(const otherReal&  y) {
            mult(Number(y));
        }
        template <class otherReal>
        void operator/=(const otherReal & y) {
            div(Number(y));
        }
        void operator+=(const Vector3<Number> & y) {
            add(y);
        }
        void operator*=(const Vector3<Number> & y) {
            mult(y);
        }

        Point3 operator*(const float & y) const {
            Point3 result(*this);
            result.mult(Number(y));
            return result;
        }
        Point3 operator*(const double & y) const {
            Point3 result(*this);
            result.mult(Number(y));
            return result;
        }
        Point3 operator*(const int & y) const {
            Point3 result(*this);
            result.mult(Number(y));
            return result;
        }

        Point3 operator/(const float & y) const {
            Point3 result(*this);
            result.div(Number(y));
            return result;
        }
        Point3 operator/(const double & y) const {
            Point3 result(*this);
            result.div(Number(y));
            return result;
        }
        Point3 operator/(const int & y) const {
            Point3 result(*this);
            result.div(Number(y));
            return result;
        }

        Point3 operator-() const {
            Point3 result(*this);
            result.negate();
            return result;
        }
        Point3 operator+(const Point3 & y) const {
            Point3 result(*this);
            result.add(y);
            return result;
        }
        Vector3<Number> operator-(const Point3 & y) const {
            Vector3<Number> result(asVector(*this));
            result.sub(y);
            return result;
        }
        Point3 operator-(const Vector3<Number> & y) const {
            Point3 result(*this);
            result.sub(y);
            return result;
        }
        void add(const Vector3<Number> & y) {
            Base::add(asPoint(y));
        }
        void sub(const Vector3<Number> & y) {
            Base::sub(asPoint(y));
        }
        void mult(const Vector3<Number> & y) {
            Base::mult(asPoint(y));
        }
        void div(const Vector3<Number> & y) {
            Base::div(asPoint(y));
        }
        void add(Number y) {
            Base::add(Number(y));
        }
        void sub(Number y) {
            Base::sub(Number(y));
        }
        void mult(Number y) {
            Base::mult(Number(y));
        }
        void div(Number y) {
            Base::div(Number(y));
        }
    };
    template <class Number>
    Point3<Number> product(const Point3<Number> & v, Number y) {
        Point3<Number> result(v);
        result.mult(y);
        return result;
    }

    template<class Number>
    Point3<Number> operator*(const float & y, const Point3<Number> & v) {
        return product(v, typename Point3<Number>::value_type(y));
    }
    template<class Number>
    Point3<Number> operator*(const double & y, const Point3<Number> & v) {
        return product(v, typename Point3<Number>::value_type(y));
    }
    template<class Number>
    Point3<Number> operator*(const int & y, const Point3<Number> & v) {
        return product(v, typename Point3<Number>::value_type(y));
    }

    template<class Number>
	class Point4 : public QuadrupleOf<Number> {
	public:
        typedef QuadrupleOf<Number> Base;
        Point4() : Base() {};
		Point4(const Point4<Number> & p) : Base(QuadrupleOf<Number>(p)) {};
		Point4(const FixedVector<4, Number> & p) : QuadrupleOf<Number>(p) {};
		Point4(Number a, Number b, Number c, Number d) : Base(QuadrupleOf<Number>(a,b,c,d)) { }
		Point4(const Number* t) : Base(QuadrupleOf<Number>(t)) {};
        Point4(const Number * theBegin, const Number * theEnd) : Base(theBegin, theEnd) {}
		Point4 & operator=(const Point4 & t) {
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
        void operator+=(const Vector4<Number> & y) {
            add(y);
        }
        void operator*=(const Vector4<Number> & y) {
            mult(y);
        }

        Point4 operator*(const float & y) const {
            Point4 result(*this);
            result.mult(Number(y));
            return result;
        }
        Point4 operator*(const double & y) const {
            Point4 result(*this);
            result.mult(Number(y));
            return result;
        }
        Point4 operator*(const int & y) const {
            Point4 result(*this);
            result.mult(Number(y));
            return result;
        }

        Point4 operator/(const float & y) const {
            Point4 result(*this);
            result.div(Number(y));
            return result;
        }
        Point4 operator/(const double & y) const {
            Point4 result(*this);
            result.div(Number(y));
            return result;
        }
        Point4 operator/(const int & y) const {
            Point4 result(*this);
            result.div(Number(y));
            return result;
        }

        Point4 operator-() const {
            Point4 result(*this);
            result.negate();
            return result;
        }
        Point4 operator+(const Point4 & y) const {
            Point4 result(*this);
            result.add(y);
            return result;
        }
        Vector4<Number> operator-(const Point4 & y) const {
            Vector4<Number> result(asVector(*this));
            result.sub(y);
            return result;
        }
        Point4 operator-(const Vector4<Number> & y) const {
            Point4 result(*this);
            result.sub(y);
            return result;
        }
        void add(const Vector4<Number> & y) {
            Base::add(asPoint(y));
        }
        void sub(const Vector4<Number> & y) {
            Base::sub(asPoint(y));
        }
        void mult(const Vector4<Number> & y) {
            Base::mult(asPoint(y));
        }
        void div(const Vector4<Number> & y) {
            Base::div(asPoint(y));
        }
        void add(Number y) {
            Base::add(Number(y));
        }
        void sub(Number y) {
            Base::sub(Number(y));
        }
        void mult(Number y) {
            Base::mult(Number(y));
        }
        void div(Number y) {
            Base::div(Number(y));
        }
    };
    template <class Number>
    Point4<Number> product(const Point4<Number> & v, Number y) {
        Point4<Number> result(v);
        result.mult(y);
        return result;
    }

    template<class Number>
    Point4<Number> operator*(const float & y, const Point4<Number> & v) {
        return product(v, typename Point4<Number>::value_type(y));
    }
    template<class Number>
    Point4<Number> operator*(const double & y, const Point4<Number> & v) {
        return product(v, typename Point4<Number>::value_type(y));
    }
    template<class Number>
    Point4<Number> operator*(const int & y, const Point4<Number> & v) {
        return product(v, typename Point4<Number>::value_type(y));
    }

    typedef Point2<float>  Point2f;
    typedef Point2<double> Point2d;
    typedef Point2<int>  Point2i;

    typedef Point3<float>  Point3f;
    typedef Point3<double> Point3d;
    typedef Point3<int>  Point3i;

    typedef Point4<float>  Point4f;
    typedef Point4<double> Point4d;
    typedef Point4<int>  Point4i;

    // sometimes we want to treat vectors as points and vice versa
	// but we do it explicitly using the following functions:
    template<class Number>
	const Point2<Number> & asPoint(const Vector2<Number> & v) {
		 return reinterpret_cast<const Point2<Number> &>(v);
	}
    template<class Number>
	const Point3<Number> & asPoint(const Vector3<Number> & v) {
		 return reinterpret_cast<const Point3<Number> &>(v);
	}
    template<class Number>
	const Point4<Number> & asPoint(const Vector4<Number> & v) {
		 return reinterpret_cast<const Point4<Number> &>(v);
	}

    template<class Number>
	const Vector2<Number> & asVector(const Point2<Number> & p) {
		 return reinterpret_cast<const Vector2<Number> &>(p);
	}
    template<class Number>
	const Vector3<Number> & asVector(const Point3<Number> & p) {
		 return reinterpret_cast<const Vector3<Number> &>(p);
	}
    template<class Number>
	const Vector4<Number> & asVector(const Point4<Number> & p) {
		 return reinterpret_cast<const Vector4<Number> &>(p);
	}

    // Vector = Point - Point
	template<class Number>
	Vector2<Number> operator-(const Point2<Number> & a, const Point2<Number> & b) {
		Vector2<Number> result = asVector(a);
		result.sub(b);
		return result;
	}
	template<class Number>
	Vector3<Number> operator-(const Point3<Number> & a, const Point3<Number> & b) {
		Vector3<Number> result = asVector(a);
		result.sub(b);
		return result;
	}
	template<class Number>
	Vector4<Number> operator-(const Point4<Number> & a, const Point4<Number> & b) {
		Vector4<Number> result = asVector(a);
		result.sub(b);
		return result;
	}

    // Point = Point - Vector
    template<class Number>
	Point2<Number> operator-(const Point2<Number> & a, const Vector2<Number> & b) {
		Point2<Number> result = a;
		result.sub(b);
		return result;
	}
    template<class Number>
	Point3<Number> operator-(const Point3<Number> & a, const Vector3<Number> & b) {
		Point3<Number> result = a;
		result.sub(b);
		return result;
	}
    template<class Number>
	Point4<Number> operator-(const Point4<Number> & a, const Vector4<Number> & b) {
		Point4<Number> result = a;
		result.sub(b);
		return result;
	}

    // Point = Point + Vector
    template<class Number>
	Point2<Number> operator+(const Point2<Number> & a, const Vector2<Number> & b) {
		Point2<Number> result = a;
		result.add(b);
		return result;
	}
    template<class Number>
	Point3<Number> operator+(const Point3<Number> & a, const Vector3<Number> & b) {
		Point3<Number> result = a;
		result.add(b);
		return result;
	}
    template<class Number>
	Point4<Number> operator+(const Point4<Number> & a, const Vector4<Number> & b) {
		Point4<Number> result = a;
		result.add(b);
		return result;
	}

    // distance vector-vector
    template<class Number>
	Number distance(const Point2<Number> & p1, const Point2<Number> & p2) {
		return length(p2-p1);
	}

    template<class Number>
	Number distance(const Point3<Number> & p1, const Point3<Number> & p2) {
		return length(p2-p1);
	}

    template<class Number>
	Number distance(const Point4<Number> & p1, const Point4<Number> & p2) {
		return length(p2-p1);
	}

    /* @} */
} // namespace asl;
#endif
