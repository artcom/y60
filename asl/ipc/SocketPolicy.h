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
#ifndef __asl_SocketPolicy_included
#define __asl_SocketPolicy_included

#include "ConduitPolicy.h"

#include <asl/INetEndpoint.h>
#include <asl/Exception.h>

namespace asl {

/*! \addtogroup ipcPolicies */
/* @{ */

//! Abstract base class for all socket-based conduit policies.  
    
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
/* @{ */
}
#endif
