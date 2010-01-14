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
// Description:  Helper functions for generic programming
//
// Last Review: pavel 30.11.2005
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : ok
//    formatting             : ok
//    documentation          : poor
//    test coverage          : poor
//    names                  : fair
//    style guide conformance: ok
//    technical soundness    : fair
//    dead code              : ok
//    readability            : fair
//    understandabilty       : fair
//    interfaces             : fair
//    confidence             : fair
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : fair
//
//    overall review status  : fair
//
//    recommendation:
//       - check if really necessary
//       - very short file, potentially merge with other helpers
//       - improve documentation
//       - improve test coverage
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ASL_BEGIN_END_H_INCLUDED_
#define _ASL_BEGIN_END_H_INCLUDED_

#include "asl_base_settings.h"

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */

    template <class T> inline T * begin(T & x) { return &x; }
    template <class T> inline T * end  (T & x) { return &x+1; }

    template <class T> inline const T * begin(const T & x) { return &x; }
    template <class T> inline const T * end  (const T & x) { return &x+1; }

    // iterators into containers with one template argument
    template <class ELEM, template <class> class CONTAINER>
    inline typename CONTAINER<ELEM>::iterator begin(CONTAINER<ELEM> & c) { return c.begin(); }
    template <class ELEM, template <class> class CONTAINER>
    inline typename CONTAINER<ELEM>::iterator end  (CONTAINER<ELEM> & c) { return c.end(); }

    template <class ELEM, template <class> class CONTAINER>
    inline typename CONTAINER<ELEM>::const_iterator begin(const CONTAINER<ELEM> & c) { return c.begin(); }
    template <class ELEM, template <class> class CONTAINER>
    inline typename CONTAINER<ELEM>::const_iterator end  (const CONTAINER<ELEM> & c) { return c.end(); }

#if !defined(_SETTING_GCC_TEMPLATE_MATCHING_BUG_WORKAROUND_)
    template <class ELEM, template <class> class CONTAINER>
    inline ELEM* begin_ptr(CONTAINER<ELEM> & c) { return c.empty() ? NULL : &*c.begin(); }
    template <class ELEM, template <class> class CONTAINER>
    inline ELEM* end_ptr  (CONTAINER<ELEM> & c) { return c.empty() ? NULL : &*c.end(); }

    template <class ELEM, template <class> class CONTAINER>
    inline const ELEM* begin_ptr(const CONTAINER<ELEM> & c) { return c.empty() ? NULL : &*c.begin(); }
    template <class ELEM, template <class> class CONTAINER>
    inline const ELEM* end_ptr  (const CONTAINER<ELEM> & c) { return c.empty() ? NULL : &*c.end(); }
#endif //!defined(_SETTING_GCC_TEMPLATE_MATCHING_BUG_WORKAROUND_)

    // iterators into containers with two template arguments
    template <class ELEM, class A, template <class,class> class CONTAINER>
    inline typename CONTAINER<ELEM,A>::iterator begin(CONTAINER<ELEM,A> & c) { return c.begin(); }
    template <class ELEM, class A, template <class,class> class CONTAINER>
    inline typename CONTAINER<ELEM,A>::iterator end  (CONTAINER<ELEM,A> & c) { return c.end(); }

    template <class ELEM, class A, template <class,class> class CONTAINER>
    inline typename CONTAINER<ELEM,A>::const_iterator begin(const CONTAINER<ELEM,A> & c) { return c.begin(); }
    template <class ELEM, class A, template <class,class> class CONTAINER>
    inline typename CONTAINER<ELEM,A>::const_iterator end  (const CONTAINER<ELEM,A> & c) { return c.end(); }

#if !defined(_SETTING_GCC_TEMPLATE_MATCHING_BUG_WORKAROUND_)
    template <class ELEM, class A, template <class,class> class CONTAINER>
    inline ELEM* begin_ptr(CONTAINER<ELEM,A> & c) { return c.empty() ? NULL : &*c.begin(); }
    template <class ELEM, class A, template <class,class> class CONTAINER>
    inline ELEM* end_ptr  (CONTAINER<ELEM,A> & c) { return c.empty() ? NULL : &*c.end(); }

    template <class ELEM, class A, template <class,class> class CONTAINER>
    inline const ELEM* begin_ptr(const CONTAINER<ELEM,A> & c) { return c.empty() ? NULL : &*c.begin(); }
    template <class ELEM, class A, template <class,class> class CONTAINER>
    inline const ELEM* end_ptr  (const CONTAINER<ELEM,A> & c) { return c.empty() ? NULL : &*c.end(); }
#endif //!defined(_SETTING_GCC_TEMPLATE_MATCHING_BUG_WORKAROUND_)

    // iterators into containers with three template arguments
    template <class ELEM, class T, class A, template <class,class,class> class CONTAINER>
    inline typename CONTAINER<ELEM,T,A>::iterator begin(CONTAINER<ELEM,T,A> & c) { return c.begin(); }
    template <class ELEM, class T, class A, template <class,class,class> class CONTAINER>
    inline typename CONTAINER<ELEM,T,A>::iterator end  (CONTAINER<ELEM,T,A> & c) { return c.end(); }

    template <class ELEM, class T, class A, template <class,class,class> class CONTAINER>
    inline typename CONTAINER<ELEM,T,A>::const_iterator begin(const CONTAINER<ELEM,T,A> & c) { return c.begin(); }
    template <class ELEM, class T, class A, template <class,class,class> class CONTAINER>
    inline typename CONTAINER<ELEM,T,A>::const_iterator end  (const CONTAINER<ELEM,T,A> & c) { return c.end(); }

#if !defined(_SETTING_GCC_TEMPLATE_MATCHING_BUG_WORKAROUND_)
    template <class ELEM, class T, class A, template <class,class,class> class CONTAINER>
    inline ELEM* begin_ptr(CONTAINER<ELEM,T,A> & c) { return c.empty() ? NULL : &*c.begin(); }
    template <class ELEM, class T, class A, template <class,class,class> class CONTAINER>
    inline ELEM* end_ptr  (CONTAINER<ELEM,T,A> & c) { return c.empty() ? NULL : &*c.end(); }

    template <class ELEM, class T, class A, template <class,class,class> class CONTAINER>
    inline const ELEM* begin_ptr(const CONTAINER<ELEM,T,A> & c) { return c.empty() ? NULL : &*c.begin(); }
    template <class ELEM, class T, class A, template <class,class,class> class CONTAINER>
    inline const ELEM* end_ptr  (const CONTAINER<ELEM,T,A> & c) { return c.empty() ? NULL : &*c.end(); }
#endif //!defined(_SETTING_GCC_TEMPLATE_MATCHING_BUG_WORKAROUND_)

#if defined(_SETTING_GCC_TEMPLATE_MATCHING_BUG_WORKAROUND_)
    template <class CONTAINER>
    inline typename CONTAINER::value_type* begin_ptr(CONTAINER & c) { return c.empty() ? NULL : &*c.begin(); }
    template <class CONTAINER>
    inline typename CONTAINER::value_type* end_ptr  (CONTAINER & c) { return c.empty() ? NULL : &*c.end(); }

    template <class CONTAINER>
    inline const typename CONTAINER::value_type* begin_ptr(const CONTAINER & c) { return c.empty() ? NULL : &*c.begin(); }
    template <class CONTAINER>
    inline const typename CONTAINER::value_type* end_ptr  (const CONTAINER & c) { return c.empty() ? NULL : &*c.end(); }
#endif //defined(_SETTING_GCC_TEMPLATE_MATCHING_BUG_WORKAROUND_)

    // iterators into C arrays
    template <typename T, const std::size_t N> inline T* begin(T (&array)[N]) { return array; }
    template <typename T, const std::size_t N> inline T* end  (T (&array)[N]) { return array+N; }

    // iterators into const C arrays
    template <typename T, const std::size_t N> inline const T* begin(const T (&array)[N]) { return array; }
    template <typename T, const std::size_t N> inline const T* end  (const T (&array)[N]) { return array+N; }

    //! size of C array
    template <typename T, const std::size_t N>
    inline std::size_t size(T const (&)[N]) {
        return N;
    }

    namespace detail { // disregard
        template <typename T, const std::size_t N>
        const char ( &static_array_size_(T const (&)[N]) ) [N];
    }

    //! size of C array, yields a compile-time const
#   define asl_static_array_size(a)  sizeof(::asl::detail::static_array_size_(a))

    /* @} */

} // namespace asl
#endif
