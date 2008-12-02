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
//    $RCSfile: subraster.h,v $
//
//   $Revision: 1.5 $
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
#ifndef _ASL_RASTER_SUBRASTER_H_INCLUDED_
#define _ASL_RASTER_SUBRASTER_H_INCLUDED_

#include <iterator>
#include <algorithm>

#include "piterator.h"
#include "raster.h"

namespace asl {

template <class T, class D = AC_OFFSET_TYPE>
class subraster;
template <class T, class D = AC_OFFSET_TYPE>
class const_subraster;

// base class, do not use directly
template <class RASTER>
class subraster_base_iterator
    : public std::iterator<
            std::random_access_iterator_tag,
			typename RASTER::value_type,
			typename RASTER::difference_type,
			typename RASTER::pointer,
			typename RASTER::reference>
{
    public:
        typedef typename RASTER::value_type value_type;
        typedef typename RASTER::pointer base_ptr;
        typedef typename RASTER::reference reference;
        typedef typename RASTER::difference_type difference_type;

        RASTER * _map;
        base_ptr _cur;

        base_ptr _end_of_line;
        base_ptr _begin_of_line;

        base_ptr ptr() const {return _cur;}

        void calc_begin_and_end_of_line() 
        {
            if (_map->xsize() != _map->stride()) {
                _begin_of_line = _cur - x();
                _end_of_line = _begin_of_line + _map->xsize();
            }
            else {
                _begin_of_line = 0;
                _end_of_line = 0 ;
            }
        }

        void increment()
        {
            _cur++;
            if ( _cur == _end_of_line) {
                _begin_of_line += _map->stride();
                _end_of_line+=_map->stride();
                _cur = _begin_of_line;
            }
        }

        void decrement()
        {
            if (_cur != _begin_of_line) {
                _cur--;
            }
            else {
                _begin_of_line -= _map->stride();
                _end_of_line -= _map->stride();
                _cur = _end_of_line - 1;
            }
        }

        void add(difference_type n) {
            if ( (_map->stride() == _map->xsize())) {
                _cur+=n;
            }
            else {
                _cur = _map->get_ptr((x() + n) % _map->xsize(),
                        (y() + n) / _map->xsize());
            }
        }
        difference_type x() const { return  _map->x(_cur);}
        difference_type y() const { return  _map->y(_cur);}

        bool at_begin_of_line() {return _map->x(_cur) == 0; }
        bool at_end_of_line() {return _map->x(_cur) == _map->xsize() - 1; }
        bool is_inside() {
            difference_type x_ = x(); difference_type y_ = y();
            return (x_>0) && (y_>0) && (x_<_map->xsize()) && (y_<_map->ysize());
        }

        // TODO: Find a way to remove the const_cast again!
        operator void* const() {return static_cast<void*>(const_cast<value_type*>(_cur));}
};


/// This iterator is the standard iterator type for a subraster container
/// It uses the origin and the stride from the supplied RASTER class to properly advance
/// to the next line.
/// 
template <class RASTER>
class subraster_iterator : public subraster_base_iterator<RASTER>
{
    public:
        typedef subraster_iterator<RASTER> iterator;
        typedef typename RASTER::difference_type difference_type; // avoid gcc warning
        typedef typename RASTER::reference reference; // avoid gcc warning


        subraster_iterator() {}

        subraster_iterator(RASTER * x) {
            this->_map = x;
            this->_cur = this->_map->dataptr();
            this->calc_begin_and_end_of_line();
        }

        subraster_iterator(RASTER * x, typename RASTER::pointer cur) {
            this->_map = x;
            this->_cur = cur;
            this->calc_begin_and_end_of_line();
        }	 

        reference operator*() const {
#ifdef RANGE_CHECK
            if(_cur < _map->find(0, 0).ptr()) {
                std::cerr << "dereference invalid iterator (too small): " 
                    <<  (void*)_cur << "<" << (void*)_map->find(0, 0).ptr() << " , xy = " << xy() << std::endl;
                assert(0);
            }
            if(_cur > _map->find(_map->xsize()-1, _map->ysize() -1).ptr())  {
                std::cerr << "dereference invalid iterator (too large): " 
                    <<  (void*)_cur << ">" << (void*)_map->find(x()-1, y()-1).ptr() << " , xy = " << xy() << std::endl;
                assert(0);
            };
#endif
            return *(this->_cur);
        }

        iterator& operator++() { this->increment(); return *this; }
        iterator& operator--() { this->decrement(); return *this; }

#define RETURN_VALUE_ON_POST_IN_AND_DECREMENT
#ifdef RETURN_VALUE_ON_POST_IN_AND_DECREMENT    
        iterator operator++(int) {
            subraster_iterator tmp = *this;
            this->increment();
            return tmp;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            this->decrement();
            return tmp;
        }
#else
        void operator++(int) { this->increment(); }

        void operator--(int) { this->decrement(); }
#endif

        iterator & operator+=(difference_type n) {
            add(n);
            return *this;
        }
        iterator & operator-=(difference_type n) { return *this += -n; }
        iterator operator+(difference_type n) const {
            iterator tmp = *this;
            return tmp += n;
        }
        iterator operator-(difference_type n) const {
            iterator tmp = *this;
            return tmp -= n;
        }
        reference operator[](difference_type n) const { return *(*this + n); }

        difference_type operator-(const iterator& i) const {
            difference_type dx = this->x() - i.x();
            difference_type dy = this->y() - i.y();
            return dy * this->_map->xsize() + dx;
        }

        bool operator==(const subraster_iterator& it) const { return this->_cur == it._cur; }
        bool operator!=(const subraster_iterator& it) const { return this->_cur != it._cur; };

};

/// const version of subraster_iterator
template <class RASTER>
class subraster_const_iterator : public subraster_base_iterator<const RASTER>
{
    public:
        typedef subraster_const_iterator<RASTER> const_iterator;
        typedef typename RASTER::const_reference const_reference;
        typedef typename RASTER::difference_type difference_type; // avoid gcc warning
        typedef typename RASTER::reference reference; // avoid gcc warning
        
        subraster_const_iterator() {}
        subraster_const_iterator(const RASTER * x) {
            this->_map = x;
            this->_cur = this->_map->dataptr();
            this->calc_begin_and_end_of_line();
        }

        subraster_const_iterator(const RASTER * x, typename RASTER::pointer cur) {
            this->_map = x;
            this->_cur = cur;
            this->calc_begin_and_end_of_line();
        }
        subraster_const_iterator(const subraster_iterator<RASTER> & it) {
            this->_map = it._map;
            this->_cur = it._cur;
        } 	 

        subraster_const_iterator & operator=(const subraster_iterator<RASTER> & it) {
            this->_map = it._map;
            this->_cur = it._cur;
            return *this;
        } 	 

        const_reference operator*() const {
#ifdef RANGE_CHECK
            if(_cur < _map->find(0, 0).ptr()) {
                std::cerr << "dereference invalid iterator (too small): " 
                    <<  (void*)_cur << "<" << (void*)_map->find(dim_type(0, 0)).ptr() << " , xy = " << xy() << std::endl;
                assert(0);
            }
            if(_cur > _map->find(_map->xsize()-1, _map->ysize() -1).ptr())  {
                std::cerr << "dereference invalid iterator (too large): " 
                    <<  (void*)_cur << ">" << (void*)_map->find(dim_type(x()-1, y()-1)).ptr() << " , xy = " << xy() << std::endl;
                assert(0);
            };
#endif
            return *(this->_cur);
        }

        const_iterator & operator++() { this->increment(); return *this; }
        const_iterator & operator--() { this->decrement(); return *this; }

#ifdef RETURN_VALUE_ON_POST_IN_AND_DECREMENT    
        const_iterator operator++(int) {
            subraster_const_iterator tmp = *this;
            this->increment();
            return tmp;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            this->decrement();
            return tmp;
        }
#else
        void operator++(int) { increment(); }

        void operator--(int) { decrement(); }
#endif

        const_iterator & operator+=(difference_type n) {
            add(n);
            return *this;
        }
        const_iterator & operator-=(difference_type n) { return *this += -n; }
        const_iterator operator+(difference_type n) const {
            const_iterator tmp = *this;
            return tmp += n;
        }
        const_iterator operator-(difference_type n) const {
            const_iterator tmp = *this;
            return tmp -= n;
        }
        reference operator[](difference_type n) const { return *(*this + n); }

        difference_type operator-(const const_iterator& i) const {
            difference_type dx = this->x() - i.x();
            difference_type dy = this->y() - i.y();
            return dy * this->_map->xsize() + dx;
        }

        bool operator==(const subraster_const_iterator & it) const { return this->_cur == it._cur; }
        bool operator!=(const subraster_const_iterator & it) const { return this->_cur != it._cur; };
};

} // namespace asl

template <class RASTER>
inline std::random_access_iterator_tag
iterator_category(const asl::subraster_const_iterator<RASTER> &) {
  return std::random_access_iterator_tag();
}

template <class RASTER>
inline 
typename asl::subraster_const_iterator<RASTER>::value_type * 
value_type(const asl::subraster_const_iterator<RASTER> &) 
{
  return (typename asl::subraster_const_iterator<RASTER>::value_type *)(0);
}

template <class RASTER>
inline 
typename asl::subraster_const_iterator<RASTER>::difference_type * 
distance_type(const asl::subraster_const_iterator<RASTER> &) 
{
  return (typename asl::subraster_const_iterator<RASTER>::difference_type *)(0);
}

namespace asl {

template <class T, class D>
class const_subraster
{
    public:  // TYPES
        typedef const_subraster<T, D> raster_type;

        typedef T value_type;
        typedef const T * pointer;
        typedef size_t size_type;
        typedef D difference_type;
        typedef const value_type & reference;
        typedef const value_type & const_reference;

        typedef subraster_const_iterator<raster_type> const_iterator;
        typedef const T * const_horizontal_line_iterator;
        typedef step_iterator<const T *,T,const T &,D> const_vertical_line_iterator;
        typedef rect_iterator<const_horizontal_line_iterator, 
                const_vertical_line_iterator, T, const T &, D> const_sub_iterator;

        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        friend class subraster_base_iterator<raster_type>;
        friend class subraster_const_iterator<raster_type>;

    public:  // CONSTRUCTORS

        const_subraster() { }

        const_subraster(const const_subraster& m) {
            set( m.xsize(), m.ysize(), m.stride(), m.dataptr() );
        }
        const_subraster(const subraster<T, D>& m) {
            set( m.xsize(), m.ysize(), m.stride(), m.dataptr() );
        }

        const_subraster(const const_subraster& m, D xpos, D ypos, D Xsize, D Ysize) {
            set( Xsize, Ysize, m.stride(), (const T*)&m(xpos, ypos));
        }

        template <class RT, class Alloc, class RD>
        const_subraster(const raster<RT, Alloc, RD>& m) {
            set( m.xsize(), m.ysize(), m.xsize(), (const T*)(&(*(m.begin()))) );
        }

        template <class RT, class Alloc, class RD>
        const_subraster(const raster<RT, Alloc, RD>& m, RD xpos, RD ypos, RD Xsize, RD Ysize) {
            set( Xsize, Ysize, m.xsize(), (const T*)(&(*(m.find(xpos, ypos)))) );
        }

        const_subraster(D Xsize, D Ysize) {
            set(Xsize, Ysize, Xsize, 0);
        }

        const_subraster(D Xsize, D Ysize, const T* Dataptr) {
            set(Xsize, Ysize, Xsize, Dataptr);
        }

        const_subraster(D Xsize, D Ysize, D Stride, const T* Dataptr) {
            set(Xsize, Ysize, Stride, Dataptr);
        }

        const_subraster(T* Begin, T* End, D Stride) {

            set((End-Begin)%Stride, (End-Begin)/Stride, Stride, Begin);
        }

        const_subraster(D Xsize, D Ysize, D Stride, const_iterator it) { 
            set(Xsize, Ysize, Stride, &(*it));
        }

        ~const_subraster() { }

    public:  // GETTER
        const D & stride() const { return _stride;}
        const D & hsize() const { return _xsize;}
        const D & vsize() const { return _ysize;}
        const D & xsize() const { return _xsize;}
        const D & ysize() const { return _ysize;}
        const T * dataptr() const {return _dataptr;}

    public:  // SETTER

        void set(D Xsize, D Ysize) {
            _xsize = Xsize; _ysize = Ysize; _stride = Xsize; _dataptr = 0;
        }
        void set(D Xsize, D Ysize, D Stride) {
            _xsize = Xsize; _ysize = Ysize; _stride = Stride; _dataptr = 0;
        }
        void set(D Xsize, D Ysize, const T* Dataptr) {
            _xsize = Xsize; _ysize = Ysize; _stride = Xsize; _dataptr = (T*)Dataptr;
        }
        void set(D Xsize, D Ysize, const D& Stride,const T* Dataptr) {
            _xsize = Xsize; _ysize = Ysize; _stride = Stride; _dataptr = (T*)Dataptr;
        }

    protected:  // SETTER
        void xsize(D val) { _xsize = val; }
        void ysize(D val) { _ysize = val; }
        void hsize(D val) { _xsize = val; }
        void vsize(D val) { _ysize = val; }
        void stride(const D & s) { _stride = s; }
        void dataptr(const T * dp) { _dataptr = dp; }

    public:  // ITERATORS

        const_iterator begin() const { return const_iterator(this, _dataptr);}
        const_iterator end() const { return const_iterator(this, _dataptr+_stride*_ysize);}

        const_sub_iterator begin(D x, D y, D xsize, D ysize) const { return const_sub_iterator(hfind(x, y), vfind(x, y), xsize);}
        const_sub_iterator end(D x, D y, D xsize, D ysize) const { return const_sub_iterator(hfind(x, y), vfind(x, y), xsize, hfind(x, y+ysize));}

        const_horizontal_line_iterator hfind(D x = 0, D y = 0) const { return const_horizontal_line_iterator(_dataptr + y * _stride + x);}
        const_horizontal_line_iterator hfind(const T * ptr) const { return const_horizontal_line_iterator(ptr);}
        const_horizontal_line_iterator hfind(const_vertical_line_iterator vit) const { return const_horizontal_line_iterator(vit.base());}

        const_vertical_line_iterator vfind(D x, D y) const { return const_vertical_line_iterator( _dataptr + y * _stride + x, _stride);}
        const_vertical_line_iterator vfind(const T * ptr) const { return const_vertical_line_iterator(ptr, _stride);}

        const_horizontal_line_iterator hbegin() const { return const_horizontal_line_iterator(_dataptr);}
        const_vertical_line_iterator vbegin() const { return const_vertical_line_iterator(_dataptr, _stride);}

        const_horizontal_line_iterator hend() const { return const_horizontal_line_iterator( _dataptr + _ysize * _stride);}
        const_vertical_line_iterator vend() const { return const_vertical_line_iterator( _dataptr + _ysize * _stride, _stride);}

        const_reverse_iterator rbegin() const {  return const_reverse_iterator(end());  }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    public:  // UTILITY: return column x and line y for a given pointer

        D size() const { return _xsize*_ysize;}
        D x(const T * cur) const { return ( cur - dataptr()) % stride();}
        D y(const T * cur) const { return ( cur - dataptr()) / stride();}

    public:  // FACILITY: subref & find
        const_iterator find(D X, D Y) const { return const_iterator(this, get_ptr(X, Y)); }

        template <class RT, class Alloc, class RD>
        void copy_to_raster(raster<RT, Alloc, RD> & result) const {
                result.resize(xsize(), ysize());
                copy(begin(), end(), result.begin());
            }

        void debug(const char *name = "") const {
            std::cerr << "BEGIN DEBUG OUTPUT of subraster " << name << " @ " << (void*)this << ":" << std::endl;
            std::cerr << (void*)this << "->_xsize = " << _xsize << std::endl;
            std::cerr << (void*)this << "->_ysize = " << _ysize << std::endl;
            std::cerr << (void*)this << "->_stride = " << _stride << std::endl;
            std::cerr << (void*)this << "->_dataptr = " << (void*)_dataptr << std::endl;
            std::cerr << "END   DEBUG OUTPUT of subraster @ " << (void*)this << "." << std::endl;
        }
        void dump(const char *name = "") const {
            std::cerr << name << std::endl;
            std::cerr << *this << std::endl;
        }
    public: // OPERATORS
        const_reference operator()(D X, D Y) const { return *(find(X, Y)); }
#if _MSC_VER == 1500
    public:
#else
    protected: // MEMBER FUNCTIONS
#endif
        const T * get_ptr(D X, D Y) const { 
            return _dataptr + _stride * Y + X;
        }
    protected: // MEMBER CLASSES
        D _xsize;
        D _ysize;
        D _stride;
        mutable T * _dataptr;
};

//
///  generic ostream output operator template for subraster templates
//
template <class VALUE, class D>
std::ostream& operator<<(std::ostream& o, const const_subraster<VALUE, D> & s)
{
    if (!(o << s.xsize() << ' ' << s.ysize() << std::endl)) {
        std::cerr << "const_subraster : ostream& operator<<(istream& i, D& d) - write size failed" << std::endl;
    }
    typename const_subraster<VALUE, D>::const_iterator i;
    for (i = s.begin(); i != s.end(); ++i) {
        if (!(o << *i)) {
            std::cerr << "const_subraster : ostream& operator<<(istream& i, D& d) - write data failed" << std::endl;
        };
        if (i.at_end_of_line()) o << '\n';
        else o << ' ';
    }
    return o;
}
template <class T, class D>
inline bool operator==(const const_subraster<T, D> & x, const const_subraster<T, D>& y) {
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class D>
inline bool operator<(const const_subraster<T, D> & x, const const_subraster<T, D>& y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

#if 0
// The two folllowing function should allow a subrect copy extraction from any raster type
using the same expression
template <class T, class Alloc, class D>
raster<T, Alloc, D>
make_raster(const raster<T, Alloc, D>& m,
        D xpos, D ypos, 
        D xsize, D ysize)
{
    return const_subraster<T, Alloc, D>(m, xpos, ypos, xsize, ysize).as_raster();
}

template <class T, class Alloc, class D>
raster<T, Alloc, D, TPTR>&
make_raster(const const_subraster<T, Alloc, D>& m,
        D xpos, D ypos, 
        D xsize, D ysize)
{
    return const_subraster<T, Alloc, D>(m, xpos, ypos, xsize, ysize).as_raster();
}
#endif

template <class T, class D>
class subraster : public const_subraster<T, D>
{
    public:  // TYPES
        typedef subraster<T, D> raster_type;
        typedef const_subraster<T, D> const_base;

        typedef typename const_base::value_type value_type;
        typedef typename const_base::size_type size_type;
        typedef typename const_base::difference_type difference_type;
        typedef T & reference;
        typedef T * pointer;
        typedef typename const_base::const_reference const_reference;

        typedef subraster_iterator<raster_type> iterator;
        typedef T * horizontal_line_iterator;
        typedef step_iterator<T*,T,T&,D> vertical_line_iterator;
        typedef rect_iterator<horizontal_line_iterator, vertical_line_iterator, T, T&, D> sub_iterator;

        typedef typename const_base::const_iterator const_iterator;
        typedef typename const_base::const_horizontal_line_iterator const_horizontal_line_iterator;
        typedef typename const_base::const_vertical_line_iterator const_vertical_line_iterator;
        typedef typename const_base::const_sub_iterator const_sub_iterator;

        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;

        friend class subraster_base_iterator<raster_type>;
        friend class subraster_iterator<raster_type>;
        //friend class const_iterator; // fails with GCC 4.2.3; according to como, typedefs may not be used in an elaborated type specifier

    public:  // CONSTRUCTORS

        subraster() { }

        subraster(subraster & m) {
            set( m.xsize(), m.ysize(), m.stride(), m.dataptr() );
        }

        subraster(subraster & m, D xpos, D ypos, D Xsize, D Ysize) {
            set( Xsize, Ysize, m.stride(), (T*)&m(xpos, ypos));
        }

        template <class RT, class Alloc, class RD>
        subraster(raster<RT, Alloc, RD> & m) {
           set( m.xsize(), m.ysize(), m.xsize(), (&(*(m.begin()))) );
        }

        template <class RT, class Alloc, class RD>
        subraster(raster<RT, Alloc, RD> & m, RD xpos, RD ypos, RD Xsize, RD Ysize) {
            set( Xsize, Ysize, m.xsize(), (&(*(m.find(xpos, ypos)))) );
        }

        subraster(D Xsize, D Ysize) {
            set(Xsize, Ysize, Xsize, 0);
        }

        subraster(D Xsize, D Ysize, T* Dataptr) {
            set(Xsize, Ysize, Xsize, Dataptr);
        }

        subraster(D Xsize, D Ysize, D Stride, T* Dataptr) {
            set(Xsize, Ysize, Stride, Dataptr);
        }

        subraster(T* Begin, T* End, D Stride) {

            set((End-Begin)%Stride, (End-Begin)/Stride, Stride, Begin);
        }

        subraster(D Xsize, D Ysize, D Stride, const_iterator it) { 
            set(Xsize, Ysize, Stride, &(*it));
        }

        subraster(D Xsize, D Ysize, D Stride, iterator it) {
            set(Xsize, Ysize, Stride, &(*it));
        }

        ~subraster() { }

    public:  // GETTER
        T* dataptr() {return this->_dataptr;}

    public:  // SETTER
        void set(D Xsize, D Ysize, T* Dataptr) {
            this->_xsize = Xsize; this->_ysize = Ysize; this->_stride = Xsize; this->_dataptr = Dataptr;
        }
        void set(D Xsize, D Ysize, const D& Stride, T* Dataptr) {
            this->_xsize = Xsize; this->_ysize = Ysize; this->_stride = Stride; this->_dataptr = Dataptr;
        }

    protected:  // SETTER

        void dataptr(T* dp) { this->_dataptr = dp; }

    public:  // ITERATORS

        iterator begin() { return iterator(this, this->_dataptr);}
        iterator end() { return iterator(this, this->_dataptr+this->_stride*this->_ysize);}
        const_iterator begin() const { return const_base::begin();}
        const_iterator end() const { return const_base::begin();}

        sub_iterator begin(D x, D y, D xsize, D ysize) { return sub_iterator(hfind(x, y), vfind(x, y), xsize);}
        sub_iterator end(D x, D y, D xsize, D ysize) { return sub_iterator(hfind(x, y), vfind(x, y), xsize, hfind(x, y+ysize));}
        const_sub_iterator begin(D x, D y, D xsize, D ysize) const { return const_base::begin(x, y, xsize, ysize); }
        const_sub_iterator end(D x, D y, D xsize, D ysize) const { return const_base::end(x, y, xsize, ysize); }
        
        horizontal_line_iterator hfind(D x = 0, D y = 0) { return horizontal_line_iterator(this->_dataptr + y * this->_stride + x);}
        horizontal_line_iterator hfind(T* ptr) { return hbegin(y(ptr));}
        horizontal_line_iterator hfind(vertical_line_iterator vit) { return horizontal_line_iterator(vit.base());}
        const_horizontal_line_iterator hfind(D x = 0, D y = 0) const { return const_base::hfind(x, y);}
        const_horizontal_line_iterator hfind(const T * ptr) const { return const_base::hfind(ptr);}
        const_horizontal_line_iterator hfind(const_vertical_line_iterator vit) const { return const_base::hfind(vit);}

        vertical_line_iterator vfind(D x, D y) { return vertical_line_iterator(this->_dataptr + y * this->_stride + x, this->_stride);}
        vertical_line_iterator vfind(T* ptr) { return vertical_line_iterator(ptr, this->_stride);}
        const_vertical_line_iterator vfind(D x, D y) const { return const_base::vfind(x, y);}
        const_vertical_line_iterator vfind(const T * ptr) const { return const_base::vfind(ptr);}

        horizontal_line_iterator hbegin() { return horizontal_line_iterator(this->_dataptr);}
        vertical_line_iterator vbegin() { return vertical_line_iterator(this->_dataptr, this->_stride);}
        const_horizontal_line_iterator hbegin() const { return const_base::hbegin();}
        const_vertical_line_iterator vbegin() const { return const_base::vbegin();}

        horizontal_line_iterator hend() { return horizontal_line_iterator(this->_dataptr + this->_ysize * this->_stride);}
        vertical_line_iterator vend() { return vertical_line_iterator(this->_dataptr + this->_ysize * this->_stride, this->_stride);}
        const_horizontal_line_iterator hend() const { return const_base::hend();}
        const_vertical_line_iterator vend() const { return const_base::vend();}

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const {  return const_base::rbegin();  }
        const_reverse_iterator rend() const { return const_base::rend(); }

    public:  // FACILITY: subref & find

        iterator find(D X, D Y) { return iterator(this, get_ptr(X, Y));}
        const_iterator find(D X, D Y) const { return const_base::find(X, Y); }

        void dump(const char *name = "") const {
            std::cerr << name << std::endl;
            std::cerr << *this << std::endl;
        }
    public: // OPERATORS

        reference operator()(D X, D Y) { return *(find(X, Y)); }
        const_reference operator()(D X, D Y) const { return const_base::operator()(X, Y); }

    protected: // MEMBER FUNCTIONS

        T* get_ptr(D X, D Y) const { 
            return this->_dataptr + this->_stride * Y + X;
        }

};

template <class SubMatrix>
SubMatrix clip_mat(const SubMatrix& m, int h_pos, int v_pos, int h_clip_size, int v_clip_size) {

    int ha_min = h_pos;
    int va_min = v_pos;
    int ha_max = h_pos + m.hsize();
    int va_max = v_pos + m.vsize();

    int hb_min = 0;
    int vb_min = 0;
    int hb_max = h_clip_size;
    int vb_max = v_clip_size;

    int h_min = ha_min > hb_min ? ha_min : hb_min;
    int h_max = ha_max < hb_max ? ha_max : hb_max;

    int v_min = va_min > vb_min ? va_min : vb_min;
    int v_max = va_max < vb_max ? va_max : vb_max;

    int h_size = h_max - h_min;
    int v_size = v_max - v_min;

    int h = h_min - h_pos;
    int v = v_min - v_pos;

    return SubMatrix(m, h, v, h_size, v_size);     
} 

} // namespace asl
#endif
