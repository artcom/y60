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

#ifndef MACH_O_BACKTRACE_INCLUDED
#define MACH_O_BACKTRACE_INCLUDED

#include "TraceUtils.h"

#include <vector>
#include <string>

namespace asl {

/** Policy to create backtraces on systems using the MachO executable
 * format (MacOS X). It is used in conjunction with the StackTracer
 * template.
 * The code is adapted form the harmonia package (http://harmonia.cs.berkeley.edu)
 * in file <pre>common/backtrace.cc</pre>
 */
class MachOBacktrace {
    public:
        typedef StackFrameBase StackFrame;

        static void trace(std::vector<StackFrame> & theStack, int theMaxDepth);

    private:
        MachOBacktrace();
        static std::string getFunctionName( void * theReturnAddress);
};

}

#endif // MACH_O_BACKTRACE_INCLUDED
