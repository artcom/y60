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
//    $RCSfile: TCPServer.h,v $
//
//     $Author: pavel $
//
//   $Revision: 1.7 $
//
// Description:
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#ifndef INCL_INET_TCPSERVER
#define INCL_INET_TCPSERVER

#include "TCPSocket.h"
#include "INetEndpoint.h"

namespace inet {

    /**
     * Implements a TCP Server (Listener).
     *
     * @code
     * try {
     *      myTCPServer = new TCPServer(serverPort, INADDR_ANY);
     *      break;
     * }
     * catch (SocketException & se)
     * {
     *      AC_ERROR << "failed to listen on port " << serverPort << ":" << se.where() << endl;
     *      myTCPServer = 0;
     * }
     *
     * TCPServer * myTCPServer = (TCPServer*)arg; // wait for a client to connect
     *
     * inet::TCPSocket *mySocket = myTCPServer->createNewConnection(); // get new connection
     *
     * mySocket->write("READY",5)
     * mySocket->read(myInputBuffer, 5);
     *
     * delete mySocket; // disconnect from the client
     * @endcode
     */
    class TCPServer {
        public:
            /// creates a new TCPServer listening on port theport on address fromhost.
            TCPServer(asl::Unsigned32 myHost, asl::Unsigned16 myPort, bool theReusePortFlag=false);
            ~TCPServer();

            void close();
            /// waits for a client to connect.
            // @returns a new TCPSocket for each new connection
            TCPSocket* waitForConnection() const;

        private:
            asl::Unsigned32 _myHost;
            asl::Unsigned16 _myPort;
            asl::INetEndpoint _myFromAddr;
            int fd;
    };

}
#endif
