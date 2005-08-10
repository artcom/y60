//============================================================================
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile : $
//   $Author  : $
//   $Revision: 1.8 $
//   $Date    : $
//
//
//  Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "settings.h"
#include "Arguments.h"
#include "Logger.h"
#include "Revision.h"

#include <sstream>

using namespace std;

#define DB(x) // x

namespace asl {
    Arguments::Arguments(){
}

Arguments::Arguments(const AllowedOption * allowedOptions)  {
    addAllowedOptions(allowedOptions);
}

void
Arguments::addAllowedOptions(const AllowedOption * allowedOptions) {
    int i = 0;
    while ((string(allowedOptions[i].theName) != "" ||  string(allowedOptions[i].theArgumentName) != "")
            && i < MAX_OPTIONS) {
        if (allowedOptions[i].theName != "") {
            _allowedOptions[allowedOptions[i].theName] = allowedOptions[i].theArgumentName;
        } else {
            _argumentNames.push_back(allowedOptions[i].theArgumentName);
        }
        ++i;
    }
    _allowedOptions["--revision"] = "";
    _allowedOptions["--version"] = "";
}

const string &
Arguments::getOptionArgument(const string & option) const {
    map<string, string>::const_iterator it = _allowedOptions.find(option);

    if (it == _allowedOptions.end()) {
        throw IllegalOptionQuery(option,PLUS_FILE_LINE);
    }

    if (!it->second.empty()) {
        map<string,int>::const_iterator foundItem = _options.find(option);
        if (foundItem != _options.end()) {
            return _allArguments[foundItem->second + 1];
        }
    }
    static string emptyString;
    return emptyString;
}


bool
Arguments::haveOption(const string & option) const {
    if (_allowedOptions.find(option) == _allowedOptions.end()) {
        throw IllegalOptionQuery(option,PLUS_FILE_LINE);
    }
    return _options.find(option) != _options.end();
}

std::vector<std::string>
Arguments::getOptionNames() const {
    vector<string> myOptionNames;
    map<string, int>::const_iterator it = _options.begin();
    for (; it != _options.end(); ++it) {
        myOptionNames.push_back(it->first);
    }
    return myOptionNames;
}


bool
Arguments::parse(std::vector<string> myArgs, int errorHandlingPolicy) {
    vector<const char *> myArgv;

    for (int i = 0; i < myArgs.size(); ++i) {
        myArgv.push_back(myArgs[i].c_str());
    }
    return parse(myArgv.size(), &(*myArgv.begin()), errorHandlingPolicy);
}

bool
Arguments::parse(int argc, const char * const argv[], int errorHandlingPolicy) {
    bool myIgnoreFurtherOptionsFlag = false;
    for (int i = 0; i < argc; ++i) {
        _allArguments.push_back(string(argv[i]));
        DB(AC_DEBUG << "Parsing '" << argv[i] << "'" << endl);

        if (myIgnoreFurtherOptionsFlag) {
            _justArguments.push_back(i);
        } else if (string("--") == argv[i]) {
            DB(AC_DEBUG << "Ignoring" << endl);
            myIgnoreFurtherOptionsFlag = true;
        } else if (argv[i][0] == OPTION_START_CHAR) {
            map<string,string>::const_iterator foundItem = _allowedOptions.find(argv[i]);
            if (foundItem != _allowedOptions.end()) {
                if (foundItem->first == "--revision") {
                    printRevision();
                    return false;
                }
                if (foundItem->first == "--version") {
                    printVersion();
                    return false;
                }
                _options[argv[i]] = i;
                if (foundItem->second != "") {
                    // needs argument
                    if (argc <= i+1) {
                        //no argument available
                        AC_ERROR << "Argument missing for option '" << argv[i] << "'" << endl;
                        printUsage();
                        return false;
                    }
                    // take next argument
                    ++i;
                    _allArguments.push_back(argv[i]);
                }
            } else {
                if (errorHandlingPolicy & BailOnUnknownOptions) {
                    AC_ERROR << "Illegal option: " << argv[i];
                    printUsage();
                    return false;
                }
            }
        } else {
            if (i) {
                _justArguments.push_back(i);
            } else {
                _programName = argv[0];
                string::size_type myRealNameStart = _programName.rfind(theDirectorySeparator);
                _programName = _programName.substr(++myRealNameStart);
                printCopyright();
            }
        }

    }
    return argc != 0;
}

void
Arguments::printCopyright() const {
    AC_PRINT << _programName << " Copyright (C) 2003-2005 ART+COM";
}

void
Arguments::printRevision() const {
    AC_PRINT << "Revision: " << ourRevision;
}

void
Arguments::printVersion() const {
    AC_PRINT << "build on " << __DATE__ << " at " << __TIME__
             << " (Rev: " << asl::ourRevision << ")";
}

void
Arguments::printUsage() const {
    ostringstream myUsage;
    myUsage << "Usage: " << _programName << " ";
    for (map<string,string>::const_iterator it = _allowedOptions.begin();
            it != _allowedOptions.end(); ++it) {
        myUsage << "[" << it->first;
        if (it->second != "") {
            myUsage << " <" << it->second << ">";
        }
        myUsage << "] ";
    }
    for (unsigned i = 0; i < _argumentNames.size(); ++i) {
        myUsage << _argumentNames[i] << " ";
    }

    if (_myArgumentDescription.size() > 0) {
        myUsage << _myArgumentDescription;
    }
    AC_PRINT << myUsage.str();
}

void
Arguments::setArgumentDescription(const char * theArgumentDescription) {
    _myArgumentDescription = theArgumentDescription;
}

} // namespace asl

//=============================================================================
//
// $Log: Arguments.cpp,v $
// Revision 1.8  2005/04/24 00:30:18  pavel
// make use of  asl::Logger for majority of messages
//
// Revision 1.7  2005/01/18 12:36:38  martin
// added argument parser to JS
//
// Revision 1.6  2003/10/30 16:37:12  david
//  - fixed build
//
// Revision 1.5  2003/10/29 18:11:21  david
//  - initial checkin of serial device classes
//
// Revision 1.4  2003/06/28 22:37:14  pavel
// fixed a usage argument name print glitch
//
// Revision 1.3  2003/05/14 12:45:15  uzadow
// no message
//
// Revision 1.6  2002/10/30 16:24:47  uzadow
// Massive windows compatibility changes
//
// Revision 1.5  2002/10/18 11:02:49  wolfger
// no message
//
//
//=============================================================================

