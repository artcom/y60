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
//    documentation           :      poor
//    test coverage           :      ok
//    names                   :      fair
//    style guide conformance :      fair
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
//    recommendations: change namespace, fix names
*/

//own header
#include "SocketException.h"

#include "net.h"

#include <asl/base/string_functions.h>

#include <iostream>
#include <algorithm>
#include <string>

#ifndef WIN32
#include <errno.h>
#endif

using namespace std;

namespace inet {


    SocketException::SocketException(int theErrorCode, const std::string & where)
        : asl::Exception(getSocketErrorMessage(theErrorCode), where, "inet::SocketException" ), 
          _myErrorCode ( theErrorCode ) 
    {
    }
    
    SocketException::SocketException (const std::string & where)
        : _myErrorCode (getLastSocketError())
    {
        set(getSocketErrorMessage(_myErrorCode), where, "inet::SocketException");
    }

}
