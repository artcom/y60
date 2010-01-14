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

// own header
#include "StackTrace.h"

#include "Dashboard.h" // contains the class Table

using namespace std;

namespace asl {

template <class TRACE_POLICY>
StackTracer<TRACE_POLICY>::StackTracer() {
    TRACE_POLICY::trace(_myStack, MAX_DEPTH);
}

template <class TRACE_POLICY>
size_t
StackTracer<TRACE_POLICY>::size() const {
    return _myStack.size();
}

template <class TRACE_POLICY>
ostream &
StackTracer<TRACE_POLICY>::print(ostream & os) const {
	for (unsigned i = 0; i < _myStack.size(); ++i) {
        os << "#" << _myStack.size() - 1 - i << " " << beautify( _myStack[i].name ) << endl;
    }
	if( _myStack.size() == 0 ) {
		os << "no symbols found" << endl;
	}
    return os;
}

#ifdef LINUX
template class StackTracer<GlibcBacktrace>;
#elif defined( OSX )
template class StackTracer<MachOBacktrace>;
#elif defined( _WIN32 )
template class StackTracer<Win32Backtrace>;
#endif

} // end of namespace

