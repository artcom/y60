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
//  Description: A little logger class for dogs and bones
//
//=============================================================================

#include "Logger.h"

#include <time.h>
#include <iostream>

using namespace std;

Logger::Logger() : _myFile(0) {}
Logger::~Logger() {
    if (_myFile) {
        delete _myFile;
    }
}
void
Logger::openLogFile(const std::string & theLogFilename) {
    if (_myFile) {
        delete _myFile;
    }
    if (!theLogFilename.empty()) {
         _myFile = new ofstream(theLogFilename.c_str(), ios::out|ios::app);
        if (!_myFile) {
            cerr << string("Watchdog - Could not open log file ") + theLogFilename << endl;
        } else {
            (*_myFile) << "---------------------------------------------------------------------" << endl;
        }
    }
}
void
Logger::logToFile(const string& theMessage) {
    if (_myFile) {
        time_t myTime;
        time(&myTime);
        struct tm * myPrintableTime = localtime(&myTime);
        (*_myFile) << asctime(myPrintableTime) << theMessage << endl;
        (*_myFile) << "---------------------------------------------------------------------" << endl;
    }
}


