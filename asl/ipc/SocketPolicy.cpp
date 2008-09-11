/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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

#include "SocketPolicy.h"

#include <asl/net/net.h>
#include <asl/base/string_functions.h>
#include <asl/base/Logger.h>

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
    ::shutdown(theHandle, SHUT_RDWR);
    ::close(theHandle);
#endif
    theHandle = 0;
}

void 
SocketPolicy::stopListening(Handle theHandle) {
#ifdef WIN32        
    closesocket(theHandle);
#else
    ::shutdown(theHandle, SHUT_RDWR);
    ::close(theHandle);
#endif
    theHandle = 0;
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
#ifdef SO_NOSIGPIPE // turn off SIGPIPE on systems supporting this socket option    
    socklen_t boolTrue = 1;
    if(setsockopt(newFD, SOL_SOCKET, SO_NOSIGPIPE, (void *)&boolTrue, sizeof(boolTrue)) < 0) {
        throw ConduitException(string("Could not set SO_NOSIGPIPE: ")+
                getSocketErrorMessage(getLastSocketError()), PLUS_FILE_LINE);
    }
#endif
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
            disconnect(theHandle);
            return false;
        }
    }
    if (FD_ISSET(theHandle, &myWriteSet)) {
        if (!sendData(theHandle, theOutQueue)) {
            disconnect(theHandle);
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
#ifdef MSG_NOSIGNAL // prevent SIGPIPE on systems supporting this flag    
    int byteswritten=::send(theHandle, &((*myOutBuffer)[0]), myOutBuffer->size(), MSG_NOSIGNAL);
#else
    int byteswritten=::send(theHandle, &((*myOutBuffer)[0]), myOutBuffer->size(), 0);
#endif    
    if (byteswritten < 0)
    {
        int myLastError = getLastSocketError();
        switch (myLastError) {
			case EPIPE:{
                AC_WARNING << "broken pipe: send failed";
				return false;}
            default:
                throw ConduitException(string("SocketPolicy::send failed - ") +
                    getSocketErrorMessage(myLastError), PLUS_FILE_LINE);
        }
    }
    DB(AC_TRACE << "sent " << byteswritten << " bytes " << endl);
    if (byteswritten < myOutBuffer->size()) {
        myOutBuffer->erase(myOutBuffer->begin(), myOutBuffer->begin()+byteswritten);
        theOutQueue.push_front(myOutBuffer);
    }
    return true;
}

}
