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
//    $RCSfile: SingletonBase.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.3 $
//
// Description:
//
//=============================================================================

#ifndef _SINGLETON_BASE_INCLUDED_
#define _SINGLETON_BASE_INCLUDED_

namespace asl {


/*! \addtogroup aslbase */
/* @{ */

class SingletonBase {
public:
    // The Singleton Manager will call this method on all singletons before it starts deleting them.
    // This way singletons can correctly shut down, independent of the order of destruction.
    virtual void stop() {};

	virtual ~SingletonBase() {}
};

/* @} */


}

#endif
