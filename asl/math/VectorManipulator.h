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
//    $RCSfile: VectorManipulator.h,v $
//
//    $Author: david $
//
//    $Revision: 1.2 $
//
//=============================================================================


#ifndef ASL_VECTOR_MANIPULATOR_INCLUDED
#define ASL_VECTOR_MANIPULATOR_INCLUDED

#include <iostream>
#include <ios>

namespace asl {

class FixedVectorStreamFormatter {
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
