//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "WiiScanner.h"

#include <asl/Logger.h>

using namespace std;
using namespace asl;

namespace y60 {


WiiScanner::WiiScanner() :
    PosixThread( scan ),
    _isScanning( true )
{
    fork();
}

WiiScanner::~WiiScanner() {
    _isScanning = false;
    join();
    AC_PRINT << "=== joined";
}

void
WiiScanner::poll(vector<string> & theNewWiimoteIds) {

    if (_myLock.nonblock_lock() == 0) {
        while ( ! _myQueue.empty() ) {
            theNewWiimoteIds.push_back( _myQueue.front() );
            _myQueue.pop();
        }
        _myLock.unlock();
    }
}

void
WiiScanner::scan( asl::PosixThread & theThread ) {
    WiiScanner & mySelf = dynamic_cast<WiiScanner&>( theThread );

    while (mySelf._isScanning) {
        AC_PRINT << "== SCAN";
        sleep(1);
    }
}

} // end of namespace

