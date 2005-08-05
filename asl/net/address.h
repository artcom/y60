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
//    $RCSfile: address.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
// Description: 
//
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//
//=============================================================================

#ifndef INCL_INET_ADDRESS
#define INCL_INET_ADDRESS

#include "SocketException.h"

#include <asl/settings.h>

#ifdef WIN32
  #include <winsock2.h>
#else
  #include <netdb.h>
  #include <sys/socket.h>
#endif  

namespace inet {

    /// Liefert eine asl::Unsigned32 aus einem string vom format a.b.c.d
    asl::Unsigned32 addrFromStr(const char *dotaddr);
    /// Schreibt in einen String eine dezimal dotted adresse a.b.c.d,
    // kann bis zu 16 bytes lang sein
    void addr2Str(asl::Unsigned32 hostaddr, char *dotaddrdest);
    /// Schreibt in einen String eine dezimal dotted adresse a.b.c.d,
    std::string addr2Str(asl::Unsigned32 hostaddr);

    /// Schreibt den von gethostbyname(3N) gelieferten String nach namedest
    // und liefert true
    // Wenn hostname nicht bekannt, wird <NAME UNKNOWN> eingetragen und
    // false zurueckgeliefert.
    int getHostName(asl::Unsigned32 hostaddr, char *namedest, int maxnamelen );

    /// returns the value of gethostbyname(3N) in a string.
    // @throws SocketException
    std::string getHostName(asl::Unsigned32 hostaddr);
    /// returns the host-address from a hostname or a dotted-quad address.
    // @throws SocketException
    asl::Unsigned32 getHostAddress(const char *hostspec);
    /// Liefert die Adresse des eigenen Hosts zurueck
    asl::Unsigned32 getLocalAddr();
    
}
#endif
