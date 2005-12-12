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
//   $RCSfile: SocketPolicy.cpp,v $
//   $Author: pavel $
//   $Revision: 1.4 $
//   $Date: 2005/04/24 00:30:18 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================


#include "SocketPolicy.h"

#include <asl/net.h>
#include <asl/string_functions.h>

#include <errno.h>

using namespace std;
using namespace inet;

namespace asl {

#ifndef WIN32
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#define DB(x) // x
    
void
SocketPolicy::disconnect(Handle & theHandle) {
#ifdef WIN32        
    closesocket(theHandle);
#else    
    ::close(theHandle);
#endif
    theHandle = 0;
}

void 
SocketPolicy::stopListening(Handle theHandle) {
#ifdef WIN32        
    closesocket(theHandle);
#else
    ::close(theHandle);
#endif
}

SocketPolicy::Handle 
SocketPolicy::createOnConnect(Handle & theListenHandle, unsigned theMaxConnectionCount, 
        int theTimeout) {

    int retval = 0;
    fd_set myReadSet;

    FD_ZERO(&myReadSet);
    FD_SET(theListenHandle, &myReadSet);
    if (theTimeout >= 0) {
        struct timeval tv;
        tv.tv_sec = theTimeout / 1000;
        tv.tv_usec = theTimeout % 1000;
        retval = select(theListenHandle+1, &myReadSet, NULL, NULL, &tv);
    } else {
        retval = select(theListenHandle+1, &myReadSet, NULL, NULL, NULL);
    }
    if (retval == 0) {
        return 0;
    }
    DB(AC_TRACE << theListenHandle << "starting accept call" << endl);  
    Handle newFD;
    if ((newFD=accept(theListenHandle, 0, 0))<0) {
        int myLastError = getLastSocketError();
        DB(AC_TRACE << "accept call error " << myLastError << endl);  
#ifdef WIN32
        if (myLastError == WSAEINTR) { // interrupted system call
#else
        if (myLastError == EINTR) { // interrupted system call
#endif
            return 0;
        }
        throw ConduitException(string("SocketPolicy::createOnConnect: ")+
                getSocketErrorMessage(myLastError), PLUS_FILE_LINE);
    }
    DB(AC_TRACE << "accept call successful" << endl);  
    return newFD;
}


bool 
SocketPolicy::handleIO(Handle & theHandle, BufferQueue & theInQueue, BufferQueue & theOutQueue, int theTimeout) {
    int myRetVal = 0;
    fd_set myReadSet;
    fd_set myWriteSet;

    FD_ZERO(&myReadSet);
    FD_SET(theHandle, &myReadSet);
    FD_ZERO(&myWriteSet);
    FD_SET(theHandle, &myWriteSet);
    
    if (theTimeout >= 0) {
        struct timeval tv;
        tv.tv_sec = theTimeout / 1000;
        tv.tv_usec = theTimeout % 1000;
        myRetVal = select(theHandle+1, &myReadSet, &myWriteSet, NULL, &tv);
    } else {
        myRetVal = select(theHandle+1, &myReadSet, &myWriteSet, NULL, NULL);
    }
    if (myRetVal == 0) {
        // Nothing to do
        return true;
    }
    DB(AC_TRACE << "HandleIO select returned " << myRetVal <<  endl);
    if (myRetVal == SOCKET_ERROR) {
        int myLastError = getLastSocketError();
        throw ConduitException(string("Handle I/O: select returned (") + as_string(myLastError) +
                ") - " + getSocketErrorMessage(myLastError), PLUS_FILE_LINE);
    }
    if (FD_ISSET(theHandle, &myReadSet)) {
        if (!receiveData(theHandle, theInQueue)) {
            return false;
        }
    }
    if (FD_ISSET(theHandle, &myWriteSet)) {
        if (!sendData(theHandle, theOutQueue)) {
            return false;
        }
    }
    return true;
}

bool
SocketPolicy::receiveData(Handle & theHandle, BufferQueue & theInQueue)
{
    CharBufferPtr myInputBuffer = CharBufferPtr(new CharBuffer(1024));
 
    int bytesread = recv(theHandle, &((*myInputBuffer)[0]), myInputBuffer->size(), 0);
    if (bytesread == 0) {
        // Client disconnected gracefully
        return false;
    } else if (bytesread < 0) {
        int lastError = getLastSocketError();
        switch (lastError) {
#ifdef WIN32
            case WSAECONNABORTED :
            case WSAECONNRESET :
#else
            case EINTR:
#endif
                return false;
                break;
            default:
                throw ConduitException(string("SocketPolicy::Receive failed - ")+
                    getSocketErrorMessage(lastError), PLUS_FILE_LINE);
        }
    }
    myInputBuffer->resize(bytesread);
    theInQueue.push_back(myInputBuffer);
    DB(AC_TRACE << "recv " << bytesread << " bytes " << endl);
    return true;
}

bool
SocketPolicy::sendData(Handle & theHandle, BufferQueue & theOutQueue)
{
    DB(AC_TRACE << "sendData " << endl);
    if (theOutQueue.empty()) {
        return true;
    }

    CharBufferPtr myOutBuffer = theOutQueue.front();
    theOutQueue.pop_front();
    
    int byteswritten=::send(theHandle, &((*myOutBuffer)[0]), myOutBuffer->size(), 0);
    if (byteswritten < 0)
    {
        throw ConduitException(string("SocketPolicy::send failed - ") +
                getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
    }
    DB(AC_TRACE << "sent " << byteswritten << " bytes " << endl);
    if (byteswritten < myOutBuffer->size()) {
        myOutBuffer->erase(myOutBuffer->begin(), myOutBuffer->begin()+byteswritten);
        theOutQueue.push_front(myOutBuffer);
    }
    return true;
}

}
