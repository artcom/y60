//==============================================================================
//
// Copyright (C) 2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

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
