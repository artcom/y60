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
//       - rename to "shell_functions.h/cpp"
//       - doxygenize documentation
//       - add missing tests
//       - unify names with styleguide in mind
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_os_functions_
#define _included_asl_os_functions_

#include "asl_base_settings.h"

#include "string_functions.h"

#include <typeinfo>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef LINUX
    #include <unistd.h>
#endif
#ifdef _WIN32
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
    ASL_BASE_EXPORT const std::string & hostname();

    // returns the name of the application without path and possibly truncated
    // to a handsome length
    ASL_BASE_EXPORT const std::string & appname();

    // return a concatenation of hostname and appname separated by a colon (':')
    ASL_BASE_EXPORT const std::string & hostappid();

    // returns a string identifying login session
    std::string getSessionId();

    ASL_BASE_EXPORT unsigned long getThreadId();

    void precision_sleep(double theSeconds);

    // environment variable handling

    // expands embedded ${ENVVAR} with their values
    ASL_BASE_EXPORT std::string expandEnvironment(const std::string & theString);
    ASL_BASE_EXPORT bool get_environment_var(const std::string & theVariable, std::string & theValue);
    ASL_BASE_EXPORT void set_environment_var(const std::string & theVariable, const std::string & theValue);

    template <class T>
    bool get_environment_var_as(const std::string & theVariable, T& theValue);
    
    template <class T>
    T getenv(const std::string & theVariable, const T & theDefaultValue) {
        T myResult = theDefaultValue;
        try {
            get_environment_var_as(theVariable, myResult);
        }
        catch (asl::ParseException & /*ex*/) {
            throw asl::ParseException(std::string("Could not convert value of environment variable '"+theVariable+"' to type "+
                                      typeid(myResult).name()), PLUS_FILE_LINE); 
        }
        return myResult;
    }

#ifdef _WIN32
		bool hResultIsOk(HRESULT hr, std::string & theMessage);
#endif		
    /* @} */

} //Namespace asl

#endif

