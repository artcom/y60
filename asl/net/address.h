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
//    documentation           :      disaster
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
//    recommendations: merge with net_functions.h, translate german comments to english 
*/

#ifndef INCL_INET_ADDRESS
#define INCL_INET_ADDRESS

#include "asl_net_settings.h"

#include "SocketException.h"

#include <asl/base/settings.h>

#ifdef _WIN32
  #include <winsock2.h>
#else
  #include <netdb.h>
  #include <sys/socket.h>
#endif  

namespace inet {

    /// Liefert eine asl::Unsigned32 aus einem string vom format a.b.c.d
    ASL_NET_EXPORT asl::Unsigned32 addrFromStr(const char *dotaddr);
    /// Schreibt in einen String eine dezimal dotted adresse a.b.c.d,
    // kann bis zu 16 bytes lang sein
    ASL_NET_EXPORT void addr2Str(asl::Unsigned32 hostaddr, char *dotaddrdest);
    /// Schreibt in einen String eine dezimal dotted adresse a.b.c.d,
    ASL_NET_EXPORT std::string addr2Str(asl::Unsigned32 hostaddr);

    /// Schreibt den von gethostbyname(3N) gelieferten String nach namedest
    // und liefert true
    // Wenn hostname nicht bekannt, wird <NAME UNKNOWN> eingetragen und
    // false zurueckgeliefert.
    ASL_NET_EXPORT int getHostName(asl::Unsigned32 hostaddr, char *namedest, int maxnamelen );

    /// returns the value of gethostbyname(3N) in a string.
    // @throws SocketException
    ASL_NET_EXPORT std::string getHostName(asl::Unsigned32 hostaddr);
    /// returns the host-address from a hostname or a dotted-quad address.
    // @throws SocketException
    ASL_NET_EXPORT asl::Unsigned32 getHostAddress(const char *hostspec);
    /// Liefert die Adresse des eigenen Hosts zurueck
    ASL_NET_EXPORT asl::Unsigned32 getLocalAddr();
    
}
#endif
