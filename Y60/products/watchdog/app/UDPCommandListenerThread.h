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

#ifndef INCL_UDPCOMMANDLISTENERTHREAD_H
#define INCL_UDPCOMMANDLISTENERTHREAD_H


class Projector;

#include <asl/PosixThread.h>
#include <dom/Nodes.h>

#include <string>
#include <vector>

class Application;
class UDPCommandListenerThread : public asl::PosixThread {
    public:
        UDPCommandListenerThread(std::vector<Projector *> theProjectors,
                                 Application & theApplication,
                                 const dom::NodePtr & theConfigNode);
        virtual ~UDPCommandListenerThread();

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
        int                     _myUDPPort;
        Application &           _myApplication;
        bool                    _myPowerDownProjectorsOnHalt;
        bool                    _myShutterCloseProjectorsOnStop;
        bool                    _myShutterCloseProjectorsOnReboot;
        std::string             _mySystemHaltCommand;
        std::string             _myRestartAppCommand;
        std::string             _mySystemRebootCommand;
        std::string             _myStopAppCommand;
        std::string             _myStartAppCommand;
};

#endif
