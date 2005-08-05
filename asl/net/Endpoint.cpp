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
//    $RCSfile: Endpoint.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.4 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#include "Endpoint.h"
#include "net_functions.h"

#include <string.h>

using namespace asl;

namespace inet {
    Endpoint::Endpoint(const char * theHost, Unsigned16 myPort) {
        memset(this, 0, sizeof(this));
        sin_family=AF_INET;
        sin_addr.s_addr=htonl(asl::hostaddress(theHost));
        sin_port=htons(myPort);
    }

    Endpoint::Endpoint(Unsigned32 theHost, Unsigned16 myPort) {
        memset(this, 0, sizeof(this));
        sin_family=AF_INET;
        sin_addr.s_addr=htonl(theHost);
        sin_port=htons(myPort);
    }

    Endpoint::Endpoint() {
        memset(this, 0, sizeof(this));
        sin_family=AF_INET;
    }
}
