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
//    recommendations: change namespace
//                     explain use of this class
*/

#include "INetEndpoint.h"
#include "net_functions.h"

#include <string.h>

namespace asl {
    INetEndpoint::INetEndpoint(const char * theHost, Unsigned16 myPort) {
        memset(this, 0, sizeof(*this));
        sin_family=AF_INET;
        sin_addr.s_addr=htonl(asl::hostaddress(theHost));
        sin_port=htons(myPort);
    }

    INetEndpoint::INetEndpoint(Unsigned32 theHost, Unsigned16 myPort) {
        memset(this, 0, sizeof(*this));
        sin_family=AF_INET;
        sin_addr.s_addr=htonl(theHost);
        sin_port=htons(myPort);
    }

    INetEndpoint::INetEndpoint() {
        memset(this, 0, sizeof(*this));
        sin_family=AF_INET;
    }
    std::ostream & 
    INetEndpoint::print(std::ostream & os) const {
        return os << as_dotted_address(ntohl(sin_addr.s_addr)) << ":" << ntohs(sin_port);
    }

    std::ostream & operator << (std::ostream & os, const INetEndpoint & theEndpoint) {
        return theEndpoint.print(os);
    }
}
