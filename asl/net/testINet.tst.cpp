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
// Description:
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      ok
//    documentation           :      fair
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
//    interfaces              :      ok
//    confidence              :      ok
//    integration             :      ok
//    dependencies            :      ok
//    error handling          :      ok
//    logging                 :      notapp
//    cheesyness              :      ok
//
//    overall review status   :      ok
//
//    recommendations: add comments
*/

#ifndef _WIN32
#include "TestINetMonitor.h"
#endif
#include "TestSocket.h"
#include "net.h"

#include <asl/base/Logger.h>
#include <asl/base/settings.h>

using namespace std;
using namespace inet;

int main( int argc, char *argv[] ) {
    initSockets();

    UnitTestSuite mySuite ("INet tests", argc, argv);

#ifndef _WIN32
    mySuite.addTest (new TestINetMonitor);
#endif
    mySuite.addTest (new TestSocket);

    mySuite.run();

    AC_INFO << ">> Finished test '" << argv[0] << "'"
           << ", return status = " << mySuite.returnStatus();

    terminateSockets();

    return mySuite.returnStatus();
}

