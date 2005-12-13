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
//   $RCSfile: UnixSocketPolicy.h,v $
//   $Author: david $
//   $Revision: 1.4 $
//   $Date: 2005/01/07 16:27:18 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#ifndef __asl_UnixSocketPolicy_included
#define __asl_UnixSocketPolicy_included

#include "SocketPolicy.h"

#include <asl/INetEndpoint.h>
#include <asl/Exception.h>

#include <sys/socket.h>
#include <sys/un.h>

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

class UnixAddress : public sockaddr_un {
    public:
        UnixAddress(const std::string & thePath);
        UnixAddress(const char * thePath);
        UnixAddress();
        static const std::string PIPE_PREFIX;
};
    
class UnixSocketPolicy : public SocketPolicy {
    public:
        typedef UnixAddress Endpoint;
        /// Creates a (client) conduit connected to the remote endpoint
        // @throws ConduitException
        static Handle connectTo(Endpoint theRemoteEndpoint);

        // Acceptor methods
        static Handle startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount);
        static void stopListening(Handle theHandle);
};
/* @} */
}
#endif
