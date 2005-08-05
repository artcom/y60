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
//   $RCSfile: SocketPolicy.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2004/10/17 16:12:10 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#ifndef __asl_SocketPolicy_included
#define __asl_SocketPolicy_included

#include "ConduitPolicy.h"

#include <asl/Endpoint.h>
#include <asl/Exception.h>

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

class SocketPolicy : public ConduitPolicy {
    public:
        typedef int Handle; 
      
        static void disconnect(Handle & theHandle);
        /// Handles pending IO operations - should be called periodically (e.g. per frame) 
        // @returns true if the conduit is still valid 
        // @throws ConduitException
        static bool 
        handleIO(Handle & theHandle, BufferQueue & theInQueue, BufferQueue & theOutQueue, int theTimeout = 0);
        // Acceptor methods
        static void stopListening(Handle theHandle);
        static Handle createOnConnect(Handle & theListenHandle, unsigned theMaxConnectionCount, 
                int theTimeout);
    protected:
        static bool receiveData(Handle & theHandle, BufferQueue & theInQueue);
        static bool sendData(Handle & theHandle, BufferQueue & theOutQueue);
};
/* @} */
}
#endif
