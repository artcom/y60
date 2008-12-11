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
// Description:  UTF-8 to locale encoding conversion class 
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ASL_PATH_INCLUDED_
#define _ASL_PATH_INCLUDED_

#include "string_functions.h"

#include <iostream>
#include <string>

#ifdef OSX
    #include <Carbon/Carbon.h>
#endif

/*! \addtogroup aslbase */
/* @{ */

namespace asl {

class Path {
public:
    Path();
    Path(const char * theString, StringEncoding theEncoding);
    Path(const std::string & theString, StringEncoding theEncoding);
    Path(const Path &);
    virtual ~Path();
    Path & operator=(const Path& theOther);
    bool operator==(const Path& theOther) const;
    bool empty() const;

    void assign(const char * theString, StringEncoding theEncoding);
    std::string toLocale() const;
    std::string toUTF8() const;
private:
#ifdef OSX
	CFStringRef _myString;
#else
    char * _myLocaleChars;
#endif
    void free();
};

std::ostream & operator << (std::ostream & os, const asl::Path & thePath);

}

/* @} */


#endif
