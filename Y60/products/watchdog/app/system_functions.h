//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: system_functions.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2005/04/27 12:42:06 $
//
//
//  Description: The watchdog restarts the application, if it is closed
//               manually or by accident
//
//
//=============================================================================

#ifndef INCL_SYSTEM_FUNCTIONS
#define INCL_SYSTEM_FUNCTIONS

#include <string>
#include <fstream>


void initiateSystemReboot();
void initiateSystemShutdown();
void dumpWinError(const std::string& theErrorLocation);

#endif

