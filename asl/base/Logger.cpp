//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Logger.cpp,v $
//   $Author: pavel $
//   $Revision: 1.12 $
//   $Date: 2005/04/29 08:09:22 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#include "Logger.h"
#include "LogMessageFormatters.h"
#include "LogMessageSinks.h"

#include "error_functions.h"
#include "string_functions.h"

using namespace asl;
using namespace std;

const char * asl::SeverityName[] = {"PRINT","FATAL", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE", "DISABLED", 0};

/// Environment variable name for per-module verbosities
const char * LOG_MODULE_VERBOSITY_ENV = "AC_LOG_MODULE_VERBOSITY";
const char * LOG_GLOBAL_VERBOSITY_ENV = "AC_LOG_VERBOSITY";

const char * LOG_VISUAL_STUDIO_FORMAT_ENV = "AC_LOG_VISUAL_STUDIO_FORMAT";
const char * LOG_VISUAL_STUDIO_FILTER_ENV = "AC_LOG_VISUAL_STUDIO_FILTER";

const char * LOG_CONSOLE_FORMAT_ENV = "AC_LOG_CONSOLE_FORMAT";
const char * LOG_CONSOLE_FILTER_ENV = "AC_LOG_CONSOLE_FILTER";

const char * LOG_FILE_NAME_ENV   = "AC_LOG_FILE_NAME";
const char * LOG_FILE_FORMAT_ENV = "AC_LOG_FILE_FORMAT";
const char * LOG_FILE_FILTER_ENV = "AC_LOG_FILE_FILTER";

Ptr<LogMessageFormatter> createFormatter(const string theFormatterName, vector<string> & theTriedNames) {
    if (theFormatterName == FullLogMessageFormatter::name()) {
        return Ptr<LogMessageFormatter>(new FullLogMessageFormatter);
    }
    theTriedNames.push_back(FullLogMessageFormatter::name());

    if (theFormatterName == TerseLogMessageFormatter::name()) {
        return Ptr<LogMessageFormatter>(new TerseLogMessageFormatter);
    }
    theTriedNames.push_back(TerseLogMessageFormatter::name());

    if (theFormatterName == VisualStudioLogMessageFormatter::name()) {
        return Ptr<LogMessageFormatter>(new VisualStudioLogMessageFormatter);
    }
    theTriedNames.push_back(VisualStudioLogMessageFormatter::name());

    return Ptr<LogMessageFormatter>(0);
}

Ptr<LogMessageFormatter>
createFormatter(const char * theFormatterNameVar, const char * theDefaultFormatterName, const char * theSinkName) {
    const char * myEnv = getenv(theFormatterNameVar);
    vector<string> myTriedNames;
    Ptr<LogMessageFormatter> myFormatter(0);
    if (myEnv) {
        myFormatter = createFormatter(myEnv, myTriedNames);
    }
    if (!myFormatter) {
        if (myEnv) {
            cerr << "### WARNING: createFormatter: could not find formatter '"<< myEnv <<"' (defined in environment variable "
                 << theFormatterNameVar << ") for sink "
                 <<theSinkName<<"', using formatter '"<<theDefaultFormatterName<<"'"
                 << ", valid formatters are: ";
            for (int i=0; i < myTriedNames.size(); ++i) {
                cerr << "'"<<myTriedNames[i]<<"' ";
            }
            cerr <<endl;
        }
        myFormatter = createFormatter(theDefaultFormatterName, myTriedNames);
    }
    return myFormatter;
}

asl::Severity
getSeverity(const char * theSeverityVarName, asl::Severity theDefaultValue) {
    const char * myEnv = getenv(theSeverityVarName);
    if (myEnv) {
        try {            
            asl::Severity mySeverity = (asl::Severity) asl::getEnumFromString(std::string(myEnv), asl::SeverityName);
            return mySeverity;
        }
        catch (asl::ParseException &) {
            cerr << "### WARNING: getSeverity: could not find formatter '"<< myEnv <<"' (defined in environment variable "
                 << theSeverityVarName <<", using default value " << asl::SeverityName[theDefaultValue]<< endl;
        }
    }
    return theDefaultValue;
}

Logger::Logger() :
    _myFormatter(Ptr<LogMessageFormatter>(new FullLogMessageFormatter)),
    _myGlobalVerbosity(SEV_WARNING)
{
    Ptr<LogMessageFormatter> myConsoleFormatter = createFormatter(LOG_CONSOLE_FORMAT_ENV, TerseLogMessageFormatter::name(), "console");
    asl::Severity myConsoleSeverity = getSeverity(LOG_CONSOLE_FILTER_ENV, SEV_WARNING);
    if (myConsoleSeverity != SEV_DISABLED) {  
        addMessageSink(Ptr<MessageSink>(new StreamPrinter(std::cerr)), myConsoleFormatter, myConsoleSeverity);
    }

#ifdef WIN32
    Ptr<LogMessageFormatter> myVisualStudioFormatter = createFormatter(LOG_VISUAL_STUDIO_FORMAT_ENV, VisualStudioLogMessageFormatter::name(), "console");
    asl::Severity myVisualStudioSeverity = getSeverity(LOG_VISUAL_STUDIO_FILTER_ENV, SEV_DISABLED);
    if (myVisualStudioSeverity != SEV_DISABLED) {  
        addMessageSink(Ptr<MessageSink>(new OutputWindowPrinter), myVisualStudioFormatter, myVisualStudioSeverity);
    }
#endif
    Ptr<LogMessageFormatter> myFileFormatter = createFormatter(LOG_FILE_FORMAT_ENV, FullLogMessageFormatter::name(), "console");
    asl::Severity myFileSeverity = getSeverity(LOG_FILE_FILTER_ENV, SEV_DISABLED);
    if (myFileSeverity != SEV_DISABLED) { 
        char * myLogFileName = getenv(LOG_FILE_NAME_ENV);
        if (!myLogFileName) {
            myLogFileName = "asl_logfile.txt";
        }
        _myLogFile.open(myLogFileName);
        if (_myLogFile) {
            addMessageSink(Ptr<MessageSink>(new StreamPrinter(_myLogFile)), myFileFormatter, myFileSeverity);
        } else {
            cerr << "### WARNING: Logger: could not open formatter '"<< myLogFileName <<"' (may be defined in environment variable "
                 << LOG_FILE_NAME_ENV <<")"<< endl;
        }
    }


    const char * myEnv = getenv(LOG_GLOBAL_VERBOSITY_ENV);
    if (myEnv) {
        setVerbosity((asl::Severity) asl::getEnumFromString(std::string(myEnv), asl::SeverityName));
    }

    myEnv = getenv(LOG_MODULE_VERBOSITY_ENV);
    if (myEnv) {
        std::string myLogLevelString(myEnv);
        std::string::size_type myColon;
        while ((myColon = myLogLevelString.find(":")) != std::string::npos) {
            setModuleVerbosity(myLogLevelString.substr(0, myColon));
            myLogLevelString = myLogLevelString.substr(myColon+1);
        }
        if (myLogLevelString.size()) {
            setModuleVerbosity(myLogLevelString);
        }
    }
}

/**
return true if theSeverity is higher (numerically smaller) than the verbosity setting
a different verbosity can be defined for any id range in any module; if there are different
verbosity settings for an overlapping id region in the same module, the setting for the
smallest id-range takes precedence.
*/
bool
Logger::IfLog(Severity theSeverity, const char * theModule, int theId) {
    Severity myVerbosity = _myGlobalVerbosity;
    if (!_myVerbositySettings.empty()) {
        const std::string myModule(file_string(theModule)); // remove everything before the last backslash
        // find all setting for a particular module
        std::multimap<std::string,ModuleVerbosity>::const_iterator myLowerBound =
            _myVerbositySettings.lower_bound(myModule);
        if (myLowerBound != _myVerbositySettings.end()) {
            std::multimap<std::string,ModuleVerbosity>::const_iterator myUpperBound =
                _myVerbositySettings.upper_bound(myModule);

            // find smallest range containing theId with matching module name
            unsigned int myRange = std::numeric_limits<unsigned int>::max();
            for (std::multimap<std::string,ModuleVerbosity>::const_iterator myIter = myLowerBound;
                myIter != myUpperBound; ++myIter)
            {
                if (myIter->first == myModule) {
                    int myMinId = myIter->second.myMinId;
                    int myMaxId = myIter->second.myMaxId;
                    if (theId >= myMinId && theId <= myMaxId) {
                        unsigned int myNewRange = myMaxId - myMinId;
                        if (myNewRange < myRange) {
                            myVerbosity = myIter->second.myVerbosity;
                            myRange = myNewRange;
                        }
                    }
                }
            }
        }
    }
    return theSeverity <= myVerbosity;
}

void
Logger::log(asl::Time theTime, Severity theSeverity, const char * theModule, int theId, const std::string & theText) {
    std::string myMessage;
    LAST_ERROR_TYPE myError = lastError(); // do not affect error state by
    if (theSeverity > SEV_PRINT) {
        myMessage = _myFormatter->format(theTime, theSeverity, theModule, theId, theText);
    } else {
        myMessage = theText;
    }
    for (int i = 0; i < _myMessageSinks.size(); ++i) {     
        if (theSeverity > SEV_PRINT) {
            if (_mySinkFormatter[i]) {
                myMessage = _mySinkFormatter[i]->format(theTime, theSeverity, theModule, theId, theText);
            } else {
                myMessage = _myFormatter->format(theTime, theSeverity, theModule, theId, theText);
            }
        } else {
            myMessage = theText;
        }
        _myMessageSinks[i]->push(myMessage);
    }
    setLastError(myError);
}


void
Logger::getModuleVerbosity(int i, std::vector<ModuleVerbosityEntry> & theVerbositySetting)
{
    theVerbositySetting.resize(0);
    for (std::multimap<std::string,ModuleVerbosity>::const_iterator myIter = _myVerbositySettings.begin();
        myIter !=_myVerbositySettings.end(); ++ myIter)
    {
        theVerbositySetting.push_back(
            ModuleVerbosityEntry(
                myIter->first,
                myIter->second.myVerbosity,
                myIter->second.myMinId,
                myIter->second.myMaxId));
    }
}

void
Logger::setVerbosity(Severity theVerbosity) {
    _myGlobalVerbosity = theVerbosity;
}

Severity
Logger::getVerbosity() const {
    return _myGlobalVerbosity;
}


void
Logger::setModuleVerbosity(Severity theVerbosity,
                           const std::string & theModule,
                           int theMinId, int theMaxId)
{
    // TODO: remove item when there is an exact match
    _myVerbositySettings.insert(std::pair<const std::string, ModuleVerbosity>(
       file_string(theModule.c_str()), ModuleVerbosity(theVerbosity, theMinId, theMaxId)));
}

void
Logger::setModuleVerbosity(const std::string & theVerbosityString) {
    std::string myVerbosityString = theVerbosityString;
    std::vector<std::string> mySubStrings;

    std::string::size_type mySlash;
    while ((mySlash = myVerbosityString.find('/')) != std::string::npos) {
        std::string myString = myVerbosityString.substr(0, mySlash);
        mySubStrings.push_back(myString);
        myVerbosityString = myVerbosityString.substr(mySlash+1);
    }
    if (myVerbosityString.size()) {
        mySubStrings.push_back(myVerbosityString);
    }

    if (mySubStrings.size() < 2) {
        if (!theVerbosityString.empty()) {
            std::cerr << "### Insufficient arguments for module verbosity '" << theVerbosityString << "'" << std::endl;
        }
        return;
    }

    Severity mySeverity = Severity(getEnumFromString(mySubStrings[0], asl::SeverityName));
    std::string myModule = mySubStrings[1];
    int myMinId = 0, myMaxId = std::numeric_limits<int>::max();
    if (mySubStrings.size() > 2 && mySubStrings[2].size()) {
        myMinId = asl::as_int(mySubStrings[2]);
    }
    if (mySubStrings.size() > 3 && mySubStrings[3].size()) {
        myMaxId = asl::as_int(mySubStrings[3]);
    }
    setModuleVerbosity(mySeverity, myModule, myMinId, myMaxId);
}
 
void
Logger::addMessageSink(Ptr<MessageSink> theSink, Ptr<LogMessageFormatter> theFormatter, Severity theSeverityFilter) {
    _myMessageSinks.push_back(theSink);
    _mySinkFormatter.push_back(theFormatter);
    _mySinkSeverityFilter.push_back(theSeverityFilter);
}

void
Logger::setMessageFormatter(Ptr<LogMessageFormatter> theFormatter) {
    _myFormatter = theFormatter;
}

void
Logger::setMessageFormatter(Ptr<MessageSink> theSink, Ptr<LogMessageFormatter> theFormatter, Severity theSeverityFilter) {
    for (int i = 0; i < _myMessageSinks.size(); ++i) {
        if (theSink == _myMessageSinks[i]) {
            _mySinkFormatter[i] = theFormatter;
            _mySinkSeverityFilter[i] = theSeverityFilter;
            return;
        }
    }
    throw UnknownSinkExeption(JUST_FILE_LINE);
}
