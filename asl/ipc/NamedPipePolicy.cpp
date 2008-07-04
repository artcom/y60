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

#include "NamedPipePolicy.h"

#include <asl/error_functions.h>
#include <asl/string_functions.h>
#include <asl/Logger.h>

#include <string.h>
#include <pthread.h>

using namespace std;

static HANDLE ourServerThreadID = 0;

//#define DBT(x) { std::ostringstream s; s << "TRACE:" << x; std::cout << s.str(); }
#define DBT(x) {AC_DEBUG << x;}

namespace asl {


const int CONNECTION_LOST = -1;
const int RECEIVED_DATA = 1;
const int STILL_WAITING = 0;

const int BUFFER_SIZE=1024;
const string PIPE_PREFIX = "\\\\.\\pipe\\ART+COM_";

NamedPipePolicy::Handle 
NamedPipePolicy::connectTo(Endpoint theRemoteEndpoint) {
    string myPipeName = PIPE_PREFIX+theRemoteEndpoint;

    HANDLE hPipe = INVALID_HANDLE_VALUE;
    if (WaitNamedPipe(myPipeName.c_str(), NMPWAIT_USE_DEFAULT_WAIT)) {
        hPipe = CreateFile( 
                myPipeName.c_str(),   // pipe name 
                GENERIC_READ |  // read and write access 
                GENERIC_WRITE, 
                0,              // no sharing 
                NULL,           // default security attributes
                OPEN_EXISTING,  // opens existing pipe 
                FILE_FLAG_OVERLAPPED, // default attributes 
                NULL);          // no template file 
    }
    if (hPipe == INVALID_HANDLE_VALUE) {
        throw ConduitException(string("NamedPipePolicy::ctor: connect - ") + 
                asl::errorDescription(lastError()), PLUS_FILE_LINE);
    }
    Handle myNewHandle(hPipe, theRemoteEndpoint);
    DBT(myNewHandle << " connected as client" << endl);
    return myNewHandle;
}

void
NamedPipePolicy::disconnect(Handle & theHandle) {
    DBT(theHandle << " disconnecting." << endl);
    CancelIo(theHandle.pipeInstance);
    if (theHandle.isServerHandle) {
        DisconnectNamedPipe(theHandle.pipeInstance);
    }
    CloseHandle(theHandle.pipeInstance);
    theHandle.pipeInstance = 0;
}

NamedPipePolicy::Handle 
NamedPipePolicy::createOnConnect(Handle & theListenHandle, unsigned theMaxConnectionCount, 
        int theTimeout) 
    {
    if (theListenHandle.accept_overlap.hEvent == 0) {
        theListenHandle.accept_overlap.hEvent = CreateEvent(NULL, // default security attributes 
                                       TRUE, // manual-reset event
                                       TRUE, // initial signaled state
                                       NULL);// unnamed object event

    }
    SetEvent(theListenHandle.accept_overlap.hEvent);
	theListenHandle.accept_overlap.Internal= 0;
	theListenHandle.accept_overlap.InternalHigh = 0;
	theListenHandle.accept_overlap.Offset = 0;
	theListenHandle.accept_overlap.OffsetHigh = 0;
    if (ConnectNamedPipe(theListenHandle.pipeInstance, &(theListenHandle.accept_overlap))) {
        throw ConduitException(string("NamedPipePolicy::ConnectNamePipe failed - ")+
                errorDescription(lastError()), PLUS_FILE_LINE);
    }
    switch (int myLastError = GetLastError()) {
        case ERROR_PIPE_CONNECTED :
            break; // ok, we are connected
        case ERROR_IO_PENDING:
			{
				DWORD myDummy;
				int myRetVal;
            	do {
		            myRetVal = waitForOverlapped(theListenHandle, 
					    &myDummy , false, theTimeout);
		            pthread_testcancel();
	            } while (myRetVal == STILL_WAITING);

                if (myRetVal != RECEIVED_DATA) {
					return Handle();
				}
				break;
			}
        case ERROR_NO_DATA :
            return Handle();
        default:
            throw ConduitException(string("NamedPipePolicy::createOnConnect - ")+
                    asl::errorDescription(myLastError), PLUS_FILE_LINE);
    }
    if (!ourServerThreadID) {
        ourServerThreadID = theListenHandle.pipeInstance;
    }
    Handle myNewConnection = theListenHandle;
    myNewConnection.isServerHandle = true;
    // now make a new listener handle to hand back to the server
    // so it can keep listening
    DBT(myNewConnection << " is now a server" << endl);

    theListenHandle = createListenHandle(myNewConnection.pipeName, theMaxConnectionCount, false); 
    return myNewConnection;
}

bool
NamedPipePolicy::handleIO(Handle & theHandle, BufferQueue & theInQueue, BufferQueue & theOutQueue, int theTimeout) {
    if (!theHandle.isValid) {
        return false;
    }
    if (!theHandle.isSending && !theOutQueue.empty()) {
        theHandle.outQueue = &theOutQueue;
        sendNextBuffer(theHandle);
    }
    if (!theHandle.isReceiving) {
        theHandle.inQueue = &theInQueue;
        receiveNextBuffer(theHandle);
    }
    SleepEx(theTimeout, true);
    return theHandle.isValid;
}

int 
NamedPipePolicy::waitForOverlapped(Handle & theHandle,
        LPDWORD theBytesTransferred, bool isSending, int theTimeout) 
{
    bool myTryAgainFlag = true;
    int myLastError = ERROR_SUCCESS;
    while (myTryAgainFlag) {
        switch (WaitForSingleObjectEx(theHandle.accept_overlap.hEvent, 
                    theTimeout >=0 ? theTimeout : INFINITE, true)) 
        {
            case WAIT_IO_COMPLETION :
                {
                    // int myLastError = lastError();
                    myTryAgainFlag = true;
                }
            case WAIT_TIMEOUT :
                return STILL_WAITING;
            case WAIT_OBJECT_0 :
                if (GetOverlappedResult(theHandle.pipeInstance, &theHandle.accept_overlap, 
                            theBytesTransferred, FALSE))
                {
                    CloseHandle(theHandle.accept_overlap.hEvent);
                    theHandle.accept_overlap.hEvent = 0;
                    return RECEIVED_DATA;
                }
                break;
            default:
                throw ConduitException(string("NamedPipePolicy::createOnConnect - ")+
                        asl::errorDescription(lastError()), PLUS_FILE_LINE);
        }

        myLastError = GetLastError();
        if (myLastError == ERROR_BROKEN_PIPE) {
            return CONNECTION_LOST;
        }
    }
    throw ConduitException(string("NamedPipePolicy::waitForOverlapped: ")+
            as_string(myLastError) + asl::errorDescription(myLastError), PLUS_FILE_LINE);
}

void 
NamedPipePolicy::receiveNextBuffer(Handle & theHandle) {
    if (theHandle.isReceiving) {
        throw ConduitException(string("NamedPipePolicy::receiveNextBuffer failed - recv already in progress!"),
                 PLUS_FILE_LINE);
    }
    theHandle.isReceiving = true;
    theHandle.inBuffer.resize(1024);
    LPOVERLAPPED myOverlap = new OVERLAPPED;
    memset(myOverlap, 0, sizeof(OVERLAPPED));
    DBT(theHandle << "ReadFileEx with overlap @ " << myOverlap << endl);   
    myOverlap->hEvent = &theHandle;
    ReadFileEx( 
         theHandle.pipeInstance,        // handle to pipe 
         &(theHandle.inBuffer[0]),      // buffer to write from 
         theHandle.inBuffer.size(), // number of bytes to write 
         myOverlap,     
         NamedPipePolicy::onReadCompleted);
    int myLastError = lastError();
    switch (myLastError) {
        case ERROR_SUCCESS :
            break;
        case ERROR_BROKEN_PIPE :
            theHandle.isValid = false;            
            break;
        default:
            DBT(theHandle << " ReadFileEx failed: " << myLastError << endl);
            throw ConduitException(string("NamedPipePolicy::ReadFileEx returned ")+as_string(myLastError) 
                +" - " + errorDescription(myLastError), PLUS_FILE_LINE);
    }
}

void 
NamedPipePolicy::sendNextBuffer(Handle & theHandle) {
    if (theHandle.isSending) {
        throw ConduitException(string("NamedPipePolicy::sendNextBuffer failed - send already in progress!"),
                 PLUS_FILE_LINE);
    }
    if (theHandle.outQueue->empty()) {
        return;
    }
    theHandle.isSending = true;
    if (!theHandle.outBuffer) {
        theHandle.outBuffer = theHandle.outQueue->front();
        theHandle.outQueue->pop_front();
    }

    LPOVERLAPPED mySendOverlap = new OVERLAPPED;
    memset(mySendOverlap, 0, sizeof(OVERLAPPED));

    DBT(theHandle << " starting to send '" << string(&((*theHandle.outBuffer)[0]), theHandle.outBuffer->size()) << "'" << " with overlap @" << mySendOverlap << endl);
    mySendOverlap->hEvent = &theHandle; // we can save user data here
    WriteFileEx( 
         theHandle.pipeInstance,        // handle to pipe 
         &((*theHandle.outBuffer)[0]),      // buffer to write from 
         theHandle.outBuffer->size(), // number of bytes to write 
         mySendOverlap,   // number of bytes written 
         NamedPipePolicy::onWriteCompleted);
    int myLastError = lastError();
    switch (myLastError) {
        case ERROR_SUCCESS :
            break;
        case ERROR_NO_DATA :
            theHandle.isValid = false;            
            break;
        default:
            DBT(theHandle << " WriteFileEx failed: " << myLastError << endl);
            throw ConduitException(string("NamedPipePolicy::sendNextBuffer - ")+
                    errorDescription(myLastError), PLUS_FILE_LINE);
    }
}

void WINAPI
NamedPipePolicy::onReadCompleted(DWORD theError, DWORD theBytesTransferred, LPOVERLAPPED theOverlap) {

    Handle * myHandle = reinterpret_cast<Handle*>(theOverlap->hEvent);
    delete theOverlap;

    switch (theError) {
        case ERROR_SUCCESS :
            break;
        case ERROR_BROKEN_PIPE :
            DBT(*myHandle << " pipe broken on reading, Overlap@" << theOverlap << endl);
            myHandle->isValid = false;
            return;
        case ERROR_OPERATION_ABORTED :
            DBT(*myHandle << " Operation canceled " << endl);
            return;
        default :            
            DBT(*myHandle << "onReadCompleted failed: (" << theError << ") " << errorDescription(theError)<< endl);
            throw ConduitException(string("NamedPipePolicy::onReadCompleted - ")+
                errorDescription(theError), PLUS_FILE_LINE);
    }
    DBT(*myHandle << " received '" << string(&(myHandle->inBuffer[0]), theBytesTransferred) << 
            "', length=" << theBytesTransferred <<", Overlap@" << theOverlap << endl);
    if (theBytesTransferred > 0) {
        char * myBufferStart = &(myHandle->inBuffer[0]);
        myHandle->inQueue->push_back(CharBufferPtr(new CharBuffer(myBufferStart, myBufferStart+theBytesTransferred)));
    }
    myHandle->isReceiving = false;
    // receiveNextBuffer(*myHandle);
}
void WINAPI
NamedPipePolicy::onWriteCompleted(DWORD theError, DWORD theBytesTransferred, LPOVERLAPPED theOverlap) {
    Handle * myHandle = reinterpret_cast<Handle*>(theOverlap->hEvent);
    delete theOverlap;

    switch (theError) {
        case ERROR_SUCCESS :
            break;
        case ERROR_OPERATION_ABORTED :
			{AC_WARNING << *myHandle << " pipe broken on writing" << endl;}
            return;
        case ERROR_BROKEN_PIPE :
            DBT(*myHandle << " pipe broken on writing" << endl);
            myHandle->isValid = false;
            return;
        default :            
            DBT(*myHandle << "onWriteCompleted failed: " << theError << endl);
            throw ConduitException(string("NamedPipePolicy::onWriteCompleted - [")+asl::as_string(theError)+"] "+
                errorDescription(theError), PLUS_FILE_LINE);
    }
    DBT(*myHandle << " sent '" << string(&((*myHandle->outBuffer)[0]), theBytesTransferred) << "' Overlap@" << theOverlap << endl);
    if (theBytesTransferred < myHandle->outBuffer->size()) {
        // not everything was transferred, reinsert the rest of the buffer
        char * myBufferStart = &((*myHandle->outBuffer)[0]);
        myHandle->outBuffer->erase(myBufferStart, myBufferStart+theBytesTransferred);
    } else {
        myHandle->outBuffer = asl::Ptr<CharBuffer>(0);
    }
    myHandle->isSending = false;
    //sendNextBuffer(*myHandle);
}
// ///////////////////////////////////////////////////////////
//
// Acceptor methods
//
// ///////////////////////////////////////////////////////////
NamedPipePolicy::Handle 
NamedPipePolicy::createListenHandle(Endpoint theEndpoint, unsigned theMaxConnectionCount, bool theMasterListener) {
    string myPipeName = PIPE_PREFIX+theEndpoint;
    DWORD myOpenMode = PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED;
    if (theMasterListener) {
        myOpenMode |= FILE_FLAG_FIRST_PIPE_INSTANCE;
    }
    HANDLE myHandle=CreateNamedPipe(myPipeName.c_str(), myOpenMode, 
            PIPE_TYPE_BYTE | PIPE_WAIT, theMaxConnectionCount, BUFFER_SIZE, BUFFER_SIZE, 0, 0);

    if (myHandle == INVALID_HANDLE_VALUE) {
        int myLastError = lastError();
        switch (myLastError) {
            case ERROR_ACCESS_DENIED:
                throw ConduitInUseException(string("NamedPipePolicy::NamedPipePolicy (")+
                    as_string(myLastError) + ") - " + errorDescription(myLastError), PLUS_FILE_LINE);
            default:
                throw ConduitException(string("NamedPipePolicy::NamedPipePolicy (")+
                    as_string(myLastError) + ") - " + errorDescription(myLastError), PLUS_FILE_LINE);
        }
    }
    Handle myListener(myHandle, theEndpoint);
    DBT(myListener << " new Listener created" << endl);
    return myListener;
}

NamedPipePolicy::Handle 
NamedPipePolicy::startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount) {
    return createListenHandle(theEndpoint, theMaxConnectionCount, true);
}


void 
NamedPipePolicy::stopListening(Handle theHandle) {
    CloseHandle(theHandle.pipeInstance);
    if (theHandle.accept_overlap.hEvent) {
        CloseHandle(theHandle.accept_overlap.hEvent);
        theHandle.accept_overlap.hEvent = 0;
    }
}
}

