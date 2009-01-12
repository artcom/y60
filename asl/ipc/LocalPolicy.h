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
// Description: 
//     Classes for networked or local communication between processes
//
// Last Review:  ms 2007-08-15
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      ok
//    documentation           :      ok
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
//    interfaces              :      ok
//    confidence              :      ok
//    integration             :      ok
//    dependencies            :      ok
//    error handling          :      ok
//    logging                 :      notapp
//    cheesyness              :      ok
//
//    overall review status   :      ok
//
//    recommendations:
*/
#ifndef __asl_LocalPolicy_included
#define __asl_LocalPolicy_included

#include "asl_ipc_settings.h"

#ifdef WIN32

#include "NamedPipePolicy.h"
namespace asl {
/*! \addtogroup ipcPolicies */
/* @{ */
    typedef NamedPipePolicy LocalPolicy;
/* @} */
}

#else

#include "UnixSocketPolicy.h"
namespace asl {
/*! \addtogroup ipcPolicies */
/* @{ */
    /// Platform abstraction: This typedefed to UnixSocketPolicy (POSIX) or NamedPipePolicy (Win32).
    typedef UnixSocketPolicy LocalPolicy;
/* @} */
}

#endif

#endif
