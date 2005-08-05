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
//    $RCSfile: ITimeSource.h,v $
//
//     $Author: david $
//
//   $Revision: 1.2 $
//
// Description: Simple time source interface. getCurrentTime() should return 
//              the time in seconds since the TimeSource was constructed. 
//
//
//=============================================================================

#ifndef ASL_I_TIME_SOURCE_INCLUDED
#define ASL_I_TIME_SOURCE_INCLUDED

namespace asl {

/*! \addtogroup aslbase */
/* @{ */
    
class ITimeSource {
    public:
        virtual double getCurrentTime() = 0;
};


/* @} */

}
#endif // ASL_I_TIME_SOURCE_INCLUDED
