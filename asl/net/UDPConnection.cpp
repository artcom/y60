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

#include "UDPConnection.h"
#include "SocketException.h"
#include "net.h"

#include <errno.h>
#ifndef WIN32
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
            throw SocketException(err, "UDPConnection::Connect()");
        }
        return true;
    }

}
