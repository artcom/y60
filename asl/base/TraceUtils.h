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
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef TRACE_UTILS_INCLUDED
#define TRACE_UTILS_INCLUDED

#include "asl_base_settings.h"

#include <string>
#include <typeinfo>

#ifdef __GNUC__
#   include <cxxabi.h>
#endif

struct StackFrameBase {
    ptrdiff_t frame;
    std::string name;
};

std::string beautify(const std::string & theSymbol);

// TODO: put the rest of this in namespace asl
namespace asl {

template <typename T>
std::string
demangled_name() {
#   ifdef __GNUC__
        const size_t name_buffer_size = 1024;
        size_t size = name_buffer_size;
        char buffer[name_buffer_size];
        int status = 0;
        abi::__cxa_demangle(typeid(T).name(), buffer, & size, & status);
        if ( ! status ) {
            return std::string( buffer );
        }
        return std::string( typeid(T).name() );
#   else
        //TODO: MSVC demangleing ... code should be in the win32 stack tracer
        return std::string(typeid(T).name());
#   endif
}

} // end of namespace asl

#endif // TRACE_UTILS_INCLUDED

