/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */
    
    template <class T>
    inline T * begin(T & x) {
        return &x;
    }

    template <class T>
    inline T * end(T & x) {
        return &x+1;
    }

    template <class T>
    inline const T * begin(const T & x) {
        return &x;
    }

    template <class T>
    inline const T * end(const T & x) {
        return &x+1;
    }

    template <class ELEM, template <class> class CONTAINER>
    inline typename CONTAINER<ELEM>::iterator begin(CONTAINER<ELEM> & c) {
        return c.begin();
    }

    template <class ELEM, template <class> class CONTAINER>
    inline typename CONTAINER<ELEM>::iterator end(CONTAINER<ELEM> & c) {
        return c.end();
    }

    template <class ELEM, template <class> class CONTAINER>
    inline typename CONTAINER<ELEM>::const_iterator begin(const CONTAINER<ELEM> & c) {
        return c.begin();
    }

    template <class ELEM, template <class> class CONTAINER>
    inline typename CONTAINER<ELEM>::const_iterator end(const CONTAINER<ELEM> & c) {
        return c.end();
    }
    //! begin iterator into C array
    template <typename T, const std::size_t N>
    inline T* begin(T (&array)[N]) {
        return array;
    }

    //! end iterator into C array
    template <typename T, const std::size_t N>
    inline T* end(T (&array)[N]) {
        return array+N;
    }

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
