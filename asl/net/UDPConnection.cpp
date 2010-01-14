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
//    documentation           :      fair
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      fair
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
//    recommendations: change namespace, styleguide conformance
*/

//own header
#include "UDPConnection.h"

#include "SocketException.h"
#include "net.h"

#include <errno.h>
#ifndef _WIN32
#include <unistd.h>
#endif

namespace inet {

    UDPConnection::UDPConnection(asl::Unsigned32 thehost, asl::Unsigned16 theport):UDPSocket(thehost, theport)
    {
    }

    bool UDPConnection::connect(asl::Unsigned32 thehost, asl::Unsigned16 theport)
    {
        setRemoteAddr(thehost, theport);

        if ( ::connect(fd,(struct sockaddr*)&_myRemoteEndpoint,sizeof(_myRemoteEndpoint)) != 0 ) {
            int err = getLastSocketError();
            throw SocketError(err, "UDPConnection::Connect()");
        }
        return true;
    }

}
