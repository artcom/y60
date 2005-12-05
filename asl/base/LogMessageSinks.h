/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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

#include "Logger.h"

namespace asl {

    class StreamPrinter : public MessageSink {
    public:
        StreamPrinter(std::ostream & theStream) : _myStream(theStream) {}
        void push(const std::string & theMessage) {
            myLock.lock();
            _myStream << theMessage << std::endl;
            myLock.unlock();
        };
    private:
        static ThreadLock myLock;
        std::ostream & _myStream;
    };

#ifdef WIN32
#include "windows.h"
    class OutputWindowPrinter : public MessageSink {
    public:
        OutputWindowPrinter() {}
        void push(const std::string & theMessage) {
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

