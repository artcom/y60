/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2004, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: begin_end.h,v $
//
//   $Revision: 1.2 $
//
// Description: helper for generic programming
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
// originally written by Pavel - Tue Jun 17 19:18:17 CEST 1997
// major refactoring during spare time 2003/2004
//
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
