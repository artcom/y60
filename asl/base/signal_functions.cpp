#include "signal_functions.h"

#include "Exception.h"
#include "StackTrace.h"
#include "string_functions.h"

#include <signal.h>

using namespace std;

namespace asl {

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

