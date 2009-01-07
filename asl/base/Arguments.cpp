/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: command line argument parser
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    formatting             : perfect
//    documentation          : ok
//    test coverage          : ok
//    names                  : ok
//    style guide conformance: ok
//    technical soundness    : ok
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    cheesyness             : ok
//
//    overall review status  : ok
//
//    recommendations: none
//       
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Arguments.h"
#include "Path.h"
#include "Logger.h"
#include "Revision.h"

#include <sstream>

using namespace std;

#define DB(x) // x

namespace asl {
Arguments::Arguments(const AllowedOption * allowedOptions) {
    if (allowedOptions) {
        addAllowedOptions(allowedOptions);
    }
}

void
Arguments::addAllowedOptions(const AllowedOption * allowedOptions) {
    int i = 0;
    while ((string(allowedOptions[i].theName) != "" ||  string(allowedOptions[i].theArgumentName) != "")
            && i < MAX_OPTIONS) {
                if ( !std::string(allowedOptions[i].theName).empty() ) {
            _allowedOptions[allowedOptions[i].theName] = allowedOptions[i].theArgumentName;
        } else {
            _argumentNames.push_back(allowedOptions[i].theArgumentName);
        }
        ++i;
    }
    _allowedOptions["--revision"] = "";
    _allowedOptions["--version"] = "";
    _allowedOptions["--copyright"] = "";
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
Arguments::parse(std::vector<string> myArgs, StringEncoding theEncoding, int errorHandlingPolicy) {
    vector<const char *> myArgv;

    for (std::vector<string>::size_type i = 0; i < myArgs.size(); ++i) {
        myArgv.push_back(myArgs[i].c_str());
    }
    return parse(myArgv.size(), &(*myArgv.begin()), theEncoding, errorHandlingPolicy);
}

bool
Arguments::parse(int argc, const char * const argv[], StringEncoding theEncoding, int errorHandlingPolicy) {
    bool myIgnoreFurtherOptionsFlag = false;
    for (int i = 0; i < argc; ++i) {
        _allArguments.push_back(Path(argv[i], theEncoding).toUTF8());
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
                 if (foundItem->first == "--copyright") {
                    printCopyright();
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
                    _allArguments.push_back(Path(argv[i], theEncoding).toUTF8());
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
                //printCopyright();
            }
        }

    }
    return argc != 0;
}

void
Arguments::printCopyright() const {
    AC_PRINT << _programName << " Copyright (C) 2003-2006 ART+COM";
}

void
Arguments::printRevision() const {
    AC_PRINT << "Revision: " << ourRevision;
}

void
Arguments::printVersion() const {
    AC_PRINT << "Build on " << __DATE__ << " at " << __TIME__ << " (Rev: " << asl::ourRevision << ")";
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


