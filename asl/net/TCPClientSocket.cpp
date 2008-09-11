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
//    formatting              :      fair
//    documentation           :      ok
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
//    cheesyness              :      fair
//
//    overall review status   :      ok
//
//    recommendations: change namespace, fix unreferenced local variable work-around
*/

#include "TCPClientSocket.h"
#include "net.h"

#include <asl/base/Logger.h>
#include <asl/base/Time.h>

#ifndef WIN32
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

#include <errno.h>

using namespace asl;
using namespace std;

namespace inet {

    TCPClientSocket::TCPClientSocket()
        : TCPSocket(0, 0)
    {
    }


    void TCPClientSocket::connect()
    {
        open();
        
        if( ::connect(fd,(struct sockaddr*)&_myRemoteEndpoint,sizeof(_myRemoteEndpoint)) != 0 ) {
            int err = getLastSocketError();
            throw SocketException(err, "TCPClientSocket::ConnectSocket()");
        }
    }

    int TCPClientSocket::retry(int n)
    {
        for (int retry=1 ;retry<=n ; retry++ )
        {
            try {
                connect();
                return true;
            } catch (SocketException & se)
            {
                se;  // Avoid unreferenced variable warning.
                AC_DEBUG << retry << ". retry failed" << endl;
                msleep(2000);
            }
        }
        return false;
    }

}
