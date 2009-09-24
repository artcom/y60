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
#include <asl/base/Exception.h>

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
            throw SocketError(err, "inet::initSockets()");
        }
        
        if (LOBYTE( wsaData.wVersion ) != 2 ||
            HIBYTE( wsaData.wVersion ) != 2 )
        {
            throw asl::Exception("No appropriate winsock DLL version found. inet::initSockets()", PLUS_FILE_LINE);
        }
#endif
    }
    
    void terminateSockets() {
#ifdef _WIN32
        AC_DEBUG << "Calling WSACleanup().";
        if ( WSACleanup() != 0 ) {
            int err = getLastSocketError();
            throw SocketError(err, "inet::terminateSockets()");
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
    struct SocketErrorString {
        int ID;
        const char* Message;

        SocketErrorString(int id, const char* msg = 0) :
            ID(id), 
            Message(msg) 
        { 
        }

        bool operator<(const SocketErrorString& rhs) const {
            return ID < rhs.ID;
        }
    };

#ifdef _WIN32
    SocketErrorMessage SocketErrorList[] = {
        SocketErrorString(0,                  "No error"),
        SocketErrorString(WSAEINTR,           "Interrupted system call"),
        SocketErrorString(WSAEBADF,           "Bad file number"),
        SocketErrorString(WSAEACCES,          "Permission denied"),
        SocketErrorString(WSAEFAULT,          "Bad address"),
        SocketErrorString(WSAEINVAL,          "Invalid argument"),
        SocketErrorString(WSAEMFILE,          "Too many open sockets"),
        SocketErrorString(WSAEWOULDBLOCK,     "Operation would block"),
        SocketErrorString(WSAEINPROGRESS,     "Operation now in progress"),
        SocketErrorString(WSAEALREADY,        "Operation already in progress"),
        SocketErrorString(WSAENOTSOCK,        "Socket operation on non-socket"),
        SocketErrorString(WSAEDESTADDRREQ,    "Destination address required"),
        SocketErrorString(WSAEMSGSIZE,        "Message too long"),
        SocketErrorString(WSAEPROTOTYPE,      "Protocol wrong type for socket"),
        SocketErrorString(WSAENOPROTOOPT,     "Bad protocol option"),
        SocketErrorString(WSAEPROTONOSUPPORT, "Protocol not supported"),
        SocketErrorString(WSAESOCKTNOSUPPORT, "Socket type not supported"),
        SocketErrorString(WSAEOPNOTSUPP,      "Operation not supported on socket"),
        SocketErrorString(WSAEPFNOSUPPORT,    "Protocol family not supported"),
        SocketErrorString(WSAEAFNOSUPPORT,    "Address family not supported"),
        SocketErrorString(WSAEADDRINUSE,      "Address already in use"),
        SocketErrorString(WSAEADDRNOTAVAIL,   "Can't assign requested address"),
        SocketErrorString(WSAENETDOWN,        "Network is down"),
        SocketErrorString(WSAENETUNREACH,     "Network is unreachable"),
        SocketErrorString(WSAENETRESET,       "Net connection reset"),
        SocketErrorString(WSAECONNABORTED,    "Software caused connection abort"),
        SocketErrorString(WSAECONNRESET,      "Connection reset by peer"),
        SocketErrorString(WSAENOBUFS,         "No buffer space available"),
        SocketErrorString(WSAEISCONN,         "Socket is already connected"),
        SocketErrorString(WSAENOTCONN,        "Socket is not connected"),
        SocketErrorString(WSAESHUTDOWN,       "Can't send after socket shutdown"),
        SocketErrorString(WSAETOOMANYREFS,    "Too many references, can't splice"),
        SocketErrorString(WSAETIMEDOUT,       "Connection timed out"),
        SocketErrorString(WSAECONNREFUSED,    "Connection refused"),
        SocketErrorString(WSAELOOP,           "Too many levels of symbolic links"),
        SocketErrorString(WSAENAMETOOLONG,    "File name too long"),
        SocketErrorString(WSAEHOSTDOWN,       "Host is down"),
        SocketErrorString(WSAEHOSTUNREACH,    "No route to host"),
        SocketErrorString(WSAENOTEMPTY,       "Directory not empty"),
        SocketErrorString(WSAEPROCLIM,        "Too many processes"),
        SocketErrorString(WSAEUSERS,          "Too many users"),
        SocketErrorString(WSAEDQUOT,          "Disc quota exceeded"),
        SocketErrorString(WSAESTALE,          "Stale NFS file handle"),
        SocketErrorString(WSAEREMOTE,         "Too many levels of remote in path"),
        SocketErrorString(WSASYSNOTREADY,     "Network system is unavailable"),
        SocketErrorString(WSAVERNOTSUPPORTED, "Winsock version out of range"),
        SocketErrorString(WSANOTINITIALISED,  "WSAStartup not yet called"),
        SocketErrorString(WSAEDISCON,         "Graceful shutdown in progress"),
        SocketErrorString(WSAHOST_NOT_FOUND,  "Host not found"),
        SocketErrorString(WSANO_DATA,         "No host data of that type was found")
    };
    const int NumMessages = sizeof(SocketErrorList) / sizeof(SocketErrorString);
#endif

    string 
    getSocketErrorMessage(int ErrorID)
    {
#ifdef _WIN32    
        string ErrorStr;

        // Tack appropriate canned message onto end of supplied message 
        // prefix. Note that we do a binary search here: gaErrorList must be
        // sorted by the error constant's value.
        SocketErrorString* End = SocketErrorList + NumMessages;
        SocketErrorString Target(ErrorID);
        SocketErrorString* it = lower_bound(SocketErrorList, End, Target);
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
