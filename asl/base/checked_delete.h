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
// Description:  Memory Block handling functions; should be used whenever
//               using memcpy is considered; features
//               - typesafe conversion,
//		         - compile-time memory read-write control, 
//               - convenient windows api interfaces and
//               - also a copy-on-write (COW) implementation
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : fair
//    formatting             : poor
//    documentation          : disaster
//    test coverage          : ok
//    names                  : ok
//    style guide conformance: disaster
//    technical soundness    : ok
//    dead code              : ok
//    readability            : ok
//    understandabilty       : poor
//    interfaces             : ok
//    confidence             : ok
//    integration            : poor
//    dependencies           : ok
//    cheesyness             : fair
//
//    overall review status  : poor
//
//    recommendation:
//       - review & complete &documentation
//       - remove dead code
//       - explain usage
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef BOOST_CHECKED_DELETE_HPP_INCLUDED
#define BOOST_CHECKED_DELETE_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

//
//  boost/checked_delete.hpp
//
//  Copyright (c) 1999, 2000, 2001, 2002 boost.org
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//

namespace asl
{


/*! \addtogroup aslbase */
/* @{ */
    
// verify that types are complete for increased safety

template< typename T > inline void checked_delete(T * x)
{
    typedef char type_must_be_complete[sizeof(T)];

    //AC_TRACE << "checked_delete(x) called for ptr " << x << endl;
    delete x;
}

template< typename T > inline void checked_array_delete(T * x)
{
    typedef char type_must_be_complete[sizeof(T)];
    delete [] x;
}

template<class T> struct checked_deleter
{
    typedef void result_type;
    typedef T * argument_type;

    void operator()(T * x)
    {
        checked_delete(x);
    }
};

// checked_deleter<void> is needed by shared_ptr<void>::reset(0)

template<> struct checked_deleter<void>
{
    typedef void result_type;
    typedef void * argument_type;

    void operator()(void * x)
    {
        ::operator delete(x); // avoid g++ warning
    }
};

template<class T> struct checked_array_deleter
{
    typedef void result_type;
    typedef T * argument_type;

    void operator()(T * x)
    {
        checked_array_delete(x);
    }
};

/* @} */

} // namespace boost

#endif  // #ifndef BOOST_CHECKED_DELETE_HPP_INCLUDED
