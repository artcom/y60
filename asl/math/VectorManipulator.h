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
//    $RCSfile: VectorManipulator.h,v $
//
//    $Author: david $
//
//    $Revision: 1.2 $
//
//=============================================================================


#ifndef ASL_VECTOR_MANIPULATOR_INCLUDED
#define ASL_VECTOR_MANIPULATOR_INCLUDED

#include "asl_math_settings.h"

#include <iostream>
#include <ios>

namespace asl {

class ASL_MATH_EXPORT FixedVectorStreamFormatter {
    public:
        FixedVectorStreamFormatter(const char theStartToken, 
                                   const char theEndToken, 
                                   const char theDelimiter, 
                                   bool theOneElementPerLineFlag);
        FixedVectorStreamFormatter( const FixedVectorStreamFormatter & otherFormatter);

        void setStreamInfo(std::ios_base & theStream) const;


        static const int ourIsFormattedFlag;
        static const int ourStartTokenIndex;
        static const int ourEndTokenIndex;
        static const int ourDelimiterIndex;
        static const int ourOneElementPerLineFlagIndex;


        
    private:
        FixedVectorStreamFormatter();
        FixedVectorStreamFormatter & operator=(const FixedVectorStreamFormatter & otherFormatter);

        const char _myStartToken;
        const char _myEndToken;
        const char _myDelimiter;
        bool       _myOneElementPerLineFlag; 
};

FixedVectorStreamFormatter
formatFixedVector(const char theStartToken, 
                  const char theEndToken, 
                  const char theDelimiter, 
                  bool theOneElementPerLineFlag);

std::ostream &
operator << (std::ostream & theStream, const FixedVectorStreamFormatter & theFormatter);

std::istream &
operator >> (std::istream & theStream, const FixedVectorStreamFormatter & theFormatter);

}

#endif // ASL_VECTOR_MANIPULATOR_INCLUDED
