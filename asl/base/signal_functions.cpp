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
#include "signal_functions.h"

#include <cstring>

#include "Exception.h"
#include "StackTrace.h"
#include "string_functions.h"

#include <signal.h>

using namespace std;

namespace asl {

#ifdef OSX
typedef sig_t sighandler_t;
#endif

void traceAndReraiseSignal(int theSignal);

void initSignalHandling() {
    traceSignal( SIGINT );  // Ctrl-C (do we want this or is it to noisy?) [DS]
    traceSignal( SIGABRT );
    traceSignal( SIGTERM );
    traceSignal( SIGSEGV );
    traceSignal( SIGBUS );
}

void traceSignal( int theSignal ) {
    sighandler_t myReturnCode( 0 );
    myReturnCode = signal( theSignal, & traceAndReraiseSignal );
    if (myReturnCode == SIG_ERR) {
        throw asl::Exception(string("Failed to install signal handler for signal ") +
            as_string( theSignal ) + " (" + strsignal( theSignal ) + ")", PLUS_FILE_LINE);
    }
}

void traceAndReraiseSignal(int theSignal) {
    // Only a few functions are guaranteed to work properly in signal handlers.
    // See signal (2) manpage for a full list. Not on the list are: 
    //    - the stack tracing code used by StackTrace
    //    - C++ iostreams
    //    - strsignal
    // Calling signal() and raise() is safe. [DS]
    StackTrace myStack;
    cerr << "Caught signal " << theSignal << " (" << strsignal( theSignal )
         << ") at: " << endl
         << myStack;
    signal( theSignal, SIG_DFL );
    raise( theSignal );
}

} // end of namespace asl

