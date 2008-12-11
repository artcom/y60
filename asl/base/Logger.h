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
// Description:  Simple time source interface. getCurrentTime() should return 
//               the time in seconds since the TimeSource was constructed.
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : perfect
//    formatting             : ok
//    documentation          : ok
//    test coverage          : fair
//    names                  : ok
//    style guide conformance: ok
//    technical soundness    : ok
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : ok
//
//    recommendations:
//       - make a documentation
//       - make general use of this throughout the whole framework, at least throughout asl
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ac_asl_Logger_h_included_
#define _ac_asl_Logger_h_included_

#include "Time.h"
#include "Ptr.h"
#include "Singleton.h"
#include "ThreadLock.h"
#include "Exception.h"

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <limits>


namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */

    enum Severity {SEV_PRINT, SEV_FATAL, SEV_ERROR, SEV_WARNING, SEV_INFO, SEV_DEBUG, SEV_TRACE, SEV_DISABLED};
    extern const char * SeverityName[];

    class MessageSink {
    public:
        virtual void push(Severity theSeverity, const std::string & theMessage) = 0;
    };
    DEFINE_EXCEPTION(UnknownSinkExeption, Exception);

    /**
    Interface for a function that print the supplied log message parameter into
    */
    class LogMessageFormatter {
    public:
        virtual std::string format( asl::Time           theTime,
                                    Severity            theSeverity,
                                    const std::string & theModule,
                                    int                 theId,
                                    const std::string & theText) = 0;
    };

 
// some assholes at Microsoft define max as macro, so if we want
// to use std::numeric_traits::max() we have to get rid of it.
#ifdef max
	#ifndef _SETTING_NO_UNDEF_WARNING_ 
		#warning Symbol 'max' defined as macro, undefining. (Outrageous namespace pollution by Microsoft, revealing arrogance and incompetence)
	#endif
#undef max
#endif

    /**
     * Reads the environment variables 'AC_LOG_VERBOSITY' and 'AC_MODULE_VERBOSITY'
     * to initialize the global logging verbosity and the per-module verbosities.
     *
     * Per-module verbosity has the following format:
     * AC_MODULE_VERBOSITY := MODULE_VERBOSITY | AC_MODULE_VERBOSITY:MODULE_VERBOSITY
     * MODULE_VERBOSITY := <LEVEL>/<MODULE>[/<FROMLINE>[/<TOLINE>]]
     */
    class Logger : public Singleton<Logger> {
        struct ModuleVerbosity {
            ModuleVerbosity() {}
            ModuleVerbosity(Severity theVerbosity,int theMinId, int myMaxId)
                : myVerbosity(theVerbosity), myMinId(theMinId), myMaxId(myMaxId)
            {}

            Severity myVerbosity;
            int myMinId;
            int myMaxId;
        };
    public:
        /**
        used to store external verbosity entries
        */
        struct ModuleVerbosityEntry : public ModuleVerbosity {
            ModuleVerbosityEntry() {}
            ModuleVerbosityEntry(const std::string & theModule, Severity theVerbosity,int theMinId, int myMaxId)
                : myModule(theModule), ModuleVerbosity(theVerbosity,theMinId,myMaxId)
            {}
            std::string myModule;
        };
        /** Default constructor sets the Verbosity to SEV_WARNING level and a StreamSink(std::cerr) as output channel
            and SimpleLogMessageFormatter as formatter.
        */
        Logger();

        /**
        Used to detect if a message should be logged depending on its severity and the logger severity settings.
        returns true if theSeverity is higher (numerically smaller) than the verbosity setting
        a different verbosity can be defined for any id range in any module; if there are different
        verbosity settings for an overlapping id region in the same module, the setting for the
        smallest id-range takes precedence.
        */
        bool IfLog(Severity theSeverity, const char * theModule, int theId);

        /**
        formats a message and pushes it to all installed sinks.
        */
        void log(asl::Time theTime, Severity theSeverity, const char * theModule, int theId, const std::string & theText);

        /**
        returns all verbosity setting entries to allow inspection or save it in a config file
        */
        void getModuleVerbosity(int i, std::vector<ModuleVerbosityEntry> & theVerbositySetting);

        /**
        sets the global verbosity that is applied if no special verbosity is defined for a particular module
        */
        void setVerbosity(Severity theVerbosity);

        /**
         * @return the global verbosity.
         */
        Severity getVerbosity() const;

        /**
        sets the verbosity for a particular module; an id range can be supplied;
        the default settings will be that all module ids (line numbers) in a range will be selected.
        */
        void setModuleVerbosity(Severity theVerbosity,
            const std::string & theModule,
            int theMidId = 0,
            int theMaxId = std::numeric_limits<int>::max() );

        /** sets the verbosity for a particular module; an id range can be supplied;
            the default settings will be that all module ids (line numbers) in a range will be selected. */
        void setModuleVerbosity(const std::string & theVerbosity);

        /** adds a new Message sink where all log message will be copied to;
            if theFormatter is 0 the global formatter will be used */
        void addMessageSink(Ptr<MessageSink> theSink, Ptr<LogMessageFormatter> theFormatter = Ptr<LogMessageFormatter>(0), Severity theSeverityFilter=SEV_TRACE);

        /** sets the global Message Formatter; the global formatter will be called just once and the result copied to all sinks
        that have no private formatter define */
        void setMessageFormatter(Ptr<LogMessageFormatter> theFormatter);
        
        /** sets a new Message private message Formatter for a sink; if the sink is undefined, an exception is thrown*/
        void setMessageFormatter(Ptr<MessageSink> theSink, Ptr<LogMessageFormatter> theFormatter, Severity theSeverityFilter=SEV_TRACE);
    private:
        std::vector<Ptr<MessageSink> > _myMessageSinks;
        Ptr<LogMessageFormatter> _myFormatter;
        std::vector<Ptr<LogMessageFormatter> > _mySinkFormatter;
        std::vector<asl::Severity> _mySinkSeverityFilter;
        Severity _myGlobalVerbosity;
        std::multimap<std::string,ModuleVerbosity> _myVerbositySettings;
        std::ofstream _myLogFile;
    };

    /**
    This class is used to collect the output and deliver it to the Logger on destruction
    */
    class MessagePort {
    public:
        MessagePort(Severity theSeverity, const char * theModule, int theId)
            : mySeverity(theSeverity), myModule(theModule), myId(theId)
        {}
        ~MessagePort() {
            Logger::get().log(myTime, mySeverity, myModule, myId, stream.str());
        }
        /* This getter is used in the gcc branch to avoid printing of the first message token
         * as a hex address. I still do not understand what exactly goes wrong when accessing
         * the member directly. However, this solves bug #24.
         */
        inline std::ostringstream & getStream() {
            return stream;
        }
        std::ostringstream stream;
        const Time myTime;
        const Severity mySeverity;
        const char * myModule;
        const int myId;
    };
}

#define MAKEUNIQUE(x) myVar_##x
#ifdef WIN32
#   define AC_LOG_CHECK(SEVERITY,MODULE,MSGID) asl::Logger::get().IfLog(SEVERITY,MODULE,MSGID) && const_cast<std::ostream&>( static_cast<const std::ostream&>(asl::MessagePort(SEVERITY,MODULE,MSGID).stream) )
#   if defined(DEBUG)
#       define AC_LOG(SEVERITY,MODULE,MSGID) asl::Logger::get().IfLog(SEVERITY,MODULE,MSGID) && const_cast<std::ostream&>( static_cast<const std::ostream&>(asl::MessagePort(SEVERITY,MODULE,MSGID).stream) )
#   else
#       define AC_LOG(SEVERITY,MODULE,MSGID) static bool MAKEUNIQUE(MSGID) = asl::Logger::get().IfLog(SEVERITY,MODULE,MSGID) ; MAKEUNIQUE(MSGID) && const_cast<std::ostream&>( static_cast<const std::ostream&>(asl::MessagePort(SEVERITY,MODULE,MSGID).stream) )
#   endif
#else
#   define AC_LOG_CHECK(SEVERITY,MODULE,MSGID) asl::Logger::get().IfLog(SEVERITY,MODULE,MSGID) && (asl::MessagePort(SEVERITY,MODULE,MSGID).getStream())
#   define AC_LOG(SEVERITY,MODULE,MSGID) static bool MAKEUNIQUE(MSGID) = asl::Logger::get().IfLog(SEVERITY,MODULE,MSGID) ; MAKEUNIQUE(MSGID) && (asl::MessagePort(SEVERITY,MODULE,MSGID).getStream())
#endif

// Although it might look obscure, this macro should generate no code at all and remove the logging
// call expression which requires an ostream &; we use it to remove all trace logging when compiling
// in non-debug mode
// TODO: make this configurable on the build command line 
#define AC_DONT_LOG 0 && std::cerr

/**
Use AC_PRINT << "myMessage" in situations when a message should be printed and logged anyway, possibly without formatting */
#define AC_PRINT AC_LOG_CHECK(asl::SEV_PRINT, __FILE__, __LINE__)
/**
Use AC_FATAL << "myErrorMessage" in situations when a severe condition (e.g. memory corruption) was
detected and you are going to abort program execution. */
#define AC_FATAL AC_LOG(asl::SEV_FATAL, __FILE__, __LINE__)
#define AC_FATAL_CHECK AC_LOG_CHECK(asl::SEV_FATAL, __FILE__, __LINE__)
/**
Use AC_ERROR << "myErrorMessage" in situations when an error occured that 
is likely to have significant impact on further computation and
might the reason for undesired or unexpected behavior of the application*/
#define AC_ERROR AC_LOG(asl::SEV_ERROR, __FILE__, __LINE__)
#define AC_ERROR_CHECK AC_LOG(asl::SEV_ERROR, __FILE__, __LINE__)
/**
Use AC_WARNING << "myWarningMessage" in situations when an error occured that will be
compensated but might degrade user experience (e.g. performance, lack of features) */
#define AC_WARNING AC_LOG(asl::SEV_WARNING, __FILE__, __LINE__)
#define AC_WARNING_CHECK AC_LOG_CHECK(asl::SEV_WARNING, __FILE__, __LINE__)
/**
Use AC_INFO << "myInfoMessage" in situations to inform about resource usage,
performance, or completion of important tasks or important events */
#define AC_INFO AC_LOG(asl::SEV_INFO, __FILE__, __LINE__)
#define AC_INFO_CHECK AC_LOG_CHECK(asl::SEV_INFO, __FILE__, __LINE__)
/**
Use AC_DEBUG << "myDebugMessage" to inform about creation or destruction of important objects,
overall flow of control and general input/output events; keep in mind that debug message should
still permit the application to perform its work; use them sparsely*/
#define AC_DEBUG AC_LOG(asl::SEV_DEBUG, __FILE__, __LINE__)
#define AC_DEBUG_CHECK AC_LOG_CHECK(asl::SEV_DEBUG, __FILE__, __LINE__)
/**
Use AC_TRACE << "myDebugMessage" to trace flow of control and produce an amount of information
so detailed that the program might be too slow to perform anything reasonable; in general
AC_TRACE Messages may be within an DB()-Macro and will not be compiled in; you will have
to enable them on a per-file basis*/
#ifdef _SETTING_WITH_TRACE_LOG_
#define AC_TRACE AC_LOG(asl::SEV_TRACE, __FILE__, __LINE__)
#define AC_TRACE_CHECK AC_LOG_CHECK(asl::SEV_TRACE, __FILE__, __LINE__)
#else
#define AC_TRACE AC_DONT_LOG
#endif

/**
Use AC_SPAM << "myDebugMessage" if you think that a message spoils your debugging experience, but
you do not want to delete it in case it might be handy some day. On per-file basis it may be 
more desirable to use the DB() macro to provide a comple-time switch; AC_SPAM is just useful for
really annoying mesaages.*/
#define AC_SPAM AC_DONT_LOG

#define AC_TRACE_EXPRESSION(myExpression) AC_TRACE << #myExpression << "=" << (myExpression) << std::endl;

/** Use if (AC_TRACE_ENABLED) { ... } to conditionally exectute code when AC_TRACE logging is enabled */
#define AC_TRACE_ON (asl::Logger::get().IfLog(asl::SEV_TRACE,__FILE__,__LINE__))

/** Use if (AC_DEBUG_ON) { ... } to conditionally exectute code when AC_DEBUG logging is enabled */
#define AC_DEBUG_ON (asl::Logger::get().IfLog(asl::SEV_DEBUG,__FILE__,__LINE__))

/** Use if (AC_INFO_ON) { ... } to conditionally exectute code when AC_INFO logging is enabled */
#define AC_INFO_ON (asl::Logger::get().IfLog(asl::SEV_INFO,__FILE__,__LINE__))


    /* @} */
#endif

