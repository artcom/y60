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
//    $RCSfile: fraction.h,v $
//
//   $Revision: 1.2 $
//
// Description: fractional number class; see Knuth,Vol.2,Page 313 (Fractions)
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
// originally written by Pavel - Tue Jun 17 19:18:17 CEST 1997
// major refactoring during spare time 2003/2004
//
#ifndef _ASL_RASTER_FRACTION_H_INCLUDED_
#define _ASL_RASTER_FRACTION_H_INCLUDED_

#include <math.h>
#include <iostream>

namespace asl {

template <class Int>
inline Int odd(Int n) {return(n&1);}

template <class Int>
inline Int even(Int n) {return !odd(n);}

template <class Int>
inline Int gcd(Int u, Int v) {
    assert(u>0);
    assert(v>0);

    //    cerr << "gcd(" << u << "," << v << ") = ";
    Int k = 0;
    while(even(u) && even(v)) {
        ++k;
        u/=2;
        v/=2;
    }
    Int t;
    if (odd(u))
        t = -v;
    else
        t = u;
    do {
        while (even(t)) t/=2;
        if (t>0)
            u = t;
        else
            v = -t;
        t = u - v;
    } while (t);
    Int result = u * (1<<k);
    //    cerr << result << endl;
    return result;
};

/// base_frac ist Bruchklasse, die maximal schnell bei arithmetischen
/// Operationen ist; Zaehler und Nenner koennen aber evtl. sehr gross
/// werden zum kuerzen kann explicit norm() gerufen werden. 
/// Das Vorzeichen sollte im Zaehler stehen,.d.h. der Nenner positiv
/// sein; norm() stellt auch dieses sicher.
/// Wenn haufig ein Ergebnis in gekuerzter Form benoetigt wird,
/// ist frac schneller.

template <class Int>
struct base_frac {
    Int p;
    Int q;
    base_frac() {}
    base_frac(Int n) { p = n; q = 1;}
    base_frac(Int P, Int Q) { p = P; q = Q;}
    base_frac(const base_frac& x) { p = x.p; q = x.q;}

    void norm_sign() const {
        if (q<0) {
            const_cast<base_frac<Int>*>(this)->q = -q;
            const_cast<base_frac<Int>*>(this)->p = -p;
        }
    }

    void norm() const {
        assert(q);
        if (p==0) {
            const_cast<base_frac<Int>*>(this)->q = 1;
            return;
        }
        norm_sign();
        if (p>0) {
            Int d = gcd(p, q);
            const_cast<base_frac<Int>*>(this)->p/=d;
            const_cast<base_frac<Int>*>(this)->q/=d;
        }
        else {
            Int d = gcd(-p, q);
            const_cast<base_frac<Int>*>(this)->p/=d;
            const_cast<base_frac<Int>*>(this)->q/=d;
        }
    }

    void add(const base_frac& f) {
        if (q != f.q) {
            p = p * f.q + f.p * q;
            q = q * f.q;
        } 
        else p+=f.p;
    }
    void subtract(const base_frac& f) {
        if (q != f.q) {
            p = p * f.q - f.p * q;
            q = q * f.q;
        } 
        else p-=f.p;
    }
    void mult(const base_frac& f) {
        p*=f.p;
        q*=f.q;
    }
    void div(const base_frac& f) {
        p*=f.q;
        q*=f.p;
    }
    void negate() {
        p = -p;
    }
    base_frac operator+(const base_frac& n) const {
        base_frac result = *this;
        result.add(n);
        return result;
    }
    base_frac& operator+=(const base_frac& n) {
        add(n);
        return *this;
    }
    base_frac operator-() const {
        base_frac result = *this;
        result.negate();
        return result;
    }
    base_frac operator-(const base_frac& n) const {
        base_frac result = *this;
        result.subtract(n);
        return result;
    }
    base_frac& operator-=(const base_frac& n) {
        subtract(n);
        return *this;
    }
    base_frac operator*(const base_frac& n) const {
        base_frac result = *this;
        result.mult(n);
        return result;
    }
    base_frac& operator*=(const base_frac& n) {
        mult(n);
        return *this;
    }
    base_frac operator/(const base_frac& n) const {
        base_frac result = *this;
        result.div(n);
        return result;
    }
    base_frac& operator/=(const base_frac& n) {
        div(n);
        return *this;
    }
    bool less_than(const base_frac& f) const {
        return (p * f.q < f.p * q);
    }
    bool greater_than(const base_frac& f) const {
        return (p * f.q > f.p * q);
    }
    bool less_or_equal_than(const base_frac& f) const {
        return (p * f.q <= f.p * q);
    }
    bool greater_or_equal_than(const base_frac& f) const {
        return (p * f.q >= f.p * q);
    }
    bool equal_to(const base_frac& f) const {
        if ((p == f.p) && (q == f.q)) return 1;
        return (p * f.q == f.p * q);
    }
    bool not_equal_to(const base_frac& f) const {
        return !equal_to(f);
    }
};

template <class Int>
inline bool operator<(const base_frac<Int>& f1, const base_frac<Int>& f2){
    return f1.less_than(f2);
}
template <class Int>
inline bool operator>(const base_frac<Int>& f1, const base_frac<Int>& f2){
    return f1.greater_than(f2);
}
template <class Int>
inline bool operator<=(const base_frac<Int>& f1, const base_frac<Int>& f2){
    return f1.less_or_equal_than(f2);
}
template <class Int>
inline bool operator>=(const base_frac<Int>& f1, const base_frac<Int>& f2){
    return f1.greater_or_equal_than(f2);
}
template <class Int>
inline bool operator==(const base_frac<Int>& f1, const base_frac<Int>& f2){
    return f1.equal_to(f2);
}
template <class Int>
inline bool operator!=(const base_frac<Int>& f1, const base_frac<Int>& f2){
    return f1.not_equal_to(f2);
}

template <class Int>
inline std::ostream& operator<<(std::ostream& o, const base_frac<Int>& f)
{
    return o << "(" << f.p << "/" << f.q << ")";
}

} // namespace asl

template <class Int>
Int floor(const asl::base_frac<Int>& f) {
    f.norm_sign();
    if (f.p % f.q == 0) return f.p/f.q;
    if (f.p >= 0)
        return f.p/f.q;
    else
        return f.p/f.q - 1;
}
template <class Int>
asl::base_frac<Int> floor_fraction(const asl::base_frac<Int>& f) {
    return f - floor(f);
}

template <class Int>
Int ceil(const asl::base_frac<Int>& f) {
    f.norm_sign();
    if (f.p % f.q == 0) return f.p/f.q;
    if (f.p >= 0)
        return f.p/f.q + 1;
    else
        return f.p/f.q;
}
template <class Int>
asl::base_frac<Int> ceil_fraction(const asl::base_frac<Int>& f) {
    return asl::base_frac<Int>(ceil(f)) - f;
}

template <class Int>
Int trunc(const asl::base_frac<Int>& f) {
    f.norm_sign();
    if (f.p % f.q == 0) return f.p/f.q;
    if (f.p >= 0)
        return f.p/f.q;
    else
        return f.p/f.q;
}
template <class Int>
asl::base_frac<Int> trunc_fraction(const asl::base_frac<Int>& f) {
    return f - trunc(f);
}


template <class Int>
Int round(const asl::base_frac<Int>& f) {
    f.norm_sign();
    if (f.p % f.q == 0) return f.p/f.q;
    if (f.p >= 0)
        return (2*f.p+f.q)/(2*f.q);
    else {
        //	return (2*f.p-f.q)/(2*f.q);
        return floor(f + asl::base_frac<Int>(1, 2));
    }
} 
template <class Int>
asl::base_frac<Int> round_fraction(const asl::base_frac<Int>& f) {
    return f - round(f);
}

namespace asl {

template <class Int>
double as_float(const base_frac<Int>& f) {
    return (float)f.p / (float)f.q;
}
template <class Int>
double as_double(const base_frac<Int>& f) {
    return (double)f.p / (double)f.q;
}

/// frac ist Bruchklasse, die Zaehler und Nenner automatisch so
/// klein wie moeglich haelt. 

template <class Int>
struct frac : base_frac<Int> {
    typedef base_frac<Int> base;
    frac() {}
    frac(Int n) { base::p = n; base::q = 1;}
    frac(Int P, Int Q) { base::p = P; base::q = Q;base::norm();}
    frac(const frac& x) { base::p = x.p; base::q = x.q;}
    //   frac(const base_frac<Int>& x) { p = x.p; q = x.q;norm();}

    //    frac fractional_part() const {return *this - frac(integer_part());}

    void add(const frac& f) {
        base::add(f);
        base::norm();
    }
    void subtract(const frac& f) {
        base::subtract(f);
        base::norm();
    }
    void mult(const frac& f) {
        //	cerr << *this << " * " << f << endl;
        base::mult(f);
        base::norm();
        //	cerr << " == " << *this << endl;
    }
    void div(const frac& f) {
        base::div(f);
        base::norm();
    }
    frac operator+(const frac& n) const {
        frac result = *this;
        result.add(n);
        return result;
    }
    frac& operator+=(const frac& n) {
        add(n);
        return *this;
    }
    frac operator-(const frac& n) const {
        frac result = *this;
        result.subtract(n);
        return result;
    }
    frac operator-() const {
        frac result = *this;
        result.negate();
        return result;
    }
    frac& operator-=(const frac& n) {
        subtract(n);
        return *this;
    }
    frac operator*(const frac& n) const {
        frac result = *this;
        result.mult(n);
        return result;
    }
    frac& operator*=(const frac& n) {
        mult(n);
        return *this;
    }
    frac operator/(const frac& n) const {
        frac result = *this;
        result.div(n);
        return result;
    }
    frac& operator/=(const frac& n) {
        div(n);
        return *this;
    }
    bool less_than(const frac& f) const {
        return (base::p * f.q < f.p * base::q);
    }
    bool greater_than(const frac& f) const {
        return (base::p * f.q > f.p * base::q);
    }
    bool less_or_equal_than(const frac& f) const {
        return (base::p * f.q <= f.p * base::q);
    }
    bool greater_or_equal_than(const frac& f) const {
        return (base::p * f.q >= f.p * base::q);
    }
    bool equal_to(const frac& f) const {
        return (base::p == f.p) && (base::q == f.q);
    }
    bool not_equal_to(const frac& f) const {
        return (base::p != f.p) || (base::q != f.q);
    }
};

template <class Int>
inline bool operator<(const frac<Int>& f1, const frac<Int>& f2){
    return f1.less_than(f2);
}
template <class Int>
inline bool operator>(const frac<Int>& f1, const frac<Int>& f2){
    return f1.greater_than(f2);
}
template <class Int>
inline bool operator<=(const frac<Int>& f1, const frac<Int>& f2){
    return f1.less_or_equal_than(f2);
}
template <class Int>
inline bool operator>=(const frac<Int>& f1, const frac<Int>& f2){
    return f1.greater_or_equal_than(f2);
}
template <class Int>
bool operator==(const frac<Int>& f1, const frac<Int>& f2){
    return f1.equal_to(f2);
}
template <class Int>
inline bool operator!=(const frac<Int>& f1, const frac<Int>& f2){
    return f1.not_equal_to(f2);
}
    
template <class Int>
inline std::ostream& operator<<(std::ostream& o, const frac<Int>& f)
{
    return o << "(" << f.p << "/" << f.q << ")";
}

} // namespace asl

template <class Int>
Int floor(const asl::frac<Int>& f) {
    if (f.q == 1) return f.p;
    if (f.p >= 0)
        return f.p/f.q;
    else
        return f.p/f.q - 1;
}
template <class Int>
asl::frac<Int> floor_fraction(const asl::frac<Int>& f) {
    return f - floor(f);
}


template <class Int>
Int ceil(const asl::frac<Int>& f) {
    if (f.q == 1) return f.p;
    if (f.p >= 0)
        return f.p/f.q + 1;
    else
        return f.p/f.q;
}
template <class Int>
asl::frac<Int> ceil_fraction(const asl::frac<Int>& f) {
    return asl::frac<Int>(ceil(f)) - f;
}


template <class Int>
Int trunc(const asl::frac<Int>& f) {
    if (f.q == 1) return f.p;
    if (f.p >= 0)
        return f.p/f.q;
    else
        return f.p/f.q;
}
template <class Int>
asl::frac<Int> trunc_fraction(const asl::frac<Int>& f) {
    return f - trunc(f);
}


template <class Int>
Int round(const asl::frac<Int>& f) {
    if (f.p >= 0)
        return (2*f.p+f.q)/(2*f.q);
    else {
        //	return (2*f.p-f.q)/(2*f.q);
        return floor(f + asl::frac<int>(1, 2));
    }
} 

template <class Int>
asl::frac<Int> round_fraction(const asl::frac<Int>& f) {
    return f - round(f);
}

namespace asl {

template <class Int>
double as_float(const frac<Int>& f) {
    return (float)f.p / (float)f.q;
}
template <class Int>
double as_double(const frac<Int>& f) {
    return (double)f.p / (double)f.q;
}



typedef frac<int> ifrac;
typedef frac<int> ibase_frac;

} // namespace asl

#endif
