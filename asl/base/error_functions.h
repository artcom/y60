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
//    $RCSfile: error_functions.h,v $
//
//   $Revision: 1.3 $
//
// Description: file helper functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_asl_error_functions_
#define _included_asl_error_functions_

#include "string_functions.h"
#include <errno.h>

#ifdef WIN32
# include <windows.h>
#   define LAST_ERROR_TYPE DWORD
#else
#   define LAST_ERROR_TYPE int
#endif

namespace asl {

/*! \addtogroup aslbase */
/* @{ */

inline
LAST_ERROR_TYPE lastError() {
#ifdef WIN32
    return GetLastError();
#else
    return errno;
#endif
}

inline
void
setLastError(LAST_ERROR_TYPE theError) {
#ifdef WIN32
    SetLastError(theError);
#else
    errno = theError;
#endif
}

inline
std::string errorDescription(LAST_ERROR_TYPE err) {
#ifdef WIN32
    LPVOID lpMsgBuf;
    if (!FormatMessage(
                       FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       err,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                       (LPTSTR) &lpMsgBuf,
                       0,
                       NULL ))
	{
            // Handle the error.
            return std::string("unknown error code=")+asl::as_string(err);
	}
    std::string myResult = static_cast<const char*>(lpMsgBuf);
    LocalFree( lpMsgBuf );
    return myResult;
#else
    return strerror(err);
#endif
}

/* @} */

} // end of namespace asl
#endif
