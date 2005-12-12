//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SocketException.cpp,v $
//
//     $Author: martin $
//
//   $Revision: 1.7 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#include "SocketException.h"
#include "net.h"

#include <asl/string_functions.h>

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
