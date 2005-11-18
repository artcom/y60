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
//    $RCSfile: Socket.h,v $
//
//     $Author: pavel $
//
//   $Revision: 1.10 $
//
// Description:
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#ifndef INCL_INET_SOCKET
#define INCL_INET_SOCKET

#include "SocketException.h"
#include "Endpoint.h"
#include "address.h"

#ifdef WIN32
  #include <winsock2.h>
#else
  #include <netdb.h>
  #include <sys/socket.h>
#endif

namespace inet {

const int MAX_UDP_PACKET_SIZE = 1472;

/**
 *  Low-Level Socket operations.
 *
 **/
class Socket {
    public:
        /// creates a new socket with local endpoint on theport and interface thehost.
        Socket(asl::Unsigned32 thehost, asl::Unsigned16 theport);
        virtual ~Socket();

        /// initializes destination binding address and port.
        virtual void setRemoteAddr(asl::Unsigned32 thehost, 
                asl::Unsigned16 theport);

        virtual bool isValid() const { return (fd != -1); }

        virtual void close();

        /// reads up to len bytes from a connected socket.
        // @returns the number of bytes read
        // @throws SocketException
        virtual unsigned receive(void *data, const unsigned maxlen);

        /// writes up to len bytes to a connected socket.
        // @returns the number of bytes written
        // @throws SocketException
        virtual unsigned send(const void *data, unsigned len);

        /// checks the input queue.
        // @returns the number of bytes waiting to be received
        virtual int peek(int n);

        virtual void setBlockingMode(bool isBlocking);

        asl::Unsigned32 getLocalAddress() const {
            return ntohl(_myLocalEndpoint.sin_addr.s_addr);
        }
        unsigned short getLocalPort() const {
            return ntohs(_myLocalEndpoint.sin_port);
        }
        asl::Unsigned32 getRemoteAddress() const {
            return ntohl(_myRemoteEndpoint.sin_addr.s_addr);
        }
        unsigned short getRemotePort() const {
            return ntohs(_myRemoteEndpoint.sin_port);
        }

        // Sets the size of the send buffer in bytes (windows only for now)
        void setSendBufferSize(int theSize);
        int getSendBufferSize() const;

        // Sets the size of the receive buffer in bytes (windows only for now)
        void setReceiveBufferSize(int theSize);
        int getReceiveBufferSize() const;
    protected:
        Endpoint _myLocalEndpoint;

        // TODO: Should be moved to COSocket.
        Endpoint _myRemoteEndpoint;

        int fd;

    private:
};

}
#endif
