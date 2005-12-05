//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: TCPPolicy.cpp,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/01/05 15:47:52 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================


#include "TCPPolicy.h"

#include <asl/net.h>
#include <asl/string_functions.h>
#include <errno.h>

using namespace std;
using namespace inet;

namespace asl {

#ifdef WIN32
#define EADDRINUSE WSAEADDRINUSE 
#else
#define INVALID_SOCKET -1
#endif

TCPPolicy::Handle 
TCPPolicy::connectTo(Endpoint theRemoteEndpoint) {
    Handle myHandle = socket(AF_INET,SOCK_STREAM,0);   
    if (myHandle == INVALID_SOCKET) {
        throw ConduitException(string("TCPPolicy::ctor: create - ") + 
                getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
    }
    if( ::connect(myHandle,(struct sockaddr*)&theRemoteEndpoint,sizeof(theRemoteEndpoint)) != 0 )
    {
        throw ConduitException(string("TCPPolicy::ctor: connect - ") + 
                getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
    }
    return myHandle;
}


TCPPolicy::Handle
TCPPolicy::startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount) {
    Handle myHandle=socket(AF_INET,SOCK_STREAM,0);    

    if (bind(myHandle,(struct sockaddr*)&theEndpoint,sizeof(theEndpoint))<0) {
        int myLastError = getLastSocketError();
        if (myLastError == EADDRINUSE) {
            throw ConduitInUseException(string("TCPPolicy::ctor create"), PLUS_FILE_LINE);
        } else {
            throw ConduitException(string("TCPPolicy::TCPPolicy bind - ")+
                    getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
        }
    }
    if (listen(myHandle, theMaxConnectionCount)<0) {
        throw ConduitException("TCPPolicy::TCPPolicy: listen - "+
                getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
    }
    return myHandle;
}
}

