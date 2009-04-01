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
//    $RCSfile: UnitTest.cpp,v $
//
//   $Revision: 1.5 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "console_functions.h"

#include "settings.h"
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif

using namespace std;

// XXX: This considers only cerr and cout as TTYs, which is somewhat incorrect.
bool asl::isTTY(ostream & os) {
#ifndef _WIN32
    int myFD = -1;
    if (&os == &cout) {
        myFD = fileno(stdout);
    }
    if (&os == &cerr) {
        myFD = fileno(stderr);
    }

    if(myFD == -1) {
        return false;
    }

    return isatty( myFD );
#else
    int myFD = -1;
    if (&os == &cout) {
        myFD = _fileno(stdout);
    }
    if (&os == &cerr) {
        myFD = _fileno(stderr);
    }

    if(myFD == -1) {
        return false;
    }

    return 0 != _isatty( myFD );
#endif
}

#ifdef _WIN32
HANDLE GetConsole(ostream & os) {
    HANDLE myHandle = 0;
    if (&os == &cout) {
        myHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    if (&os == &cerr) {
        myHandle = GetStdHandle(STD_ERROR_HANDLE);
    }
    return myHandle;
}
#endif

ostream & asl::TTYRED ( ostream & os ) {
    if (isTTY(os)) {
#if _WIN32
        os.flush();
        SetConsoleTextAttribute(GetConsole(os), FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
        os << "\033[0;31m";
#endif
    }
    return os;
}
ostream & asl::TTYGREEN ( ostream & os ) {
    if (isTTY(os)) {
#if _WIN32
        os.flush();
        SetConsoleTextAttribute(GetConsole(os), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
        os << "\033[0;32m";
#endif
    }
    return os;
}


ostream & asl::TTYBLUE ( ostream & os ) {
    if (isTTY(os)) {
#if _WIN32
        os.flush();
        SetConsoleTextAttribute(GetConsole(os), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
        os << "\033[0;34m";
#endif
    }
    return os;
}
ostream & asl::TTYYELLOW ( ostream & os ) {
    if (isTTY(os)) {
#if _WIN32
        os.flush();
        SetConsoleTextAttribute(GetConsole(os), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
        os << "\033[0;33m";
#endif
    }
    return os;
}
ostream & asl::ENDCOLOR ( ostream & os ) {
    if (isTTY(os)) {
#if _WIN32
        os.flush();
        SetConsoleTextAttribute(GetConsole(os), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#else
        os << "\033[0m";
#endif
    }
    return os;
}

