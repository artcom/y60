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
//
//    $RCSfile: TimeStreamFormater.h,v $
//
//     $Author: david $
//
//   $Revision: 1.2 $
//
// Description: Class for simple measurement of time, resolution depends on
//              hardware, maximal 1 micro second
//
//
//=============================================================================

#ifndef ASL_TIME_STREAM_FORMATER_INCLUDED
#define ASL_TIME_STREAM_FORMATER_INCLUDED

#include <iostream>

namespace asl {


/*! \addtogroup aslbase */
/* @{ */
    
class TimeStreamFormater {
    public:
        TimeStreamFormater(const char * theFormatString);
        TimeStreamFormater(const TimeStreamFormater & otherFormater);

        void setStreamInfo(std::ios_base & theStream) const;

        static const int ourIsFormatedFlagIndex;
        static const int ourFormatStringIndex;

    private:
        TimeStreamFormater();
        TimeStreamFormater & operator=( const TimeStreamFormater & otherFormater);

        const char * _myFormatString;
};

TimeStreamFormater
formatTime(const char * theFormatString);

std::ostream &
operator << (std::ostream & theStream, const TimeStreamFormater & theFormater);

}

/* @} */

#endif // ASL_TIME_STREAM_FORMATER_INCLUDED
