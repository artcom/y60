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

#include "TraceUtils.h"

#include <string>
#include <cxxabi.h>

using namespace std;

#ifndef WIN32

// demangling for gcc
std::string 
beautify(const std::string & theSymbol) {
    string::size_type myEndPos = theSymbol.find(':');
    if (myEndPos == string::npos) {
        myEndPos = theSymbol.size();
    }
    string::size_type myStartPos = 0;
    // XXX
    if (theSymbol[0] == '_' && theSymbol[1] == '_') {
        myStartPos = 1;
    }
    std::string myMangledSymbol = theSymbol.substr(myStartPos, myEndPos);
    int myStatus;
    char * myDemangledCString = abi::__cxa_demangle(myMangledSymbol.c_str(), 0, 0, &myStatus);
    string myResult;
    if (myStatus != 0) {
        myResult = myMangledSymbol;
    } else {
        myResult = myDemangledCString;
    }
    if (myDemangledCString) {
        free(myDemangledCString);
    }
    return myResult;
}

#endif

