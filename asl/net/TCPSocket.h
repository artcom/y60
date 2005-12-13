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
//    $RCSfile: TCPSocket.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.5 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#ifndef INCL_INET_TCPSOCKET
#define INCL_INET_TCPSOCKET

#include "Socket.h"
#include "INetEndpoint.h"

namespace inet {

    class TCPSocket : public Socket {
        public:
            TCPSocket(asl::Unsigned32 thehost, asl::Unsigned16 theport);

            // Used by TCPServer.
            TCPSocket(int myFD, const asl::INetEndpoint & localEndpoint, 
                      const asl::INetEndpoint & remoteEndpoint);

            virtual void open();
    };
}
#endif
