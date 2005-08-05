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
//    $RCSfile: TimeStreamFormater.cpp,v $
//
//     $Author: david $
//
//   $Revision: 1.1 $
//
// Description: Class for simple measurement of time, resolution depends on
//              hardware, maximal 1 micro second
//
//
//=============================================================================

#include "TimeStreamFormater.h"

#include <ios>

namespace asl {

const int TimeStreamFormater::ourIsFormatedFlagIndex( std::ios_base::xalloc() );
const int TimeStreamFormater::ourFormatStringIndex( std::ios_base::xalloc() );

TimeStreamFormater::TimeStreamFormater(const char * theFormatString) :
    _myFormatString(theFormatString)
{}

TimeStreamFormater::TimeStreamFormater(const TimeStreamFormater & otherFormater) :
    _myFormatString(otherFormater._myFormatString)
{}

void
TimeStreamFormater::setStreamInfo(std::ios_base & theStream) const {
    theStream.iword(ourIsFormatedFlagIndex) = true;
    theStream.pword(ourFormatStringIndex) = (void *) _myFormatString ;
}

TimeStreamFormater
formatTime(const char * theFormatString) {
    return TimeStreamFormater(theFormatString);
}

std::ostream &
operator << (std::ostream & theStream, const TimeStreamFormater & theFormater) {
    theFormater.setStreamInfo( theStream );
    return theStream;
}


} // end of namespace asl
