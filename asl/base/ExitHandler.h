/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_ExitHandler_
#define _included_asl_ExitHandler_

namespace asl {

// This class exists so the constructor gets called once on initialization
// of the program and sets windows exception handling stuff appropriately.
// There is only one instance and that instance is declared statically in the
// cpp file.
class __declspec(dllexport) ExitHandler {
public:
    ExitHandler();

    // Call this to test exception handling.
    static void segFault();

private:
    // XXX This variable is initialized in Exception.cpp (!). For some reason,
    // that prevents the Linker from optimizing it away.
    static ExitHandler _myExitHandler;
};

}

#endif
