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
//    $RCSfile: Singleton.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.5 $
//
// Description:
//
//=============================================================================

#ifndef _SINGLETON_INCLUDED_
#define _SINGLETON_INCLUDED_

#include "SingletonManager.h"

namespace asl {


/*! \addtogroup aslbase */
/* @{ */

template<class T>
class Singleton : public SingletonBase {
public:
	static T& get() {
		// A Singleton caches the instance returned by SingletonManager
		// in a local static variable.
		static T& myInstance(SingletonManager::get().SingletonManager::getSingletonInstance<T>());
		return myInstance;
	}
};

/* @} */


}

#endif
