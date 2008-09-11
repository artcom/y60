/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: net_functions.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: misc operating system & environment helper functions 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifdef WIN32
  #define _WIN32_WINNT 0x0500
#endif

#include "net_functions.h"
#include "SocketException.h"

#include <asl/base/os_functions.h>
#include <asl/base/settings.h>
#include <asl/base/string_functions.h>
#include <asl/base/file_functions.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/Time.h>
#include <asl/base/Exception.h>
#include <asl/base/Logger.h>

// cycle counter

#include <string>
#include <algorithm>

#ifdef WIN32
  #include <windows.h>
  #include <Iphlpapi.h>
#else
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <sys/ioctl.h>
#endif
#ifdef LINUX
  #include <linux/if.h>
  #include <linux/if_ether.h>
  #include <linux/sockios.h>
#endif
#ifdef OSX
  #include <ifaddrs.h>
  #include <net/if_dl.h>
#endif

using namespace std;
using namespace inet;

namespace asl {

    string hostname(unsigned long hostAddress) {
        unsigned long netHostAddr = htonl(hostAddress); 

        struct hostent *hp;
        hp=gethostbyaddr((char *)&netHostAddr, sizeof(netHostAddr),AF_INET);

        if (hp==0) {
            return as_dotted_address(hostAddress);
        } else {
            return hp->h_name;
        }
    }

    unsigned long from_dotted_address(const string & dottedAddress) {
        unsigned long myResultAddress = 0;

        string::const_iterator it = dottedAddress.begin();

        for (int i = 0; i < 4; ++i) {
            string::const_iterator dotPos = find(it,dottedAddress.end(),'.');
            unsigned int myPart;
            string myPartString = string(&(*it),dotPos-it) ;
            if (fromString(myPartString, myPart)) {

                if (myPart > 255) {
                    return 0;
                }
                myResultAddress=myResultAddress | myPart<<((3-i)*8);
            }
            it = dotPos;
            if (it != dottedAddress.end()) {
                ++it;
            }
        }
        return myResultAddress;
    }

    string as_dotted_address(unsigned long theAddress) {
        return as_string((theAddress>>24) & 0xff) + "."
            +as_string((theAddress>>16) & 0xff) + "."
            +as_string((theAddress>>8) & 0xff) + "."
            +as_string((theAddress) & 0xff);
    }

    unsigned long hostaddress(const string & theHost)
    {
        if (theHost == "INADDR_ANY") {
            return INADDR_ANY;
        }
        else if (theHost == "BROADCAST") {
            return INADDR_BROADCAST;
        }

        unsigned long myAddress = from_dotted_address(theHost);
        if (myAddress) {
            return myAddress;
        }

        struct hostent *hp;
        hp=gethostbyname(theHost.c_str());
        if (hp == 0) {
            AC_WARNING << "asl::hostaddress(): no address returned for host '" << theHost << "'" << endl;
            return 0;
        }
        unsigned long netHostAddress;
        memcpy(&netHostAddress,hp->h_addr, sizeof(netHostAddress));
        myAddress = ntohl(netHostAddress);
        return myAddress;
    }

    string localhostname() {
        char myHostNameBuffer[1024];
        string myResult;
        if (gethostname(myHostNameBuffer,1024) == 0) {
            myResult = myHostNameBuffer;
        }
        return myResult;
    }
    
    asl::Block getHardwareAddress(const std::string & theInterfaceName) {
        asl::Block myHardwareMac;
#ifdef WIN32
        const std::string myInterfaceName = theInterfaceName.size() ? theInterfaceName : asl::getenv<std::string>("AC_NET_HWIF", "0");
        // TODO: check interface name or use name as index instead
        PIP_ADAPTER_INFO pAdapterInfo;
        PIP_ADAPTER_INFO pAdapter = NULL;
        DWORD dwRetVal = 0;

        pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
        ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

        // Make an initial call to GetAdaptersInfo to get
        // the necessary size into the ulOutBufLen variable
        if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS) {
            free (pAdapterInfo);
            pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);
        }

        // iterate through all adapters. The adapter with the lowest index
        // is the primary adapter
        if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
            pAdapter = pAdapterInfo;
            unsigned myMinIndex = NumericTraits<unsigned>::max();
            while (pAdapter) {
                AC_INFO << "Adapter Name:" << pAdapter->AdapterName;
                AC_INFO << "Adapter Desc:" << pAdapter->Description;
                AC_INFO << "Adapter Type:" << pAdapter->Type;
                AC_INFO << "Adapter Index:" << pAdapter->Index;
                if (pAdapter->Index < myMinIndex && pAdapter->Type == MIB_IF_TYPE_ETHERNET) {
                    myHardwareMac.resize(pAdapter->AddressLength);
                    for (UINT i = 0; i < pAdapter->AddressLength; i++) {
                        myHardwareMac[i] = pAdapter->Address[i];
                    }
                }
                pAdapter = pAdapter->Next;
            }
        }
        free (pAdapterInfo);
#else
        int fd = socket (AF_INET,SOCK_DGRAM,0);
        if (fd == -1) {
            throw SocketException(PLUS_FILE_LINE); 
        }

#ifdef OSX
        const std::string myInterfaceName = theInterfaceName.size() ? theInterfaceName : asl::getenv<std::string>("AC_NET_HWIF", "en0");
        struct ifaddrs *ifap0 = 0;
        if (getifaddrs(&ifap0) < 0) {
            throw SocketException(PLUS_FILE_LINE); 
        }
        struct ifaddrs * ifap = ifap0;

        bool myFoundIfFlag = false;
        while (ifap && myFoundIfFlag == false) {
            if (strcmp(ifap->ifa_name, myInterfaceName.c_str()) == 0 && ifap->ifa_addr->sa_family == AF_LINK) {
                struct sockaddr_dl * dl = (struct sockaddr_dl *) ifap->ifa_addr;
                unsigned myIfNameLen = strlen("en0");
                /*AC_PRINT << ifap->ifa_name << " index=" << dl->sdl_index << " len=" << (int)(dl->sdl_alen) << " data=" << dl->sdl_data;
                for (unsigned i = 0; i < dl->sdl_alen; ++i) {
                    AC_PRINT << hex << (unsigned)(dl->sdl_data[myIfNameLen + i]) << dec;
                }*/
                myHardwareMac.resize(dl->sdl_alen);
                memcpy (&myHardwareMac[0], dl->sdl_data + myIfNameLen, dl->sdl_alen);
                myFoundIfFlag = true;
            }
            ifap = ifap->ifa_next;
        }
        freeifaddrs(ifap0);
#else
        const std::string myInterfaceName = theInterfaceName.size() ? theInterfaceName : asl::getenv<std::string>("AC_NET_HWIF", "eth0");
        ifreq myInterface;
        strcpy (myInterface.ifr_name, myInterfaceName.c_str());
        if (ioctl (fd, SIOCGIFHWADDR, &myInterface) < 0) {
            throw SocketException(PLUS_FILE_LINE); 
        }
        myHardwareMac.resize(ETH_ALEN);
        memcpy (&myHardwareMac[0], myInterface.ifr_hwaddr.sa_data, ETH_ALEN);
#endif
#endif
        return myHardwareMac;
    };

} //Namespace asl
