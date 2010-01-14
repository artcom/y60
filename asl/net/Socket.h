/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: Base class for socket wrappers
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      ok
//    documentation           :      fair
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      fair
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
//    interfaces              :      ok
//    confidence              :      ok
//    integration             :      ok
//    dependencies            :      ok
//    error handling          :      ok
//    logging                 :      notapp
//    cheesyness              :      ok
//
//    overall review status   :      ok
//
//    recommendations: change namespace, fix doxygen doctags
*/

#ifndef INCL_INET_SOCKET
#define INCL_INET_SOCKET

#include "asl_net_settings.h"

#include <asl/base/Ptr.h>

#include "SocketException.h"
#include "INetEndpoint.h"
#include "address.h"

#ifdef _WIN32
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
class ASL_NET_DECL Socket {
    public:
        /// creates a new socket with local endpoint on theport and interface thehost.
        Socket(asl::Unsigned32 thehost, asl::Unsigned16 theport);
        virtual ~Socket();

        /// initializes destination binding address and port.
        virtual void setRemoteAddr(asl::Unsigned32 thehost,
                asl::Unsigned16 theport);

        //XXX When the function is virtual there is a segfault
        // at the end of testInet.tst under Linux64 in
        // non-debug mode; reason is not fully understood
        /*virtual*/ bool isValid() const { return (fd != -1); }

        bool isConnected() const { return _myIsConnected; }

        virtual void close();

        /// reads up to len bytes from a connected socket.
        ///@returns the number of bytes read
        ///@throws SocketException
        virtual unsigned receive(void *data, const unsigned maxlen);

        /// writes up to len bytes to a connected socket.
        // @returns the number of bytes written
        // @throws SocketException
        virtual unsigned send(const void *data, unsigned len);

        /// checks the input queue.
        // @returns the number of bytes waiting to be received
        virtual int peek(int n);

        virtual void setBlockingMode(bool isBlocking);

        /// @returns seconds to wait before raising an socket exception
        unsigned int getConnectionTimeout() const {
            return _myTimeOut;
        }

        /// Sets the seconds to wait before raising an socket
        /// exception.
        /// A value of zero disables this feature.
        /// @note only implemented for receive right now
        void setConnectionTimeout(unsigned int theTimeOut) {
            _myTimeOut = theTimeOut;
        }

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
        // opportunity for subclasses to pass flags to send(2),
        // allowing suppresion of SIGPIPE and other gory details.
        virtual int getSendFlags();

    protected:
        asl::INetEndpoint _myLocalEndpoint;

        // TODO: Should be moved to COSocket.
        asl::INetEndpoint _myRemoteEndpoint;

        int fd;

        // XXX: hack for tcp socket disconnect
        bool _myIsConnected;

    private:

        unsigned int _myTimeOut;
};

typedef asl::Ptr<Socket> SocketPtr;

}
#endif
