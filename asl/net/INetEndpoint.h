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
//    $RCSfile: Endpoint.h,v $
//
//     $Author: martin $
//
//   $Revision: 1.3 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#ifndef INCL_INET_ENDPOINT
#define INCL_INET_ENDPOINT

#include <asl/settings.h>

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <netdb.h>
    #include <sys/socket.h>
#endif  

#ifdef OSX
    #include <netinet/in.h>
#endif

namespace asl {

    // This is a POD type. Don't add virtual functions.
    class INetEndpoint: public sockaddr_in {
        public:
            INetEndpoint(const char * myHost, asl::Unsigned16 myPort);
            INetEndpoint(asl::Unsigned32 myHost, asl::Unsigned16 myPort);
            INetEndpoint();
    };

}
#endif
