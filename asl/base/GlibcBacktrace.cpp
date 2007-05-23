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

#include "GlibcBacktrace.h"
#include "TraceUtils.h"

#include <execinfo.h>
#include <iostream>
#include <sstream>

using namespace std;

namespace asl {

void 
GlibcBacktrace::trace(std::vector<StackFrame> & theStack, int theMaxDepth) {
    void * myBuffer[theMaxDepth];
    int myDepth = backtrace(myBuffer, theMaxDepth);
    if (myDepth < 0) {
        cerr << "### ERROR: Failed to get backtrace" << endl;
        return;
    }

    char ** mySymbols = backtrace_symbols(myBuffer, myDepth);

    for (int i = myDepth - 1; i >= 0; --i) {
        StackFrame myItem;
        myItem.frame = (ptrdiff_t)myBuffer[i];
        string mySymbol( mySymbols[i] );
        string::size_type myPos1;
        string::size_type myPos2;
        myPos1 = mySymbol.rfind("_Z");
        myPos2 = mySymbol.rfind('+');
        if (myPos2 == string::npos) {
            myPos2 = mySymbol.rfind(')');
        }
        
        if (myPos1 != string::npos) {
            myItem.name = mySymbol.substr( myPos1, myPos2 - myPos1);
            // demangle ...
        } else {
            myPos1 = mySymbol.rfind('(');
            if (myPos1 != string::npos) {
                myItem.name = mySymbol.substr(myPos1 + 1, myPos2 - myPos1 - 1);
            } else {
                myPos2 = mySymbol.rfind('[');
                myItem.name = mySymbol.substr(0, myPos2 - 1);
            }
        }
        theStack.push_back(myItem);
    }
}

}

