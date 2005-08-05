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
//    $RCSfile: net.h,v $
//
//     $Author: martin $
//
//   $Revision: 1.2 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#ifndef INCL_INET_NET
#define INCL_INET_NET

#include <string>

namespace inet {
    void initSockets();
    void terminateSockets();
    int getLastSocketError();
    std::string getSocketErrorMessage(int ErrorID);
}
#endif
