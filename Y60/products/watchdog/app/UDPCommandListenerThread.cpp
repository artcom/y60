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

#include "UDPCommandListenerThread.h"
#include "system_functions.h"

#include "Projector.h"

#define _WIN32_WINNT 0x500

#include <asl/UDPSocket.h>
#include <asl/net.h>

#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

#include "Application.h"

using namespace inet;
using namespace std;

UDPCommandListenerThread::UDPCommandListenerThread(std::vector<Projector *> theProjectors,
                                                   Application & theApplication,
                                                   const dom::NodePtr & theConfigNode) :
    _myProjectors(theProjectors),
    _myApplication(theApplication),
    _myUDPPort(2342),
    _myPowerDownProjectorsOnHalt(false),
    _myShutterCloseProjectorsOnStop(false),
    _myShutterCloseProjectorsOnReboot(false),
    _mySystemHaltCommand(""), 
    _mySystemRebootCommand(""),
    _myRestartAppCommand(""), 
    _myStopAppCommand(""), 
    _myStartAppCommand("")    
{
    // check for UDP port
    if (theConfigNode->getAttribute("port")) {
        _myUDPPort = asl::as<int>(theConfigNode->getAttribute("port")->nodeValue());
        AC_DEBUG << "_myPort: " << _myUDPPort;
    }
    // check for system halt command configuration
    if (theConfigNode->childNode("SystemHalt")) {
        const dom::NodePtr & mySystemHaltNode = theConfigNode->childNode("SystemHalt");
        _myPowerDownProjectorsOnHalt = asl::as<bool>(mySystemHaltNode->getAttribute("powerDownProjectors")->nodeValue());
        _mySystemHaltCommand = mySystemHaltNode->getAttributeString("command");
        AC_DEBUG <<"_mySystemHaltCommand: " << _mySystemHaltCommand;
    }
    // check for system reboot command configuration
    if (theConfigNode->childNode("SystemReboot")) {
        const dom::NodePtr & mySystemRebootNode = theConfigNode->childNode("SystemReboot");
        _myShutterCloseProjectorsOnReboot = asl::as<bool>(mySystemRebootNode->getAttribute("shutterCloseProjectors")->nodeValue());
        _mySystemRebootCommand = mySystemRebootNode->getAttributeString("command");
        AC_DEBUG <<"_mySystemRebootCommand: " << _mySystemRebootCommand;
        AC_DEBUG <<"_myShutterCloseProjectorsOnReboot: " << _myShutterCloseProjectorsOnReboot;
    }
    // check for application restart command configuration
    if (theConfigNode->childNode("RestartApplication")) {
        const dom::NodePtr & myRestartAppNode = theConfigNode->childNode("RestartApplication");
        _myRestartAppCommand = myRestartAppNode->getAttributeString("command");
        AC_DEBUG <<"_myRestartAppCommand: " << _myRestartAppCommand;
    }
    // check for application stop command configuration
    if (theConfigNode->childNode("StopApplication")) {
        const dom::NodePtr & myStopAppNode = theConfigNode->childNode("StopApplication");
        _myShutterCloseProjectorsOnStop = asl::as<bool>(myStopAppNode->getAttribute("shutterCloseProjectors")->nodeValue());
        _myStopAppCommand = myStopAppNode->getAttributeString("command");
        AC_DEBUG <<"_myStopAppCommand: " << _myStopAppCommand;
        AC_DEBUG <<"_myShutterCloseProjectorsOnStop: " << _myShutterCloseProjectorsOnStop;
    }
    // check for application start command configuration
    if (theConfigNode->childNode("StartApplication")) {
        const dom::NodePtr & myStartAppNode = theConfigNode->childNode("StartApplication");
        _myStartAppCommand = myStartAppNode->getAttributeString("command");
        AC_DEBUG <<"_myStartAppCommand: " << _myStartAppCommand;
    }

    if (_myProjectors.size() > 0 ) {
        cout << "Projectors: " << endl;    
        for (int i = 0; i < _myProjectors.size(); i++) {
            cout << (i+1) << ": " << _myProjectors[i]->getDescription()<< endl;;
        }
    }
}

UDPCommandListenerThread::~UDPCommandListenerThread() {
}

void 
UDPCommandListenerThread::setSystemHaltCommand(const string & theSystemhaltCommand) {
    _mySystemHaltCommand = theSystemhaltCommand;
}

void 
UDPCommandListenerThread::setRestartAppCommand(const string & theRestartAppCommand) {
    _myRestartAppCommand = theRestartAppCommand;
}

void 
UDPCommandListenerThread::setStopAppCommand(const string & theStopAppCommand) {
    _myStopAppCommand = theStopAppCommand;
}

void 
UDPCommandListenerThread::setStartAppCommand(const string & theStartAppCommand) {
    _myStartAppCommand = theStartAppCommand;
}

void 
UDPCommandListenerThread::setSystemRebootCommand(const string & theSystemRebootCommand) {
    _mySystemRebootCommand = theSystemRebootCommand;
}

bool 
UDPCommandListenerThread::controlProjector(const std::string & theCommand)
{
    if (_myProjectors.size() == 0) {
        cerr << "No projectors configured, ignoring '" << theCommand << "'" << endl;
        return true;
    }

    bool myCommandHandled = true;
    for (unsigned i = 0; i < _myProjectors.size(); ++i) {
        myCommandHandled &= _myProjectors[i]->command(theCommand);
    }

    return myCommandHandled;
}

void
UDPCommandListenerThread::initiateShutdown() {
    
    if (_myPowerDownProjectorsOnHalt) {
        // shutdown all connected projectors
        controlProjector("projector_off");
    }
    initiateSystemShutdown();
}

void
UDPCommandListenerThread::initiateReboot() {
    
    if (_myShutterCloseProjectorsOnReboot) {
        // close shutter on all connected projectors
        controlProjector("projector_shutter_close");
    }
    initiateSystemReboot();
}

void
UDPCommandListenerThread::run() {
    cout << "Halt listener activated." << endl;
    cout << "* UDP Listener on port: " << _myUDPPort << endl;
    cout << "* Commands:" << endl;
    cout << "      System Halt  : " << _mySystemHaltCommand << endl;
    cout << "      System Reboot: " << _mySystemRebootCommand << endl;
    cout << "Application restart: " << _myRestartAppCommand << endl;
    cout << "Application stop   : " << _myStopAppCommand << endl;
    cout << "Application start  : " << _myStartAppCommand << endl;


    try {
        initSockets();
        UDPSocket myUDPServer(INADDR_ANY, _myUDPPort);

        while (true) {
            char myInputBuffer[2048];
            unsigned myBytesRead = myUDPServer.receiveFrom(0,0,myInputBuffer,sizeof(myInputBuffer)-1);
            myInputBuffer[myBytesRead] = 0;
            if (_mySystemHaltCommand != "" && !strcmp(myInputBuffer, _mySystemHaltCommand.c_str())) {
                cerr << "Client received halt packet" << endl;
                initiateShutdown();
                break;
            } else if (_mySystemRebootCommand != "" && !strcmp(myInputBuffer, _mySystemRebootCommand.c_str())) {
                cerr << "Client received reboot packet" << endl;
                initiateReboot();
                break;
            } else if (_myRestartAppCommand != "" && !strcmp(myInputBuffer, _myRestartAppCommand.c_str())) {
                cerr << "Client received restart application packet" << endl;
                _myApplication.terminate(string("Restart from Network"), true);
                _myApplication.setPaused(false);
            } else if (_myStopAppCommand != "" && !strcmp(myInputBuffer, _myStopAppCommand.c_str())) {
                cerr << "Client received stop application packet" << endl;
                if (_myShutterCloseProjectorsOnStop) {
                    // close shutter on all connected projectors
                    controlProjector("projector_shutter_close");
                }
                _myApplication.setPaused(true);
                _myApplication.terminate(string("Stop from Network"), true);
            } else if (_myStartAppCommand != "" && !strcmp(myInputBuffer, _myStartAppCommand.c_str())) {
                cerr << "Client received start application packet" << endl;
                _myApplication.setPaused(false);
                if (_myShutterCloseProjectorsOnStop) {
                    // open shutter on all connected projectors
                    controlProjector("projector_shutter_open");
                }
            } else if (controlProjector(std::string(myInputBuffer)) == true) {
                // pass
            } else {
                cerr << "### UDPHaltListener: Unexpected packet '" << myInputBuffer << "'. Ignoring" << endl;
            }
        }
    } catch (SocketException & se) {
        cerr << "### UDPHaltListener: " << se.what() << endl;
    }
}

