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
//   $RCSfile: LogMessageSinks.h,v $
//   $Author: pavel $
//   $Revision: 1.1 $
//   $Date: 2005/04/29 08:09:22 $
//
//=============================================================================

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

