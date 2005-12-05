/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2004, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: contain.h,v $
//
//   $Revision: 1.3 $
//
// Description: light weigt pixel container base classes
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
// originally written by Pavel - Tue Jun 17 19:18:17 CEST 1997
// major refactoring during spare time 2003/2004
// Typen: Sequence, Pair, Triple, Quad
//
#ifndef _ASL_RASTER_CONTAIN_H_INCLUDED_
#define _ASL_RASTER_CONTAIN_H_INCLUDED_

#include <iostream>

namespace asl {

// SequenceBase is a factoring of same size pixel tupel class stuff like RGB_t etc.
// we can not derive RGB_t from it also because we would loose packing of pixels
template <int SIZE, class T>
struct SequenceBase {
    typedef SequenceBase<SIZE, T> base_type;
    typedef T value_type;
    typedef T sequence_type[SIZE];
    typedef T * iterator;
    typedef const T * const_iterator;

    T& operator[](int i) {return reinterpret_cast<T*>(this)[i];}
    const T& operator[](int i) const {return reinterpret_cast<const T*>(this)[i];};
    static int size() { return SIZE;}
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


template <int SIZE, class T>
struct Sequence {
    typedef Sequence<SIZE, T> my_type;
    typedef T value_type;
    typedef T sequence_type[SIZE];
    typedef T * iterator;
    typedef const T * const_iterator;

    Sequence() {}
    Sequence(const Sequence& s) {
        int i;
        for (i=0;i<size();i++) {
            value[i] = s[i]; 
        }
    }
    Sequence(const T& v) {
        int i;
        for (i=0;i<size();i++) {
            value[i] = v; 
        }
    }
    T& operator[](int i) {return value[i];}
    const T& operator[](int i) const {return value[i];};
    static int size() { return SIZE;}
    T * begin() {
        return &value[0];
    }
    T * end() {
        return &value[SIZE];
    }
    const T * begin() const {
        return &value[0];
    }
    const T * end() const {
        return &value[SIZE];
    }
    my_type operator*(const my_type& y) {
        my_type result;
        int i;
        for (i=0;i<size();i++) { result[i] = this->x[i] * this->y[i]; }
        return result;
    }
    my_type operator+(const my_type& y) {
        my_type result;
        int i;
        for (i=0;i<size();i++) { result[i] = this->x[i] + this->y[i]; }
        return result;
    }
    my_type operator-(const my_type& y) {
        my_type result;
        int i;
        for (i=0;i<size();i++) { result[i] = this->x[i] - this->y[i]; }
        return result;
    }
    my_type operator/(const my_type& y) {
        my_type result;
        int i;
        for (i=0;i<size();i++) { result[i] = this->x[i] / this->y[i]; }
        return result;
    }
    my_type operator^(const my_type& y) {
        my_type result;
        int i;
        //for (i=0;i<size();i++) { result[i] = xor(x[i], y[i]); }
        for (i=0;i<size();i++) { result[i] = this->x[i] ^ this->y[i]; }
        return result;
    }
    my_type operator&(const my_type& y) {
        my_type result;
        int i;
        //for (i=0;i<size();i++) { result[i] = and(x[i], y[i]); }
        for (i=0;i<size();i++) { result[i] = this->x[i] & this->y[i]; }
        return result;
    }
    my_type operator|(const my_type& y) {
        my_type result;
        int i;
        for (i=0;i<size();i++) { result[i] = this->x[i] | this->y[i]; }
        return result;
    }
    protected:
    sequence_type value;
};

// A base class type for two different value types
// T1 and T2 must be different types
// DERIVED is the derived class to allow operators return 
// a derived class
template <class T1, class T2, class DERIVED>
struct Pair {
    typedef Pair<T1, T2, DERIVED> my_type;
    Pair() { }
    Pair(const Pair & x) { first(x.first()); second(x.second());}
    Pair(const T1 & a, const T2 & b) { first(a); second(b);}

    int size() { return 2;}

    void first(const T1 & x) {_first = x;}
    void second(const T2 & x) {_second = x;}

    T1 & first() {return _first;}
    const T1& first() const {return _first;}

    T2 & second() {return _second;}
    const T2 & second() const {return _second;}

    T1 & get(const T1 &) {return first();}
    const T1 & get(const T1 &) const {return first();}

    T2 & get(const T2 &) {return second();}
    const T2 & get(const T2 &) const {return second();}

    void set(const T1 & x) {first(x);}
    void set(const T2 & x) {second(x);}

    my_type & operator+=(const my_type & x) {
        first+=x.first(); second+=x.second(); return *this;
    }

    DERIVED operator*(const DERIVED & y) const {
        return DERIVED(first() * y.first(), second() * y.second());
    }
    DERIVED operator+(const my_type & y) const {
        return DERIVED(first() + y.first(), second() + y.second());
    }
    DERIVED operator-(const DERIVED & y) const {
        return DERIVED(first() - y.first(), second() - y.second());
    }
    DERIVED operator/(const DERIVED & y) const {
        return DERIVED(first() / y.first(), second() / y.second());
    }
    DERIVED operator&(const DERIVED & y) const {
        return DERIVED(first() & y.first(), second()& y.second());
    }
    DERIVED operator|(const DERIVED & y) const {
        return DERIVED(first() | y.first(), second()| y.second());
    }
    DERIVED operator^(const DERIVED & y) const {
        return DERIVED(first() ^ y.first(), second()^ y.second());
    }
    
    template <class NUMBER>
    DERIVED operator*(NUMBER y) const {
        return DERIVED(first() * y, second() * y);
    }
    template <class NUMBER>
    DERIVED operator+(NUMBER y) const {
        return DERIVED(first() + y, second() + y);
    }
    template <class NUMBER>
    DERIVED operator-(NUMBER y) const {
        return DERIVED(first() - y, second() - y);
    }
    template <class NUMBER>
    DERIVED operator/(NUMBER y) const {
        return DERIVED(first() / y, second() / y);
    }

    bool operator==(const DERIVED & y) const {
	    return (first() == y.first() && second() == y.second() );
    }
    bool operator!=(const DERIVED & y) const {
	    return (first() != y.first() || second() != y.second() );
    }
protected:
    T1 _first;
    T2 _second;
};

template <class T1, class T2, class DERIVED>
inline std::ostream& operator<<(std::ostream& o, const Pair<T1, T2, DERIVED>& s)
{
    o << s.first() <<" " << s.second();
    return o;
}

template <class T1, class T2, class DERIVED>
inline std::istream & operator>>(std::istream & i, const Pair<T1, T2, DERIVED>& s)
{
    if (i >> s.first()) {
        if (i>> s.second()) {
            return i;
        }
    }
    i.setstate(std::ios::failbit);
    return i;
}

template <class T1, class T2, class T3, class DERIVED>
struct Triple {
    typedef Triple<T1, T2, T3, DERIVED> my_type;
    Triple() { }
    Triple(const Triple & x) { first(x.first()); second(x.second()); third(x.third());}
    Triple(const T1 & a, const T2 & b, const T3 & c) { first(a); second(b); third(c);}

    int size() { return 4;}

    void first(const T1 & x) {_first = x;}
    void second(const T2 & x) {_second = x;}
    void third(const T3 & x) {_third = x;}

    T1 & first() {return _first;}
    const T1 & first() const {return _first;}

    T2 & second() {return _second;}
    const T2 & second() const {return _second;}

    T3 & third() {return _third;}
    const T3 & third() const {return _third;}

    T1 & get(const T1 &) {return first();}
    const T1 & get(const T1 &) const {return first();}

    T2 & get(const T2 &) {return second();}
    const T2 & get(const T2 &) const {return second();}

    T3 & get(const T3 &) {return third();}
    const T3 & get(const T3 &) const {return third();}

    void set(const T1 & x) {first(x);}
    void set(const T2 & x) {second(x);}
    void set(const T3 & x) {third(x);}
    
    my_type & operator+=(const my_type & x) {
        _first+=x.first(); _second+=x.second(); _third+=x.third();return *this;
    }
    my_type & operator-=(const my_type & x) {
        _first-=x.first(); _second-=x.second(); _third-=x.third();return *this;
    }
    my_type & operator*=(const my_type & x) {
        _first*=x.first(); _second*=x.second(); _third*=x.third();return *this;
    }
    my_type & operator/=(const my_type & x) {
        _first/=x.first(); _second/=x.second(); _third/=x.third();return *this;
    }

    DERIVED operator*(const DERIVED & y) const {
        return DERIVED(first() * y.first(), second()* y.second(), third() * y.third());
    }
    DERIVED operator+(const DERIVED & y) const {
        return DERIVED(first() + y.first(), second()+ y.second(), third() + y.third());
    }
    DERIVED operator-(const DERIVED & y) const {
        return DERIVED(first() - y.first(), second()- y.second(), third() - y.third());
    }
    DERIVED operator/(const DERIVED & y) const {
        return DERIVED(first() / y.first(), second()/ y.second(), third() / y.third());
    }
    DERIVED operator&(const DERIVED & y) const {
        return DERIVED(first() & y.first(), second()& y.second(), third() & y.third());
    }
    DERIVED operator|(const DERIVED & y) const {
        return DERIVED(first() | y.first(), second()| y.second(), third() | y.third());
    }
    DERIVED operator^(const DERIVED & y) const {
        return DERIVED(first() ^ y.first(), second()^ y.second(), third() ^ y.third());
    }
   
    template <class NUMBER>
    DERIVED operator*(NUMBER y) const {
        return DERIVED(first() * y, second() * y, third() * y);
    }
    template <class NUMBER>
    DERIVED operator+(NUMBER y) const {
        return DERIVED(first() + y, second() + y, third() + y);
    }
    template <class NUMBER>
    DERIVED operator-(NUMBER y) const {
        return DERIVED(first() - y, second() - y, third() - y);
    }
    template <class NUMBER>
    DERIVED operator/(NUMBER y) const {
        return DERIVED(first() / y, second() / y, third() / y);
    }

    bool operator==(const DERIVED & y) const {
        return (first() == y.first() && second() == y.second() && third() == y.third() );
    }

    bool operator!=(const DERIVED & y) const {
        return (first() != y.first() || second() != y.second() || third() != y.third());
    }
private:
    T1 _first;
    T2 _second;
    T3 _third;
};

template <class T1, class T2, class T3, class DERIVED>
inline std::ostream& operator<<(std::ostream& o, const Triple<T1, T2, T3, DERIVED> & s)
{
    o << s.first() << ' ' << s.second() << ' ' << s.third();
    return o;
}
template <class T1, class T2, class T3, class DERIVED>
inline std::istream & operator>>(std::istream & i, Triple<T1, T2, T3, DERIVED> & s)
{
    if (i >> s.first()) {
        if ( i>> s.second()) {
            if ( i >> s.third()) {
                return i;
            }
        }
    }
    i.setstate(std::ios::failbit);
    return i;
}

template <class T1, class T2, class T3, class T4, class DERIVED>
struct Quad  {
    typedef Quad<T1, T2, T3, T4, DERIVED> my_type;
    Quad() { }
    Quad(const my_type & x) {
        first(x.first()); second(x.second()); third(x.third());fourth(x.fourth());
    }    

    Quad(const T1 & x1, const T2 & x2, const T3 & x3, const T4 & x4) {
        first(x1); second(x2); third(x3);fourth(x4);
    }

    void first(const T1 & x) {_first = x;}
    void second(const T2 & x) {_second = x;}
    void third(const T3 & x) {_third = x;}
    void fourth(const T4 & x) {_fourth = x;}

    T1 & first() {return _first;}
    const T1& first() const {return _first;}

    T2 & second() {return _second;}
    const T2 & second() const {return _second;}

    T3 & third() {return _third;}
    const T3 & third() const {return _third;}

    T4 & fourth() {return _fourth;}
    const T4 & fourth() const {return _fourth;}

    T1 & get(const T1 &) {return first();}
    const T1 & get(const T1 &) const {return first();}

    T2 & get(const T2 &) {return second();}
    const T2 & get(const T2 &) const {return second();}

    T3 & get(const T3 &) {return third();}
    const T3 & get(const T3 &) const {return third();}

    T4 & get(const T4 &) {return fourth();}
    const T4 & get(const T4 &) const {return fourth();}

    void set(const T1 & x) {first(x);}
    void set(const T2 & x) {second(x);}
    void set(const T3 & x) {third(x);}
    void set(const T4 & x) {fourth(x);}

    my_type & operator+=(const my_type & x) {
        _first+=x.first(); _second+=x.second(); _third+=x.third();_fourth+=x.fourth();return *this;
    }
    my_type & operator-=(const my_type & x) {
        _first-=x.first(); _second-=x.second(); _third-=x.third();_fourth-=x.fourth();return *this;
    }
    my_type & operator*=(const my_type & x) {
        _first*=x.first(); _second*=x.second(); _third*=x.third();_fourth*=x.fourth();return *this;
    }
    my_type & operator/=(const my_type & x) {
        _first/=x.first(); _second/=x.second(); _third/=x.third();_fourth/=x.fourth();return *this;
    }

    DERIVED operator*(const DERIVED & y) const {
        return DERIVED(first() * y.first(), second()* y.second(), third() * y.third(), fourth()*y.fourth());
    }
    DERIVED operator+(const DERIVED & y) const {
        return DERIVED(first() + y.first(), second()+ y.second(), third() + y.third(), fourth()+y.fourth());
    }
    DERIVED operator-(const DERIVED & y) const {
        return DERIVED(first() - y.first(), second()- y.second(), third() - y.third(), fourth()-y.fourth());
    }
    DERIVED operator/(const DERIVED & y) const {
        return DERIVED(first() / y.first(), second()/ y.second(), third() / y.third(), fourth()/y.fourth());
    }
    DERIVED operator&(const DERIVED & y) const {
        return DERIVED(first() & y.first(), second()& y.second(), third() & y.third(), fourth()&y.fourth());
    }
    DERIVED operator|(const DERIVED & y) const {
        return DERIVED(first() | y.first(), second()| y.second(), third() | y.third(), fourth()|y.fourth());
    }
    DERIVED operator^(const DERIVED & y) const {
        return DERIVED(first() ^ y.first(), second()^ y.second(), third() ^ y.third(), fourth()^y.fourth());
    }
    
    template <class NUMBER>
    DERIVED operator*(NUMBER y) const {
        return DERIVED(first() * y, second() * y, third() * y, fourth() * y);
    }
    template <class NUMBER>
    DERIVED operator+(NUMBER y) const {
        return DERIVED(first() + y, second() + y, third() + y, fourth() + y);
    }
    template <class NUMBER>
    DERIVED operator-(NUMBER y) const {
        return DERIVED(first() - y, second() - y, third() - y, fourth() - y);
    }
    template <class NUMBER>
    DERIVED operator/(NUMBER y) const {
        return DERIVED(first() / y, second() / y, third() / y, fourth() / y);
    }


    bool operator==(const DERIVED & y) const {
        return (first() == y.first() && second() == y.second() && third() == y.third() && fourth() == y.fourth());
    }
    bool operator!=(const DERIVED & y) const {
        return (first() != y.first() || second() != y.second() || third() != y.third() || fourth() != y.fourth());
    }

private:
    T1 _first;
    T2 _second;
    T3 _third;
    T4 _fourth;
};

template <class T1, class T2, class T3, class T4, class DERIVED>
inline std::ostream & operator<<(std::ostream & o, const Quad<T1, T2, T3, T4, DERIVED> & s)
{
    o << s.first() << ' ' << s.second() << ' '<< s.third() << ' '<< s.fourth();
    return o;
}

template <class T1, class T2, class T3, class T4, class DERIVED>
inline std::istream & operator>>(std::istream & i, Quad<T1, T2, T3, T4, DERIVED> & s)
{
    if (i >> s.first()) {
        if (i>> s.second()) {
            if (i >> s.third()) {
                if (i >> s.fourth()) {
                    return i;
                }
            }
        }
    }
    i.setstate(std::ios::failbit);
    return i;
}

} // namespace asl

#endif
