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
//    $RCSfile: testThreadSemFactory.tst.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1.1.1 $//
// Description: 
//
// (CVS log at the bottom of this file)
//
//
//=============================================================================

#include "UnitTest.h"

#include "ThreadSemFactory.h"
#include "ThreadSem.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <signal.h> 
#include <pthread.h> 


using namespace std;  // automatically added!



using namespace asl;


const int NumLocks = 10;


void ThreadFunction() {
    for ( int i = 0; i < NumLocks; i++ ) {
        ThreadSem * myLock = ThreadSemFactory::instance()->newLock();
        usleep( 10 );
    }
    return;
}



class LockFactoryTest : public UnitTest {
    public:
        LockFactoryTest() : UnitTest("ThreadSemFactory") { }
        void run() {

            
            ENSURE( Recycling() )
            //ENSURE( MultiProcessIPC() )
            ENSURE( MultiThreadIPC() )
            ENSURE( SignalHandlerInstalled() )
        }


    private:
        bool Recycling() {
            ThreadSemFactory::instance();
            
            // get a lock
            ThreadSem * myLock = ThreadSemFactory::instance()->newLock();
            int oldLockSetId    = myLock->getSemaphoreSetId();
            int oldLockSetIndex = myLock->getSemaphoreSetIndex();
            // free it again
            ThreadSemFactory::instance()->freeLock( myLock );
            // ... and check if we get the same lock back if we allocate another one..
            myLock = ThreadSemFactory::instance()->newLock();
            if( ( myLock->getSemaphoreSetId()    == oldLockSetId ) &&
                ( myLock->getSemaphoreSetIndex() == oldLockSetIndex ) ) {
                ThreadSemFactory::instance()->freeLock( myLock );
                return true;
            } else {
                ThreadSemFactory::instance()->freeLock( myLock );
                return false;
            }
            
            delete ThreadSemFactory::instance(); 

        }

/* not implemented!
        bool MultiProcessIPC() {

            ThreadSemFactory::instance();
            
            pid_t ChildPID = fork();
            if ( ChildPID == -1 ) {
                return false;
            } else if ( ChildPID != 0 ) {
                // This is the parent process

                wait( 0 );
                if ( ThreadSemFactory::instance()->get_totalLocks() == NumLocks ) {
                    delete ThreadSemFactory::instance(); 
                    return true;
                } else {
                    delete ThreadSemFactory::instance(); 
                    return false;
                }
                
                
            } else { 
                // This is the child
                for ( int i = 0; i < NumLocks; i++ ) {
                    ThreadSem * myLock = ThreadSemFactory::instance()->newLock();
                    usleep( 10 );
                }
                exit( 0 );
            } 

        }
*/

        bool MultiThreadIPC() {

            pthread_t ThreadId = 0;
            if ( pthread_create( & ThreadId, 0, (void * (*)(void *))(ThreadFunction), 0) == 0 ) {
                pthread_join( ThreadId, 0 );
                if ( ThreadSemFactory::instance()->get_totalLocks() == NumLocks ) {
                    delete ThreadSemFactory::instance(); 
                    return true;
                } else {
                    delete ThreadSemFactory::instance(); 
                    return false;
                }
            } else {
                return false;
            }
        }

        bool SignalHandlerInstalled() {
            ThreadSemFactory::instance();
            ENSURE(ThreadSemFactory::hasInstance());
            ThreadSemFactory::instance()->debug_setExitOnSignal(false);
            raise(SIGINT);
            return (ThreadSemFactory::hasInstance() == false);
        }
};


class MyTestSuite : public UnitTestSuite {
    public:
        MyTestSuite( const char * myName ) : UnitTestSuite( myName ) {}
        void setup() {
            UnitTestSuite::setup(); // print the launch Message
            addTest( new LockFactoryTest );
        }
};

int
main( int argc, char * argv[] ) {

    MyTestSuite mySuite( argv[0] );

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;


    return mySuite.returnStatus();

}



//==============================================================================
//
// $Log: testThreadSemFactory.tst.cpp,v $
// Revision 1.1.1.1  2003/05/12 14:20:23  uzadow
// no message
//
// Revision 1.1.1.1  2002/09/17 15:37:04  wolfger
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
// Revision 1.1.2.5  2002/08/30 16:25:29  martin
// more std-ization
//
// Revision 1.1.2.4  2002/04/24 09:22:50  stefan
// fixed a missing include
//
// Revision 1.1.2.3  2002/04/22 14:52:31  valentin
// /tmp/cvsKMV3gi
//
// Revision 1.1.2.2  2002/02/25 12:55:05  stefan
// - added signal handling to semaphore factory. It should clean up behind
//   itself now. ATTENTION: Not saving any previous signal handling status,
//   handling SIGINT and SIGTERM
//
// - reenabled the testThreadSem* tests again, since they failed because of
//   a lack of system semaphores, which probably were all used up by
//   interrupted applications that did not delete the ThreadSemFactory
//   instance. I hope that improves now.
//
//   IF YOU USE THREADS AND THREADSEMAPHORES, OR DERIVED CLASSES LIKE
//   THREADFIFOs, DIRECTLY OR INDIRECTLY, YOU HAVE TO DELETE THE FACTORY
//   INSTANCE BEFORE EXITTING! ALLWAYS.
//
//   if you are unable to allocate new semaphores, and the command 'ipcs'
//   shows a lot of semaphores, use
//       '/usr/share/Performer/bin/rmsem'
//   to free them
//
// Revision 1.1.2.1  2002/02/07 09:28:03  stefan
// - semaphores for threads. unlike the MpLock/MpLockFactory classes these
//   sempahore classes do not rely upon Performer, and are not suitable
//   for multi-processing
//
//
//==============================================================================

