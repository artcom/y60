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

#ifndef WIN32_BACKTRACE_INCLUDED
#define WIN32_BACKTRACE_INCLUDED

#include "TraceUtils.h"

#include <vector>
#include <string>

namespace asl {

/** Policy to create backtraces on Win32 systems
 * It is used in conjunction with the StackTracer 
 * template.
 */
class Win32Backtrace {
    public:
        typedef StackFrameBase StackFrame;

        static void trace(std::vector<StackFrame> & theStack, int theMaxDepth);

    private:
        Win32Backtrace();

};

}

#endif // WIN32_BACKTRACE_INCLUDED
