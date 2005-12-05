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
// Description:  misc operating system & environment helper functions
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : ok
//    formatting             : ok
//    documentation          : poor (not yet doxygenized)
//    test coverage          : poor
//    names                  : poor
//    style guide conformance: poor
//    technical soundness    : ok
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : poor
//
//    recommendations:
//       - doxygenize documentation
//       - add missing tests
//       - unify names with styleguide in mind
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_os_functions_
#define _included_asl_os_functions_


#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef LINUX
    #include <unistd.h>
#endif
#ifdef WIN32
	#include <windows.h>
#endif

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */

    // The following three functions may throw an exception when they are called
    // for the first time; on subsequent calls just a stored is returned

    // returns the hostname
    // @warning Returns only the NetBIOS name under Win9x,
    //          all other Windows versions return the fully qualified DNS name.
    const std::string & hostname();

    // returns the name of the application without path and possibly truncated
    // to a handsome length
    const std::string & appname();

    // return a concatenation of hostname and appname separated by a colon (':')
    const std::string & hostappid();

    // returns a string identifying login session
    std::string getSessionId();

    unsigned long getThreadId();

    void precision_sleep(double theSeconds);

    // environment variable handling

    // expands embedded ${ENVVAR} with their values
    std::string expandEnvironment(const std::string & theString);
    bool get_environment_var(const std::string & theVariable, std::string & theValue);
    void set_environment_var(const std::string & theVariable, const std::string & theValue);

#ifdef WIN32
		bool hResultIsOk(HRESULT hr, std::string & theMessage);
#endif		
    /* @} */

} //Namespace asl

#endif

