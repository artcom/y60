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
// Description: process & system memory related functions
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _asl_proc_functions_
#define _asl_proc_functions_

#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#endif

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */
 
    /// Get amount of system memory in KB.
    unsigned getTotalMemory();

    /// Get amount of used system memory in KB.
    unsigned getUsedMemory();

    /// Get amount of free system memory in KB.
    unsigned getFreeMemory();

    /**
     * Get process memory usage in KB.
     * @param thePid process ID, default is to get for own process.
     */
#ifdef WIN32
    typedef DWORD ProcessID;
#else
    typedef pid_t ProcessID;
#endif
    unsigned getProcessMemoryUsage(ProcessID thePid = 0);

    /* @} */
}

#endif

