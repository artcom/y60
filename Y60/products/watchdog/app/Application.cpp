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
//   $RCSfile: Application.cpp,v $
//   $Author: ulrich $
//   $Revision: 1.11 $
//   $Date: 2005/04/07 12:27:16 $
//
//  Description: A simple application class representing a watched process
//
//=============================================================================

#include "Application.h"
#include "Logger.h"
#include "system_functions.h"

#include <time.h>

#include <asl/MappedBlock.h>
#include <asl/Exception.h>
#include <asl/file_functions.h>
#include <asl/os_functions.h>
#include <asl/proc_functions.h>


#include <Tlhelp32.h>

using namespace std;  // Manually included.
using namespace dom;  // Manually included.

const char * ourWeekdayMap[] = {
    "monday",
    "tuesday",
    "wednesday",
    "thursday",
    "friday",
    "saturday",
    "sunday",
    0
};


Application::Application(Logger & theLogger):
    _myAllowMissingHeartbeats(3), _myHeartbeatFrequency(0), _myPerformECG(false),_myRestartedToday(false),
    _myRestartMemoryThreshold(0),_myRestartTimeInSecondsToday(0), _myCheckMemoryTimeInSecondsToday(0),
    _myAppStartTimeInSeconds(0), _myRestartMode(UNSET),_myCheckedMemoryToday(false),
    _myRestartCheck(false), _myMemoryThresholdTimed(0),_myStartTimeInSeconds(0),
    _myMemoryIsFull(false), _myItIsTimeToRestart(false), _myHeartIsBroken(false),
    _myDayChanged(false), _myLogger(theLogger), _myWindowTitle(""), _myApplicationPaused(false),
    _myRestartDelay(10), _myStartDelay(0)
{
}

Application::~Application() {
}

void 
Application::setupEnvironment(const NodePtr & theEnvironmentSettings) {
    for (int myEnvNr = 0; myEnvNr < theEnvironmentSettings->childNodesLength(); myEnvNr++) {
        const dom::NodePtr & myEnvNode = theEnvironmentSettings->childNode(myEnvNr);
        if (myEnvNode->nodeType() == dom::Node::ELEMENT_NODE) {
            string myEnviromentVariable = myEnvNode->getAttributeString("name");
            if (myEnviromentVariable.size()>0 && myEnvNode->childNodesLength() == 1) {
                string myEnvironmentValue = myEnvNode->firstChild()->nodeValue();
                AC_DEBUG <<"Environment variable : "<<myEnvNr << ": " << myEnviromentVariable 
                        << " -> " << myEnvironmentValue ;
                _myEnvironmentVariables.push_back(EnvironmentSetting(myEnviromentVariable, myEnvironmentValue));
            }
        }
    }
}

bool Application::setup(const dom::NodePtr & theAppNode) {
    _myFileName = asl::expandEnvironment(theAppNode->getAttribute("binary")->nodeValue());
    if (theAppNode->getAttribute("windowtitle")) {
        _myWindowTitle = theAppNode->getAttribute("windowtitle")->nodeValue();
    }
    AC_DEBUG <<"_myFileName: " << _myFileName;
    if (_myFileName.empty()){
        cerr <<"### ERROR, no application binary to watch." << endl;
        return false;
    }
    if (theAppNode->childNode("EnvironmentVariables")) {
        setupEnvironment(theAppNode->childNode("EnvironmentVariables"));
    }
    if (theAppNode->childNode("Arguments")) {
        const dom::NodePtr & myArguments = theAppNode->childNode("Arguments");
        for (int myArgumentNr = 0; myArgumentNr < myArguments->childNodesLength(); myArgumentNr++) {
            const dom::NodePtr & myArgumentNode = myArguments->childNode(myArgumentNr);
            if (myArgumentNode->nodeType() == dom::Node::ELEMENT_NODE) {
                _myArguments += (*myArgumentNode)("#text").nodeValue();
                _myArguments += " ";
            }
            _myArguments = asl::expandEnvironment(_myArguments);
            AC_DEBUG <<"Argument : "<<myArgumentNr << ": " << _myArguments ;
        }
    }
    if (theAppNode->childNode("RestartDay")) {
        _myRestartCheck = true;
        _myRestartDay = (*theAppNode->childNode("RestartDay"))("#text").nodeValue();
        _myRestartMode |= RESTARTDAY;
        std::transform(_myRestartDay.begin(), _myRestartDay.end(), _myRestartDay.begin(), toupper);
        AC_DEBUG <<"_myRestartDay : " << _myRestartDay;
    }
    if (theAppNode->childNode("RestartTime")) {
        _myRestartCheck = true;
        std::string myRestartTime = (*theAppNode->childNode("RestartTime"))("#text").nodeValue();
        std::string myHours = myRestartTime.substr(0, myRestartTime.find_first_of(':'));
        std::string myMinutes = myRestartTime.substr(myRestartTime.find_first_of(':')+1, myRestartTime.length());
        _myRestartTimeInSecondsToday = atoi(myHours.c_str()) * 3600;
        _myRestartTimeInSecondsToday += atoi(myMinutes.c_str()) * 60;
        _myRestartMode |= RESTARTTIME;
        AC_DEBUG <<"_myRestartTimeInSecondsToday : " << _myRestartTimeInSecondsToday;
    }
    if (theAppNode->childNode("CheckMemoryTime")) {
        _myRestartCheck = true;
        std::string myCheckMemoryTime = (*theAppNode->childNode("CheckMemoryTime"))("#text").nodeValue();
        std::string myHours = myCheckMemoryTime.substr(0, myCheckMemoryTime.find_first_of(':'));
        std::string myMinutes = myCheckMemoryTime.substr(myCheckMemoryTime.find_first_of(':')+1, myCheckMemoryTime.length());
        _myCheckMemoryTimeInSecondsToday = atoi(myHours.c_str()) * 3600;
        _myCheckMemoryTimeInSecondsToday += atoi(myMinutes.c_str()) * 60;
        _myRestartMode |= CHECKMEMORYTIME;
        AC_DEBUG <<"_myCheckMemoryTimeInSecondsToday : " << _myCheckMemoryTimeInSecondsToday;
    }
    if (theAppNode->childNode("CheckTimedMemoryThreshold")) {
        _myRestartCheck = true;
        _myMemoryThresholdTimed = asl::as<int>((*theAppNode->childNode("CheckTimedMemoryThreshold"))("#text").nodeValue());
        _myRestartMode |= CHECKTIMEDMEMORYTHRESHOLD;
        AC_DEBUG <<"_myMemoryThresholdTimed : " << _myMemoryThresholdTimed;
    }
    if (theAppNode->childNode("Memory_Threshold")) {
        _myRestartCheck = true;
        _myRestartMemoryThreshold = asl::as<int>((*theAppNode->childNode("Memory_Threshold"))("#text").nodeValue());
        _myRestartMode |= MEMTHRESHOLD;
        AC_DEBUG <<"_myRestartMemoryThreshold : " << _myRestartMemoryThreshold;
    }
    if (theAppNode->childNode("WaitDuringStartup")) {
        _myStartDelay = asl::as<unsigned>((*theAppNode->childNode("WaitDuringStartup"))("#text").nodeValue());
        AC_DEBUG <<"_myStartDelay: " << _myStartDelay ;
    }
    if (theAppNode->childNode("WaitDuringRestart")) {
        _myRestartDelay = asl::as<unsigned>((*theAppNode->childNode("WaitDuringRestart"))("#text").nodeValue());
        AC_DEBUG <<"_myRestartDelay: " << _myRestartDelay ;
    }

    if (theAppNode->childNode("Heartbeat")) {
        const dom::NodePtr & myHeartbeatNode = theAppNode->childNode("Heartbeat");
        if (myHeartbeatNode->childNode("Heartbeat_File")) {
            _myHeartbeatFile = asl::expandEnvironment((*myHeartbeatNode->childNode("Heartbeat_File"))("#text").nodeValue());
            AC_DEBUG <<"_myHeartbeatFile : " << _myHeartbeatFile ;
        }
        if (myHeartbeatNode->childNode("Allow_Missing_Heartbeats")) {
            _myAllowMissingHeartbeats = asl::as<int>((*myHeartbeatNode->childNode("Allow_Missing_Heartbeats"))("#text").nodeValue());
            AC_DEBUG <<"_myAllowMissingHeartbeats : " << _myAllowMissingHeartbeats ;
        }
        if (myHeartbeatNode->childNode("Heartbeat_Frequency")) {
            _myHeartbeatFrequency = asl::as<int>((*myHeartbeatNode->childNode("Heartbeat_Frequency"))("#text").nodeValue());
            AC_DEBUG <<"_myHeartbeatFrequency : " << _myHeartbeatFrequency ;
        }
        _myPerformECG = true;
        if ((_myHeartbeatFrequency == 0) || (_myHeartbeatFile.empty())) {
            _myPerformECG = false;
        }
        if (myHeartbeatNode->childNode("FirstHeartBeatDelay")) {
            _myAppStartTimeInSeconds = asl::as<int>((*myHeartbeatNode->childNode("FirstHeartBeatDelay"))("#text").nodeValue());
            AC_DEBUG <<"_myAppStartTimeInSeconds : " << _myAppStartTimeInSeconds;
        }
    }
    return true;
}

void 
Application::setEnvironmentVariables() {
    for (int myEnvIndex = 0 ; myEnvIndex != _myEnvironmentVariables.size(); myEnvIndex++) {
        EnvironmentSetting & myEnvSetting = _myEnvironmentVariables[myEnvIndex];
        string & myEnviromentVariable = _myEnvironmentVariables[myEnvIndex]._myVariable;
        string & myEnvironmentValue = _myEnvironmentVariables[myEnvIndex]._myValue;
        SetEnvironmentVariable(myEnviromentVariable.c_str(), myEnvironmentValue.c_str());
        _myLogger.logToFile(string("Set environment variable: ") + myEnviromentVariable +
                            "=" + myEnvironmentValue);
        cerr << "Set environment variable: " << myEnviromentVariable <<
                "=" << myEnvironmentValue << endl;

    }
}

void
Application::terminate(const std::string & theReason, bool theWMCloseAllowed){
    _myLogger.logToFile(string("Terminate because: ") + theReason);

    // Send WM_CLOSE to window
    if (theWMCloseAllowed && _myWindowTitle != "") {
        HWND myHWnd = FindWindow (0, _myWindowTitle.c_str());
        if (!myHWnd) {
            _myLogger.logToFile(string("Could not find Window ") + _myWindowTitle);
        } else {
            // Find process handle for window
            DWORD myWindowProcessId;
            DWORD myWindowThreadId = GetWindowThreadProcessId(myHWnd, &myWindowProcessId);

            SendMessage(myHWnd, WM_CLOSE, 0, 0);
            _myLogger.logToFile("WM_CLOSE sent.");
            asl::msleep(500); // Give it some time to work...

            // Terminate process
            HANDLE myWindowProcessHandle = OpenProcess(0, false, myWindowProcessId);
            if (myWindowProcessHandle) {
                TerminateProcess(myWindowProcessHandle, 0);
                CloseHandle(myWindowProcessHandle);
            }
        }
    }
    else {
        // Terminate process
        _myLogger.logToFile("Try to terminate application.");
        TerminateProcess(_myProcessInfo.hProcess, 0);
        _myLogger.logToFile("O.k., terminated.");
    }

    _myProcessResult = WAIT_OBJECT_0;
    asl::msleep(500); // Give it some time to work...
    closeAllThreads();
}

bool
Application::checkForRestart() {
    if (_myMemoryIsFull || _myItIsTimeToRestart || _myHeartIsBroken) {
        string myMessage;
        if (_myMemoryIsFull) {
            myMessage = "Memory threshold reached.";
        }
        if (_myItIsTimeToRestart) {
            myMessage = "Time to restart reached.";
        }
        if (_myHeartIsBroken) {
            myMessage = "Failed to detect heartbeat.";
        }
        if (_myApplicationPaused) {
            myMessage = "Application was intentionally stopped.";
        }
        terminate(myMessage, true);
        return true;
    } else {
        return false;
    }
}

void
Application::launch() {
    setEnvironmentVariables();
    _myCommandLine = _myFileName + " " + _myArguments;

    STARTUPINFO StartupInfo = {
        sizeof(STARTUPINFO),
        NULL, NULL, NULL, 0, 0, 0, 0, 0, 0,
        0, STARTF_USESHOWWINDOW, SW_SHOWDEFAULT,
        0, NULL, NULL, NULL, NULL
    };

    bool myResult = CreateProcess(NULL, (&_myCommandLine[0]),
                    NULL, NULL, TRUE, 0,
                    NULL, NULL, &StartupInfo,
                    &_myProcessInfo);
    _myStartTimeInSeconds = getElapsedSecondsToday();

    // Error handling
    if (!myResult) {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
        );

        char myMessage[255];
        strcpy( myMessage, (LPCSTR) lpMsgBuf );
        strcat( myMessage, "\n\n" );
        strcat( myMessage, _myCommandLine.c_str() );

        cerr << myMessage << endl;

        LocalFree(lpMsgBuf);
        exit(-1);
    }
    _myLogger.logToFile("Started successfully: " + _myCommandLine);
    _myProcessResult = WAIT_TIMEOUT;

    _myItIsTimeToRestart   = false;
    _myHeartIsBroken       = false;
    _myMemoryIsFull        = false;
}

void
Application::checkHeartbeat() {
    if (_myPerformECG && (getElapsedSecondsToday() - _myStartTimeInSeconds >= _myAppStartTimeInSeconds) ) {
        string myHeartbeatStr;
        asl::readFile(_myHeartbeatFile, myHeartbeatStr );
        if ( myHeartbeatStr.empty()) {
            _myLogger.logToFile("Could not read heartbeatfile: " + _myHeartbeatFile);
            _myHeartIsBroken =  false;
            return;
        }
        dom::Document myHeartbeatXml;
        myHeartbeatXml.parse(myHeartbeatStr);

        string mySecondsSince1970Str = myHeartbeatXml.firstChild()->getAttributeString("secondsSince1970");
        __time64_t myCurrentSecondsSince_1_1_1970;
        _time64( &myCurrentSecondsSince_1_1_1970 );

        long long myLastHeartbeatAge =  myCurrentSecondsSince_1_1_1970 - (_atoi64(mySecondsSince1970Str.c_str()) / 1000);
        AC_DEBUG <<" myCurrentSecondsSince_1_1_1970 : " << myCurrentSecondsSince_1_1_1970 ;
        AC_DEBUG <<" last heartbeat sec since 1.1.70: "<<  (_atoi64(mySecondsSince1970Str.c_str())/ 1000) ;
        AC_DEBUG <<" last age : " << myLastHeartbeatAge ;
        if ( myLastHeartbeatAge > _myHeartbeatFrequency * _myAllowMissingHeartbeats) {
            _myHeartIsBroken = true;
        } else {
            _myHeartIsBroken =  false;
        }
    } else {
        _myHeartIsBroken =  false;
    }
}

void
Application::closeAllThreads() {
    // clean up the application: collect all threads and kill em
    AC_DEBUG << "clean up the application: collect all threads and kill em ";
    _myLogger.logToFile("Clean up the application: collect all threads and kill em");
    THREADENTRY32  myThreadInfo;
    myThreadInfo.dwSize = sizeof( THREADENTRY32 );
    HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD,0);//TH32CS_SNAPPROCESS, 0 );
    int myThreadNum = 0;
    if( hProcessSnap == INVALID_HANDLE_VALUE ) {
        cerr <<"Application::closeAllThreads() failed " << endl;
    } else {
        if (Thread32First(hProcessSnap, &myThreadInfo)){
            do {
                _myLogger.logToFile(string("### found a running thread with id: ") + asl::as_string(myThreadInfo.th32ThreadID));
                if( myThreadInfo.th32OwnerProcessID == _myProcessInfo.dwProcessId )    {
                    myThreadNum++;
                    AC_DEBUG << "### found a running thread with id:" << myThreadInfo.th32ThreadID << ", terminate...";
                    HANDLE myChildThreadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, myThreadInfo.th32ThreadID);
                    CloseHandle(myChildThreadHandle);
                }
            } while(Thread32Next(hProcessSnap, &myThreadInfo));
        } else {
            dumpWinError( "Thread32First" );  // Show cause of failure
        }
        CloseHandle(hProcessSnap);
    }
    if (myThreadNum > 0) {
        cerr <<"Process had still " << myThreadNum << " threads, terminated them" << endl << endl;
    }
    _myLogger.logToFile(string("Closed all threads: ") + asl::as_string(myThreadNum));
}

void
Application::shutdown() {
    // Terminate process
    terminate("Application shutdown", false);

    // Close process and thread handles.
    CloseHandle( _myProcessInfo.hProcess );
    CloseHandle( _myProcessInfo.hThread );
}

std::string
Application::runUntilNextCheck(int theWatchFrequency) {
    // Wait until child process exits.
    _myProcessResult = WaitForSingleObject( _myProcessInfo.hProcess, 1000 * theWatchFrequency );

    if (_myProcessResult == WAIT_FAILED) {
        LPVOID lpMsgBuf;
        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       GetLastError(),
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPTSTR) &lpMsgBuf,
                       0,
                       NULL );
        std::string myProcessOutputString  = ((LPCTSTR)lpMsgBuf);
        LocalFree( lpMsgBuf );
        return string("Return code of process: ") + myProcessOutputString;
    }
    return string("Internal quit.");
}

void
Application::checkState() {

    if (_myRestartCheck) {
        // get memory available/free
        unsigned myAvailMem = 0;
#if 1 
        myAvailMem = asl::getFreeMemory() / 1024;
#else
        MEMORYSTATUS myMemoryStatus;
        GlobalMemoryStatus (&myMemoryStatus);
        myAvailMem = myMemoryStatus.dwAvail / 1024; 
#endif
        _myMemoryIsFull = (myAvailMem <= _myRestartMemoryThreshold);
        if (_myMemoryIsFull) {
            _myLogger.logToFile(std::string("Avail. phy. mem.: ") + asl::as_string(myAvailMem));
        }

        long myElapsedSecondsToday = getElapsedSecondsToday();

        // get the day of the week
        time_t myTime;
        time(&myTime);
        struct tm * myPrintableTime = localtime(&myTime);
        std::string myWeekday = ourWeekdayMap[myPrintableTime->tm_wday];
        if (myWeekday != _myLastWeekday) {
            if ( ! _myLastWeekday.empty()) {
                _myDayChanged = true;
            }
            _myLastWeekday = myWeekday;
        } else {
            _myDayChanged = false;
        }

        std::transform(myWeekday.begin(), myWeekday.end(), myWeekday.begin(), toupper);

        // time check
        if (_myRestartMode & RESTARTTIME) {
            if (!_myRestartedToday) {
                if (myElapsedSecondsToday > _myRestartTimeInSecondsToday) {
                    if ( _myRestartMode & RESTARTDAY) {
                        if (myWeekday == _myRestartDay) {
                            _myItIsTimeToRestart = true;
                            _myRestartedToday = true;
                        }
                    } else {
                        _myItIsTimeToRestart = true;
                        _myRestartedToday = true;
                    }
                }
            }
        }

        // day check
        if ( _myDayChanged ) {
            _myRestartedToday = false;
            _myCheckedMemoryToday = false;
            if (!(_myRestartMode & RESTARTTIME)) {
                if ( _myRestartMode & RESTARTDAY) {
                    if (myWeekday == _myRestartDay) {
                        _myItIsTimeToRestart = true;
                    }
                }
            }
        }

        // yellow section
        if ((_myRestartMode & CHECKMEMORYTIME) && (_myRestartMode & CHECKTIMEDMEMORYTHRESHOLD)) {
            if (!_myCheckedMemoryToday) {
                if (myElapsedSecondsToday > _myCheckMemoryTimeInSecondsToday) {
                    _myMemoryIsFull = myAvailMem <= _myMemoryThresholdTimed;
                    _myCheckedMemoryToday = true;
                }
            }
        }
    }
}

unsigned Application::getRestartDelay() const {
    return _myRestartDelay;
}

unsigned Application::getStartDelay() const {
    return _myStartDelay;
}

bool Application::paused() const {
    return _myApplicationPaused;
}

bool Application::performECG() const {
    return _myPerformECG;
}

bool Application::restartedToday() const {
    return _myRestartedToday;
}

std::string Application::getHeartbeatFile() const {
    return _myHeartbeatFile;
}

long Application::getRestartTimeInSecondsToday() const {
    return _myRestartTimeInSecondsToday;
}

DWORD Application::getProcessResult() const {
    return _myProcessResult;
}

std::string Application::getFilename() const {
    return _myFileName;
}

std::string Application::getArguments() const {
    return _myArguments;
}

void Application::setPaused(bool thePausedFlag) {
    _myApplicationPaused = thePausedFlag;
}

void Application::setRestartedToday(bool theRestartedTodayFlag) {
    _myRestartedToday = theRestartedTodayFlag;
}
       
long getElapsedSecondsToday() {
    __time64_t ltime;
    struct tm *newtime;
    _time64( &ltime );
    newtime = _localtime64( &ltime );
    long myElapsedSecondsToday = newtime->tm_hour * 3600;
    myElapsedSecondsToday += newtime->tm_min * 60;
    myElapsedSecondsToday += newtime->tm_sec;
    return myElapsedSecondsToday;
}
