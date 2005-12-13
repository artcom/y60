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
//    $RCSfile: TCPSocket.cpp,v $
//
//     $Author: build $
//
//   $Revision: 1.6 $
//
// Description:
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//============================================================================

#include "TCPSocket.h"
#include "SocketException.h"

using namespace asl;

namespace inet {

    TCPSocket::TCPSocket(asl::Unsigned32 thehost, asl::Unsigned16 theport)
        : Socket(thehost, theport)
    {
        open();
    }

    TCPSocket::TCPSocket(int myFD, const INetEndpoint & localEndpoint,
                      const INetEndpoint & remoteEndpoint)
        : Socket (0,0)
    {
          _myLocalEndpoint = localEndpoint;
          _myRemoteEndpoint = remoteEndpoint;
          fd = myFD;
    }


    void TCPSocket::open()
    {
        fd=socket(AF_INET,SOCK_STREAM,0);
    }
}
