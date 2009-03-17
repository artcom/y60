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
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      ok
//    documentation           :      poor
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
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
//    recommendations: change namespace, explain use of this class
*/

#ifndef INCL_INET_ENDPOINT
#define INCL_INET_ENDPOINT

#include "asl_net_settings.h"

#include <asl/base/settings.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <netdb.h>
    #include <sys/socket.h>
#endif  

#ifdef OSX
    #include <netinet/in.h>
#endif

#include <ostream>

namespace asl {

    // This is a POD type. Don't add virtual functions.
    class ASL_NET_DECL INetEndpoint: public sockaddr_in {
        public:
            INetEndpoint(const char * myHost, asl::Unsigned16 myPort);
            INetEndpoint(asl::Unsigned32 myHost, asl::Unsigned16 myPort);
            INetEndpoint();
            std::ostream & print(std::ostream & os) const;
    };

    std::ostream & operator << (std::ostream & os, const INetEndpoint & theEndpoint);
}
#endif
