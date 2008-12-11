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
//    Check whether internet access is available
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      poor
//    formatting              :      ok
//    documentation           :      poor
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
//    interfaces              :      ok
//    confidence              :      fair
//    integration             :      ok
//    dependencies            :      poor
//    error handling          :      ok
//    logging                 :      notapp
//    cheesyness              :      fair
//
//    overall review status   :      poor
//
//    recommendations: move this to its own component 
*/

#ifndef INCL_INETMONITOR
#define INCL_INETMONITOR

#include <asl/base/PosixThread.h>

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
