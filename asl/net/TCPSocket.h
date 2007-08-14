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
//    recommendations: change namespace, add doxygen doctags,
//                     explain use of this class, if abstract, make dtor pure virtual
*/

#ifndef INCL_INET_TCPSOCKET
#define INCL_INET_TCPSOCKET

#include "Socket.h"
#include "INetEndpoint.h"

namespace inet {

    class TCPSocket : public Socket {
        public:
            TCPSocket(asl::Unsigned32 thehost, asl::Unsigned16 theport);

            // Used by TCPServer.
            TCPSocket(int myFD, const asl::INetEndpoint & localEndpoint, 
                      const asl::INetEndpoint & remoteEndpoint);

            virtual void open();
    };
}
#endif
