//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: VectorManipulator.cpp,v $
//
//    $Author: david $
//
//    $Revision: 1.2 $
//
//=============================================================================

#include "VectorManipulator.h"

namespace asl {

#if 1
const int FixedVectorStreamFormatter::ourIsFormattedFlag( std::ios_base::xalloc());
const int FixedVectorStreamFormatter::ourStartTokenIndex( std::ios_base::xalloc());
const int FixedVectorStreamFormatter::ourEndTokenIndex( std::ios_base::xalloc());
const int FixedVectorStreamFormatter::ourDelimiterIndex( std::ios_base::xalloc());
const int FixedVectorStreamFormatter::ourOneElementPerLineFlagIndex( std::ios_base::xalloc());

#endif


FixedVectorStreamFormatter::FixedVectorStreamFormatter(const char theStartToken, 
                                                       const char theEndToken, 
                                                       const char theDelimiter, 
                                                       bool theOneElementPerLineFlag) :
    _myStartToken(theStartToken),
    _myEndToken(theEndToken),
    _myDelimiter(theDelimiter),
    _myOneElementPerLineFlag(theOneElementPerLineFlag) 
{
}

FixedVectorStreamFormatter::FixedVectorStreamFormatter( const FixedVectorStreamFormatter & otherFormatter) :
    _myStartToken(otherFormatter._myStartToken),
    _myEndToken(otherFormatter._myEndToken),
    _myDelimiter(otherFormatter._myDelimiter),
    _myOneElementPerLineFlag(otherFormatter._myOneElementPerLineFlag) 
{
};

void
FixedVectorStreamFormatter::setStreamInfo(std::ios_base & theStream) const {
    theStream.iword(ourStartTokenIndex)            = static_cast<char>(_myStartToken);
    theStream.iword(ourEndTokenIndex)              = static_cast<char>(_myEndToken);
    theStream.iword(ourDelimiterIndex)             = static_cast<char>(_myDelimiter);
    theStream.iword(ourOneElementPerLineFlagIndex) = _myOneElementPerLineFlag;
    theStream.iword(ourIsFormattedFlag)            = true;
}

FixedVectorStreamFormatter
formatFixedVector(const char theStartToken, 
                  const char theEndToken, 
                  const char theDelimiter, 
                  bool theOneElementPerLineFlag)
{
    return FixedVectorStreamFormatter(theStartToken, theEndToken, theDelimiter, 
                                      theOneElementPerLineFlag);
}

std::ostream &
operator << (std::ostream & theStream, const FixedVectorStreamFormatter & theFormatter) {
    theFormatter.setStreamInfo(theStream);
    return theStream;
}

std::istream &
operator >> (std::istream & theStream, const FixedVectorStreamFormatter & theFormatter) {
    theFormatter.setStreamInfo(theStream);
    return theStream;
}

}
