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
//    $RCSfile: ThreadHelper.h,v $
//
//     $Author: david $
//
//   $Revision: 1.2 $
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_THREADHELPER
#define INCL_THREADHELPER

#include <pthread.h>

#include <string>


/*! \addtogroup aslbase */
/* @{ */

bool
threadPriorityFromStrings (const std::string & thePriorityClassStr, const std::string & thePriorityStr,
        int & thePriorityClass, int & thePriority);

/* @} */

#endif 
