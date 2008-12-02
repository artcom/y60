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
//    $RCSfile: raster.h,v $
//
//   $Revision: 1.6 $
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
#ifndef _ASL_RASTER_RASTER_H_INCLUDED_
#define _ASL_RASTER_RASTER_H_INCLUDED_

#include <stddef.h>
#include <algorithm>
#include <vector>
#include "piterator.h"

namespace asl {

template <class T>
struct PackedRaster {
	enum {Factor = 1};
};


template <class T, class Alloc = std::allocator<T>, class D = AC_OFFSET_TYPE>
class raster {
    public:
        typedef T value_type;
        typedef value_type * pointer;
        typedef value_type * iterator;
        typedef const value_type * const_iterator;
        typedef value_type& reference;
        typedef const value_type & const_reference;
        typedef AC_SIZE_TYPE size_type;
        typedef AC_OFFSET_TYPE difference_type;

        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;

        // raster specific types:

        typedef T* horizontal_line_iterator;
        typedef const T* const_horizontal_line_iterator;

        typedef step_iterator<T*,T,T&,D> vertical_line_iterator;
        typedef step_iterator<const T*,T,const T&,D> const_vertical_line_iterator;

        typedef rect_iterator<horizontal_line_iterator, vertical_line_iterator, T, T&, D> sub_iterator;
        typedef rect_iterator<const_horizontal_line_iterator, const_vertical_line_iterator, T, const T&, D> const_sub_iterator;
        //
        const std::vector<T,Alloc> & getDataVector() const {
            return _data;
        };
        std::vector<T,Alloc> & getDataVector() {
            return _data;
        };
    protected:
        size_type _stride;
        std::vector<T,Alloc> _data;
    public:
        //  raster specific funtions go here:
        D xsize() const { return _stride;}
        D hsize() const { return _stride;} 
        D ysize() const { return _stride ? size() * PackedRaster<T>::Factor / _stride : 0;}
        D vsize() const { return _stride ? size() * PackedRaster<T>::Factor / _stride : 0;}

        D stride() const { return _stride;}

        void resize(D X, D Y, const T& x) { _stride = X; resize_vec(_stride * Y, x);}
        void resize(D X, D Y) { resize(X, Y, T());}

        raster() : _stride(0) { }
        raster(const raster<T, Alloc, D>& x)
            : _stride(x.stride()), _data(x.begin(),x.end()) {}

        raster(const_iterator first, const_iterator last, size_type stride)
            : _stride(stride), _data(first, last) {}
        
        raster(const D& x, const D& y) 
            : _stride(x), _data(x * y / PackedRaster<T>::Factor) {}

        raster(const D& x, const D& y, const_iterator first)
            : _stride(x), _data(first, first + x * y / PackedRaster<T>::Factor) {}
        
        raster(const D& x, const D& y,  const T& value)
            : _stride(x), _data(x * y / PackedRaster<T>::Factor, value) {}
        
        raster(const raster<T, Alloc, D>& x, D xmin, D ymin, D xsize, D ysize)
            : _stride(xsize),
              _data(x.begin(xmin,ymin,xsize,ysize),x.end(xmin,ymin,xsize,ysize)) {}

        ~raster() { }

        size_type offset(D x, D y) const { return y * _stride + x;}

        reference operator()(D X, D Y) { return *(begin() + offset(X, Y)); }
        const_reference operator()(D X, D Y) const { return *(begin() + offset(X, Y)); }

        iterator find(D Xpos, D Ypos) { return begin() + offset(Xpos, Ypos);}

        const_iterator find(D Xpos, D Ypos) const { return begin() + offset(Xpos, Ypos);}

        D x(const_iterator it) const { return ( it - begin()) % _stride;}

        D y(const_iterator it) const { return ( it - begin()) / _stride;}


        // line & column iterators

        sub_iterator begin(D x, D y, D xsize, D ysize) {
            return sub_iterator(hfind(x, y), vfind(x, y), xsize);
        }
        const_sub_iterator begin(D x, D y, D xsize, D ysize) const {
            return const_sub_iterator(hfind(x, y), vfind(x, y), xsize);
        }
        sub_iterator end(D x, D y, D xsize, D ysize) {
            return sub_iterator(hfind(x, y), vfind(x, y), xsize, hfind(x, y+ysize));
        }
        const_sub_iterator end(D x, D y, D xsize, D ysize) const {
            return const_sub_iterator(hfind(x, y), vfind(x, y), xsize, hfind(x, y+ysize));
        }
        horizontal_line_iterator hfind(D x = 0, D y = 0) {
            return horizontal_line_iterator(begin() + y * _stride + x);
        }
        const_horizontal_line_iterator hfind(D x = 0, D y = 0) const {
            return const_horizontal_line_iterator(begin() + y * _stride + x);
        }
        horizontal_line_iterator hfind(T* ptr) {
            return hbegin(y(ptr));
        }
        const_horizontal_line_iterator hfind(const T* ptr) const {
            return const_horizontal_line_iterator(ptr);
        }
        horizontal_line_iterator hfind(vertical_line_iterator vit) {
            return horizontal_line_iterator(vit.base());
        }
        const_horizontal_line_iterator hfind(const_vertical_line_iterator vit) const {
            return const_horizontal_line_iterator(vit.base());
        }
        vertical_line_iterator vfind(D x, D y) {
            return vertical_line_iterator(begin() + y * _stride + x, _stride);
        }
        const_vertical_line_iterator vfind(D x, D y) const {
            return const_vertical_line_iterator( begin() + y * _stride + x, _stride);
        }
        vertical_line_iterator vfind(T* ptr) {
            return vertical_line_iterator(ptr, _stride);
        }
        const_vertical_line_iterator vfind(const T* ptr) const {
            return const_vertical_line_iterator(ptr, _stride);
        }
        horizontal_line_iterator hbegin() {
            return horizontal_line_iterator(begin());
        }
        const_horizontal_line_iterator hbegin() const {
            return const_horizontal_line_iterator(begin());
        }
        vertical_line_iterator vbegin() {
            return vertical_line_iterator(begin(), _stride);
        }
        const_vertical_line_iterator vbegin() const {
            return const_vertical_line_iterator(begin(), _stride);
        }
        horizontal_line_iterator hend() {
            return horizontal_line_iterator(begin() + ysize() * _stride);
        }
        const_horizontal_line_iterator hend() const {
            return const_horizontal_line_iterator( begin() + ysize() * _stride);
        }
        vertical_line_iterator vend() {
            return vertical_line_iterator(begin() + ysize() * _stride, _stride);
        }
        const_vertical_line_iterator vend() const {
            return const_vertical_line_iterator( begin() + ysize() * _stride, _stride);
        }

        // same as std::vector:

        iterator begin() {
			return _data.empty() ? NULL : &(*_data.begin());
        }
        const_iterator begin() const {
            return _data.empty() ? NULL : &(*_data.begin());
        }
        iterator end() {
            return begin() + size();
        }
        const_iterator end() const {
            return begin() + size();
        }
        reverse_iterator rbegin() {
            return std::reverse_iterator<T*>(end());
        }
        const_reverse_iterator rbegin() const { 
            return const_reverse_iterator(end()); 
        }
        reverse_iterator rend() {
            return std::reverse_iterator<T*>(begin());
        }
        const_reverse_iterator rend() const { 
            return const_reverse_iterator(begin()); 
        }

        size_type size() const {
            return _data.size();
        }
        size_type max_size() const {
            return size_type(-1);
        }
        size_type capacity() const {
            return size_type(this->end_of_storage - begin());
        }
        bool empty() const {
            return begin() == end(); 
        }
        reference operator[](size_type n) {
            return *(begin() + n);
        }
        const_reference operator[](size_type n) const {
            return *(begin() + n);
        }

        raster<T, Alloc, D> & operator=(const raster<T, Alloc, D>& x) {
            _data = x._data;
            _stride = x._stride;
            return *this;
        };
        void swap(raster<T, Alloc, D>& x) {
            std::swap(_data,x._data);
            std::swap(_stride,x._stride);
        }
        void clear() {
            erase(begin(), end());
        }
        void debug(const char *name = "") const {
            std::cerr << "BEGIN DEBUG OUTPUT of raster " << name << " @ " << (void*)this << ":" << std::endl;
            std::cerr << "start = " << (void*)this->start << std::endl;
            std::cerr << "finish = " << (void*)this->finish << std::endl;
            std::cerr << "end_of_storage = " << (void*)this->end_of_storage << std::endl;
            std::cerr << "_stride = " << _stride << std::endl;
            std::cerr << "xsize() = " << xsize() << std::endl;
            std::cerr << "ysize() = " << ysize() << std::endl;
            std::cerr << "END   DEBUG OUTPUT of raster @ " << (void*)this << "." << std::endl;
        }
        void dump(const char *name = "") const {
            std::cerr << name << std::endl;
            std::cerr << *this << std::endl;
        }
private:
        void reserve(size_type n) {
			_data.reserve(n / PackedRaster<T>::Factor);
        }
        reference front() {
            return *begin();
        }
        const_reference front() const {
            return *begin();
        }
        reference back() {
            return *(end() - 1);
        }
        const_reference back() const {
            return *(end() - 1);
        }
        void push_back(const T& x) {
            _data.push_back(x);
        }
        iterator insert(iterator position, const T& x) {
            return _data.insert(position,x);
        }
        iterator insert(iterator position) {
             return _data.insert(position, T());
        }
        void insert (iterator position, const_iterator first, const_iterator last) {
            _data.insert(position,first,last);
        };
        void insert (iterator position, size_type n, const T& x) {
            _data.insert(_data.begin() + (position-begin()),n,x);
        };
        void pop_back() {
            _data.pop_back();
        }
        void erase(iterator position) {
            _data.erase(position);
        }
        void erase(iterator first, iterator last) {
            _data.erase(_data.begin() + (first-begin()),
                        _data.begin() + (last-begin()));

        }
        void resize_vec(size_type new_size, const T& x) {
			new_size = new_size / PackedRaster<T>::Factor;
            if (new_size < size()) 
                erase(begin() + new_size, end());
            else
                insert(end(), new_size - size(), x);
        }
        void resize_vec(size_type new_size) {
            resize_vec(new_size, T());
        }
};

template <class T, class Alloc, class D>
inline bool operator==(const raster<T, Alloc, D>& x, const raster<T, Alloc, D>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc, class D>
inline bool operator!=(const raster<T, Alloc, D>& x, const raster<T, Alloc, D>& y) {
    return !(x == y);
}

template <class T, class Alloc, class D>
inline bool operator<(const raster<T, Alloc, D>& x, const raster<T, Alloc, D>& y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}
//
///  generic ostream output operator template for raster templates
//
#ifdef USE_FORMATTED_TEST_RASTER_OUTPUT_OPERATOR
template <class T, class alloc, class D>
std::ostream& operator<<(std::ostream& o, const raster<T, alloc, D>& m)
{
    if (!(o << m.xsize() << ' ' <<  m.ysize() << std::endl)) {
        std::cerr << "raster<T>: ostream& operator<<(ostream &, ...) - write failed" << std::endl;
    }
    int x, y;
    for (y = 0; y < m.ysize(); y++) {
        for (x = 0; x < m.xsize(); x++) {
            if (!(o << m(x, y) << ' ')) {
                std::cerr << "raster: ostream& operator<<(ostream &, ...) - write failed" << std::endl;
            }
        };
        if (!(o << std::endl)) {
            std::cerr << "raster: ostream& operator<<(ostream &, ...) - write failed" << std::endl;
        };
    }
    return o;
};

template <class T, class alloc, class D>
std::istream & operator>>(std::istream & is, asl::raster<T, alloc, D>& m) {
    int xsize, ysize;
    if (is >> xsize && is >> ysize) {
        m.resize(xsize, ysize);
        int x, y;
        for (y = 0; y < ysize; y++) {
            for (x = 0; x < xsize; x++) {
                if (!(is >> m(x, y))) {
                    is.setstate(std::ios::failbit);
                    return is;
                }
            }
        }
    } else {
        is.setstate(std::ios::failbit);
    }
    return is;
}

#endif

} // namespace asl
#endif

