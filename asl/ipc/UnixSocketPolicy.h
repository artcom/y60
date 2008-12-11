/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
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
#ifndef __asl_UnixSocketPolicy_included
#define __asl_UnixSocketPolicy_included

#include "SocketPolicy.h"

#include <asl/net/INetEndpoint.h>
#include <asl/base/Exception.h>

#include <sys/socket.h>
#include <sys/un.h>

namespace asl {

/*! \addtogroup ipcPolicies */
/* @{ */

class UnixAddress : public sockaddr_un {
    public:
        UnixAddress(const std::string & thePath);
        UnixAddress(const char * thePath);
        UnixAddress();
        static const std::string PIPE_PREFIX;
        std::ostream & print(std::ostream & os) const;
};
std::ostream & operator << (std::ostream & os, const UnixAddress & theAddress);
  
/*! Conduit policy for Unix socket-based communication (POSIX only).
@note there is a typedef to asl::LocalPolicy (POSIX only)
*/

class UnixSocketPolicy : public SocketPolicy {
    public:
        typedef UnixAddress Endpoint;
        /// Creates a (client) conduit connected to the remote endpoint
        ///@throws ConduitException
        static Handle connectTo(Endpoint theRemoteEndpoint);

        // Acceptor methods
        static Handle startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount);
        static void stopListening(Handle theHandle);
};
/* @} */
}
#endif
