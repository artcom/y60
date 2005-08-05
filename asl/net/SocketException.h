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
//    $RCSfile: SocketException.h,v $
//
//     $Author: martin $
//
//   $Revision: 1.7 $
//
// Description: 
//
//
//
//=============================================================================


#ifndef SocketException_class
#define SocketException_class

#include <string>
#include <asl/Exception.h>

#ifdef WIN32
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

