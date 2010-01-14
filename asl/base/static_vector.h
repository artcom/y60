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
// Description: static_vector STL container
//              a fixed-capacity std::vector replacement
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

////////////////////////////////////////////////////////////////////////////////

#if !defined(STATIC_VECTOR_H)
#define STATIC_VECTOR_H

////////////////////////////////////////////////////////////////////////////////

#include "asl_base_settings.h"

#include <iterator>
#include <algorithm>
#include <memory>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */

    /*! vector with compile-time fixed capacity
    **
    ** This behaves just like 'std::vector<>' except for the following
    ** deviations:
    ** - it doesn't use dynamic memory, the capacity is determined at compile-
    **   time; there's a static member 'static_max_size' to find out about it
    ** - its 'swap()' member function is O(n) and might throw, if the
    **   'value_type's cctor throws
    **
    ** note  see std::vector documentation for all other members
    */
    template< typename T, std::size_t MAX_SIZE >
    class static_vector {
        public:
            // type definitions
            typedef T                                     value_type;
            typedef value_type*                           pointer;
            typedef const value_type*                     const_pointer;
            typedef value_type&                           reference;
            typedef const value_type&                     const_reference;
            typedef pointer                               iterator;
            typedef const_pointer                         const_iterator;
            typedef std::reverse_iterator<iterator>       reverse_iterator;
            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
            typedef std::size_t                           size_type;
            typedef std::ptrdiff_t                        difference_type;

            // constructors/destructors
            static_vector()
                : size_(0), elems_(reinterpret_cast<value_type*>(storage_)) {
            }

            explicit static_vector(size_type count, const value_type& val = value_type())
                : size_(0), elems_(reinterpret_cast<value_type*>(storage_)) {
                    assign(count,val);
            }

            template< typename InpIt_ >
            static_vector(InpIt_ it_beg, InpIt_ it_end)
                : size_(0), elems_(reinterpret_cast<value_type*>(storage_)) {
                assign(it_beg,it_end);
            }

            static_vector(const static_vector& rhs)
                : size_(0), elems_(reinterpret_cast<value_type*>(storage_)) {
                assign(rhs.begin(),rhs.end());
            }

            ~static_vector() {
                clear();
            }

            // assignment
            static_vector& operator=(const static_vector& rhs) {
                assign(rhs.begin(),rhs.end());return*this;
            }

            void
            assign(size_type count, const value_type& val);

            template<class InpIt_>
            void
            assign(InpIt_ it_beg, InpIt_ it_end);

            // size/capacity
            static const size_type            static_max_size = MAX_SIZE;

            bool
            empty() const {
                return 0==size();
            }

            size_type
            max_size() const {
                return static_max_size;
            }

            size_type
            size() const {
                return size_;
            }

            void
            resize(size_type newsize, const value_type& val = value_type());

            size_type
            capacity() {
                return max_size();
            }

            void reserve(size_type count) {
                check_size_(count);
            }

            // data access
            iterator
            begin() {
                return elems_;
            }

            const_iterator
            begin() const {
                return elems_;
            }

            iterator
            end() {
                return elems_+size();
            }

            const_iterator
            end() const {
                return elems_+size();
            }

            reverse_iterator
            rbegin() {
                return reverse_iterator(end());
            }

            const_reverse_iterator
            rbegin() const {
                return const_reverse_iterator(end());
            }

            reverse_iterator
            rend() {
                return reverse_iterator(begin());
            }

            const_reverse_iterator
            rend() const {
                return const_reverse_iterator(begin());
            }

            reference
            operator[](size_type idx) {
                return elems_[idx];
            }

            const_reference
            operator[](size_type idx)const {
                return elems_[idx];
            }

            reference
            at(size_type idx) {
                check_index_(idx); return operator[](idx);
            }

            const_reference
            at(size_type idx) const {
                check_index_(idx); return operator[](idx);
            }

            reference
            front() {
                return elems_[0];
            }

            const_reference
            front() const {
                return elems_[0];
            }

            reference
            back() {
                return elems_[size()-1];
            }

            const_reference
            back() const {
                return elems_[size()-1];
            }

            // stack operations
            void
            push_back(const value_type& val) {
                insert(end(),val);
            }

            void
            pop_back() {
                erase(end()-1,end());
            }

            // erasure
            void
            clear() {
                erase(begin(),end());
            }

            void
            erase(iterator it_where) {
                destroy_( std::copy(it_where+1,end(),it_where), end() );
            }

            void
            erase(iterator it_beg, iterator it_end)
            {
                destroy_( std::copy(it_end,end(),it_beg), end() );
            }

            // insertion
            iterator
            insert(iterator it_where, const value_type& val) {
                insert(it_where,static_cast<size_type>(1),val);return it_where;
            }

            void
            insert(iterator it_where, size_type count, const value_type& val);

            template< typename InpIt_ >
            void
            insert(iterator it_where, InpIt_ it_beg, InpIt_ it_end ) {
                insert_(it_where,it_beg,it_end,typename std::iterator_traits<InpIt_>::iterator_category());
            }

            // swap

            /*!
            ** note Contrary to std::vector, this has linear complexity!
            */
            void
            swap(static_vector& rhs) {
                static_vector& smaller = size()<rhs.size() ? *this : rhs;
                static_vector& bigger  = size()<rhs.size() ? rhs : *this;
                swap_(smaller,bigger);
            }

        private:
            size_type                         size_;
            char                              storage_[ sizeof(value_type[MAX_SIZE]) ];
            value_type*                       elems_;

            static
            void
            storage_exhausted_() {
                throw std::range_error("static_vector storage exhausted");
            }

            static
            void
            invalid_index_() {
                throw std::range_error("static_vector invalid index");
            }

            static
            void
            check_size_(size_type size) {
                if(size>static_max_size) {
                    storage_exhausted_();
                }
            }

            void
            check_index_(size_type idx) const {
                if(idx>=size()) {
                    invalid_index_();
                }
            }

            void
            make_room_(iterator it_where, size_type count);

            template< typename InpIt_ >
            void
            insert_(pointer it_where, InpIt_ it_beg, InpIt_ it_end, std::input_iterator_tag) {
                for( ; it_beg != it_end; ++it_beg, ++it_where ) {
                    it_where = insert(it_where,*it_beg);
                }
            }

            template< typename FwdIt_, class IterTag_ >
            void
            insert_(pointer it_where, FwdIt_ it_beg, FwdIt_ it_end, IterTag_);

            void
            append_(size_type count, const value_type& val);

            template<class FwdIt_, class IterTag_>
            void
            append_(FwdIt_ it_beg, FwdIt_ it_end, IterTag_);

            template<class InpIt_>
            void
            append_(InpIt_ it_beg, InpIt_ it_end, std::input_iterator_tag);

            void
            destroy_(pointer it_beg, pointer it_end) {for(;it_beg!=it_end;--size_)(--it_end)->~value_type();}

            static
            void
            swap_(static_vector& smaller, static_vector& bigger);

    };

    // comparisons
    template<class T, std::size_t MAX_SIZE>
    inline
    bool
    operator==(const static_vector<T,MAX_SIZE>& lhs, const static_vector<T,MAX_SIZE>& rhs) {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    bool
    operator< (const static_vector<T,MAX_SIZE>& lhs, const static_vector<T,MAX_SIZE>& rhs) {
        return std::lexicographical_compare(lhs.begin(),lhs.end(),rhs.begin(),rhs.end());
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    bool
    operator!=(const static_vector<T,MAX_SIZE>& lhs, const static_vector<T,MAX_SIZE>& rhs) {
        return !(lhs==rhs);
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    bool
    operator> (const static_vector<T,MAX_SIZE>& lhs, const static_vector<T,MAX_SIZE>& rhs) {
        return rhs<lhs;
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    bool
    operator<=(const static_vector<T,MAX_SIZE>& lhs, const static_vector<T,MAX_SIZE>& rhs) {
        return !(rhs<lhs);
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    bool
    operator>=(const static_vector<T,MAX_SIZE>& lhs, const static_vector<T,MAX_SIZE>& rhs) {
        return !(lhs<rhs);
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    void
    swap (static_vector<T,MAX_SIZE>& lhs, static_vector<T,MAX_SIZE>& rhs) {
        lhs.swap(rhs);
    }

    ////////////////////////////////////////////////////////////////////////////
    // implementations

    template<class T, std::size_t MAX_SIZE>
    inline
    void
    static_vector<T,MAX_SIZE>::assign(size_type count, const value_type& val) {
        check_size_(count);
        pointer it = begin();
        for(; count && it!=end(); --count) {
            *it++ = val;
        }
        if(it==end()) {
            append_(count,val);
        } else {
            erase(it,end());
        }
    }

    template<class T, std::size_t MAX_SIZE>
    template<class InpIt_>
    inline
    void
    static_vector<T,MAX_SIZE>::assign(InpIt_ it_beg, InpIt_ it_end) {
        pointer it = begin();
        while( it_beg!=it_end && it!=end() ) {
            *it++ = *it_beg++;
        }
        if( it==end() ) {
            append_(it_beg,it_end,typename std::iterator_traits<InpIt_>::iterator_category());
        } else {
            erase(it,end());
        }
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    void
    static_vector<T,MAX_SIZE>::resize(size_type newsize, const value_type& val) {
        if( newsize>size() ) {
            append_(newsize-size(),val);
        } else {
            erase(begin()+newsize,end());
        }
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    void
    static_vector<T,MAX_SIZE>::insert(iterator it_where, size_type count, const value_type& val) {
        if(it_where==end()) {
            append_(count,val);
        } else {
            make_room_(it_where,count);
            std::fill_n(it_where,count,val);
        }
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    void
    static_vector<T,MAX_SIZE>::make_room_(iterator it_where, size_type count) {
        const size_type new_size = size() + count;
        check_size_(new_size);

        const iterator it_where_end = it_where + count;
        if( it_where_end < end() ) {
            // some part of destination initialized, some uninitialized
            const std::size_t overlap = end() - it_where_end;
            const iterator it_overlap = it_where + overlap;
            std::uninitialized_copy( it_overlap, end(), end() );
            std::copy( reverse_iterator(it_overlap), reverse_iterator(it_where), reverse_iterator(end()) );
        } else {
            // destination wholly uninitialized
            std::uninitialized_copy(it_where,it_where_end,it_where_end);
            std::uninitialized_fill( end(), it_where_end, value_type() );
        }

        size_ = new_size;
    }

    template<class T, std::size_t MAX_SIZE>
    template< typename FwdIt_, class IterTag_ >
    inline
    void
    static_vector<T,MAX_SIZE>::insert_(pointer it_where, FwdIt_ it_beg, FwdIt_ it_end, IterTag_) {
        const size_type count = std::distance(it_beg,it_end);
        if( it_where == end() ) {
            append_(it_beg,it_end,typename std::iterator_traits<FwdIt_>::iterator_category());
        } else {
            make_room_(it_where,count);
            std::copy(it_beg,it_end,it_where);
        }
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    void
    static_vector<T,MAX_SIZE>::append_(size_type count, const value_type& val) {
        const size_type new_size = size() + count;
        std::uninitialized_fill_n(end(),count,val);
        size_ = new_size;
    }

    template<class T, std::size_t MAX_SIZE>
    template<class FwdIt_, class IterTag_>
    inline
    void
    static_vector<T,MAX_SIZE>::append_(FwdIt_ it_beg, FwdIt_ it_end, IterTag_) {
        const size_type new_size = size() + std::distance(it_beg,it_end);
        check_size_(new_size);
        std::uninitialized_copy(it_beg,it_end,end());
        size_ = new_size;
    }

    template<class T, std::size_t MAX_SIZE>
    template<class InpIt_>
    inline
    void
    static_vector<T,MAX_SIZE>::append_(InpIt_ it_beg, InpIt_ it_end, std::input_iterator_tag) {
        /* note:
        ** we can't use 'std::uninitialized_copy()' here, because we must not step
        ** behind it_end which we don't know in advance with input iterators
        */
        pointer start_raw = end();
        pointer current_raw = start_raw;
        try {
            for(;;) {
                new(current_raw++) value_type(*it_beg++);
                ++size;
                if( it_beg == it_end ) {
                    break;
                }
                if( current_raw == elems_+max_size() ) {
                    storage_exhausted_();
                }
            }
        } catch(...) {
            destroy_(start_raw,current_raw );
            throw;
        }
    }

    template<class T, std::size_t MAX_SIZE>
    inline
    void
    static_vector<T,MAX_SIZE>::swap_(static_vector& smaller, static_vector& bigger) {
        iterator beginB = bigger .begin();
        iterator endB   = bigger .end  ();
        beginB = std::swap_ranges(smaller.begin(),smaller.end(),beginB);
        smaller.append_(beginB,endB,typename std::iterator_traits<iterator>::iterator_category());
        bigger.destroy_(beginB,endB);
    }

/* @} */

}

////////////////////////////////////////////////////////////////////////////////

#endif //defined(STATIC_VECTOR_H)

////////////////////////////////////////////////////////////////////////////////
//EOF
