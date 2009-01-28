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
//    recommendations: merge with net_functions.h
*/

//own header
#include "net.h"

#include "SocketException.h"

#include <asl/base/string_functions.h>
#include <asl/base/Logger.h>

#include <cstring>
#include <algorithm>

#ifdef _WIN32
  #include <winsock2.h>
#else
  #include <errno.h>
#endif  

using namespace std;

namespace inet {
    void initSockets() {
#ifdef _WIN32
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD( 2, 2 );
         
        AC_DEBUG << "Calling WSAStartup().";
        err = WSAStartup( wVersionRequested, &wsaData );
        if ( err != 0 ) {
            throw SocketException(err, "inet::initSockets()");
        }
        
        if (LOBYTE( wsaData.wVersion ) != 2 ||
            HIBYTE( wsaData.wVersion ) != 2 )
        {
            throw SocketException("No appropriate winsock DLL version found. inet::initSockets()");
        }
#endif
    }
    
    void terminateSockets() {
#ifdef _WIN32
        AC_DEBUG << "Calling WSACleanup().";
        if ( WSACleanup() != 0 ) {
            int err = getLastSocketError();
            throw SocketException(err, "inet::terminateSockets()");
        }
#endif 
    }

    int 
    getLastSocketError() {
#ifdef _WIN32
        return WSAGetLastError();
#else
        return errno;
#endif                
    }
    // List of error constants mapped to an interpretation string.
    // Note that this list must remain sorted by the error constants'
    // values, because we do a binary search on the list when looking up
    // items.
    struct SocketError {
        int ID;
        const char* Message;

        SocketError(int id, const char* msg = 0) : 
            ID(id), 
            Message(msg) 
        { 
        }

        bool operator<(const SocketError& rhs) const {
            return ID < rhs.ID;
        }
    };

#ifdef _WIN32
    SocketError SocketErrorList[] = {
        SocketError(0,                  "No error"),
        SocketError(WSAEINTR,           "Interrupted system call"),
        SocketError(WSAEBADF,           "Bad file number"),
        SocketError(WSAEACCES,          "Permission denied"),
        SocketError(WSAEFAULT,          "Bad address"),
        SocketError(WSAEINVAL,          "Invalid argument"),
        SocketError(WSAEMFILE,          "Too many open sockets"),
        SocketError(WSAEWOULDBLOCK,     "Operation would block"),
        SocketError(WSAEINPROGRESS,     "Operation now in progress"),
        SocketError(WSAEALREADY,        "Operation already in progress"),
        SocketError(WSAENOTSOCK,        "Socket operation on non-socket"),
        SocketError(WSAEDESTADDRREQ,    "Destination address required"),
        SocketError(WSAEMSGSIZE,        "Message too long"),
        SocketError(WSAEPROTOTYPE,      "Protocol wrong type for socket"),
        SocketError(WSAENOPROTOOPT,     "Bad protocol option"),
        SocketError(WSAEPROTONOSUPPORT, "Protocol not supported"),
        SocketError(WSAESOCKTNOSUPPORT, "Socket type not supported"),
        SocketError(WSAEOPNOTSUPP,      "Operation not supported on socket"),
        SocketError(WSAEPFNOSUPPORT,    "Protocol family not supported"),
        SocketError(WSAEAFNOSUPPORT,    "Address family not supported"),
        SocketError(WSAEADDRINUSE,      "Address already in use"),
        SocketError(WSAEADDRNOTAVAIL,   "Can't assign requested address"),
        SocketError(WSAENETDOWN,        "Network is down"),
        SocketError(WSAENETUNREACH,     "Network is unreachable"),
        SocketError(WSAENETRESET,       "Net connection reset"),
        SocketError(WSAECONNABORTED,    "Software caused connection abort"),
        SocketError(WSAECONNRESET,      "Connection reset by peer"),
        SocketError(WSAENOBUFS,         "No buffer space available"),
        SocketError(WSAEISCONN,         "Socket is already connected"),
        SocketError(WSAENOTCONN,        "Socket is not connected"),
        SocketError(WSAESHUTDOWN,       "Can't send after socket shutdown"),
        SocketError(WSAETOOMANYREFS,    "Too many references, can't splice"),
        SocketError(WSAETIMEDOUT,       "Connection timed out"),
        SocketError(WSAECONNREFUSED,    "Connection refused"),
        SocketError(WSAELOOP,           "Too many levels of symbolic links"),
        SocketError(WSAENAMETOOLONG,    "File name too long"),
        SocketError(WSAEHOSTDOWN,       "Host is down"),
        SocketError(WSAEHOSTUNREACH,    "No route to host"),
        SocketError(WSAENOTEMPTY,       "Directory not empty"),
        SocketError(WSAEPROCLIM,        "Too many processes"),
        SocketError(WSAEUSERS,          "Too many users"),
        SocketError(WSAEDQUOT,          "Disc quota exceeded"),
        SocketError(WSAESTALE,          "Stale NFS file handle"),
        SocketError(WSAEREMOTE,         "Too many levels of remote in path"),
        SocketError(WSASYSNOTREADY,     "Network system is unavailable"),
        SocketError(WSAVERNOTSUPPORTED, "Winsock version out of range"),
        SocketError(WSANOTINITIALISED,  "WSAStartup not yet called"),
        SocketError(WSAEDISCON,         "Graceful shutdown in progress"),
        SocketError(WSAHOST_NOT_FOUND,  "Host not found"),
        SocketError(WSANO_DATA,         "No host data of that type was found")
    };
    const int NumMessages = sizeof(SocketErrorList) / sizeof(SocketError);
#endif

    string 
    getSocketErrorMessage(int ErrorID)
    {
#ifdef _WIN32    
        string ErrorStr;

        // Tack appropriate canned message onto end of supplied message 
        // prefix. Note that we do a binary search here: gaErrorList must be
        // sorted by the error constant's value.
        SocketError* End = SocketErrorList + NumMessages;
        SocketError Target(ErrorID);
        SocketError* it = lower_bound(SocketErrorList, End, Target);
        if ((it != End) && (it->ID == Target.ID)) {
            ErrorStr = it->Message;
        }
        else {
            // Didn't find error in list, so make up a generic one
            ErrorStr = "unknown error";
        }
        ErrorStr += string(" (") + asl::as_string(Target.ID) + ")";

        // Finish error message off and return it.
        return ErrorStr;
#else
        return strerror(ErrorID);
#endif                
    }
}
