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
//     Classes for networked or local communication between processes
//
// Last Review:  ms 2007-08-15
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      fair
//    documentation           :      ok
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      fair
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
//    recommendations: add high-level documentation, improve doxygen documentation 
*/
#include "TCPPolicy.h"

#include <asl/net/net.h>
#include <asl/base/string_functions.h>
#include <errno.h>

using namespace std;
namespace asl {

#ifdef WIN32
#define EADDRINUSE WSAEADDRINUSE 
#else
#define INVALID_SOCKET -1
#endif

TCPPolicy::Handle 
TCPPolicy::connectTo(Endpoint theRemoteEndpoint) {
    inet::initSockets();
    
    Handle myHandle = socket(AF_INET,SOCK_STREAM,0);   
    if (myHandle == INVALID_SOCKET) {
        throw ConduitException(string("TCPPolicy::ctor: create - ") + 
                inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
    }
    if( ::connect(myHandle,(struct sockaddr*)&theRemoteEndpoint,sizeof(theRemoteEndpoint)) != 0 )
    {
        throw ConduitException(string("TCPPolicy::ctor: connect - ") + 
                inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
    }
    return myHandle;
}

void
TCPPolicy::disconnect(Handle & theHandle) {
    SocketPolicy::disconnect(theHandle);
    inet::terminateSockets();
}

void 
TCPPolicy::stopListening(Handle theHandle) {
    SocketPolicy::stopListening(theHandle);
    inet::terminateSockets();
}


TCPPolicy::Handle 
TCPPolicy::createOnConnect(Handle & theListenHandle, unsigned theMaxConnectionCount, 
        int theTimeout) {
    Handle myHandle = SocketPolicy::createOnConnect(theListenHandle, theMaxConnectionCount, theTimeout);
    if (myHandle > 0) {
        inet::initSockets();
    }
    
    return myHandle;
}

TCPPolicy::Handle
TCPPolicy::startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount) {
    inet::initSockets();

    Handle myHandle=socket(AF_INET,SOCK_STREAM,0);    
   
#ifndef WIN32    
    int myReuseSocketFlag = 1;
    if (setsockopt(myHandle, SOL_SOCKET, SO_REUSEADDR, (char*)&myReuseSocketFlag, sizeof(myReuseSocketFlag)) != 0) {
        throw ConduitException("can`t set already bound rcv socket to reuse.", PLUS_FILE_LINE);
    }
#endif    

    if (bind(myHandle,(struct sockaddr*)&theEndpoint,sizeof(theEndpoint))<0) {
        int myLastError = inet::getLastSocketError();
        if (myLastError == EADDRINUSE) {
            throw ConduitInUseException(string("TCPPolicy::startListening while calling ::bind"), PLUS_FILE_LINE);
        } else {
            throw ConduitException(string("TCPPolicy::TCPPolicy bind - ")+
                    inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
        }
    }
    if (listen(myHandle, theMaxConnectionCount)<0) {
        throw ConduitException("TCPPolicy::TCPPolicy: listen - "+
                inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
    }
    return myHandle;
}
}

