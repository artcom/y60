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

    for (int i = 0; i < myDepth; ++i) {
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

