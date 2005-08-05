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
//    $RCSfile: PlugInBase.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.3 $
//
// Description:
//
//=============================================================================

#include <iostream>

#include "../base/PlugInBase.h"
#include "../base/SingletonManager.h"
#include "../base/Logger.h"

using namespace std;
using namespace asl;

#define DB(x) // x

/*
#ifdef LINUX
#else
__declspec (dllexport)
BOOL WINAPI DllMain (HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved) {
	static PlugInBase* thePlugInInstance = 0;
	if (ul_reason_for_call==DLL_PROCESS_ATTACH) {
	} else if (ul_reason_for_call==DLL_PROCESS_DETACH) {
		delete thePlugInInstance;
		thePlugInInstance = 0;
	}
	return TRUE;
}
#endif
*/

PlugInBase :: PlugInBase(DLHandle theDLHandle) :
    _myDLHandle(theDLHandle)
{}

void PlugInBase :: _initializeSingletonManager(SingletonManager* theSingletonManager) {
	// make sure our local SingletonManager delegates its calls
	// the the master SingletonManager of our process.
	AC_DEBUG << "SingletonManager in _initializeSingletonManager" << &SingletonManager::get() << std::endl;
	SingletonManager::get().setDelegate(theSingletonManager);
}

DLHandle PlugInBase :: getDLHandle() const {
	return _myDLHandle;
}


