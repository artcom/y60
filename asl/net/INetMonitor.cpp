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
//    $RCSfile: INetMonitor.cpp,v $
//
//     $Author: build $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
//
//=============================================================================

#include "INetMonitor.h"

#include <asl/Logger.h>
#include <asl/Time.h>
#include <curl/curl.h>

#include <string>
#ifdef LINUX
#include <unistd.h>
#endif

using namespace std;
using namespace asl;


namespace inet {

    INetMonitor INetMonitor::_theInstance;

    const int CHECK_INTERVAL=30; // seconds
    const long CONNECT_TIMEOUT=10; // seconds
    const long TRANSFER_TIMEOUT=10; // seconds
    const long DNS_CACHE_TIMEOUT=3600; // seconds, 0=disable DNS caching

    size_t eatWebsite(void* buffer, size_t size, size_t memb, void* userp) {
        return size*memb;
    }

    INetMonitor::INetMonitor() :_myThreadEnable(true) {
        _myIsInternetAvailable = true;
        _myIsInitialized = false;
    }

    INetMonitor::~INetMonitor() {
        _myThreadEnable = false;
        if (_myIsInitialized) {
            join();
            curl_easy_cleanup(_myCurl);
            delete[] _myErrorBuffer;
        }
    }

    void INetMonitor::setHostList(vector<string>& myHostNames) {
        AC_DEBUG << "INetMonitor: setting host list.";
        if (!_theInstance._myIsInitialized) {
            _theInstance._myHostNames = myHostNames;
            _theInstance.init();
        } else {
            _theInstance._myThreadEnable = false;
            _theInstance.join();
            _theInstance._myHostNames = myHostNames;
            _theInstance._myThreadEnable = true;
            _theInstance.fork();
        }
    }

    void INetMonitor::_dumpStatus() {

        AC_DEBUG << "INetMonitor Status: " << _myErrorBuffer;

        CURLcode myCode;
        double myNameLookupTime;
        myCode = curl_easy_getinfo(_myCurl, CURLINFO_NAMELOOKUP_TIME, &myNameLookupTime);
        AC_DEBUG << "INetMonitor Status: CURLINFO_NAMELOOKUP_TIME:" << myNameLookupTime;

        double myConnectTime;
        myCode = curl_easy_getinfo(_myCurl, CURLINFO_CONNECT_TIME, &myConnectTime);
        AC_DEBUG << "INetMonitor Status: CURLINFO_CONNECT_TIME:" << myConnectTime;

        double myPreTransferTime;
        myCode = curl_easy_getinfo(_myCurl, CURLINFO_PRETRANSFER_TIME, &myPreTransferTime);
        AC_DEBUG << "INetMonitor Status: CURLINFO_PRETRANSFER_TIME:" << myPreTransferTime;

        double myStartTranferTime;
        myCode = curl_easy_getinfo(_myCurl, CURLINFO_STARTTRANSFER_TIME, &myStartTranferTime);
        AC_DEBUG << "INetMonitor Status: CURLINFO_STARTTRANFSER_TIME:" << myStartTranferTime;

        double myTotalTime;
        myCode = curl_easy_getinfo(_myCurl, CURLINFO_TOTAL_TIME, &myTotalTime);
        AC_DEBUG << "INetMonitor Status: CURLINFO_TOTAL_TIME:" << myTotalTime;
    }

    void INetMonitor::init() {
        _myCurl = curl_easy_init();
        _myErrorBuffer = new char[CURL_ERROR_SIZE];

        if(_myCurl) {
            curl_easy_setopt(_myCurl, CURLOPT_FAILONERROR, 0);
            curl_easy_setopt(_myCurl, CURLOPT_ERRORBUFFER, _myErrorBuffer);
            curl_easy_setopt(_myCurl, CURLOPT_WRITEFUNCTION, eatWebsite);
            curl_easy_setopt(_myCurl, CURLOPT_CONNECTTIMEOUT, CONNECT_TIMEOUT);
            curl_easy_setopt(_myCurl, CURLOPT_TIMEOUT, TRANSFER_TIMEOUT);
            curl_easy_setopt(_myCurl, CURLOPT_DNS_CACHE_TIMEOUT, DNS_CACHE_TIMEOUT);
            curl_easy_setopt(_myCurl, CURLOPT_FORBID_REUSE, long(1));
            //            curl_easy_setopt(_myCurl, CURLOPT_VERBOSE, 1);
            _myThreadEnable = true;
            fork();
        } else {
            _myIsInternetAvailable = false;
            AC_WARNING << "INetMonitor: curl initialization failed." << endl;
        } 
        _myIsInitialized = true;
    }

    bool INetMonitor::isInternetAvailable() {
        return _theInstance._myIsInternetAvailable;
    }

    void INetMonitor::run() {
        while (_myThreadEnable) {

            // Check several websites. If one of them reacts, we have internet.
            bool myINetAvailable = false;
            for (int i=0; i<_myHostNames.size(); i++) {
                if (testWebsite (_myHostNames[i])) {
                    myINetAvailable = true;
                }
            }

            if (_myIsInternetAvailable != myINetAvailable) {
                // Status changed
                _myIsInternetAvailable = myINetAvailable;
                if (_myIsInternetAvailable) {
                    AC_INFO << "INetMonitor: Internet is available." << endl;
                } else {
                    AC_INFO << "INetMonitor: Internet is not available." << endl;
                    AC_DEBUG << "INetMonitor: last curl error was: " << _myErrorBuffer;
                }
            }
            // Actually, we want to sleep five minutes. But we also want to be able
            // to react to a join() quickly...
            for (int i=0; i<CHECK_INTERVAL; i++) {
                msleep(1000);
                if (!_myThreadEnable) {
                    break;
                }
            }
        }
    }

    bool INetMonitor::testWebsite(const string& theURL) {
        curl_easy_setopt(_myCurl, CURLOPT_URL, theURL.c_str());

        AC_DEBUG << "INetMonitor: connecting to " << theURL;
        CURLcode res = curl_easy_perform(_myCurl);

        bool isOk= (res == CURLE_OK ||
                res == CURLE_TOO_MANY_REDIRECTS);

        AC_DEBUG << "INetMonitor: " << theURL << "=" << isOk;
        //_dumpStatus();

        return isOk;
    }

}

