/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description:  basic plugin interface
//
// Last Review: pavel 8.12.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : ok
//    formatting             : ok
//    documentation          : poor
//    test coverage          : poor
//    names                  : ok
//    style guide conformance: ok
//    technical soundness    : ok
//    dead code              : poor
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : fair
//
//    recommendations:
//       - write documentation
//       - add some tests
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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

typedef PlugInBase * (*InstantiatePlugInFunctionPtr)(DLHandle theHandle);
/*
extern "C" EXPORT PlugInBase* instantiatePlugIn(DLHandle theHandle);
*/

//std::ostream & operator << (std::ostream & out, const PlugInBase & pi);

}

/* @} */


#endif
