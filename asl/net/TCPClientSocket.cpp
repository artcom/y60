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
//    $RCSfile: TCPClientSocket.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.3 $
//
// Description:
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//============================================================================

#include "TCPClientSocket.h"
#include <asl/Logger.h>
#include <asl/Time.h>

#ifndef WIN32
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

#include <errno.h>

using namespace asl;
using namespace std;

namespace inet {

    TCPClientSocket::TCPClientSocket()
        : TCPSocket(0, 0)
    {
    }


    void TCPClientSocket::connect()
    {
        open();
        if( ::connect(fd,(struct sockaddr*)&_myRemoteEndpoint,sizeof(_myRemoteEndpoint)) != 0 )
            {
                throw SocketException("TCPClientSocket::ConnectSocket: can't connect");
            }
    }

    int TCPClientSocket::retry(int n)
    {
        for (int retry=1 ;retry<=n ; retry++ )
        {
            try {
                connect();
                return true;
            } catch (SocketException & se)
            {
                se;  // Avoid unreferenced variable warning.
                AC_DEBUG << retry << ". retry failed" << endl;
                msleep(2000);
            }
        }
        return false;
    }

}
