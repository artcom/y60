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
//    error handling          :      fair
//    logging                 :      notapp
//    cheesyness              :      ok
//
//    overall review status   :      ok
//
//    recommendations: change namespace, check coding conventions, check TODOs
*/

//own header
#include "Socket.h"

#include "SocketException.h"
#include "net.h"
#include "net_functions.h"

#include <asl/base/Logger.h>

#ifndef _WIN32
#include <unistd.h>
#endif
#include <fcntl.h>

#include <stdio.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <assert.h>

#include <string.h>

#include <asl/base/string_functions.h>

#ifdef min
#undef min
#endif

using namespace std;
using namespace asl;

namespace inet {

    Socket::Socket(asl::Unsigned32 thehost, Unsigned16 theport)
        : _myLocalEndpoint(thehost, theport),
          fd(-1),
          _myIsConnected(true),
          _myTimeOut(0)
    {
        initSockets();
    }

    Socket::~Socket()
    {
        close();
        terminateSockets();
    }

    void Socket::setRemoteAddr(asl::Unsigned32 myhost, Unsigned16 myport)
    {
        _myRemoteEndpoint = asl::INetEndpoint(myhost, myport);
    }

    void Socket::close() {
        if (isValid()) {
#ifdef _WIN32
            int rc = closesocket(fd);
            if (rc == SOCKET_ERROR) {
                int err = getLastSocketError();
                throw SocketError(err, "Socket::close() failed.");
            }
#else
            // TODO: error handling.
            ::close(fd);
#endif
            fd = -1;
            AC_DEBUG <<  "Socket closed" << endl;
        }
    }

    unsigned Socket::receive(void *data, const unsigned maxlen)
    {
    	if(_myTimeOut) {
			struct timeval tv;

			fd_set readset;
			FD_ZERO(&readset);
#if defined(_MSC_VER)
#pragma warning(push,1)
#endif //defined(_MSC_VER)
			FD_SET(fd, &readset);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

			// Initialize time out struct
			tv.tv_sec = getConnectionTimeout();
			tv.tv_usec = 0;

			int result = select(fd + 1, &readset, NULL, NULL, &tv);
			if (result <= 0){
				int err = getLastSocketError();
				throw SocketError(err, std::string(
									  "disconnect or timeout while receiveing from socket " +
									  hostname(getRemoteAddress()) + ":" + as_string(getRemotePort())));
			}
    	}

        int bytesread = recv(fd, (char*)data, maxlen, 0);

        if (bytesread>0){
            return bytesread;
        } else if (bytesread == 0) {
        	_myIsConnected = false; // XXX: hack for tcp disconnect
        	//throw SocketDisconnected(PLUS_FILE_LINE);
        } else {
            int err = getLastSocketError();
            if((err == OS_SOCKET_ERROR(EWOULDBLOCK))) {
            	return 0;
            } else {
            	throw SocketError(err, "receive() failed");
            }
        }

        return 0;
    }

    unsigned Socket::send(const void *data, unsigned len)
    {
        int byteswritten;
        if ((byteswritten=::send(fd, (char*)data, len, getSendFlags())) != static_cast<int>(len))
        {
            int err = getLastSocketError();
            throw SocketError(err, "Socket::send() failed.");
        }
        return byteswritten;
    }

    int Socket::peek(int n)
    {
        int rc;
#ifndef _WIN32
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
                throw SocketError(getLastSocketError(), "Socket::peek() failed.");
            }
        }
        if (!(status & FNONBLOCK)) {
            fcntl(fd,F_SETFL,status);
        }
        delete[] buf;
        return rc;
#else
        //TODO: Bug #564
        u_long theBytesInBuffer;
        rc = ioctlsocket(fd, FIONREAD, &theBytesInBuffer);

        if (rc == SOCKET_ERROR) {
            int err = getLastSocketError();
            throw SocketError(err, "Socket::peek() failed.");
        }
        return min(theBytesInBuffer, static_cast<u_long>(n));  // For compatibility with the linux version.
#endif
    }

    void PrintStatus(int fd)
    {
#ifndef _WIN32
        int status;
        status=fcntl(fd, F_GETFL, 0);
        AC_INFO << "STATUS of fd:";
        switch(status & O_ACCMODE)
        {
            case O_RDONLY:{AC_INFO << "O_RDONLY, ";break;}
            case O_WRONLY:{AC_INFO << "O_WRONLY, ";break;}
            case O_RDWR:{AC_INFO << "O_RDWR, ";break;}
            default:{AC_INFO << "O_?ILLEGAL?, ";break;}
       }
        if (status & FNDELAY) {AC_INFO <<   "FNDELAY, ";}
        if (status & FAPPEND) {AC_INFO <<   "FAPPEND, ";}
        if (status & FFSYNC) {AC_INFO <<    "FSYNC, ";}
        //   if (status & FRCACH) AC_INFO <<  "FRCACH, ";}
        if (status & FASYNC) {AC_INFO <<    "FASYNC, ";}
        if (status & FNONBLOCK) {AC_INFO << "FNONBLK";}
        AC_INFO << endl;
#endif
    }

    void Socket::setBlockingMode(bool isBlocking) {
#ifndef _WIN32
        //    PrintStatus(fd);
        long theFlag = O_NONBLOCK;
        if (isBlocking) {
            theFlag = 0;
        }
        if (fcntl(fd, F_SETFL, theFlag)== -1)
        {
            throw SocketError(getLastSocketError(), "Socket::setBlockingMode failed");
        }
#else
        u_long theFlag = 1;
        if (isBlocking) {
            theFlag = 0;
        }
        int theRC = ioctlsocket(fd, FIONBIO, &theFlag);
        if (theRC == SOCKET_ERROR) {
            int err = getLastSocketError();
            throw SocketError(err, "Socket::setBlockingMode failed");
        }
#endif
    }

    void
    Socket::setSendBufferSize(int theSize) {
#ifdef _WIN32
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
#ifdef _WIN32
        int myIntSize = sizeof(mySize);
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
#ifdef _WIN32
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
#ifdef _WIN32
        int myIntSize = sizeof(mySize);
        if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*) &mySize, &myIntSize) < 0) {
            AC_ERROR << "Socket::getReceiveBufferSize: Unable to set SO_SNDBUF";
        }
#else
        AC_WARNING << "Socket::getReceiveBufferSize() not yet implemented for Linux/MaxOS";
#endif
        return mySize;
    }

    int
    Socket::getSendFlags() {
        return 0;
    }

}
