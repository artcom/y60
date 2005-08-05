//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: testThreadSemFactoryPerformance.tst.cpp,v $
//
//     $Author: david $
//
//   $Revision: 1.2 $
//
// Description: 
//
// (CVS log at the bottom of this file)
//
//
//=============================================================================

#include "ThreadSemFactory.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sys/time.h>


using namespace std;  // automatically added!



using namespace asl;

const int NumLocks = 1000;



double
getTime()
{
    struct timeval tt;
    gettimeofday(&tt, 0);
    return (tt.tv_sec + 1.0e-6 * tt.tv_usec);
} 


bool
singleProcessAllocation() {
    vector<ThreadSem*> LockList;
    
    double StartTime = getTime();
    for ( int i = 0; i < 2*NumLocks; i++ ) { 
        LockList.push_back( ThreadSemFactory::instance()->newLock() );
    }
    double AllocTime = getTime() - StartTime;
    fprintf( stderr, "One process allocated %d semaphores in %lf seconds\n", 
             2 * NumLocks, AllocTime);
    
    StartTime = getTime();
    while ( ! LockList.empty() ) {
        ThreadSemFactory::instance()->freeLock( LockList.back() );
        LockList.pop_back();
    }
    double DeallocTime = getTime() - StartTime;
    fprintf( stderr, "One process deallocated %d semaphores in %lf seconds\n", 
             2 * NumLocks, DeallocTime);

    return true;
}

bool
concurentAllocation() {

    ThreadSem * ParentDoneSemaphore = ThreadSemFactory::instance()->newLock(0);
    ThreadSem * ChildDoneSemaphore  = ThreadSemFactory::instance()->newLock(0);

    pid_t ChildPID = fork();
    if ( ChildPID == -1 ) {
        return false;
    } else if ( ChildPID != 0 ) {
        // This is the parent process
        vector<ThreadSem*> ParentLockList;
        double StartTime = getTime();
        
        for ( int i = 0; i < NumLocks; i++ ) {
            ParentLockList.push_back( ThreadSemFactory::instance()->newLock() );
        }
        
        ParentDoneSemaphore->unlock();
        ChildDoneSemaphore->lock();
        
        double elapsedTime = getTime() - StartTime;
        fprintf( stderr, "Two processes allocated %d semaphores in %lf seconds\n",
                 2 * NumLocks, elapsedTime);

        StartTime = getTime();
        
        while ( ! ParentLockList.empty() ) {
            ThreadSemFactory::instance()->freeLock( ParentLockList.back() );
            ParentLockList.pop_back();
        }

        waitpid(ChildPID, 0, 0);
        
        elapsedTime = getTime() - StartTime;
        fprintf( stderr, "Two processes deallocated %d semaphores in %lf seconds\n",
                 2 * NumLocks, elapsedTime);


    } else { 
        // This is the child
        vector<ThreadSem*> ChildLockList;

        for ( int i = 0; i < NumLocks; i++ ) {
            ChildLockList.push_back( ThreadSemFactory::instance()->newLock() );
        }
        
        ChildDoneSemaphore->unlock();
        ParentDoneSemaphore->lock();

        while ( ! ChildLockList.empty() ) {
            ThreadSemFactory::instance()->freeLock( ChildLockList.back() );
            ChildLockList.pop_back();
        }
        exit(0);
    } 

    return true;
}


int
main( int argc, char * argv[] ) {

    ThreadSemFactory::instance();

    singleProcessAllocation();

    // if this returns false it was the child process, so we do 
    // nothing until the parent returns
    if (concurentAllocation()) {;
        delete ThreadSemFactory::instance();
    }
}


//=============================================================================
//
// $Log: testThreadSemFactoryPerformance.tst.cpp,v $
// Revision 1.2  2003/06/30 15:07:16  david
//  - switched from wait() to waitpid() 'cause this seems to avoid some
//    zombie problems with our test framework
//
// Revision 1.1.1.1  2003/05/12 14:20:23  uzadow
// no message
//
// Revision 1.2  2002/10/30 17:20:17  uzadow
// Linux compatibility changes
//
// Revision 1.1.1.1  2002/09/17 15:37:05  wolfger
// initial checkin
//
// Revision 1.2  2002/09/06 18:13:32  valentin
// merged linuxport_2nd_try branch into trunk
// -branch tag before merge : branch_before_merge_final
// -trunk tag before merge  : trunk_before_merge
//
//
// -Merge conflicts:
//  image/glutShowImage/Makefile
//  image/glutShowImage/glShowImage.C
//  loader/libPfIv/pfiv.c++
//  lso/script/vrfm/Intercom.h
//  tools/Pfconvert/pfconvert.c
//  vrap/libMover/HMD.c++
//
//  - compiled && testrun
//  - commit
//  - merged trunk tag : trunk_after_merge
//
// Revision 1.1.2.3  2002/08/30 16:25:29  martin
// more std-ization
//
// Revision 1.1.2.2  2002/02/14 13:56:27  stefan
// - this test fails during continous build, probably due to allocating
//   more semaphores than available
//
// Revision 1.1.2.1  2002/02/07 09:28:03  stefan
// - semaphores for threads. unlike the MpLock/MpLockFactory classes these
//   sempahore classes do not rely upon Performer, and are not suitable
//   for multi-processing
//
//
//=============================================================================

