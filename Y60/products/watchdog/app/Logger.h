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
//   $RCSfile: Logger.h,v $
//   $Author: ulrich $
//   $Revision: 1.3 $
//   $Date: 2004/11/20 17:19:00 $
//
//=============================================================================

//
// Logger.h : A simple Logger for watchdogs and bones
//
#ifndef INCL_LOGGER
#define INCL_LOGGER

#include <fstream>
#include <string>

class Logger {
    public:
        Logger();
        ~Logger();
        void openLogFile(const std::string & theLogFilename);
        void logToFile(const std::string& theMessage);
    private:
        std::ofstream * _myFile;
};
#endif // INCL_LOGGER
