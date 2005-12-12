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
//    $RCSfile: UDPSocket.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.11 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//============================================================================

#include "UDPSocket.h"
#include "SocketException.h"
#include <asl/Logger.h>

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
            throw SocketException("UDPSocket::init: can't setsockopt ");
        }

        if (bind(fd,(struct sockaddr*)&_myLocalEndpoint,sizeof(_myLocalEndpoint))<0) {
            throw SocketException("UDPSocket::init:can`t bind socket ");
        }
    }

    unsigned UDPSocket::receiveFrom(asl::Unsigned32* thehost, asl::Unsigned16 * theport, void *data, const int maxlen)
    {
#ifdef WIN32    
        int fromsize;
#else
        unsigned int fromsize;
#endif                
        fromsize=sizeof(_myLocalEndpoint);

        int bytesread;
        if ((bytesread=recvfrom(fd, (char*)data, maxlen, 0, (struct sockaddr*)&_myLocalEndpoint, &fromsize))>=0) {
            if (thehost) 
                *thehost = ntohl(_myLocalEndpoint.sin_addr.s_addr);
            if (theport)
                *theport = ntohs(_myLocalEndpoint.sin_port);
        }
        else {
            if (errno!=OS_SOCKET_ERROR(EWOULDBLOCK))
                throw SocketException("UDPSocket::ReadFrom failed");
        }
        return bytesread;
    }

    unsigned UDPSocket::sendTo(asl::Unsigned32 thehost, asl::Unsigned16 theport, const void *data, unsigned len)
    {
        setRemoteAddr(thehost, theport);

        int byteswritten;
        if ((byteswritten=sendto(fd, (char*)data, len, 0, (struct sockaddr*)&_myRemoteEndpoint, sizeof(_myRemoteEndpoint)))!=len) {
            throw SocketException("UDPSocket::SendTo failed");
        }
        return byteswritten;
    }

    void UDPSocket::reset() 
    { 
#if 1
        close();
#else
#ifndef WIN32
        ::close(fd);
#else
        closesocket(fd);
#endif                
        fd = -1;
#endif
        open();
    }

}
