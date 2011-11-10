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
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "Logger.h"

#include <cstdlib>

#include "LogMessageFormatters.h"
#include "LogMessageSinks.h"

#include "error_functions.h"
#include "string_functions.h"

//#define DEBUG_LOGGER
#ifdef DEBUG_LOGGER
#warning asl::Logger debugging is enable
#include "Dashboard.h"
#else
#define MAKE_SCOPE_TIMER(NAME)
#endif

namespace asl {

const char * SeverityName[] = {"PRINT","FATAL", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE", "DISABLED", 0};

/// Environment variable name for per-module verbosities
const char * LOG_MODULE_VERBOSITY_ENV = "AC_LOG_MODULE_VERBOSITY";
const char * LOG_GLOBAL_VERBOSITY_ENV = "AC_LOG_VERBOSITY";

const char * LOG_VISUAL_STUDIO_FORMAT_ENV = "AC_LOG_VISUAL_STUDIO_FORMAT";
const char * LOG_VISUAL_STUDIO_FILTER_ENV = "AC_LOG_VISUAL_STUDIO_FILTER";

const char * LOG_CONSOLE_FORMAT_ENV = "AC_LOG_CONSOLE_FORMAT";
const char * LOG_CONSOLE_FILTER_ENV = "AC_LOG_CONSOLE_FILTER";

const char * LOG_SYSLOG_FORMAT_ENV = "AC_LOG_SYSLOG_FORMAT";
const char * LOG_SYSLOG_FILTER_ENV = "AC_LOG_SYSLOG_FILTER";

const char * LOG_FILE_NAME_ENV   = "AC_LOG_FILE_NAME";
const char * LOG_FILE_FORMAT_ENV = "AC_LOG_FILE_FORMAT";
const char * LOG_FILE_FILTER_ENV = "AC_LOG_FILE_FILTER";

Ptr<LogMessageFormatter> createFormatter(const std::string theFormatterName, std::vector<std::string> & theTriedNames) {
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

    return Ptr<LogMessageFormatter>();
}

Ptr<LogMessageFormatter>
createFormatter(const char * theFormatterNameVar, const char * theDefaultFormatterName, const char * theSinkName) {
    const char * myEnv = getenv(theFormatterNameVar);
    std::vector<std::string> myTriedNames;
    Ptr<LogMessageFormatter> myFormatter;
    if (myEnv) {
        myFormatter = createFormatter(myEnv, myTriedNames);
    }
    if (!myFormatter) {
        if (myEnv) {
            std::cerr << "### WARNING: createFormatter: could not find formatter '"<< myEnv <<"' (defined in environment variable "
                 << theFormatterNameVar << ") for sink "
                 <<theSinkName<<"', using formatter '"<<theDefaultFormatterName<<"'"
                 << ", valid formatters are: ";
            for (std::vector<std::string>::size_type i=0; i < myTriedNames.size(); ++i) {
                std::cerr << "'"<<myTriedNames[i]<<"' ";
            }
            std::cerr << std::endl;
        }
        myFormatter = createFormatter(theDefaultFormatterName, myTriedNames);
    }
    return myFormatter;
}

static Severity getSeverityFromString(const std::string & theSeverityString,
                                      Severity theDefaultValue)
{
    try {
        Severity mySeverity = (Severity) getEnumFromString(theSeverityString, SeverityName);
        return mySeverity;
    }
    catch (ParseException &) {
        std::cerr << "### WARNING: getSeverityFromString: Unknown severity '"<< theSeverityString <<"' using default value " << SeverityName[theDefaultValue]<< std::endl;
    }
    return theDefaultValue;
}

static Severity getSeverityFromEnv(const char * theSeverityVarName,
                                   Severity theDefaultValue)
{
    const char * myEnv = getenv(theSeverityVarName);
    if (myEnv) {
        return getSeverityFromString(myEnv, theDefaultValue);
    }
    return theDefaultValue;
}

Logger::Logger() :
    _myFormatter(Ptr<LogMessageFormatter>(new FullLogMessageFormatter)),
    _myGlobalVerbosity(SEV_WARNING)
{
    Ptr<LogMessageFormatter> myConsoleFormatter = createFormatter(LOG_CONSOLE_FORMAT_ENV, TerseLogMessageFormatter::name(), "console");
    Severity myConsoleSeverity = getSeverityFromEnv(LOG_CONSOLE_FILTER_ENV, SEV_TRACE);
    if (myConsoleSeverity != SEV_DISABLED) {
        addMessageSink(Ptr<MessageSink>(new StreamPrinter(std::cerr)), myConsoleFormatter, myConsoleSeverity);
    }

#ifndef _WIN32
    Ptr<LogMessageFormatter> mySyslogFormatter = createFormatter(LOG_SYSLOG_FORMAT_ENV, TerseLogMessageFormatter::name(), "console");
    Severity mySyslogSeverity = getSeverityFromEnv(LOG_SYSLOG_FILTER_ENV, SEV_DISABLED);
    if (mySyslogSeverity != SEV_DISABLED) {
        addMessageSink(Ptr<MessageSink>(new SyslogPrinter()), mySyslogFormatter, mySyslogSeverity);
    }
#endif

#ifndef _SETTING_WITH_TRACE_LOG_
     if (myConsoleSeverity == SEV_TRACE) {
            std::cerr << "### WARNING: Logger: TRACE requested, but traces were disabled at compile time in release build. Compile with -DWITH_TRACE_LOG to compile with tracing, use debug build or modify settings.h "<<std::endl;
    }
#endif


#ifdef _WIN32
    Ptr<LogMessageFormatter> myVisualStudioFormatter = createFormatter(LOG_VISUAL_STUDIO_FORMAT_ENV, VisualStudioLogMessageFormatter::name(), "console");
    Severity myVisualStudioSeverity = getSeverityFromEnv(LOG_VISUAL_STUDIO_FILTER_ENV, SEV_DISABLED);
    if (myVisualStudioSeverity != SEV_DISABLED) {
        addMessageSink(Ptr<MessageSink>(new OutputWindowPrinter), myVisualStudioFormatter, myVisualStudioSeverity);
    }
#endif

    Ptr<LogMessageFormatter> myFileFormatter = createFormatter(LOG_FILE_FORMAT_ENV, FullLogMessageFormatter::name(), "console");
    Severity myFileSeverity = getSeverityFromEnv(LOG_FILE_FILTER_ENV, SEV_DISABLED);
    if (myFileSeverity != SEV_DISABLED) {
        const char * myLogFileName = getenv(LOG_FILE_NAME_ENV);
        if (!myLogFileName) {
            myLogFileName = "asl_logfile.txt";
        }
        _myLogFile.open(myLogFileName);
        if (_myLogFile) {
            addMessageSink(Ptr<MessageSink>(new StreamPrinter(_myLogFile)), myFileFormatter, myFileSeverity);
        } else {
            std::cerr << "### WARNING: Logger: could not open formatter '"<< myLogFileName <<"' (may be defined in environment variable "
                 << LOG_FILE_NAME_ENV <<")"<< std::endl;
        }
    }


    setVerbosity(getSeverityFromEnv(LOG_GLOBAL_VERBOSITY_ENV, SEV_WARNING));

    const char * myEnv = getenv(LOG_MODULE_VERBOSITY_ENV);
    if (myEnv && strlen(myEnv) > 0) {
        std::string myLogLevelString(myEnv);
        std::string::size_type myColon;
        while ((myColon = myLogLevelString.find_first_of(":;")) != std::string::npos) {
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
    MAKE_SCOPE_TIMER(Logger_IfLog);
    if (!_myVerbositySettings.empty()) {
        Severity myVerbosity = _myGlobalVerbosity;
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
        return theSeverity <= myVerbosity;
    }
    return theSeverity <= _myGlobalVerbosity;
}

void
Logger::log(Time theTime, Severity theSeverity, const char * theModule, int theId, const std::string & theText) {
    std::string myMessage;
    LAST_ERROR_TYPE myError = lastError(); // do not affect error state by
    if (theSeverity > SEV_PRINT) {
        myMessage = _myFormatter->format(theTime, theSeverity, theModule, theId, theText);
    } else {
        myMessage = theText;
    }
    for (std::vector<Ptr<MessageSink> >::size_type i = 0; i < _myMessageSinks.size(); ++i) {
        if (theSeverity > _mySinkSeverityFilter[i]) {
            continue;
        }
        if (theSeverity > SEV_PRINT) {
            if (_mySinkFormatter[i]) {
                myMessage = _mySinkFormatter[i]->format(theTime, theSeverity, theModule, theId, theText);
            } else {
                myMessage = _myFormatter->format(theTime, theSeverity, theModule, theId, theText);
            }
        } else {
            myMessage = theText;
        }
        _myMessageSinks[i]->push(theSeverity, myMessage);
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
#if defined(_WIN32) && defined(DEBUG_VARIANT)
    // For whatever reason, __FILE__ is lowercase in debug mode...
    std::string myModule = asl::toLowerCase(theModule);
#else
    const std::string & myModule = theModule;
#endif
    // TODO: remove item when there is an exact match
    _myVerbositySettings.insert(std::pair<const std::string, ModuleVerbosity>(
                                file_string(myModule.c_str()), ModuleVerbosity(theVerbosity, theMinId, theMaxId)));
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

    Severity mySeverity = getSeverityFromString(mySubStrings[0], SEV_DEBUG);
    std::string myModule = mySubStrings[1];
    int myMinId = 0, myMaxId = std::numeric_limits<int>::max();
    if (mySubStrings.size() > 2 && mySubStrings[2].size()) {
        myMinId = as_int(mySubStrings[2]);
    }
    if (mySubStrings.size() > 3 && mySubStrings[3].size()) {
        myMaxId = as_int(mySubStrings[3]);
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
    for (std::vector<Ptr<MessageSink> >::size_type i = 0; i < _myMessageSinks.size(); ++i) {
        if (theSink == _myMessageSinks[i]) {
            _mySinkFormatter[i] = theFormatter;
            _mySinkSeverityFilter[i] = theSeverityFilter;
            return;
        }
    }
    throw UnknownSinkExeption(JUST_FILE_LINE);
}

} // namespace
