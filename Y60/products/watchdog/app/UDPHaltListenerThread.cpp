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
//    $RCSfile: UDPHaltListenerThread.cpp,v $
//
//     $Author: valentin $
//
//   $Revision: 1.12 $
//
// Description:
//
//============================================================================

#include "UDPHaltListenerThread.h"
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

UDPHaltListenerThread::UDPHaltListenerThread(std::vector<Projector *> theProjectors, int thePort, 
                                             Application & theApplication,
                                             bool thePowerDownProjectorsOnHalt,
                                             bool theShutterCloseProjectorsOnStop,
                                             const string & theSystemhaltCommand,
                                             const string & theSystemRebootCommand,
                                             const string & theRestartAppCommand,
                                             const string & theStopAppCommand,
                                             const string & theStartAppCommand):
    _myProjectors(theProjectors), _myPort(thePort), 
    _myApplication(theApplication),
    _myPowerDownProjectorsOnHalt(thePowerDownProjectorsOnHalt),
    _myShutterCloseProjectorsOnStop(theShutterCloseProjectorsOnStop),
    _mySystemHaltCommand(theSystemhaltCommand), 
    _mySystemRebootCommand(theSystemRebootCommand),
    _myRestartAppCommand(theRestartAppCommand), 
    _myStopAppCommand(theStopAppCommand), 
    _myStartAppCommand(theStartAppCommand)
{
    if (_mySystemHaltCommand == "") {
        _mySystemHaltCommand = "halt";
    }
    if (_mySystemRebootCommand == "") {
        _mySystemRebootCommand = "reboot";
    }
    if (_myRestartAppCommand == "") {
        _myRestartAppCommand = "restart_app";
    }
    if (_myStopAppCommand == "") {
        _myStopAppCommand = "stop_app";
    }
    if (_myStartAppCommand == "") {
        _myStartAppCommand = "start_app";
    }
    if (_myProjectors.size() > 0 ) {
        cout << "Projectors: " << endl;    
        for (int i = 0; i < _myProjectors.size(); i++) {
            cout << i << " : " << _myProjectors[i]->getDescription()<< endl;;
        }
    }
}

UDPHaltListenerThread::~UDPHaltListenerThread() {
}

void 
UDPHaltListenerThread::setSystemHaltCommand(const string & theSystemhaltCommand) {
    _mySystemHaltCommand = theSystemhaltCommand;
}

void 
UDPHaltListenerThread::setRestartAppCommand(const string & theRestartAppCommand) {
    _myRestartAppCommand = theRestartAppCommand;
}

void 
UDPHaltListenerThread::setStopAppCommand(const string & theStopAppCommand) {
    _myStopAppCommand = theStopAppCommand;
}

void 
UDPHaltListenerThread::setStartAppCommand(const string & theStartAppCommand) {
    _myStartAppCommand = theStartAppCommand;
}

void 
UDPHaltListenerThread::setSystemRebootCommand(const string & theSystemRebootCommand) {
    _mySystemRebootCommand = theSystemRebootCommand;
}

bool 
UDPHaltListenerThread::controlProjector(const std::string & theCommand)
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
UDPHaltListenerThread::initiateShutdown() {
    
    if (_myPowerDownProjectorsOnHalt) {
        // shutdown all connected projectors
        controlProjector("projector_off");
    }
    initiateSystemShutdown();
}

void
UDPHaltListenerThread::initiateReboot() {
    
    if (_myPowerDownProjectorsOnHalt) {
        // shutdown all connected projectors
        controlProjector("projector_off");
    }
    initiateSystemReboot();
}

void
UDPHaltListenerThread::run() {
    cout << "Halt listener activated." << endl;
    cout << "* UDP Listener on port: " << _myPort << endl;
    cout << "* Commands:" << endl;
    cout << "      System Halt  : " << _mySystemHaltCommand << endl;
    cout << "      System Reboot: " << _mySystemRebootCommand << endl;
    cout << "Application restart: " << _myRestartAppCommand << endl;
    cout << "Application stop   : " << _myStopAppCommand << endl;
    cout << "Application start  : " << _myStartAppCommand << endl;


    try {
        initSockets();
        UDPSocket myUDPServer(INADDR_ANY, _myPort);

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

