//============================================================================
//
// Copyright (C) 2000-2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
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
