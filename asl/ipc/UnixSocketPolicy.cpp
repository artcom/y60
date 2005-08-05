//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: UnixSocketPolicy.cpp,v $
//   $Author: david $
//   $Revision: 1.4 $
//   $Date: 2005/01/07 16:27:18 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================


#include "UnixSocketPolicy.h"

#include <asl/net.h>
#include <asl/string_functions.h>

#include <errno.h>

using namespace std;
using namespace inet;

#define DB(x) // x

namespace asl {

#ifndef WIN32
#define INVALID_SOCKET -1
#endif

const int UNIX_PATH_MAX = 108;
const string UnixAddress::PIPE_PREFIX("/tmp/ArtCom_");

UnixAddress::UnixAddress(const std::string & thePath) {
    sun_family = AF_UNIX;
    string myPath = UnixAddress::PIPE_PREFIX+thePath;
    strncpy(sun_path, myPath.c_str(), UNIX_PATH_MAX);
}
    
UnixAddress::UnixAddress(const char * thePath) {
    sun_family = AF_UNIX;
    string myPath = UnixAddress::PIPE_PREFIX+thePath;
    strncpy(sun_path, myPath.c_str(), UNIX_PATH_MAX);
}
    
UnixAddress::UnixAddress() {
    sun_family = AF_UNIX;
    *sun_path = 0;
}

UnixSocketPolicy::Handle 
UnixSocketPolicy::connectTo(Endpoint theRemoteEndpoint) {
    Handle myHandle = socket(AF_UNIX,SOCK_STREAM,0);   
    if (myHandle == INVALID_SOCKET) {
        throw ConduitException(string("UnixSocketPolicy::ctor: create - ") + 
                getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
    }

    if( ::connect(myHandle,(struct sockaddr*)&theRemoteEndpoint,sizeof(theRemoteEndpoint)) != 0 )
    {
        int myLastError = getLastSocketError();
        if (myLastError == ECONNREFUSED) {
            throw ConduitRefusedException(string("UnixSocketPolicy::ctor: connect - ") + 
                getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
        } else {
            throw ConduitException(string("UnixSocketPolicy::ctor: connect - ") + 
                getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
        }
    }
    DB(cerr << "Client connected " << endl);
    return myHandle;
}


UnixSocketPolicy::Handle
UnixSocketPolicy::startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount) {
    Handle myHandle=socket(AF_UNIX,SOCK_STREAM,0);    
    if (myHandle == INVALID_SOCKET) {
        throw ConduitException(string("UnixSocketPolicy::ctor: create - ") + 
                getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
    }

    if (bind(myHandle,(struct sockaddr*)&theEndpoint,sizeof(theEndpoint))<0) {
        int myLastError = getLastSocketError();
        if (myLastError == EADDRINUSE) {
            throw ConduitInUseException(string("UnixSocketPolicy::ctor create"), PLUS_FILE_LINE);
        } else {
            throw ConduitException(string("UnixSocketPolicy::UnixSocketPolicy bind - ")+
                    getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
        }
    }

    if (listen(myHandle, theMaxConnectionCount)<0) {
        throw ConduitException("UnixSocketPolicy::UnixSocketPolicy: listen - "+
                getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
    }
    DB(cerr << "Listening " << endl);
    return myHandle;
}

void 
UnixSocketPolicy::stopListening(Handle theHandle) {
    Endpoint localEndpoint;
    socklen_t myNameLength = sizeof(localEndpoint);
    getsockname(theHandle, (struct sockaddr*)(&localEndpoint), &myNameLength);
    SocketPolicy::stopListening(theHandle);
    unlink(localEndpoint.sun_path);
}

}

