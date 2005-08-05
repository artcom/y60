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
//   $RCSfile: LogMessageFormatters.h,v $
//   $Author: pavel $
//   $Revision: 1.1 $
//   $Date: 2005/04/29 08:09:22 $
//
//=============================================================================

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


