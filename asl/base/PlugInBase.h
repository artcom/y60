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
//    $RCSfile: PlugInBase.h,v $
//
//     $Author: david $
//
//   $Revision: 1.9 $
//
// Description:
//
//=============================================================================

#ifndef _PLUGIN_BASE_INCLUDED_
#define _PLUGIN_BASE_INCLUDED_

#include <iostream>

#include "SingletonManager.h"
#include "Ptr.h"


/*! \addtogroup aslbase */
/* @{ */

#ifdef WIN32
	#include "windows.h"
	namespace asl {typedef ::HINSTANCE DLHandle;}
	#define EXPORT __declspec(dllexport)
#else
	#include "dlfcn.h"
	namespace asl {typedef void* DLHandle;}
	#define EXPORT
#endif

namespace asl {

class PlugInBase {
public:
	PlugInBase(DLHandle theDLHandle);
	virtual ~PlugInBase() {}
	DLHandle getDLHandle() const;
protected:
	virtual void _initializeSingletonManager(SingletonManager* theSingletonManager);
	friend class PlugInManager;

	DLHandle _myDLHandle;
};

typedef Ptr<PlugInBase> PlugInBasePtr;

typedef PlugInBase* (*InstantiatePlugInFunctionPtr)(DLHandle theHandle);
/*
extern "C" EXPORT PlugInBase* instantiatePlugIn(DLHandle theHandle);
*/

//std::ostream & operator << (std::ostream & out, const PlugInBase & pi);

}

/* @} */


#endif
