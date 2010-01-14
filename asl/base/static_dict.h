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
// Description: static_dict STL container
//              a std::string-to-T static size const dictionary
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

////////////////////////////////////////////////////////////////////////////////

#if !defined(STATIC_DICT_H)
#define STATIC_DICT_H

////////////////////////////////////////////////////////////////////////////////

#include "asl_base_settings.h"

#include <iterator>
#include <string>
#include <functional>

////////////////////////////////////////////////////////////////////////////////
#include "static_vector.h"

////////////////////////////////////////////////////////////////////////////////

namespace asl {

    /*! static dictionary entry type
    **
    ** use the result_type to crate C arrays to initialize the corresponding
    ** static_dict objects with
    */
    template< typename T >
    struct static_dict_entry_type {
        typedef std::string                                 key_type;
        typedef T                                           mapped_type;
        typedef std::pair< key_type, T >                    result_type;
    };

    /*! std::string-to-T static size const dictionary
    **
    ** initialize with C array of static_dict_entry_type<T>::result_type
    ** use lookup to find the mapped_type object corresponding to some
    ** std::string
    */
    template< typename T, std::size_t MaxSize_ >
    class static_dict {
    public:
        // types similar to std::map, note: no non-const iterators
        typedef typename static_dict_entry_type<T>::key_type
                                                            key_type;
        typedef typename static_dict_entry_type<T>::mapped_type
                                                            mapped_type;
        typedef typename static_dict_entry_type<T>::result_type
                                                            value_type;

        typedef static_vector<value_type,MaxSize_>          container_type;

        typedef typename container_type::pointer            pointer;
        typedef typename container_type::const_pointer      const_pointer;

        typedef typename container_type::reference          reference;
        typedef typename container_type::const_reference    const_reference;

        typedef typename container_type::size_type          size_type;
        typedef typename container_type::difference_type    difference_type;

        typedef typename container_type::const_iterator     const_iterator;
        typedef typename container_type::const_reverse_iterator
                                                            const_reverse_iterator;

        /*! constructor
        **
        ** initialize with C array of static_dict_entry_type<T>::result_type
        **
        ** note  This will throw std::invalid_argument exceptions on duplicate
        **       key objects
        **
        ** note  This will copy the objects (O(N)), then sort the stored
        **       entries (O(N)) and finally attempts to find duplicate
        **       entries (O(N)).
        */
        template< typename InpIt >
        static_dict(InpIt begin, InpIt end);

        // const iterators, note: no non-const iterators
        const_iterator
        begin() const {
            return container_.begin();
        }

        const_iterator
        end  () const {
            return container_.end  ();
        }

        const_reverse_iterator
        rbegin() const {
            return const_reverse_iterator(end  ());
        }

        const_reverse_iterator
        rend  () const {
            return const_reverse_iterator(begin());
        }

        // size
        static const std::size_t      static_max_size     = container_type::static_max_size;

        size_type
        max_size() const {
            return container_.max_size();
        }

        size_type
        size() const {
            return container_.size();
        }

        bool
        empty() const {
            return container_.empty();
        }

        // element access

        /*! lookup strings, finding matching mapped_type object
        **
        ** Given an arbitrary string, this will attempt to find an entry with
        ** the longest key that matches the beginning of the string.
        ** If such a key is found, the first element of the result will be an
        ** iterator pointing to that entry. The second element will be the
        ** size of that key.
        ** Otherwise, the first element of the result will be the dictionary's
        ** end iterator. The second element will be the maximum number of
        ** characters in the given string which matched the beginning of any
        ** key.
        */
        std::pair<const_iterator,typename key_type::size_type>
        lookup(const key_type& k, typename key_type::size_type idx = 0) const;

        /*! lookup strings by character iterators, finding matching
        **  mapped_type object
        **
        ** Given iterators into an arbitrary character sequence, this will
        ** attempt to find an entry with the longest key that matches the
        ** beginning of the string.
        ** If such a key is found, the first element of the result will be an
        ** iterator pointing to that entry. The second element will point at
        ** the first non-matching character of that key.
        ** Otherwise, the first element of the result will be the dictionary's
        ** end iterator. The second element will be the maximum number of
        ** characters in the given sequence which matched the beginning of any
        ** key.
        */
        template< typename FwdIt >
        std::pair<const_iterator,FwdIt>
        lookup(FwdIt s_b, FwdIt s_e) const;

        /*! reverse lookup, finding matching strings
        **
        ** Given a mapped_type object, this will attempt to find an entry
        ** with exactly this object.
        ** If such an entry is found, the result is an iterator pointing to
        ** it. Otherwise the result is the end iterator.
        */
        const_iterator
        rlookup(const mapped_type& obj) const;

    private:
        class key_compare : public std::binary_function<value_type,value_type,bool> {
        public:
            bool operator()(const value_type& lhs, const value_type& rhs) const {
                return std::less<key_type>()(lhs.first,rhs.first);
            }
        };

        class key_predicate : public std::unary_function<value_type,bool> {
        public:
            key_predicate(const key_type& k)
                : k_(k) {
            }
            bool operator()(const value_type& v) const {
                return k_==v;
            }
            const key_type& k_;
        };

        container_type                      container_;

        template< typename FwdItStr >
        static
        int
        compare_at_index_( typename key_type::const_iterator
                         , typename key_type::const_iterator
                         , FwdItStr&, FwdItStr );

        template< typename FwdItStr >
        static
        std::pair<const_iterator,FwdItStr>
        lookup_( FwdItStr str_begin, FwdItStr str_end
               , const_iterator dict_begin, const_iterator dict_end ) {
            return lookup_(str_begin,str_end,dict_begin,dict_end,dict_end);
        }

        template< typename FwdItStr >
        static
        std::pair<const_iterator,FwdItStr>
        lookup_( FwdItStr, FwdItStr , const_iterator, const_iterator, const_iterator );
    };

    // implementation

    template< typename T, std::size_t MaxSize_ >
    template< typename InpIt >
    inline
    static_dict<T,MaxSize_>::static_dict(InpIt begin, InpIt end)
        : container_(begin,end)
    {
        std::sort(container_.begin(),container_.end(),key_compare());
        if( container_.end() != std::adjacent_find(container_.begin(),container_.end()) ) {
            throw std::invalid_argument("duplicate keys in static_dict");
        }
    }

    template< typename T, std::size_t MaxSize_ >
    inline
    std::pair< typename static_dict<T,MaxSize_>::const_iterator
             , typename static_dict<T,MaxSize_>::key_type::size_type >
    static_dict<T,MaxSize_>::lookup(const key_type& key, typename key_type::size_type idx) const

    {
        typedef std::pair<const_iterator,typename key_type::const_iterator> lookup_result_t;
        typedef std::pair<const_iterator,typename key_type::size_type>      result_t;

        const typename key_type::const_iterator key_begin = key.begin()+idx;

        const lookup_result_t& res = lookup_( key_begin, key.end(), begin(), end() );

        const typename key_type::size_type new_idx = res.second-key.begin();

        return result_t( res.first, new_idx );

    }

    template< typename T, std::size_t MaxSize_ >
    template< typename FwdIt >
    inline
    std::pair< typename static_dict<T,MaxSize_>::const_iterator, FwdIt >
    static_dict<T,MaxSize_>::lookup(FwdIt str_begin, FwdIt str_end) const
    {
        return lookup_( str_begin, str_end, begin(), end() );
    }

    template< typename T, std::size_t MaxSize_ >
    typename static_dict<T,MaxSize_>::const_iterator
    static_dict<T,MaxSize_>::rlookup(const mapped_type& obj) const
    {
        for( const_iterator it = begin(); it != end(); ++it ) {
            if( it->second == obj ) {
                return it;
            }
        }
        return end();
    }

    template< typename T, std::size_t MaxSize_ >
    template< typename FwdItStr >
    inline
    int static_dict<T,MaxSize_>::compare_at_index_( typename key_type::const_iterator key_begin
                                                  , typename key_type::const_iterator key_end
                                                  , FwdItStr& str_begin, FwdItStr str_end )
    {
        if( str_begin == str_end )
            return 0;
        if( key_begin == key_end )
            return 0;
        if( *key_begin < *str_begin )
            return -1;
        if( *key_begin > *str_begin )
            return +1;
        return compare_at_index_(++key_begin,key_end,++str_begin,str_end);
    }

    template< typename T, std::size_t MaxSize_ >
    template< typename FwdItStr >
    inline
    std::pair< typename static_dict<T,MaxSize_>::const_iterator, FwdItStr >
    static_dict<T,MaxSize_>::lookup_( FwdItStr str_begin, FwdItStr str_end
                                    , const_iterator dict_begin, const_iterator dict_end
                                    , const_iterator dict_failure )
    {
        typedef std::pair<const_iterator,FwdItStr>    result_t;

        result_t result(dict_failure,str_begin);
        for( const_iterator dict_it=dict_begin; dict_it!=dict_end; ++dict_it ) {
            const std::string& key = dict_it->first;
            std::string::const_iterator key_it=key.begin();
            std::string::const_iterator str_it=str_begin;
            while( key_it!=key.end() && str_it!=str_end && *key_it==*str_it ) {
                ++key_it;
                ++str_it;
            }

            result_t temp;
            if( key_it == key.end() ) {
                temp.first  = dict_it;
            } else {
                temp.first = dict_failure;
            }
            temp.second = str_it;
            if( temp.second>result.second ) {
                result = temp;
            }
            if( str_it == str_end ) {
                break;
            }
        }
        return result;
    }

}

////////////////////////////////////////////////////////////////////////////////

#endif //defined(STATIC_DICT_H)

////////////////////////////////////////////////////////////////////////////////
//EOF
