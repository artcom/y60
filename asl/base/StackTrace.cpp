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
        os << "#" << i << " " << beautify( _myStack[i].name ) << endl;
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
#elif defined( WIN32 )
template class StackTracer<Win32Backtrace>;
#endif

} // end of namespace 

