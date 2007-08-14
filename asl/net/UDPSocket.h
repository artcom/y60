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
//                     shouldn't the example be in the test?
*/

#ifndef INCL_INET_UDPSOCKET
#define INCL_INET_UDPSOCKET


#include "Socket.h"

namespace inet {

    /** implements connectionless UDP, may be used for UDP-broadcasts.
     *
     * @code
     *
     * try {
     *      // create a socket listening on MyPort
     *      myUDPSocket = new UDPSocket(INADDR_ANY, myPort);
     *      break;
     *  }
     *  catch (SocketException & se)
     *  {
     *      AC_ERROR << "failed to listen on port " << myPort << ":" << se.where() << endl;
     *      myUDPServer = 0;
     *  }
     *  ENSURE(myUDPSocket);
     *
     *  asl::Unsigned32 otherHost;
     *  asl::Unsigned16 otherPort;
     *  myUDPSocket->receiveFrom(&otherHost, &otherPort, myInputBuffer, sizeof(myInputBuffer));
     *  myUDPSocket->sendTo(broadcastAddress, otherPort, data, sizeof(data));
     *
     *  delete myUDPSocket;
     *  
     * @endcode
     *
     * @TODO: create a UDPMulticast subclass (should be easy now)
     **/
    class UDPSocket : public Socket {
        public:
            /// create a new socket listening on theport on address thehost
            UDPSocket(asl::Unsigned32 thehost, asl::Unsigned16 theport);
            
            virtual void open();
            /// receives data from any host.
            // theport and thehost will contain the origin of the data.
            // @returns number of bytes received
            virtual unsigned receiveFrom(asl::Unsigned32* thehost, 
                    asl::Unsigned16 * theport, void *data, const int maxlen); 
            /// sends data either to a host or to a broadcast address.
            virtual unsigned sendTo(asl::Unsigned32 thehost, 
                    asl::Unsigned16 theport, const void *data, unsigned len);
            virtual void reset();
    };
}
#endif
