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
//   $RCSfile: NamedPipePolicy.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2004/10/17 16:12:09 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#ifndef __asl_NamedPipePolicy_included
#define __asl_NamedPipePolicy_included

#include "ConduitPolicy.h"
#include <asl/Ptr.h>

#include <string>
#include <vector>
#include <deque>

#include <windows.h>

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

    
class NamedPipePolicy : public ConduitPolicy {
    public:
        typedef std::string Endpoint; 

        struct PipeInfo {
            PipeInfo() :
                pipeInstance(0), pipeName(""),
                isServerHandle(false), isSending(false),
                outQueue(0), isReceiving(false), inQueue(0), isValid(false)
                {
                    memset(&accept_overlap,0,sizeof(OVERLAPPED));
                };
            PipeInfo(HANDLE theInstance, Endpoint theName) : 
                pipeInstance(theInstance), pipeName(theName),
                isServerHandle(false), isSending(false),
                outQueue(0), isReceiving(false), inQueue(0), isValid(true)
                {
                    memset(&accept_overlap,0,sizeof(OVERLAPPED));
                };
            inline operator bool () const { return pipeInstance!=0; };
            HANDLE pipeInstance;
            Endpoint pipeName;
            OVERLAPPED accept_overlap;
            bool isValid;
            bool isServerHandle;
            bool isSending;
            CharBufferPtr outBuffer;
            BufferQueue * outQueue;
            bool isReceiving;
            CharBuffer inBuffer;
            BufferQueue * inQueue;
        };
        typedef PipeInfo Handle;
        /// Creates a (client) conduit connected to the remote endpoint
        // @throws ConduitException
        static Handle connectTo(Endpoint theRemoteEndpoint);
        /// disconnects a conduit 
        // @throws ConduitException
        static void disconnect(Handle & theHandle);
        /// Handles pending IO operations - should be called periodically (e.g. per frame) 
        // @returns true if the conduit is still valid 
        // @throws ConduitException
        static bool 
        handleIO(Handle & theHandle, BufferQueue & theInQueue, BufferQueue & theOutQueue, int theTimeout = 0);

        // Acceptor methods
        static Handle startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount);
        static void stopListening(Handle theHandle);
        static Handle createOnConnect(Handle & theListenHandle, unsigned theMaxConnectionCount, 
                int theTimeout);

    private:
        static void WINAPI onWriteCompleted(DWORD theError, DWORD theBytesTransferred, LPOVERLAPPED theOverlap);
        static void WINAPI onReadCompleted(DWORD theError, DWORD theBytesTransferred, LPOVERLAPPED theOverlap);
        static void sendNextBuffer(Handle & theHandle);
        static void receiveNextBuffer(Handle & theHandle);
        static Handle createListenHandle(Endpoint theEndpoint, unsigned theMaxConnectionCount, bool theMasterListener);
        static int waitForOverlapped(Handle & theHandle, LPDWORD theBytesTransferred, 
                bool isSending, int theTimeout=-1); 
} ;

inline std::ostream& operator<<(std::ostream & s, const NamedPipePolicy::Handle & h) {
    s << (h.isServerHandle ? "SERVER(" : "CLIENT(") << h.pipeInstance << ")";
    return s;
}


/* @} */
} // namespace

#endif
