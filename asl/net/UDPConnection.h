//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: UDPConnection.h,v $
//
//     $Author: pavel $
//
//   $Revision: 1.3 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================
#ifndef INCL_INET_UDPCONNECTION
#define INCL_INET_UDPCONNECTION

#include "UDPSocket.h"

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
}
#endif
