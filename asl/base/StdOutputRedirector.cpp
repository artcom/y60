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


#include "StdOutputRedirector.h"
#include "TimeStreamFormater.h"

#include "Arguments.h"
#include "os_functions.h"
#include "string_functions.h"
#include "Time.h"

#include <iostream>
#include <sstream>


namespace asl {

    std::string expandString(const std::string & theString) {

        std::string myString = theString;
        std::string::size_type pos;

        // hostname
        pos = myString.find("%h");
        if (pos != std::string::npos) {
            myString.replace(pos, 2, asl::hostname());
        }

        // date
        pos = myString.find("%d");
        if (pos != std::string::npos) {
            asl::Time now;
            const char * myFormatString = "%Y-%M-%D-%h-%m";
            std::ostringstream myTimeString;
            myTimeString << asl::formatTime(myFormatString) << now;
            myString.replace(pos, 2, myTimeString.str());
        }
        return myString;
    }

    StdOutputRedirector::StdOutputRedirector(const Arguments & theArguments) 
        : _myOutFile(0)
    {
        if (theArguments.haveOption("--std-logfile")) {
            std::string myFilename = expandString(theArguments.getOptionArgument("--std-logfile"));
            //std::cerr << "Logging std output to " << myFilename<< std::endl;
            _myOutputStreamOut.open(myFilename.c_str());        
            _myOutFile = _myOutputStreamOut.rdbuf();
            std::cout.rdbuf(_myOutFile);
            std::cerr.rdbuf(_myOutFile);
        }
    }

    StdOutputRedirector::~StdOutputRedirector()  {
        if (_myOutFile) {
            _myOutputStreamOut.close();            
        }
    }
}

