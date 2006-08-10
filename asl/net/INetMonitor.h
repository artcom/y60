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
//    $RCSfile: INetMonitor.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_INETMONITOR
#define INCL_INETMONITOR

#include <asl/PosixThread.h>

#include <vector>
#include <string>

// Forward declaration.
typedef void CURL;

namespace inet {

    class INetMonitor: private asl::PosixThread {
        public:
            static void setHostList(std::vector<std::string>& myHostNames);
            static bool isInternetAvailable();

        private:
            INetMonitor();
            ~INetMonitor();
            void init();

            virtual void run();
            void _dumpStatus();
            bool testWebsite(const std::string& theURL);

            static INetMonitor _theInstance;

            bool _myIsInternetAvailable;
            bool _myThreadEnable;

            char * _myErrorBuffer;
            CURL * _myCurl;
            std::vector<std::string> _myHostNames;
            bool _myIsInitialized;
    };
}
#endif
