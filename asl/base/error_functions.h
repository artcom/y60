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
// Description:  Cross Platform Error Code 
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
//    technical soundness    : ok
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : fair
//    dependencies           : ok
//    cheesyness             : fair
//
//    overall review status  : ok
//
//    recommendations:
//       - make a documentation
//       - make general use of this throughout the whole framework, at least throughout asl
//       - rename LAST_ERROR_TYPE to something style-guide conforming
//       - pur errorDescription() implementation into a .cpp file
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_error_functions_
#define _included_asl_error_functions_

#include "string_functions.h"
#include "Exception.h"

#include <errno.h>

#ifdef OSX
#include <Carbon/Carbon.h>
#endif

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
    if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
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

#ifdef OSX

DEFINE_EXCEPTION(OSXError, asl::Exception);

static void
checkOSXError(OSErr theErrorCode, const std::string & theWhere) {
    if (theErrorCode != noErr) {
        throw OSXError(std::string("An error occured. Error code: ") +
                       as_string(theErrorCode), theWhere);
    }
}

#endif

/* @} */

} // end of namespace asl
#endif
