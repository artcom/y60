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
//    $RCSfile: TCPClientSocket.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#ifndef INCL_INET_TCPCLIENTSOCKET
#define INCL_INET_TCPCLIENTSOCKET

#include "TCPSocket.h"

namespace inet {
    /**
     * Implements a TCP Client.
     *
     * @code
     * inet::asl::Unsigned32 serverAddress = inet::Socket::getHostAddress("www.anyhost.de");
     *
     * inet::TCPClientSocket myConnection(serverPort,serverAddress);
     * if (!myConnection.isValid())
     *     myConnection.retry(10);
     *
     * ENSURE(myConnection.isValid());
     * 
     * myConnection.read(myInputBuffer,bufferSize);
     * myConnection.write(myOutputBuffer, bufferSize);
     *
     * @endcode
     */
    class TCPClientSocket : public TCPSocket {
        public:
            TCPClientSocket();
            void connect();
            int retry(int n);
    };

}
#endif
