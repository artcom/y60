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
//   $RCSfile: watchdog.cpp,v $
//   $Author: ulrich $
//   $Revision$
//   $Date: 2005/04/19 10:02:40 $
//
//
//  Description: The watchdog restarts the application, if it is closed
//               manually or by accident
//
//
//=============================================================================

#include "watchdog.h"
#include "UDPHaltListenerThread.h"
#include "system_functions.h"

#include "Projector.h"

#include <dom/Nodes.h>
#include <asl/string_functions.h>
#include <asl/file_functions.h>
#include <asl/os_functions.h>
#include <asl/Exception.h>
#include <asl/Arguments.h>
#include <asl/Time.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <time.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#define DB(x) //x

using namespace std;

const string ourDefaultConfigFile = "watchdog.xml";

asl::Arguments ourArguments;
const asl::Arguments::AllowedOption ourAllowedOptions[] = {
    {"--configfile", "XML configuration file"},
    {"", ""}
};


WatchDog::WatchDog()
    : _myPort(2342), _myEnableUDP(false), _myPowerDownProjectors(false), _mySystemHaltCommand(""), _myWatchFrequency(30),
      _myAppToWatch(_myLogger), _myRestartDelay(10), _myStartDelay(0), _myProjectorCommandUp(""), _myProjectorCommandDown(""),
      _myRebootTimeInSecondsToday(-1), _myHaltTimeInSecondsToday(-1), _myPowerUpProjectorsOnStartup(true)
{}

void
WatchDog::arm() {

#ifdef WIN32
    // allow the foreground window to be set instead of blinking in the taskbar
    // see q97925
    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
#endif

    if (_myProjectors.size() && _myPowerUpProjectorsOnStartup) {
        cerr << "Watchdog - Powering up projectors..." << endl;
        for (unsigned i = 0; i < _myProjectors.size(); ++i) {
            _myProjectors[i]->powerUp();
        }

        cerr << "Watchdog - Setting projector input" << endl;
        for (unsigned i = 0; i < _myProjectorInput.size(); ++i) {
            _myProjectors[i]->selectInput(_myProjectorInput[i]);
        }
    }

    if (_myAppToWatch._myPerformECG) {
        _myLogger.logToFile(std::string("Monitoring heartbeat file: ") + _myAppToWatch._myHeartbeatFile);
    }

    if (getElapsedSecondsToday() > _myAppToWatch._myRestartTimeInSecondsToday) {
        _myAppToWatch._myRestartedToday = true;
    }
}


bool
WatchDog::watch() {
    try {
        // Setup udp halt lister
        UDPHaltListenerThread myUDPHaltListenerThread(_myProjectors, _myPort,
                                                      _myAppToWatch,
                                                      _myPowerDownProjectors,
													  _mySystemHaltCommand,
													  _myRestartAppCommand,
													  _mySystemRebootCommand);
        if (_myEnableUDP) {
            cerr << "Watchdog - Starting udp halt listener thread" << endl;
            myUDPHaltListenerThread.fork();
        }

        if (_myStartDelay > 0) {
            cerr << "Watchdog - Application will start in " << _myStartDelay << " seconds." << endl;
            for (unsigned i = 0; i < _myStartDelay * 2; ++i) {
                cerr << ".";
                asl::msleep(500);
            }
        }

        // Main loop
        while (true) {
            bool myRestarted = false;
            std::string myReturnString("Internal quit.");
            _myAppToWatch.launch();

            while (myRestarted == false && _myAppToWatch._myProcessResult == WAIT_TIMEOUT) {

                // update projector state
                for (unsigned i = 0; i < _myProjectors.size(); ++i) {
                    _myProjectors[i]->update();
                }

                // watch application
                myReturnString = _myAppToWatch.runUntilNextCheck(_myWatchFrequency);
                _myAppToWatch.checkHeartbeat();
                _myAppToWatch.checkState();
                myRestarted = _myAppToWatch.checkForRestart();

                // system halt & reboot
				checkForHalt();
                checkForReboot();
            }

            _myAppToWatch.shutdown();
            _myLogger.logToFile(_myAppToWatch._myFileName + string(" exited: ") + myReturnString + "\nRestarting application.");

            cerr << "Watchdog - Restarting application in " << _myRestartDelay << " seconds" << endl;
            for (unsigned i = 0; i < _myRestartDelay * 2; ++i) {
                cerr << ".";
                asl::msleep(500);
            }
            cerr << endl;
        }
    } catch (const asl::Exception & ex) {
        cerr << "### Exception: " << ex << endl;
        _myLogger.logToFile(string("### Exception: " + ex.what()));
    } catch (...) {
        cerr << "### Error while starting:\n\n" + _myAppToWatch._myFileName + " " + _myAppToWatch._myArguments << endl;
        _myLogger.logToFile(string("### Error while starting:\n\n" + _myAppToWatch._myFileName + " " + _myAppToWatch._myArguments));
        exit(-1);
    }

    return FALSE;
}

void
WatchDog::checkForReboot() {
    long myElapsedSecondsToday = getElapsedSecondsToday();
    if ((_myRebootTimeInSecondsToday!= -1) && (_myRebootTimeInSecondsToday< myElapsedSecondsToday) ) {
        if (myElapsedSecondsToday - _myRebootTimeInSecondsToday < (_myWatchFrequency * 3)) {
            initiateSystemReboot();
        }
    }
}

void
WatchDog::checkForHalt() {
    long myElapsedSecondsToday = getElapsedSecondsToday();
    if ((_myHaltTimeInSecondsToday!= -1) && (_myHaltTimeInSecondsToday< myElapsedSecondsToday) ) {
        if (myElapsedSecondsToday - _myHaltTimeInSecondsToday < (_myWatchFrequency * 3)) {
            initiateSystemShutdown();
        }
    }
}


void
WatchDog::dumpWinError(const string& theErrorLocation) {
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR) &lpMsgBuf,
                  0,
                  NULL
                  );

    cerr << "Warning: " << theErrorLocation << " failed." << endl;
    cerr << "         Error was " << *(char*)lpMsgBuf << endl;
}

bool
WatchDog::init(dom::Document & theConfigDoc) {
    try {
        if (theConfigDoc("WatchdogConfig")) {
            const dom::NodePtr & myConfigNode = theConfigDoc.childNode("WatchdogConfig");

            // Setup logfile
            {
                _myLogFilename = asl::expandEnvironment(myConfigNode->getAttribute("logfile")->nodeValue());
                std::string::size_type myDotPos = _myLogFilename.rfind(".", _myLogFilename.size());
                if (myDotPos == std::string::npos) {
                    myDotPos = _myLogFilename.size();
                }
#ifdef LINUX
                time_t ltime = time(NULL);
                struct tm* today = localtime(&ltime);
#else
                __time64_t ltime;
                _time64( &ltime );
                struct tm *today = _localtime64( &ltime );
#endif
                char myTmpBuf[128];
                strftime(myTmpBuf, sizeof(myTmpBuf), "%Y_%m_%d_%H_%M", today);
                _myLogFilename = _myLogFilename.substr(0, myDotPos) + "_" + myTmpBuf + _myLogFilename.substr(myDotPos, _myLogFilename.size());
                DB(cout <<"_myLogFilename: " << _myLogFilename<<endl;)
                _myLogger.openLogFile(_myLogFilename);
            }

            // Setup watch frequency
            {
                _myWatchFrequency = asl::as<int>(myConfigNode->getAttribute("watchFrequency")->nodeValue());
                DB(cout << "_myWatchFrequency: " << _myWatchFrequency << endl;)
                if (_myWatchFrequency < 1){
                    cerr <<"### ERROR: WatchFrequency must have a value greater then 0 sec." << endl;
                    return false;
                }
            }

            // Setup power down
            if (myConfigNode->childNode("UdpControl")) {
                _myEnableUDP = true;
                const dom::NodePtr & myUdpControlNode = myConfigNode->childNode("UdpControl");
                if (myUdpControlNode->getAttribute("port")) {
                    _myPort = asl::as<int>(myUdpControlNode->getAttribute("port")->nodeValue());
                    DB(cout <<"_myPort: " << _myPort<<endl;)
                }
				// Setup projector control
				if (myUdpControlNode->childNode("ProjectorControl")) {
					const dom::NodePtr & myProjectors = myUdpControlNode->childNode("ProjectorControl");
					_myPowerUpProjectorsOnStartup = asl::as<bool>(myProjectors->getAttribute("powerUpOnStartup")->nodeValue());
					for (unsigned i = 0; i < myProjectors->childNodesLength(); ++i) {
						const dom::NodePtr & myProjectorNode = myProjectors->childNode(i);
						if (myProjectorNode->nodeType() == dom::Node::ELEMENT_NODE) {
							std::string myType = "";
							if (myProjectorNode->getAttribute("type")) {
								myType = myProjectorNode->getAttribute("type")->nodeValue();
							}

            				if (myProjectorNode->getAttribute("input")) {
   								std::string myInput = myProjectorNode->getAttribute("input")->nodeValue();
            					_myProjectorInput.push_back(myInput);
            				}

							int myPort = -1;
							if (myProjectorNode->getAttribute("port")) {
								myPort = asl::as<int>(myProjectorNode->getAttribute("port")->nodeValue());
							}

							if (myPort != -1) {
            					Projector* myProjector = Projector::getProjector(myType, myPort);
            					myProjector->setLogger(&_myLogger);
								myProjector->configure(myProjectorNode);
								_myProjectors.push_back(myProjector);
							}
						}
					}
					DB(cout <<"Found " << _myProjectors.size() << " projectors" << endl;)
				}



                // check for system halt
                if (myUdpControlNode->childNode("SystemHalt")) {
                    const dom::NodePtr & mySystemHaltNode = myUdpControlNode->childNode("SystemHalt");
                    _myPowerDownProjectors  = asl::as<bool>(mySystemHaltNode->getAttribute("powerDownProjectors")->nodeValue());
                    _mySystemHaltCommand = mySystemHaltNode->getAttributeString("command");
                    DB(cout <<"_mySystemHaltCommand: " << _mySystemHaltCommand<<endl;)
                    DB(cout <<"_myEnableUDP: " << _myEnableUDP<<endl;)
                    DB(cout <<"_myPowerDownProjectors: " << _myPowerDownProjectors<<endl;)
                }
                if (myUdpControlNode->childNode("SystemReboot")) {
                    const dom::NodePtr & mySystemHaltNode = myUdpControlNode->childNode("SystemReboot");
                    _myPowerDownProjectors  = asl::as<bool>(mySystemHaltNode->getAttribute("powerDownProjectors")->nodeValue());
                    _mySystemRebootCommand = mySystemHaltNode->getAttributeString("command");
                    DB(cout <<"_mySystemRebootCommand: " << _mySystemRebootCommand<<endl;)
                    DB(cout <<"_myEnableUDP: " << _myEnableUDP<<endl;)
                    DB(cout <<"_myPowerDownProjectors: " << _myPowerDownProjectors<<endl;)
                }
                if (myUdpControlNode->childNode("RestartApplication")) {
                    const dom::NodePtr & myRestartAppNode = myUdpControlNode->childNode("RestartApplication");
                    _myRestartAppCommand = myRestartAppNode->getAttributeString("command");
                    DB(cout <<"_myRestartAppCommand: " << _myRestartAppCommand<<endl;)
                }
            }

            if (myConfigNode->childNode("RebootTime")) {
                std::string myRebootTime = (*myConfigNode->childNode("RebootTime"))("#text").nodeValue();
                std::string myHours = myRebootTime.substr(0, myRebootTime.find_first_of(':'));
                std::string myMinutes = myRebootTime.substr(myRebootTime.find_first_of(':')+1, myRebootTime.length());
                _myRebootTimeInSecondsToday = atoi(myHours.c_str()) * 3600;
                _myRebootTimeInSecondsToday += atoi(myMinutes.c_str()) * 60;
                DB(cout <<"_myRebootTimeInSecondsToday : " << _myRebootTimeInSecondsToday<< endl;)
            }

            if (myConfigNode->childNode("HaltTime")) {
                std::string myHaltTime = (*myConfigNode->childNode("HaltTime"))("#text").nodeValue();
                std::string myHours = myHaltTime.substr(0, myHaltTime.find_first_of(':'));
                std::string myMinutes = myHaltTime.substr(myHaltTime.find_first_of(':')+1, myHaltTime.length());
                _myHaltTimeInSecondsToday = atoi(myHours.c_str()) * 3600;
                _myHaltTimeInSecondsToday += atoi(myMinutes.c_str()) * 60;
                DB(cout <<"_myHaltTimeInSecondsToday : " << _myHaltTimeInSecondsToday<< endl;)
            }

            // Setup application
            if (myConfigNode->childNode("Application")) {
                const dom::NodePtr & myApplicationNode = myConfigNode->childNode("Application");
                _myAppToWatch._myFileName = asl::expandEnvironment(myApplicationNode->getAttribute("binary")->nodeValue());
                if (myApplicationNode->getAttribute("windowtitle")) {
                    _myAppToWatch._myWindowTitle = myApplicationNode->getAttribute("windowtitle")->nodeValue();
                }
                DB(cout <<"_myFileName: " << _myAppToWatch._myFileName<<endl;)
                if (_myAppToWatch._myFileName.empty()){
                    cerr <<"### ERROR, no application binary to watch." << endl;
                    return false;
                }
                if (myApplicationNode->childNode("Arguments")) {
                    const dom::NodePtr & myArguments = myApplicationNode->childNode("Arguments");
                    for (int myArgumentNr = 0; myArgumentNr < myArguments->childNodesLength(); myArgumentNr++) {
                        const dom::NodePtr & myArgumentNode = myArguments->childNode(myArgumentNr);
                        if (myArgumentNode->nodeType() == dom::Node::ELEMENT_NODE) {
                            _myAppToWatch._myArguments += (*myArgumentNode)("#text").nodeValue();
                            _myAppToWatch._myArguments += " ";
                        }
                        _myAppToWatch._myArguments = asl::expandEnvironment(_myAppToWatch._myArguments);
                        DB(cout <<"Argument : "<<myArgumentNr << ": " << _myAppToWatch._myArguments << endl;)
                    }
                }
                if (myApplicationNode->childNode("RestartDay")) {
                    _myAppToWatch._myRestartCheck = true;
                    _myAppToWatch._myRestartDay = (*myApplicationNode->childNode("RestartDay"))("#text").nodeValue();
                    _myAppToWatch._myRestartMode |= RESTARTDAY;
                    std::transform(_myAppToWatch._myRestartDay.begin(), _myAppToWatch._myRestartDay.end(), _myAppToWatch._myRestartDay.begin(), toupper);
                    DB(cout <<"_myRestartDay : " << _myAppToWatch._myRestartDay<< endl;)
                }
                if (myApplicationNode->childNode("RestartTime")) {
                    _myAppToWatch._myRestartCheck = true;
                    std::string myRestartTime = (*myApplicationNode->childNode("RestartTime"))("#text").nodeValue();
                    std::string myHours = myRestartTime.substr(0, myRestartTime.find_first_of(':'));
                    std::string myMinutes = myRestartTime.substr(myRestartTime.find_first_of(':')+1, myRestartTime.length());
                    _myAppToWatch._myRestartTimeInSecondsToday = atoi(myHours.c_str()) * 3600;
                    _myAppToWatch._myRestartTimeInSecondsToday += atoi(myMinutes.c_str()) * 60;
                    _myAppToWatch._myRestartMode |= RESTARTTIME;
                    DB(cout <<"_myRestartTimeInSecondsToday : " << _myAppToWatch._myRestartTimeInSecondsToday<< endl;)
                }
                if (myApplicationNode->childNode("CheckMemoryTime")) {
                    _myAppToWatch._myRestartCheck = true;
                    std::string myCheckMemoryTime = (*myApplicationNode->childNode("CheckMemoryTime"))("#text").nodeValue();
                    std::string myHours = myCheckMemoryTime.substr(0, myCheckMemoryTime.find_first_of(':'));
                    std::string myMinutes = myCheckMemoryTime.substr(myCheckMemoryTime.find_first_of(':')+1, myCheckMemoryTime.length());
                    _myAppToWatch._myCheckMemoryTimeInSecondsToday = atoi(myHours.c_str()) * 3600;
                    _myAppToWatch._myCheckMemoryTimeInSecondsToday += atoi(myMinutes.c_str()) * 60;
                    _myAppToWatch._myRestartMode |= CHECKMEMORYTIME;
                    DB(cout <<"_myCheckMemoryTimeInSecondsToday : " << _myAppToWatch._myCheckMemoryTimeInSecondsToday<< endl;)
                }
                if (myApplicationNode->childNode("CheckTimedMemoryThreshold")) {
                    _myAppToWatch._myRestartCheck = true;
                    _myAppToWatch._myMemoryThresholdTimed = asl::as<int>((*myApplicationNode->childNode("CheckTimedMemoryThreshold"))("#text").nodeValue());
                    _myAppToWatch._myRestartMode |= CHECKTIMEDMEMORYTHRESHOLD;
                    DB(cout <<"_myMemoryThresholdTimed : " << _myAppToWatch._myMemoryThresholdTimed<< endl;)
                }
                if (myApplicationNode->childNode("Memory_Threshold")) {
                    _myAppToWatch._myRestartCheck = true;
                    _myAppToWatch._myRestartMemoryThreshold = asl::as<int>((*myApplicationNode->childNode("Memory_Threshold"))("#text").nodeValue());
                    _myAppToWatch._myRestartMode |= MEMTHRESHOLD;
                    DB(cout <<"_myRestartMemoryThreshold : " << _myAppToWatch._myRestartMemoryThreshold<< endl;)
                }
                if (myApplicationNode->childNode("WaitDuringStartup")) {
                    _myStartDelay = asl::as<unsigned>((*myApplicationNode->childNode("WaitDuringStartup"))("#text").nodeValue());
                    DB(cout <<"_myStartDelay: " << _myStartDelay << endl;)
                }
                if (myApplicationNode->childNode("WaitDuringRestart")) {
                    _myRestartDelay = asl::as<unsigned>((*myApplicationNode->childNode("WaitDuringRestart"))("#text").nodeValue());
                    DB(cout <<"_myRestartDelay: " << _myRestartDelay << endl;)
                }

                if (myApplicationNode->childNode("Heartbeat")) {
                    const dom::NodePtr & myHeartbeatNode = myApplicationNode->childNode("Heartbeat");
                    if (myHeartbeatNode->childNode("Heartbeat_File")) {
                        _myAppToWatch._myHeartbeatFile = asl::expandEnvironment((*myHeartbeatNode->childNode("Heartbeat_File"))("#text").nodeValue());
                        DB(cout <<"_myHeartbeatFile : " << _myAppToWatch._myHeartbeatFile << endl;)
                    }
                    if (myHeartbeatNode->childNode("Allow_Missing_Heartbeats")) {
                        _myAppToWatch._myAllowMissingHeartbeats = asl::as<int>((*myHeartbeatNode->childNode("Allow_Missing_Heartbeats"))("#text").nodeValue());
                        DB(cout <<"_myAllowMissingHeartbeats : " << _myAppToWatch._myAllowMissingHeartbeats << endl;)
                    }
                    if (myHeartbeatNode->childNode("Heartbeat_Frequency")) {
                        _myAppToWatch._myHeartbeatFrequency = asl::as<int>((*myHeartbeatNode->childNode("Heartbeat_Frequency"))("#text").nodeValue());
                        DB(cout <<"_myHeartbeatFrequency : " << _myAppToWatch._myHeartbeatFrequency << endl;)
                    }
                    _myAppToWatch._myPerformECG = true;
                    if ((_myAppToWatch._myHeartbeatFrequency == 0) || (_myAppToWatch._myHeartbeatFile.empty())) {
                        _myAppToWatch._myPerformECG = false;
                    }
                    if (myHeartbeatNode->childNode("FirstHeartBeatDelay")) {
                        _myAppToWatch._myAppStartTimeInSeconds = asl::as<int>((*myHeartbeatNode->childNode("FirstHeartBeatDelay"))("#text").nodeValue());
                        DB(cout <<"_myAppStartTimeInSeconds : " << _myAppToWatch._myAppStartTimeInSeconds<< endl;)
                    }
                }
            }
        }
    } catch (const asl::Exception & ex) {
        cerr << "### Exception: " << ex;
    } catch (...) {
        cerr << "Error, while parsing xml config file" << endl;
        exit(-1);
    }
    return true;
}



void
printUsage() {
    cerr << ourArguments.getProgramName() << " Copyright (C) 2003-2005 ART+COM" << endl;
    ourArguments.printUsage();
    cerr << "Default configfile: " << ourDefaultConfigFile << endl;
}

void
readConfigFile(dom::Document & theConfigDoc,  std::string theFileName) {
    DB(cout << "Loading configuration data..." << endl;)
    std::string myFileStr = asl::getWholeFile(theFileName);
    if (myFileStr.empty()) {
        cerr << "Watchdog::readConfigFile: Can't open configuration file "
             << theFileName << "." << endl;
        exit(-1);
    }
    theConfigDoc.parseAll(myFileStr.c_str());
    if (!theConfigDoc) {
        cerr << "Watchdog:::readConfigFile: Error reading configuration file "
             << theFileName << "." << endl;
        exit(-1);
    }
}

int
main(int argc, char* argv[] ) {
#ifdef WIN32
    // This will turn off uncaught exception handling
    SetErrorMode(SEM_NOGPFAULTERRORBOX);
#endif


    ourArguments.addAllowedOptions(ourAllowedOptions);
    if (!ourArguments.parse(argc, argv)) {
        return 0;
    }

    dom::Document myConfigDoc;
    if (ourArguments.haveOption("--configfile")) {
        readConfigFile (myConfigDoc, ourArguments.getOptionArgument("--configfile"));
    } else {
        if (asl::fileExists(ourDefaultConfigFile)) {
            readConfigFile (myConfigDoc, ourDefaultConfigFile);
        } else {
            printUsage();
            return -1;
        }
    }

    WatchDog myHasso;
    bool mySuccess = myHasso.init(myConfigDoc);
    if (mySuccess) {
        myHasso.arm();
        myHasso.watch();
    }
    exit(-1);
}

#ifdef WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif
