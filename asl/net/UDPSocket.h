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
//    $RCSfile: UDPSocket.h,v $
//     $Author: pavel $
//   $Revision: 1.7 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#ifndef INCL_INET_UDPSOCKET
#define INCL_INET_UDPSOCKET


#include "Socket.h"

namespace inet {

    /** implements connectionless UDP, may be used for UDP-broadcasts.
     *
     * @code
     *
     * try {
     *      // create a socket listening on MyPort
     *      myUDPSocket = new UDPSocket(INADDR_ANY, myPort);
     *      break;
     *  }
     *  catch (SocketException & se)
     *  {
     *      AC_ERROR << "failed to listen on port " << myPort << ":" << se.where() << endl;
     *      myUDPServer = 0;
     *  }
     *  ENSURE(myUDPSocket);
     *
     *  asl::Unsigned32 otherHost;
     *  asl::Unsigned16 otherPort;
     *  myUDPSocket->receiveFrom(&otherHost, &otherPort, myInputBuffer, sizeof(myInputBuffer));
     *  myUDPSocket->sendTo(broadcastAddress, otherPort, data, sizeof(data));
     *
     *  delete myUDPSocket;
     *  
     * @endcode
     *
     * @TODO: create a UDPMulticast subclass (should be easy now)
     **/
    class UDPSocket : public Socket {
        public:
            /// create a new socket listening on theport on address thehost
            UDPSocket(asl::Unsigned32 thehost, asl::Unsigned16 theport);
            
            virtual void open();
            /// receives data from any host.
            // theport and thehost will contain the origin of the data.
            // @returns number of bytes received
            virtual unsigned receiveFrom(asl::Unsigned32* thehost, 
                    asl::Unsigned16 * theport, void *data, const int maxlen); 
            /// sends data either to a host or to a broadcast address.
            virtual unsigned sendTo(asl::Unsigned32 thehost, 
                    asl::Unsigned16 theport, const void *data, unsigned len);
            virtual void reset();
    };
}
#endif
