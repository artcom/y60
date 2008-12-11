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
// Description:  A small collection of log formatters
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
//    technical soundness    : fair (should be a module)
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : fair
//
//    overall review status  : fair
//
//    recommendations:
//       - make more documentation
//       - make some more tests
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ac_asl_LogMessageFormatters_h_
#define _ac_asl_LogMessageFormatters_h_

#include "Logger.h"

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */

    class FullLogMessageFormatter : public LogMessageFormatter {
    public:
        static const char * name() { return "FULL"; }
        std::string format(asl::Time           theTime,
                           Severity            theSeverity,
                           const std::string & theModule,
                           int                 theId,
                           const std::string & theText);
    };

    class TerseLogMessageFormatter : public LogMessageFormatter {
    public:
        static const char * name() { return "TERSE"; }
        std::string format(asl::Time           theTime,
                           Severity            theSeverity,
                           const std::string & theModule,
                           int                 theId,
                           const std::string & theText);
    };

    class VisualStudioLogMessageFormatter : public LogMessageFormatter {
    public:
        static const char * name() { return "VISUAL_STUDIO"; }
        std::string format(asl::Time           theTime,
                           Severity            theSeverity,
                           const std::string & theModule,
                           int                 theId,
                           const std::string & theText);
    };
}

    /* @} */
#endif


