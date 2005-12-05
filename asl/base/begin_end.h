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
    T * begin(T & x) {
        return &x;
    }

    template <class T>
    T * end(T & x) {
        return &x+1;
    }

    template <class T>
    const T * begin(const T & x) {
        return &x;
    }

    template <class T>
    const T * end(const T & x) {
        return &x+1;
    }

    template <class ELEM, template <class> class CONTAINER>
    typename CONTAINER<ELEM>::iterator begin(CONTAINER<ELEM> & c) {
        return c.begin();
    }

    template <class ELEM, template <class> class CONTAINER>
    typename CONTAINER<ELEM>::iterator end(CONTAINER<ELEM> & c) {
        return c.end();
    }

    template <class ELEM, template <class> class CONTAINER>
    typename CONTAINER<ELEM>::const_iterator begin(const CONTAINER<ELEM> & c) {
        return c.begin();
    }

    template <class ELEM, template <class> class CONTAINER>
    typename CONTAINER<ELEM>::const_iterator end(const CONTAINER<ELEM> & c) {
        return c.end();
    }
    /* @} */
} // namespace asl
#endif
