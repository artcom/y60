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
//    recommendations: change namespace, fix doxygen doctags,
//                     explain what this class does (what are UDP connections?)
//                     shouldn't the example be in the test?
*/

#ifndef INCL_INET_UDPCONNECTION
#define INCL_INET_UDPCONNECTION

#include "UDPSocket.h"

#include <asl/Ptr.h>

namespace inet {
    /** implements connected UDP 
     *
     * @code
     *
     * try {
     *     myUDPConnection = new UDPConnection(myPort, INADDR_ANY);
     *     break;
     * }
     * catch (SocketException & se)
     * {
     *     AC_ERROR << "failed to listen on port " << myPort << ":" << se.where() << endl;
     *     myUDPConnection = 0;
     * }
     *
     * inet::asl::Unsigned32 otherAddress = inet::Socket::getHostAddress("www.anyhost.de");
     * 
     * myUDPConnection.connect(otherPort, otherAddress);
     * 
     * myUDPConnection.read(myInputBuffer,bufferSize);
     * myUDPConnection.write(myOutputBuffer, bufferSize);
     *
     * @endcode
     */
    class UDPConnection : public UDPSocket {
        public:
            UDPConnection(asl::Unsigned32 thehost, asl::Unsigned16 theport);
            virtual bool connect(asl::Unsigned32 thehost, asl::Unsigned16 theport);
    };

    typedef asl::Ptr<UDPConnection> UDPConnectionPtr;
    
}
#endif
