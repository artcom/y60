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
//    documentation           :      poor
//    test coverage           :      ok
//    names                   :      fair
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
//    recommendations: change namespace, fix names
*/

#ifndef SocketException_class
#define SocketException_class

#include "asl_net_settings.h"

#include <string>
#include <asl/base/Exception.h>

#ifdef _WIN32
#include <winsock2.h>

#define OS_SOCKET_ERROR(errcode) WSA##errcode
#else
#define OS_SOCKET_ERROR(errcode) errcode
#endif

namespace inet {

class SocketException : public asl::Exception
{
public:
    SocketException (const std::string & where);
 
    SocketException (int theErrorCode, const std::string & where);
 
    SocketException (int theErrorCode, const std::string & what, const std::string & where) : 
        asl::Exception(what, where, "inet::SocketException" ), 
        _myErrorCode ( theErrorCode ) 
        {};
    ~SocketException (){};

    int getErrorCode() const { return _myErrorCode; }
private:
    int _myErrorCode;
};

}
#endif

