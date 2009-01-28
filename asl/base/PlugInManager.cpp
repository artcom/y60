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
//
//    $RCSfile: PlugInManager.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.12 $
//
// Description:
//
//=============================================================================

// own header
#include "PlugInManager.h"

#include "Logger.h"
#include "string_functions.h"
#include "file_functions.h"

using namespace std;
using namespace asl;

void PlugInManager :: setSearchPath(const std::string & thePath) {
    _mySearchPath = thePath;
}

string PlugInManager :: getPlugInFileName(const string & theLibraryName, const char * theVariant) {
    std::string myFilename;
#ifndef _WIN32
    myFilename += "lib";
#endif
    myFilename += theLibraryName;
#ifndef AC_BUILT_WITH_CMAKE
    if (!theVariant) {
#ifdef DEBUG_VARIANT
        myFilename += "DBG";
#else
        myFilename += "OPT";
#endif
    }
#endif

#ifdef LINUX
    myFilename += ".so";
#endif
#ifdef OSX
    myFilename += ".dylib";
#endif
#ifdef _WIN32
    myFilename += ".dll";
#endif
    return myFilename;
}

string PlugInManager :: getSearchPath() const {
    return _mySearchPath;
}


asl::Ptr<PlugInBase> PlugInManager :: getPlugIn(const std::string & theName) {
    PlugInCache::iterator i = _myCache.find(theName);
    Ptr<PlugInBase> myPlugIn;
    if (i != _myCache.end() && (myPlugIn = i->second.lock())) {
        return myPlugIn;
    } else {
        myPlugIn = loadPlugIn(theName);
        if (myPlugIn) {
            myPlugIn->_initializeSingletonManager(&SingletonManager::get());
            //myPlugIn->initialize();
            _myCache[theName] = WeakPtr<PlugInBase>(myPlugIn);
            return myPlugIn;
        }
    }
    throw PlugInException(string("PlugIn not found: '") + theName + "'. Was looking in "+ _mySearchPath, PLUS_FILE_LINE);
}

PlugInManager :: PlugInManager() {
}

PlugInManager :: ~PlugInManager() {
}

void* PlugInManager :: getFunction(DLHandle theHandle, const std::string & theSymbol) {
#ifdef _WIN32
    void * myFunction = GetProcAddress(theHandle, theSymbol.c_str());
#else
    void * myFunction = dlsym(theHandle, theSymbol.c_str());
#endif
    return myFunction;
}

asl::Ptr<PlugInBase> PlugInManager :: loadPlugIn(const std::string & theName) {
    string myFilename = searchFile(getPlugInFileName(theName), _mySearchPath);
    if (myFilename.empty()) {
        throw PlugInException(string("PlugIn '") +getPlugInFileName(theName)+"' not found in search path '"+ _mySearchPath +"'", PLUS_FILE_LINE);
    }
    AC_INFO << "Plugging '" << myFilename << "'" << endl;
#ifdef _WIN32
    DLHandle myModule = ::LoadLibrary(myFilename.c_str());
#else
    DLHandle myModule = dlopen (myFilename.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#endif
    if (!myModule) {
        throw PlugInException("LoadLibrary/dlopen failed for PlugIn '" +theName+"': "+ lastError(), PLUS_FILE_LINE);
    }
    string myInitFunctionName = theName+"_instantiatePlugIn";
    InstantiatePlugInFunctionPtr myInstantiateFunction = (InstantiatePlugInFunctionPtr)getFunction(myModule, myInitFunctionName);
    if (myInstantiateFunction) {
        Ptr<PlugInBase> myPlugIn = Ptr<PlugInBase>((*myInstantiateFunction)(myModule));
        if (!myPlugIn) {
            throw PlugInException("PlugIn initializer function '" + myInitFunctionName +" did not return a valid PlugInBase object for " +theName, PLUS_FILE_LINE);
        }
        return myPlugIn;
    } else {
        throw PlugInException(string("'") + myInitFunctionName + "' not exported in PlugIn '" +theName + "'", PLUS_FILE_LINE);
    }
}

bool
PlugInManager :: isLoaded(const std::string & theName) const {
    PlugInCache::const_iterator i = _myCache.find(theName);
    return i != _myCache.end() && i->second;
}

string
PlugInManager :: lastError() const {
#ifdef _WIN32
    DWORD myErrorCode = GetLastError();
    LPVOID lpMsgBuf;
    if (!FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        myErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL ))
    {
        // Handle the error.
        return as_string(myErrorCode);
    }

    std::string myLastError((LPCTSTR)lpMsgBuf);
    // Free the buffer.
    LocalFree( lpMsgBuf );
#else
    std::string myLastError(dlerror());
#endif
    return myLastError;
}
