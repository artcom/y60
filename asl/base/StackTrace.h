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

#ifndef STACK_TRACE_INCLUDED
#define STACK_TRACE_INCLUDED

#ifdef LINUX
#include "GlibcBacktrace.h"
#elif defined( OSX )
#include "MachOBacktrace.h"
#elif defined( WIN32 )
#include "Win32Backtrace.h"
#endif

#include <iostream>
#include <vector>

namespace asl {

/** Creates a stack trace at the point of construction. This class
 * should not be used directly. Use the platform independent typedef
 * StackTrace instead.
 */
template <class TRACE_POLICY>
class StackTracer {
    public:
        StackTracer();

        std::ostream & print(std::ostream & os) const;

        enum MaxDepth {
            MAX_DEPTH = 64
        };

        size_t size() const;
    private:
        typedef typename TRACE_POLICY::StackFrame StackFrame;
        std::vector<StackFrame> _myStack;
};

template <class TRACE_POLICY>
std::ostream& operator<<(std::ostream& os,const StackTracer<TRACE_POLICY> & theStack) {
    theStack.print( os );
    return os;
}

#ifdef LINUX
typedef StackTracer<GlibcBacktrace> StackTrace;
#elif defined( OSX )
typedef StackTracer<MachOBacktrace> StackTrace;
#elif defined( WIN32 )
typedef StackTracer<Win32Backtrace> StackTrace;
#endif

}

#endif // STACK_TRACE_INCLUDED
