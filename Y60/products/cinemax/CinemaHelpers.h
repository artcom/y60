//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: CinemaHelpers.h,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2004/08/19 10:52:56 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_c4d_CinemaHelpers_h_
#define _ac_c4d_CinemaHelpers_h_

#include <string>

#include "c4d_include.h" 

void displayMessage(const std::string & theMessage);
void displayMessage(const String & theMessage);
void displayMessage(const char * theMessage);

const std::string & getString(const String & theC4dString);

void displayChannelProperties(BaseChannel * theColorContainer);

#endif


