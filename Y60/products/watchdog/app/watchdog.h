//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: watchdog.h,v $
//   $Author: valentin $
//   $Revision: 1.12 $
//   $Date: 2005/03/29 14:16:43 $
//
//
//  Description: The watchdog restarts the application, if it is closed
//               manually or by accident
//
//
//=============================================================================

#include "Logger.h"
#include "Application.h"
#include "UDPCommandListenerThread.h"

#include <vector>
#include <string>
#include <fstream>

namespace dom {
    class Document;
};

class Projector;

class WatchDog {
public:
    WatchDog();
    bool init(dom::Document & theConfigDoc);
    void arm();
    bool watch();

private:
    Logger           _myLogger;
    std::string      _myLogFilename;
    int              _myWatchFrequency;

    Application      _myAppToWatch;

    UDPCommandListenerThread * _myUDPCommandListenerThread;
    
    std::vector<Projector *> _myProjectors;
    bool             _myPowerUpProjectorsOnStartup;

    void             dumpWinError(const std::string& theErrorLocation);
    void             checkForReboot();
    void             checkForHalt();
    long             _myRebootTimeInSecondsToday;
    long             _myHaltTimeInSecondsToday;
    
    bool             _myApplicationPaused;
};
