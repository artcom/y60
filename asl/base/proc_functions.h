/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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

