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
//    formatting              :      ok
//    documentation           :      poor
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
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
//    recommendations: change namespace
*/

//own header
#include "TCPSocket.h"

#include "SocketException.h"

using namespace asl;

namespace inet {

    TCPSocket::TCPSocket(asl::Unsigned32 thehost, asl::Unsigned16 theport)
        : Socket(thehost, theport)
    {
        open();
        _myIsConnected = false;
    }

    TCPSocket::TCPSocket(int myFD, const INetEndpoint & localEndpoint,
                      const INetEndpoint & remoteEndpoint)
        : Socket (0,0)
    {
          _myLocalEndpoint = localEndpoint;
          _myRemoteEndpoint = remoteEndpoint;
          fd = myFD;
          _myIsConnected = true;
    }


    void TCPSocket::open()
    {
        fd=socket(AF_INET,SOCK_STREAM,0);
    }

    int TCPSocket::getSendFlags()
    {
#ifdef WIN32
        return 0;
#else
        return MSG_NOSIGNAL; // suppress SIGPIPE
#endif
    }
}
