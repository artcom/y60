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
// Description:  A small collection of log message sinks
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : ok
//    formatting             : ok
//    documentation          : poor
//    test coverage          : poor
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
//    overall review status  : fair
//
//    recommendations:
//       - make more documentation
//       - make some more tests
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _ac_asl_LogMessageSinks_h_
#define _ac_asl_LogMessageSinks_h_

#include "asl_base_settings.h"

#include <cstdlib>

#include "Logger.h"
#include "console_functions.h"

namespace asl {

    class FarewellMessageSink : public MessageSink {
    public:
        FarewellMessageSink() {}
        void push(Severity theSeverity, const std::string & theMessage) {
            if (theSeverity == SEV_FATAL) {
                abort();
            }
        };
    private:
    };

    class StreamPrinter : public MessageSink {
    public:
        StreamPrinter(std::ostream & theStream) : _myStream(theStream) {}
        void push(Severity theSeverity, const std::string & theMessage) {
            myLock.lock();
           
            switch (theSeverity) {
                case SEV_FATAL :
                case SEV_ERROR :
                    _myStream << TTYRED;
                    break;
                case SEV_WARNING:
                    _myStream << TTYYELLOW;
                    break;
                default:
                    // normal black, do nothing
                    break;
            }
            _myStream << theMessage << ENDCOLOR << std::endl;
            myLock.unlock();
        };
    private:
        static ThreadLock myLock;
        std::ostream & _myStream;
    };

    class ASL_BASE_EXPORT FilePrinter : public MessageSink {
    public:
        FilePrinter(const std::string & theFilename) : _myStream(theFilename.c_str()) {}
        void push(Severity theSeverity, const std::string & theMessage) {
            myLock.lock();
            _myStream << theMessage << std::endl;
            myLock.unlock();
        };
    private:
        static ThreadLock myLock;
        std::ofstream _myStream;
    };

#ifdef _WIN32
#include "windows.h"
    class OutputWindowPrinter : public MessageSink {
    public:
        OutputWindowPrinter() {}
        void push(Severity theSeverity, const std::string & theMessage) {
            myLock.lock();
            OutputDebugString((theMessage + "\n").c_str());
            myLock.unlock();
        }
    private:
        static ThreadLock myLock;
    };
#endif
}

    /* @} */
#endif

