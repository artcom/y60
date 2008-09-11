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

#include "address.h"

#include <asl/base/Logger.h>

#ifdef LINUX
#include <unistd.h>
#endif
#ifdef OSX
#include <netinet/in.h>
#endif
#include <fcntl.h>

#include <stdio.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <assert.h>

using namespace std;  // automatically added!

namespace inet {

    string getHostName(asl::Unsigned32 hostaddr)
    {
        char myBuffer[256];

        int myResult = getHostName(hostaddr, myBuffer, sizeof(myBuffer));
        if (myResult != 0) {
            throw SocketException(myResult, "getHostName");
        } else {
            return string(myBuffer);
        }
    }

    int getHostName(asl::Unsigned32 hostaddr, char *namedest, int maxnamelen)
    {
        int myErrorCode = 0;

        asl::Unsigned32 nethostaddr = htonl(hostaddr); 
        struct hostent * myHostEnt = 0;

#ifdef LINUX
        // reentrant version
        char tempBuffer[1024];
        struct hostent myResultHost;
        if (gethostbyaddr_r ((char *)&nethostaddr, sizeof(nethostaddr),AF_INET,
                         &myResultHost, tempBuffer, sizeof(tempBuffer),
                         &myHostEnt, &myErrorCode) != 0 || myErrorCode != 0) {
            myHostEnt = 0;
        }
        else {
            myHostEnt = &myResultHost;
        }
        AC_DEBUG << "gethostbyaddr_r " << myErrorCode << endl;
#else
        myHostEnt = gethostbyaddr ((char *)&nethostaddr, sizeof(nethostaddr),AF_INET);
        if (!myHostEnt) {
#ifdef WIN32
            myErrorCode = WSAGetLastError();
#endif
#ifdef OSX
            myErrorCode = h_errno;
#endif
        } else {
            myErrorCode = 0;
        }
#endif
        if (myHostEnt==0) {
            strncpy(namedest,"<NAME UNKNOWN>", maxnamelen);
            AC_DEBUG <<"getHostName(): address (host byte order) " << hostaddr<< endl;
        } else {
            strncpy(namedest,myHostEnt->h_name, maxnamelen);
            AC_DEBUG <<"getHostName(): address (host byte order) " << hostaddr << " - " << namedest<< endl;
        } 
        return myErrorCode;
    }

    asl::Unsigned32 addrFromStr(const char *dotaddr)
    {
        asl::Unsigned32 i1, i2, i3, i4;
        asl::Unsigned32 hostaddr;

        sscanf(dotaddr,"%d.%d.%d.%d",&i1,&i2,&i3,&i4);
        // byte order of host addresses is __BIG_ENDIAN 
        hostaddr=(i1&0xff)<<24 | (i2&0xff)<<16 | (i3&0xff)<<8 | i4&0xff;

        AC_DEBUG << "addrFromStr(): address "
            << (i1 & 0xff) << '.' << (i2 & 0xff) << '.'
            << (i3 & 0xff) << '.' << (i4 &0xff) << " - "
            << hostaddr
            << endl;

        return hostaddr;
    }

    void addr2Str(asl::Unsigned32 hostaddr, char *dotaddrdest)
    {
        // byte order of host addresses is __BIG_ENDIAN 
        sprintf(dotaddrdest, "%d.%d.%d.%d",
                (hostaddr>>24) & 0xff,
                (hostaddr>>16) & 0xff,
                (hostaddr>>8) & 0xff,
                (hostaddr) & 0xff);
    }
    string addr2Str(asl::Unsigned32 hostaddr) {
        char myBuffer[16];
        addr2Str(hostaddr, myBuffer);
        return string(myBuffer);
    }

    asl::Unsigned32 getHostAddress(const char *hostspec)
    {
        asl::Unsigned32 hostaddr;
        int myErrorCode;


        AC_DEBUG <<"getHostAddress(): Looking for hostspec " << hostspec<< endl;

        if (strcmp(hostspec, "BROADCAST") == 0)
        {
            asl::Unsigned32 bcaddr=INADDR_BROADCAST;
            AC_DEBUG <<"getHostAddress(): Returning broadcast address " << bcaddr<< endl;
            return (bcaddr);
        }

        if ((hostspec[0]>='0') && (hostspec[0]<='9'))
        {
            hostaddr=addrFromStr(hostspec);
        }
        else 
        {
            struct hostent * myHostEnt;

#ifdef LINUX
            // reentrant version
            char tempBuffer[1024];
            myHostEnt = new hostent;
            gethostbyname_r(hostspec, myHostEnt, tempBuffer, sizeof(tempBuffer), 
                            &myHostEnt, &myErrorCode);
#else
            myHostEnt = gethostbyname(hostspec);
            if (!myHostEnt) {
#ifdef WIN32
                myErrorCode = WSAGetLastError();
#endif
#ifdef OSX
                myErrorCode = h_errno;
#endif
            }
#endif            
            if (myHostEnt==0)
            {
                throw SocketException("getHostAddress()");
            }
            else
            {
                asl::Unsigned32 nethostaddr;
                memcpy((char *)&(nethostaddr),myHostEnt->h_addr_list[0],sizeof(nethostaddr));
                hostaddr = ntohl (nethostaddr);
            }
#ifdef LINUX
            delete myHostEnt;
#endif            
        }
        AC_DEBUG <<"getHostAddress(): address (host byte order) " << hostaddr << endl;
        return (hostaddr);
    }

    asl::Unsigned32 getLocalAddr()
    {
        static asl::Unsigned32 myaddr=0;
        if (myaddr==0)
        {
            char localname[99];
            if (gethostname(localname,99))
            {
                AC_FATAL << "GetLocalAddr: can't get local hostname\n";
                exit(3);
            }
            myaddr=getHostAddress(localname);
        }
        return myaddr;
    }

}
