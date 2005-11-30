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
//    $RCSfile: UDPConnection.cpp,v $
//
//     $Author: ulrich $
//
//   $Revision: 1.6 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//============================================================================


#include "UDPConnection.h"
#include "SocketException.h"

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

#if (MAKE_OS_VARIANT) == LINUX || (MAKE_OS_VARIANT) == SUNOS
        if( ::connect(fd,(struct sockaddr*)&_myRemoteEndpoint,sizeof(_myRemoteEndpoint)) == 0 )
#else
        if( ::connect(fd,&_myRemoteEndpoint,sizeof(_myRemoteEndpoint)) == 0 )
#endif
        {
            return true;
        }
        throw SocketException("UDPConnection::Connect: can't connect");
        return false;
    }

}
