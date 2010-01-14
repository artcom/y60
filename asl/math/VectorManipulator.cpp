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
//    $RCSfile: VectorManipulator.cpp,v $
//
//    $Author: david $
//
//    $Revision: 1.2 $
//
//=============================================================================

//own header
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
