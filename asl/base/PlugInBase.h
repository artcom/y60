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

#include "asl_base_settings.h"

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

class ASL_BASE_EXPORT PlugInBase {
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
