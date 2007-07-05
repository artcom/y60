//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef Y60_WII_SCANNER_INCLUDED
#define Y60_WII_SCANNER_INCLUDED

#include <asl/PosixThread.h>
#include <asl/ThreadLock.h>

#include <vector>
#include <queue>
#include <string>

namespace y60 {

class WiiScanner : public asl::PosixThread {
    public:
        WiiScanner();
        ~WiiScanner();

        void poll(std::vector<std::string> & theNewWiimoteIds );
    private:
        static void scan(asl::PosixThread & theThread);

        asl::ThreadLock _myLock;
        std::queue<std::string> _myQueue;
        bool _isScanning;
};

}


#endif // Y60_WII_SCANNER_INCLUDED


