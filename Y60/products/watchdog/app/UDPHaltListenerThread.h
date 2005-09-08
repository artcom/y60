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
//    $RCSfile: UDPHaltListenerThread.h,v $
//
//     $Author: valentin $
//
//   $Revision: 1.9 $
//
// Description:
//
//============================================================================

#ifndef INCL_UDPHALTLISTENERTHREAD
#define INCL_UDPHALTLISTENERTHREAD


class Projector;

#include <asl/PosixThread.h>

#include <string>
#include <vector>

class Application;
class UDPHaltListenerThread : public PosixThread {
    public:
        UDPHaltListenerThread(std::vector<Projector *> theProjectors, int thePort, 
                              Application & theApplication,
                              bool thePowerDownProjectorsOnHalt,
                              bool theShutterCloseProjectorsOnStop,
                              const std::string & theSystemhaltCommand,
                              const std::string & theSystemRebootCommand,
                              const std::string & theRestartAppCommand,
                              const std::string & theStopAppCommand,
                              const std::string & theStartAppCommand);
        virtual ~UDPHaltListenerThread();

        void setSystemHaltCommand(const std::string & theSystemhaltCommand);
        void setRestartAppCommand(const std::string & theRestartAppCommand);
        void setSystemRebootCommand(const std::string & theSystemRebootCommand);
        void setStopAppCommand(const std::string & theStopAppCommand);
        void setStartAppCommand(const std::string & theStartAppCommand);
    private:
        void run();
        bool controlProjector(const std::string & theCommand);
        
        void initiateShutdown();
        void initiateReboot();

        std::vector<Projector*> _myProjectors;
        int                     _myPort;
        Application &           _myApplication;
        bool                    _myPowerDownProjectorsOnHalt;
        bool                    _myShutterCloseProjectorsOnStop;
        std::string             _mySystemHaltCommand;
        std::string             _myRestartAppCommand;
        std::string             _mySystemRebootCommand;
        std::string             _myStopAppCommand;
        std::string             _myStartAppCommand;
};

#endif
