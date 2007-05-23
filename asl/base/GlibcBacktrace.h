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

#ifndef GLIBC_BACKTRACE_INCLUDED
#define GLIBC_BACKTRACE_INCLUDED

#include "TraceUtils.h"

#include <vector>
#include <string>

namespace asl {

/** Policy to create backtraces on systems using GNU libc
 * It is used in conjunction with the StackTracer 
 * template.
 * The code is adapted form the harmonia package (http://harmonia.cs.berkeley.edu)
 * in file <pre>common/backtrace.cc</pre>
 */
class GlibcBacktrace {
    public:
        struct GlibcStackFrame : public StackFrameBase {
            std::string file;
            unsigned    line;
        };
        typedef GlibcStackFrame StackFrame;

        static void trace(std::vector<StackFrame> & theStack, int theMaxDepth);

    private:
        GlibcBacktrace();

};

}

#endif // GLIBC_BACKTRACE_INCLUDED
