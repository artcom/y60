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
//    $RCSfile: ThreadLock.h,v $
//
//     $Author: thomas $
//
//   $Revision: 1.7 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef INCL_THREADLOCK
#define INCL_THREADLOCK

#include <pthread.h>

namespace asl {
 /**
  * @ingroup aslbase
  * ThreadLock is a mutual exclusion device.
  * It can be used to protect resources from concurrent access.
  */
class ThreadLock {
public:
    
    ThreadLock();
    
    virtual ~ThreadLock();
    
    void lock();
    
    bool trylock();
    
    bool timedlock(long theMicrosecondTimeout);
    
    void unlock();
    
private:
    pthread_mutex_t _myMutex;
};

}

#endif
