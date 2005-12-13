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
//    $RCSfile: Socket.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.21 $
//
// Description:
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#include "Socket.h"
#include "SocketException.h"
#include "net.h"

#include <asl/Logger.h>

#ifndef WIN32
#include <unistd.h>
#endif
#include <fcntl.h>

#include <stdio.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <assert.h>

using namespace std;
using namespace asl;

namespace inet {

    Socket::Socket(asl::Unsigned32 thehost, Unsigned16 theport)
        : fd(-1),
          _myLocalEndpoint(thehost, theport)
    {
    }

    Socket::~Socket()
    {
        close();
    }

    void Socket::setRemoteAddr(asl::Unsigned32 myhost, Unsigned16 myport)
    {
        _myRemoteEndpoint = asl::INetEndpoint(myhost, myport);
    }

    void Socket::close() {
        // TODO: error handling.
        if (isValid()) {
#ifdef WIN32        
            closesocket(fd);
#else            
            ::close(fd);
#endif
            fd = -1;
            AC_DEBUG <<  "Socket closed" << endl;
        }
    }

    unsigned Socket::receive(void *data, const unsigned maxlen)
    {
        int bytesread;
        if ((bytesread=recv(fd, (char*)data, maxlen, 0))>=0)
            return bytesread;
        else
        {
            int err = getLastSocketError();
            if ( err != OS_SOCKET_ERROR(EWOULDBLOCK))
                throw SocketException(err, "Socket::Receive failed");
        }

        return 0;
    }

    unsigned Socket::send(const void *data, unsigned len)
    {
        int byteswritten;
        if ((byteswritten=::send(fd, (char*)data, len, 0)) != len)
        {
            throw SocketException("Socket::write() failed.");
        }
        return byteswritten;
    }

    int Socket::peek(int n)
    {
        int rc;
#ifndef WIN32
        // XXX: This is inefficient. Use blocking calls instead.
        int status;
        char* buf;

        buf=new char[n];
        status=fcntl(fd, F_GETFL, 0);
        if (!(status & FNONBLOCK)) {
            fcntl(fd,F_SETFL,status & FNONBLOCK);
        }
        //     rc=(n==recvfrom(fd,buf,n,MSG_PEEK,NULL,NULL));
        rc = recvfrom(fd,buf,n,MSG_PEEK,NULL,NULL);
        if (rc == -1) {
            if (errno == EAGAIN && (status & FNONBLOCK)) {
                // this is OK
                rc = 0;
            } else {
                delete[] buf;
                throw SocketException("Socket::peek() failed.");
            }
        }
        if (!(status & FNONBLOCK)) {
            fcntl(fd,F_SETFL,status);
        }
        delete[] buf;
        return rc;
#else
        u_long theBytesInBuffer;
        rc = ioctlsocket(fd, FIONREAD, &theBytesInBuffer);
        if (rc == SOCKET_ERROR) {
            throw SocketException("Socket::peek() failed.");
        }
        return min(theBytesInBuffer, n);  // For compatibility with the linux version.
#endif
    }

    void PrintStatus(int fd)
    {
#ifndef WIN32
        int status;
        status=fcntl(fd, F_GETFL, 0);
        AC_INFO << "STATUS of fd:";
        switch(status & O_ACCMODE)
        {
            case O_RDONLY:AC_INFO << "O_RDONLY, ";break;
            case O_WRONLY:AC_INFO << "O_WRONLY, ";break;
            case O_RDWR:AC_INFO << "O_RDWR, ";break;
            default:AC_INFO << "O_?ILLEGAL?, ";break;
        }
        if (status & FNDELAY) AC_INFO <<   "FNDELAY, ";
        if (status & FAPPEND) AC_INFO <<   "FAPPEND, ";
        if (status & FFSYNC) AC_INFO <<    "FSYNC, ";
        //   if (status & FRCACH) AC_INFO <<  "FRCACH, ";
        if (status & FASYNC) AC_INFO <<    "FASYNC, ";
        if (status & FNONBLOCK) AC_INFO << "FNONBLK";
        AC_INFO << endl;
#endif
    }

    void Socket::setBlockingMode(bool isBlocking) {
#ifndef WIN32
        //    PrintStatus(fd);
        long theFlag = O_NONBLOCK;
        if (isBlocking) {
            theFlag = 0;
        }
        if (fcntl(fd, F_SETFL, theFlag)== -1)
        {
            throw SocketException("Socket::setBlockingMode failed");
        }
#else
        u_long theFlag = 1;
        if (isBlocking) {
            theFlag = 0;
        }
        int theRC = ioctlsocket(fd, FIONBIO, &theFlag);
        if (theRC == SOCKET_ERROR) {
            throw SocketException("Socket::setBlockingMode failed");
        }
#endif
    }

    void 
    Socket::setSendBufferSize(int theSize) {
#ifdef WIN32
        if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*) &theSize, sizeof(theSize)) < 0) {
            AC_ERROR << "Socket::setSendBufferSize: Unable to set SO_SNDBUF" << endl;
        }
#else
        AC_WARNING << "Socket::setSendBufferSize() not yet implemented for Linux/MaxOS";
#endif
    }
    
    int 
    Socket::getSendBufferSize() const {
        int mySize = 0;
        int myIntSize = sizeof(mySize);
#ifdef WIN32        
        if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*) &mySize, &myIntSize) < 0) {
            AC_ERROR << "Socket::getSendBufferSize: Unable to set SO_SNDBUF";
        }
#else
        AC_WARNING << "Socket::getSendBufferSize() not yet implemented for Linux/MaxOS";
#endif
        return mySize;
    }

    void 
    Socket::setReceiveBufferSize(int theSize) {
#ifdef WIN32
        if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*) &theSize, sizeof(theSize)) < 0) {
            AC_ERROR << "Socket::setReceiveBufferSize: Unable to set SO_SNDBUF" << endl;
        }
#else
        AC_WARNING << "Socket::setReceiveBufferSize() not yet implemented for Linux/MaxOS";
#endif
    }

    int 
    Socket::getReceiveBufferSize() const {
        int mySize = 0;
        int myIntSize = sizeof(mySize);
#ifdef WIN32
        if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*) &mySize, &myIntSize) < 0) {
            AC_ERROR << "Socket::getReceiveBufferSize: Unable to set SO_SNDBUF";
        }
#else
        AC_WARNING << "Socket::getReceiveBufferSize() not yet implemented for Linux/MaxOS";
#endif
        return mySize;
    }

}
