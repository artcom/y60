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
//   $RCSfile: TCPPolicy.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/10/17 16:12:10 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#ifndef __asl_TCPPolicy_included
#define __asl_TCPPolicy_included

#include "SocketPolicy.h"

#include <asl/INetEndpoint.h>
#include <asl/Exception.h>

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

class TCPPolicy : public SocketPolicy {
    public:
        typedef INetEndpoint asl::Endpoint; 
        /// Creates a (client) conduit connected to the remote endpoint
        // @throws ConduitException
        static Handle connectTo(Endpoint theRemoteEndpoint);

        // Acceptor methods
        static Handle startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount);
};
/* @} */
}
#endif
