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
//    $RCSfile: piterator.h,v $
//
//   $Revision: 1.7 $
//
// Description: pixel container algorithms
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
// originally written by Pavel - Tue Jun 17 19:18:17 CEST 1997
// major refactoring during spare time 2003/2004
//
#ifndef _ASL_RASTER_PITERATOR_H_INCLUDED_
#define _ASL_RASTER_PITERATOR_H_INCLUDED_

/////////////////////////////////////////////////////////////////////////////////
//
// piterator enthaelt folgende algorithmische Iterator-Adapter
//
// step_iterator<RandomAccessIterator, T, ...>(RandomAccessIterator x,Distance step)
// (iteriert mit <step> auf <x>)
//
// rect_iterator<class hiterator, class viterator,class T, class Reference = T>()
//  (ist ein Iterator - template , der mittels eines horizontalen
//   und eines vertikalen Iterators einen rechteckigen bereich durchlauft.
//   Durch instanzieren mit geeigneten iteratoren kann das Rechteck in beliebigen
//   Richtungen und Schrittweiten durchlaufen werden.)
//
// fraction_iterator<RandomAccessIterator, T,... >(RandomAccessIterator x, Distance Virtual_steps, Distance Real_steps)
// (iteriert in <Virtual_steps> Schritten <Real_steps> auf <x> )
//
// bresenham_iterator<RandomAccessIterator, T,... >(RandomAccessIterator it, Distance Stride, Distance H, Distance V)
// (Iteriert auf einer Linie)
//
/////////////////////////////////////////////////////////////////////////////////

#include "asl_raster_settings.h"

#include <asl/base/settings.h>
#include <asl/math/numeric_functions.h>
#include <assert.h>
#include <iterator>
#include <iostream>
#include "fraction.h"

/////////////////////////////////////////////////////////////////////////////////

#undef DB
#define DB(x) x
#define DB2(x) // x

template <class T>
inline T * value_type(const T *) {
  return (T *)(0);
}

template <class T>
inline asl::AC_OFFSET_TYPE * distance_type(const T *) {
  return 0;
}

template <class Iterator, class T>
T* pointer(Iterator& b, T*) {
//    std::cerr << "[pointer(" << (void*)&b << ") returns " << (void*)&(*b) << "]" << std::endl;
    return &(*b);
}

namespace asl {

template <class Iterator, class T>
inline T* iseek(T*a, Iterator b) {
//    std::cerr << "[iseek(" << (void*)a << "," << (void*)&(*b) << ")]" <<endl;
    return &(*b);
}

/////////////////////////////////////////////////////////////////////////////////

template <class RandomAccessIterator, class T, class Reference = T&,
          class Distance = asl::AC_OFFSET_TYPE>
class step_iterator {
    typedef step_iterator<RandomAccessIterator, T, Reference, Distance> self;

//todo:    friend self operator+ <>(const self& a, T* b);
//todo:    friend T* operator+(T* a, const self& b);
public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef Distance                   difference_type;
    typedef T*                         pointer;
    typedef Reference                  reference;

    RandomAccessIterator current;
    Distance step;
public:
    step_iterator() {}
    step_iterator(const self& x) {current = x.current; step = x.step;}
    step_iterator(RandomAccessIterator x, Distance Step) : current(x) , step(Step) {
        //  std::cerr << "step_iterator(current = " << (void*)&(*x) << ", step = " << step << ")" << std::endl;
    }
    RandomAccessIterator base() { return current; }
    Reference operator*() const { return *current; }
    self& operator++() {
        current+=step;
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        current+=step;
        return tmp;
    }
    self& operator--() {
        current-=step;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        current-=step;
        return tmp;
    }
    self operator+(Distance n) const {
        return self(current + n * step, step);
    }
    self& operator+=(Distance n) {
        current += n * step;
        return *this;
    }
    self operator-(Distance n) const {
        return self(current - n * step, step);
    }
    self& operator-=(Distance n) {
        current -= n * step;
        return *this;
    }
    Reference operator[](Distance n) { return *(*this + n * step); }
    bool operator==(const self& y) const {
        return current == y.current;
    }
    bool operator<(const self& y) const {
        return current < y.current;
    }
    bool operator>(const self& y) const {
        return current > y.current;
    }

    bool operator!=(const self& y) const {
        return !(*this == y);
    }
    bool operator>=(const self& y) const {
        return !(y < *this);
    }
    bool operator<=(const self& y) const {
        return !(y > *this);
    }

    Distance operator-(const self & y) const {
        return (current - y.current) / step; //TODO: check if like in original
    }

};
// iseek:

template <class RandomAccessIterator, class T, class Reference, class Distance, class DestIterator>
step_iterator<RandomAccessIterator, T, Reference, Distance>
iseek(const step_iterator<RandomAccessIterator, T, Reference, Distance>& a, const DestIterator& b) {
    return step_iterator<RandomAccessIterator, T, Reference, Distance>(iseek(a.current, b), a.step);
}

} // namespace asl
//
template <class RandomAccessIterator, class T, class Reference, class Distance>
inline std::random_access_iterator_tag
iterator_category(const asl::step_iterator<RandomAccessIterator, T, Reference, Distance>&) {
  return std::random_access_iterator_tag();
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline T* value_type(const asl::step_iterator<RandomAccessIterator, T, Reference, Distance>&) {
  return (T*) 0;
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline Distance* distance_type(const asl::step_iterator<RandomAccessIterator, T, Reference, Distance>&) {
  return (Distance*) 0;
}

namespace asl {

/*==============================================================================*/
/*
   rect_iterator ist ein Iterator - template , der mittels eines horizontalen
   und eines vertikalen Iterators einen rechteckigen bereich durchlauft.
   Durch instanzieren mit geeigneten iteratoren kann das Rechteck in beliebigen
   Richtungen und Schrittweiten durchlaufen werden.
 */
/*==============================================================================*/

template <class hiterator, class viterator,class T, class Reference = T &, class Distance = asl::AC_OFFSET_TYPE>
class rect_iterator {
    typedef rect_iterator<hiterator, viterator, T, Reference, Distance> self;
    typedef T* ptr_T;
public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef Distance                   difference_type;
    typedef T*                         pointer;
    typedef Reference                  reference;

    hiterator current;
    viterator begin_of_line;
    viterator end_of_line;

public:
    struct hiter_t {
        const hiterator begin;
        hiter_t(const hiterator& b) : begin(b) {};
        hiterator operator()(const viterator& x) {
            return iseek(begin, x);
        }
        hiterator operator()(const viterator&  x) const {
            return hiterator(iseek(begin, x));
        }
    } hiter;
    struct viter_t {
        const viterator begin;
        viter_t(const viterator& b) : begin(b) {};
        viterator operator()(const hiterator& x) {
            return iseek(begin, x);
        }
        viterator operator()(const hiterator& x) const {
            return viterator(iseek(begin,x));
        }
    } viter;
public:
    rect_iterator() {}
    rect_iterator(const rect_iterator& x)
        :   current(x.current),
            begin_of_line(x.begin_of_line),
            end_of_line(x.end_of_line),
            hiter(x.hiter),
            viter(x.viter)
    { }

    rect_iterator(hiterator hbegin, viterator vbegin, Distance h_size)
        : current(hbegin), hiter(hbegin), viter(vbegin)
        {
            //      assert(pointer(hbegin, ptr_T()) == pointer(vbegin, ptr_T()));
            // std::cerr <<"rect_iterator( hbegin = " << (void*)&(*hbegin) << ", vbegin = " << (void*)&(*vbegin) << ", h_size = " << h_size <<  std::endl;

            begin_of_line = vbegin;
            end_of_line = viter(hbegin + h_size);

           // debug();
        }

    /**
    This method constructs an iterator and initializes it with an arbitrary value;
    it is typically use to implement the raster's end(..) method
        (hbegin, vbegin) designate origin of rectangle
        (hsize) designates width of rectangle
        (cur) designates the initializiation value and must be within the rectangular bottomless
              area given by (hbegin...hbegin + hsize, vbegin)

    Note that no heigth is needed as this is just an iterator; the heigth is determined
    by another iterator typically returned by an raster's end(..) method.
    */
    rect_iterator(hiterator hbegin, viterator vbegin, Distance h_size, hiterator cur)
        : current(cur), hiter(hbegin) , viter(vbegin)  {
          /*        std::cerr <<"rect_iterator( hbegin = " << (void*)&(*hbegin) << ", vbegin = " << (void*)&(*vbegin) << ", h_size = " << h_size << ", cur = " << (void*)&(*cur) <<")" << std::endl;
            */
            assert(&(*hbegin) == &(*vbegin));

            begin_of_line = vbegin;

            Distance vd = viter(cur) - vbegin;
            if (vd) {
                //  std::cerr << "!!! rect_iterator: vd = " << vd << std::endl;
                begin_of_line+=vd;
            }
            end_of_line = viter(hiter(begin_of_line) + h_size);

           //  debug();
        }
public:
    Distance hdist() const {
        return pointer(++(hiter(begin_of_line)), ptr_T()) - pointer(hiter(begin_of_line), ptr_T());}
    Distance vdist() const {
        return pointer(++viter(viter.begin), ptr_T()) - pointer(viter(viter.begin), ptr_T());}
    Distance hsize() const {
        hiterator eoln = hiter(end_of_line);
        hiterator bgnln = hiter(begin_of_line);
                //cerr << "eoln =" << (void*)&(*eoln) << std::endl;
                //cerr << "bgnln =" << (void*)&(*bgnln) << std::endl;
        Distance d = eoln - bgnln;
                //cerr << "hsize() returning d =" << d << std::endl;
        return d;
    }

    hiterator base() { return current; }
    Reference operator*() const {
                // std::cerr << "mat_it deref @" << (void*)&(*current) << ", val = " << *current << std::endl;
        return *current;
    }

#if 0
    viterator vbegin() const { return begin_of_line;}
    viterator vend() const { return end_of_line;}
    hiterator hbegin() const { return hiter(begin_of_line);}
    hiterator hend() const { return hiter(end_of_line);}
#endif

public:
    void debug() const {
        std::cerr << "begin_of_line = " << (void*)&(*begin_of_line) << " ,val = " << *begin_of_line << std::endl;
        std::cerr << "end_of_line = " << (void*)&(*end_of_line) << " ,val = " << *end_of_line << std::endl;
        std::cerr << "current = " << (void*)&(*current) << " ,val = " << *current << std::endl;
    }

    void increment()
    {
        //      std::cerr << "before incr:" << std::endl;
        //      debug();
        ++current;
        //      if ( pointer(current, ptr_T()) == pointer(end_of_line, ptr_T()))
        if ( &(*current) == &(*end_of_line))
        {
            //          std::cerr << "EOL encountered..." << std::endl;
            ++begin_of_line;
            ++end_of_line;
            current = iseek(current, begin_of_line);
        }
        //      std::cerr << "after incr:" << std::endl;
        //      debug();
    }
    void vincrement()
    {
        //      std::cerr << "before vincr:" << std::endl;
        //      debug();
        viterator vcurrent = viter(current);
        ++vcurrent;
        current = hiter(vcurrent);
        ++begin_of_line;
        ++end_of_line;

        //  std::cerr << "after vincr:" << std::endl;
        //  debug();
    }
    void vadd(Distance n)
    {
        //      std::cerr << "before vadd " << n << std::endl;
        //      debug();
        viterator vcurrent = viter(current);
        vcurrent+=n;
        current = hiter(vcurrent);
        begin_of_line+=n;
        end_of_line+=n;

        //      std::cerr << "after vadd " << n << std::endl;
        //      debug();
    }

    void decrement()
    {
        if (pointer(current) != pointer(begin_of_line, ptr_T())) {
            --current;
        }
        else {
            --begin_of_line;
            --end_of_line;
            current = iseek(current, end_of_line);
            --current;
        }
    }

    Distance x() const {
        //std::cerr << "current:" << (void*)&(*current) << std::endl;
        //std::cerr << "bgnofln:" << (void*)&(*begin_of_line) << std::endl;
        //std::cerr << "hiter(bngofln):" << (void*)&(*hiter(begin_of_line)) << std::endl;
        return current - hiter(begin_of_line);
    }
    Distance y() const {return begin_of_line - viter(hiter.begin);}

    void add(Distance n) {

        Distance hn = x()+n;
        if (( hn >= 0) && (hn < hsize()))
        {
            current += n;
        }
        else {
            if (hn >= 0) {
                Distance vdiff = hn / hsize();
                Distance hpos = hn % hsize();
                begin_of_line += vdiff;
                end_of_line +=vdiff;
                current = hiter(begin_of_line) + hpos;
            }
            else {
                Distance vdiff = hn / hsize() - 1;
                Distance hpos = -hn % hsize();
                begin_of_line += vdiff;
                end_of_line +=vdiff;
                current = hiter(end_of_line) - hpos;
            }
        }
    }
public:
    self& operator++() {
        increment();
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        increment();
        return tmp;
    }
    self& operator--() {
        decrement();
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }
    self& operator+=(Distance n) {
        add(n);
        return *this;
    }
    self& operator^=(Distance n) {
        vadd(n);
        return *this;
    }
    self& operator-=(Distance n) { return *this += -n; }
    self operator+(Distance n) const {
        self tmp = *this;
        return tmp += n;
    }
    self operator^(Distance n) const {
        self tmp = *this;
        return tmp ^= n;
    }
    self operator-(Distance n) const {
        self tmp = *this;
        return tmp -= n;
    }

    Reference operator[](Distance n) { return *(*this + n); }
    bool operator==(const self& y) const {
        return current == y.current || begin_of_line == end_of_line;
    }
    bool operator<(const self& y) const {
        return current < y.current && begin_of_line != end_of_line;
    }
     bool operator>(const self& y) const {
        return current > y.current && begin_of_line != end_of_line;
    }

    bool operator!=(const self& y) const {
        return !(*this == y);
    }
    bool operator>=(const self& y) const {
        return !(y < *this);
    }
    bool operator<=(const self& y) const {
        return !(y > *this);
    }

    Distance operator-( const self & b) const {
        DB2(AC_TRACE <<  "(member) operator-(rect_iterator) :" <<
                "begin_of_line - b.begin_of_line = " << begin_of_line - b.begin_of_line << std::endl <<
                "hsize() = " << hsize() << ", x() = " << x() << ", b.x() = " << b.x() << std::endl;)
            return (begin_of_line - b.begin_of_line) * hsize() + x() - b.x();
    }
};

template <class hIterator, class vIterator,class T, class Reference, class Distance, class DestIterator>
inline rect_iterator<hIterator, vIterator, T, Reference, Distance>
iseek(const rect_iterator<hIterator, vIterator, T, Reference, Distance>& a, const DestIterator& b) {
    return rect_iterator<hIterator, vIterator, T, Reference, Distance>(
          a.hiter.begin, a.viter.begin, a.hsize(), iseek(a.current, b));
}

} // namespace asl

template <class hIterator, class vIterator,class T, class Reference, class Distance >
inline std::random_access_iterator_tag
iterator_category(const asl::rect_iterator<hIterator, vIterator, T, Reference, Distance>&) {
  return std::random_access_iterator_tag();
}

template <class hIterator, class vIterator,class T, class Reference, class Distance >
inline T* value_type(const asl::rect_iterator<hIterator, vIterator, T, Reference, Distance>&) {
  return (T*) 0;
}

template <class hIterator, class vIterator,class T, class Reference, class Distance >
inline Distance* distance_type(const asl::rect_iterator<hIterator, vIterator, T, Reference, Distance>&) {
  return (Distance*) 0;
}

namespace asl {

/*==============================================================================*/
/**
    fraction_iterator ist ein Iterator, der einen <virtualsteps> grossen bereich
    in <real_steps> schritten durchlauft, d.h. einige Werte evtl. mehrfach liefert
    wobei <real_steps> >= <virtualsteps> sein muss.
 */
/*==============================================================================*/

template <class RandomAccessIterator, class T, class Reference = T &, class Distance = asl::AC_OFFSET_TYPE>
class fraction_iterator {
    typedef fraction_iterator<RandomAccessIterator, T, Reference, Distance> self;
    //typedef frac<Distance> fraction;
    typedef double fraction;

#ifdef STD_FRIENDS
#warning "Have STD_FRIENDS"
    friend bool operator==(const self& x, const self& y);
    friend bool operator<(const self& x, const self& y);
    friend Distance operator-(const self& x, const self& y);
    friend self operator+(Distance n, const self& x);
#else
#ifdef P_DECLARE_FRIEND_OPS
#warning "Have P_DECLARE_FRIEND_OPS"
    friend bool operator== <>(const self& x, const self& y);
    friend bool operator< <>(const self& x, const self& y);
    friend Distance operator- <>(const self& x, const self& y);
    friend self operator+ <>(Distance n, const self& x);
#endif
#endif
    public:
        typedef std::random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef Distance                   difference_type;
    typedef T*                         pointer;
    typedef Reference                  reference;

    RandomAccessIterator current;
    fraction stepsize;
    fraction current_fraction;
public:
    fraction_iterator() {}
    //    fraction_iterator(RandomAccessIterator x) : begin(x) , virtual_steps(1), real_steps(1) {}

    // Virtual_steps should be >=RealSteps:

    fraction_iterator(RandomAccessIterator x, Distance Real_steps, Distance Virtual_steps)
        : current(x),
          stepsize((fraction)Real_steps/(fraction)Virtual_steps),
          current_fraction(stepsize - floor(stepsize))
    {

        //  std::cerr << " current = " << (void*)&(*current) << std::endl;
        //  std::cerr << " stepsize = " << stepsize << std::endl;
        //  std::cerr << " current_fraction = " << current_fraction << std::endl;
    }
    fraction_iterator(RandomAccessIterator x, fraction Stepsize, fraction Current_fraction)
        : current(x) ,stepsize(Stepsize), current_fraction(Current_fraction) {

            //  std::cerr << " current = " << (void*)&(*current) << std::endl;
            //  std::cerr << " stepsize = " << stepsize << std::endl;
            //  std::cerr << " current_fraction = " << current_fraction << std::endl;
        }
    fraction_iterator(RandomAccessIterator x, fraction Stepsize)
        //  : current(x) ,stepsize(Stepsize), current_fraction(1, 2) {
        : current(x),
          stepsize(Stepsize),
          current_fraction(stepsize - floor(stepsize))
        {
            //  std::cerr << " current = " << (void*)&(*current) << std::endl;
            //  std::cerr << " stepsize = " << stepsize << std::endl;
            //  std::cerr << " current_fraction = " << current_fraction << std::endl;
        }
    RandomAccessIterator base() { return current; }
    const RandomAccessIterator& base() const { return current; }
    fraction base_fraction() const { return current_fraction; }
    Reference operator*() const { return *current; }

    void increment() {
//#define DEBUG_FRACTION_ITERATOR
#ifdef DEBUG_FRACTION_ITERATOR
        std::cerr << " increment() " << std::endl;
        std::cerr << " current_fraction = " << current_fraction << std::endl;
        std::cerr << " stepsize = " << stepsize << std::endl;
#endif
        current_fraction+=stepsize;
#ifdef DEBUG_FRACTION_ITERATOR
        std::cerr << " current_fraction += stepsize= " << current_fraction << std::endl;
#endif
        Distance n = (Distance)floor(current_fraction);
#ifdef DEBUG_FRACTION_ITERATOR
        std::cerr << " n = " << n << std::endl;
#endif
        current_fraction -= fraction(n);
#ifdef DEBUG_FRACTION_ITERATOR
        std::cerr << " current_fraction -= n= " << current_fraction << std::endl;
#endif
        current+=n;
#ifdef DEBUG_FRACTION_ITERATOR
        std::cerr << " current += n= " << (void*)&(*current) << std::endl;
#endif
    }
    void decrement() {
        current_fraction-=stepsize;
        Distance n = floor(current_fraction);
        current_fraction += fraction(n);
        current-=n;
    }
    void add(Distance f) {
        //  std::cerr << " add(" << f << ") " << std::endl;
        //  std::cerr << " current_fraction = " << current_fraction << std::endl;
        //  std::cerr << " stepsize*f = " << stepsize*f << std::endl;
        current_fraction+=stepsize*f;
        //  std::cerr << " current_fraction += stepsize= " << current_fraction << std::endl;
        Distance n = (Distance)floor(current_fraction);
        //  std::cerr << " n = " << n << std::endl;
        current_fraction -= fraction(n);
        //  std::cerr << " current_fraction -= n= " << current_fraction << std::endl;
        current+=n;
        //  std::cerr << " current += n= " << (void*)&(*current) << std::endl;
    }
    void subtract(Distance f) {
        current_fraction-=stepsize*f;
        Distance n = Distance(floor(current_fraction));
        current_fraction += fraction(n);
        current-=n;
    }

    self& operator++() {
        increment();
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        increment();
        return tmp;
    }
    self& operator--() {
        decrement();
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }
    self operator+(Distance f) const {
        self result = *this;
        result.add(f);
        return result;
    }
    self& operator+=(Distance f) {
        add(f);
        return *this;
    }
    self operator-(Distance f) const {
        self result = *this;
        result.subtract(f);
        return result;
    }
    self& operator-=(Distance f) {
        subtract(f);
        return *this;
    }
    Reference operator[](Distance f) { return *(*this + f); }

    bool operator==(const self & y) const {
#ifdef DEBUG_FRACTION_ITERATOR
        std::cerr << " operator == :   current = " << (void*)&(*current) << std::endl;
        std::cerr << " operator == : y.current = " << (void*)&(*y.current) << std::endl;
        std::cerr << " operator == :   current_fraction = " << current_fraction << std::endl;
        std::cerr << " operator == : y.current_fraction = " << y.current_fraction << std::endl;
        std::cerr << " operator == : current equal = " << (current == y.current) << std::endl;
        std::cerr << " operator == : current_fraction almostEqual = " << asl::almostEqual(current_fraction,y.current_fraction) << std::endl;
#endif
        return (current == y.current) && asl::almostEqual(current_fraction,y.current_fraction);
    }
    bool operator!=(const self& y) const {
        return !(*this == y);
    }
    bool operator<(const self& y) const {
        return current < y.current || ((current == y.current) && (current_fraction < y.current_fraction));
    }
    bool operator>(const self& y) const {
        return current > y.current || ((current == y.current) && (current_fraction > y.current_fraction));
    }
    bool operator>=(const self& y) const {
        return !(y < *this);
    }
    bool operator<=(const self& y) const {
        return !(y > *this);
    }

    Distance operator-(const self & y) const {
        // [DS, TS] the volatile keyword is required to prevent gcc from optimizing this stuff.
        // If anybody has a better solution or understanding what is going on here, please tell me.
        volatile fraction f(fraction(current - y.current));
        f += (current_fraction - y.current_fraction);
        f/=stepsize;
        return (Distance)floor(f);
    }
};


template <class RandomAccessIterator, class T, class Reference, class Distance, class DestIterator>
inline fraction_iterator<RandomAccessIterator, T, Reference, Distance>
iseek(const fraction_iterator<RandomAccessIterator, T, Reference, Distance>& a, const DestIterator& b) {
    RandomAccessIterator ni = iseek(a.base(), b);
    return fraction_iterator<RandomAccessIterator, T, Reference, Distance>(ni, a.stepsize);
}

} // namespace asl

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline std::random_access_iterator_tag
iterator_category(const asl::fraction_iterator<RandomAccessIterator, T,
        Reference, Distance>&) {
    return std::random_access_iterator_tag();
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline T* value_type(const asl::fraction_iterator<RandomAccessIterator, T,
        Reference, Distance>&) {
    return (T*) 0;
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline Distance* distance_type(const asl::fraction_iterator<RandomAccessIterator, T,
        Reference, Distance>&) {
    return (Distance*) 0;
}

namespace asl {
/*=================================================================================

    bresenham_iterator

**================================================================================*/

template <
    class RandomAccessIterator,
    class T,
    class Reference = T&,
    class Distance = asl::AC_OFFSET_TYPE
>
class bresenham_iterator {
    typedef bresenham_iterator<RandomAccessIterator, T, Reference, Distance> self;
protected:
    RandomAccessIterator current;
    Distance error;
    Distance dh;
    Distance dv;
    Distance hstride;
    Distance vstride;
#define nSHORT_SLOW
#ifndef SHORT_SLOW
    bool    swapped;
#endif

public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef Distance                   difference_type;
    typedef T*                         pointer;
    typedef Reference                  reference;
    bresenham_iterator() {}

    bresenham_iterator(RandomAccessIterator it)
        : current(it)  {
            //  std::cerr << "bresenham_iterator = " << (void*)(&(*it)) << std::endl;
    }

    bresenham_iterator(RandomAccessIterator it, Distance Stride, Distance H, Distance V)
        : current(it) {
#if 0
            std::cerr << "bresenham_iterator = " << (void*)(&(*it)) << std::endl;
            std::cerr << "Stride = " << Stride << std::endl;
            std::cerr << "dh = " << H << std::endl;
            std::cerr << "dv = " << V << std::endl;
#endif

            hstride = asl::sign(H);
            vstride = asl::sign(V) * Stride;

            dh = abs(H);//if (dh>0) --dh;
            dv = abs(V);//if (dv>0) --dv;

            if (dv > dh) {
                std::swap(dh, dv);
#ifdef SHORT_SLOW
                std::swap(hstride, vstride);
#else
                swapped = 1;
#endif
            }
#ifndef SHORT_SLOW
            else {
                swapped = 0;
            }
#endif
            error = 2 * (dv > 0 ? dv -1 : dv) - (dh > 0 ? dh - 1 : dh);

#if 0
            std::cerr << "dh = " << dh << std::endl;
            std::cerr << "dv = " << dv << std::endl;
            std::cerr << "hstride = " << hstride << std::endl;
            std::cerr << "vstride = " << vstride << std::endl;
            std::cerr << "swapped = " << swapped << std::endl;
            std::cerr << "error = " << error << std::endl;
            std::cerr << "current = " << (void*)(&(*current)) << std::endl;
#endif
        }

    RandomAccessIterator base() { return current; }
    Reference operator*() const { return *current; }
#ifdef SHORT_SLOW
    void increment() {
        while (error >= 0) {
            current += vstride;
            error = error - 2 * dh;
        }
        current += hstride;
        error = error + 2 * dv;
    }
#else
    void increment() {
        while (error >= 0) {
            if (swapped) {
                //      std::cerr << ">=0 += h " << hstride << std::endl;
                current += hstride;
            } else {
                //      std::cerr << ">= += v " << vstride << std::endl;
                current += vstride;
            }
            error = error - 2 * dh;
        }
        //  std::cerr << "After loop Increment error = " << error << std::endl;
        //  if (error<0) {
        if (swapped) {
            //      std::cerr << "<0 += v " << vstride << std::endl;
            current += vstride;
        } else {
            //      std::cerr << "<0 += h " << hstride << std::endl;
            current += hstride;
        }
        error = error + 2 * dv;
        //  }
#if 0
        std::cerr << "After Increment = " << std::endl;
        std::cerr << "error = " << error << std::endl;
        std::cerr << "current = " << (void*)(&(*current)) << std::endl;
#endif
    }
#endif
    void decrement() {
        if (error >= 0) {
            if (swapped) {
                current -= hstride;
            } else {
                current -= vstride;
            }
            error = error + 2 * dh;
        }
        if (error<0) {
            if (swapped) {
                current -= hstride;
            } else {
                current -= vstride;
            }
            error = error - 2 * dv;
        }
#if 0
        std::cerr << "After Decrement = " << std::endl;
        std::cerr << "error = " << error << std::endl;
        std::cerr << "current = " << (void*)(&(*current)) << std::endl;
#endif
    }
    void add(Distance n) {
        std::cerr << "add " << n << std::endl;
        if (n>0)
            while (n--) increment();
        else
            while (n++) decrement();
        std::cerr << "OK add " << n << std::endl;

        //      current += n * real_steps / virtual_steps + error;
        //  error = error + n * virtual_steps / real_steps;
    }
    void subtract(Distance n) {
        if (n>0)
            while (n--) decrement();
        else
            while (n++) increment();
        //      current -= n * real_steps / virtual_steps + error;
        //  error = error - n * virtual_steps / real_steps
        std::cerr << "OK subtract " << n << std::endl;
    }

    self& operator++() {
        increment();
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        increment();
        return tmp;
    }
    self& operator--() {
        decrement();
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }
    self operator+(Distance n) const {
        self result = *this;
        result.add(n);
        return result;
    }
    self& operator+=(Distance n) {
        add(n);
        return *this;
    }
    self operator-(Distance n) const {
        self result = *this;
        result.subtract(n);
        return result;
    }
    self& operator-=(Distance n) {
        subtract(n);
        return *this;
    }
    Reference operator[](Distance n) {
        return *(*this + n);
    }
    bool operator==(const self & y) const {
        return (current == y.current);
    }
    bool operator<(const self & y) const {
        return current < y.current;
    }
    bool operator>(const self & y) const {
        return current > y.current;
    }
    bool operator!=(const self & y) const {
        return !(*this == y);
    }
    bool operator>=(const self & y) const {
        return !(y < *this);
    }
    bool operator<=(const self & y) const {
        return !(y > *this);
    }

};

} // namespace asl

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline std::random_access_iterator_tag
iterator_category(const asl::bresenham_iterator<RandomAccessIterator, T,
                                         Reference, Distance>&) {
  return std::random_access_iterator_tag();
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline T * value_type(const asl::bresenham_iterator<RandomAccessIterator, T,
                                            Reference, Distance>&) {
  return (T*) 0;
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline Distance * distance_type(const asl::bresenham_iterator<RandomAccessIterator, T,
                                                      Reference, Distance>&) {
  return (Distance*) 0;
}

/////////////////////////////////////////////////////////////////////////////////

#undef DB2
#undef DB


#endif
