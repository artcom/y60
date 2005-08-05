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
//    $RCSfile: TCPServer.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.13 $
//
// Description:
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//============================================================================


#include <errno.h>

#include "TCPServer.h"
#include <asl/Logger.h>
#include <asl/Time.h>

#ifdef OSX
#include <netinet/in.h>
#endif

#ifndef WIN32
#include <arpa/inet.h>
#define SOCKET_ERROR -1
#endif

using namespace std;


namespace inet {

    TCPServer::TCPServer(asl::Unsigned32 myHost, asl::Unsigned16 myPort, bool theReusePortFlag)
        : _myPort(myPort),
          _myHost(myHost),
          _myFromAddr(myHost, myPort)
    {
        fd=socket(AF_INET,SOCK_STREAM,0);

        if (bind(fd,(struct sockaddr*)&_myFromAddr,sizeof(_myFromAddr))<0) {
            // Socket was in use, try again if theReusePortFlag=true
            if (theReusePortFlag) {
                int myReuseSocketLen = sizeof(bool);
                if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&theReusePortFlag, myReuseSocketLen) == SOCKET_ERROR) {
                    throw SocketException("TCPServer::TCPServer: can`t set already bound rcv socket to reuse.");
                }

                if (bind(fd,(struct sockaddr*)&_myFromAddr,sizeof(_myFromAddr))<0 ) {
                     throw SocketException("TCPServer::TCPServer: can`t bind rcv socket ");
                }
            } else {
                throw SocketException("TCPServer::TCPServer: can`t bind socket, already is use.");
            }
        }
        if (listen(fd,8)<0) {
            throw SocketException("TCPServer::TCPServer: can`t listen ");
        }
    }

    TCPServer::~TCPServer() {
        close();
    }

    void TCPServer::close() {
#ifdef LINUX
        ::close(fd);
#endif
#ifdef WIN32
        closesocket(fd);
#endif
        fd = -1;
        AC_INFO << "TCPServer closed" << endl;
    }

    TCPSocket* TCPServer::waitForConnection() const
    {
#ifdef LINUX
        unsigned int remoteEndpointLen;
#else
        int remoteEndpointLen;
#endif
        char *myaddrstr;
        Endpoint remoteEndpoint;

        remoteEndpointLen=sizeof(remoteEndpoint);
        int newFD;
        if ((newFD=accept(fd,(sockaddr *)&remoteEndpoint,&remoteEndpointLen))<0) {
            SocketException("TCPServer::waitForConnection: can't accept connection");
        }
        TCPSocket * newSocket = new TCPSocket(newFD, _myFromAddr, remoteEndpoint);

        AC_DEBUG << "new connection accepted on socket " << newFD << endl;

        myaddrstr=inet_ntoa(remoteEndpoint.sin_addr);
        if (myaddrstr) {
            AC_DEBUG << "from host " << myaddrstr << ',' << " port " << remoteEndpoint.sin_port << endl;
        }
        return newSocket;
    }
}
