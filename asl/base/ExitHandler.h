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
*/

#ifndef _included_asl_ExitHandler_
#define _included_asl_ExitHandler_

#include "asl_base_settings.h"

namespace asl {

// This class exists so the constructor gets called once on initialization
// of the program and sets windows exception handling stuff appropriately.
// There is only one instance and that instance is declared statically in 
// Exception.cpp. 
class ExitHandler {
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
