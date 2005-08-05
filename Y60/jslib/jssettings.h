//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: jssettings.h,v $
//   $Author: janbo $
//   $Revision: 1.1 $
//   $Date: 2004/09/24 16:43:13 $
//
//
//=============================================================================

#ifndef _Y60_JSSETTINGS_INCLUDED_
#define _Y60_JSSETTINGS_INCLUDED_

#undef JS_THREADSAFE

#ifdef WIN32    
    #define XP_WIN    
#endif

#ifdef LINUX
    #define XP_UNIX    
#endif

#endif

