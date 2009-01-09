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
// Description: 
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      fair
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
//    recommendations: change namespace, add doxygen doctags,
//                     explain use of this class
*/

//own header
#include "UDPSocket.h"

#include "SocketException.h"
#include <asl/base/Logger.h>

#include <errno.h>
#ifndef WIN32
#include <unistd.h>
#else
#endif

using namespace std;  // automatically added!

namespace inet {

    UDPSocket::UDPSocket(asl::Unsigned32 thehost, asl::Unsigned16 theport)
        : Socket(thehost, theport)
    {
//        initFrom(thehost, theport);
        open();
    }

    void UDPSocket::open()
    {
        fd=socket(AF_INET,SOCK_DGRAM,0);

        int reuseaddr = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuseaddr, sizeof(reuseaddr)) < 0) {
            AC_ERROR << "UDPSocket::init: Unable to set SO_REUSEADDR" << endl;
        }

        int broadcast = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST,(char*) &broadcast, sizeof(broadcast)) < 0) {
            throw SocketException("UDPSocket::open: Can't set broadcast flag");
        }

        if (bind(fd, (struct sockaddr*)&_myLocalEndpoint, sizeof(_myLocalEndpoint)) < 0) {
            AC_TRACE << "UDPSocket::open: Failed to bind to " << hex << ntohl(_myLocalEndpoint.sin_addr.s_addr) << dec << ":" << ntohs(_myLocalEndpoint.sin_port);
            throw SocketException("UDPSocket::open: Can't bind socket");
        }
    }

    unsigned UDPSocket::receiveFrom(asl::Unsigned32* thehost, asl::Unsigned16 * theport, void *data, const int maxlen)
    {
#ifdef WIN32    
        int peerAddrSize;
#else
        unsigned int peerAddrSize;
#endif                
        peerAddrSize=sizeof(_myLocalEndpoint);

        struct sockaddr_in peerAddr;
        int bytesRead;
        if ((bytesRead=recvfrom(fd, (char*)data, maxlen, 0, (struct sockaddr*)&peerAddr, &peerAddrSize))>=0) {
            if (thehost) 
                *thehost = ntohl(peerAddr.sin_addr.s_addr);
            if (theport)
                *theport = ntohs(peerAddr.sin_port);
        }
        else {
            if (errno!=OS_SOCKET_ERROR(EWOULDBLOCK))
                throw SocketException("UDPSocket::ReadFrom failed");
        }
        AC_TRACE << "Received " << bytesRead << " from " << hex << ntohl(peerAddr.sin_addr.s_addr) << dec << ":" << ntohs(peerAddr.sin_port);

        return bytesRead;
    }

    unsigned UDPSocket::sendTo(asl::Unsigned32 thehost, asl::Unsigned16 theport, const void *data, unsigned len)
    {
        setRemoteAddr(thehost, theport);

        int byteswritten;
        AC_TRACE << "Sending to " << hex << _myRemoteEndpoint.sin_addr.s_addr << dec << ":" << _myRemoteEndpoint.sin_port;
        if ((byteswritten=sendto(fd, (char*)data, len, 0, (struct sockaddr*)&_myRemoteEndpoint, sizeof(_myRemoteEndpoint)))!=static_cast<int>(len)) {
            throw SocketException("UDPSocket::SendTo failed");
        }
        return byteswritten;
    }

    void UDPSocket::reset() 
    { 
        close();
        open();
    }
}
