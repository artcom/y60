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
//    $RCSfile: ThreadSemFactory.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1.1.1 $
//
// Description: 
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "ThreadSemFactory.h"
#include "ThreadSem.h"

#include <iostream>
#include <fstream>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <signal.h>
#include <unistd.h>


using namespace std;  // automatically added!



using namespace asl;

const int SemaphoreSetFlags = IPC_CREAT | 0600; 
const int    default_maxSemaphoreSetSize = 250;
const string ProcfsFile = "/proc/sys/kernel/sem";

ThreadSemFactory * ThreadSemFactory::_instance_ = 0;


ThreadSemFactory::ThreadSemFactory(bool doInstallSignalHandler):
    _debug ( false ),
    _curSemaphoreSetId( -1 ),
    _curSemaphoreSetIndex( 0 ),
    _exitOnSignal( true )
{
    _instance_ = this;


    if( (_myMemoryProtectionId = semget( IPC_PRIVATE, 1, SemaphoreSetFlags )) == -1 ) {
        cerr << "ThreadSemFactory::ThreadSemFactory(): " 
             << "Failed to create memory protection semaphore: "
             << strerror( errno ) << endl;
        exit( 1 );
    }
    
    ThreadSem::Semun semarg;
    semarg.val = 1;
    if ( semctl( _myMemoryProtectionId, 0, SETVAL, 1 ) == -1 ) {
        cerr << "ThreadSemFactory::ThreadSemFactory(): "
             << "Failed to set value on semaphore: "
             << strerror( errno ) << endl;
        exit( 1 );
    }

    readKernelSpecs();
    
    if (doInstallSignalHandler) {
        installSignalHandler();
    }
}



ThreadSemFactory::~ThreadSemFactory() {
    while ( ! _usedSemaphoreSets.empty() ){
        if ( semctl( _usedSemaphoreSets.back(), 0, IPC_RMID, 0 ) == -1 ) {
            cerr << "ThreadSemFactory::~ThreadSemFactory(): Failed to remove semaphore set: "
                 << strerror( errno ) << endl;
        }
        _debug && cerr << "ThreadSemFactory::~ThreadSemFactory(): Removed semaphore set id="
                       << _usedSemaphoreSets.back() << endl;
        _usedSemaphoreSets.pop_back();
    }
    
    if ( semctl( _myMemoryProtectionId, 0, IPC_RMID, 0 ) == -1 ) {
            cerr << "ThreadSemFactory::~ThreadSemFactory(): Failed to remove semaphore set: "
                 << strerror( errno ) << endl;
    }

    _instance_ = 0;
}

ThreadSemFactory *
ThreadSemFactory::instance() {
    if ( _instance_ == 0 ) {
        new ThreadSemFactory(true);
    }
    return _instance_;
}

bool
ThreadSemFactory::hasInstance() {
    return (_instance_ != 0);
}


void
ThreadSemFactory::installSignalHandler() {
    struct sigaction mySigAction;
    
    mySigAction.sa_handler = handleSignal;
    
    sigemptyset(&mySigAction.sa_mask);
    mySigAction.sa_flags = 0;
    sigaction(SIGINT, &mySigAction, 0);
    sigaction(SIGTERM, &mySigAction, 0);
}


void
ThreadSemFactory::handleSignal(int theSig) {
    if (((theSig == SIGINT) || (theSig == SIGTERM)) && (_instance_)){
        printf("ThreadSemFactory(): Received signal SIGINT/SIGTERM. "
               "Deleting ThreadSemFactory instance to free system resources\n");
        if (_instance_->_exitOnSignal) {
            delete _instance_;
            exit(-1);
        } else {
            delete _instance_;
        }
    }
}


ThreadSem *
ThreadSemFactory::newLock( int value ) {

    lock();
    LockBasketIterator myLockIt;
    if ( _recyclableLocks.empty() ) {
        if (( _usedSemaphoreSets.empty()) || ( _curSemaphoreSetIndex == _maxSemaphoreSetSize )) {
            int newSemaphoreSetId = semget( IPC_PRIVATE, _maxSemaphoreSetSize,
                                            SemaphoreSetFlags );
            if ( newSemaphoreSetId == -1 ) {
                cerr << "ThreadSemFactory::newLock(): "
                     << "Failed to create new semaphore set: "
                     << strerror( errno ) << endl;
                return 0;
            }
            _usedSemaphoreSets.push_back( newSemaphoreSetId );
            _curSemaphoreSetIndex = 0;
        }
        _debug && cerr << "creating new lock" << endl;
        ThreadSem * newLock = new ThreadSem( _usedSemaphoreSets.back(), 
                                       _curSemaphoreSetIndex, value);
 
        myLockIt = _usedLocks.insert( _usedLocks.end(),  newLock);
        _curSemaphoreSetIndex += 1; 
    
    } else {
        ThreadSem * myLock = _recyclableLocks.back();
        _recyclableLocks.pop_back();
        myLockIt = _usedLocks.insert( _usedLocks.begin(),  myLock );
        _debug && cerr << "recyling old lock Index: " << (*myLockIt)->getSemaphoreSetIndex() 
                       << endl;
        (*myLockIt)->setValue( value );

    }
    
    unlock();

    return *myLockIt;
}


void
ThreadSemFactory::freeLock( ThreadSem * myLock ){
    lock();

    LockBasketIterator it = _usedLocks.find( myLock );
    _recyclableLocks.push_back( *it );
    _usedLocks.erase( it );

    unlock();
}


void
ThreadSemFactory::readKernelSpecs(){ 

    _debug && cerr << "ThreadSemFactory::readKernelSpecs(): "
                   << "Reading maximum values for semaphore sets from '" 
                   << ProcfsFile << "'" << endl;

    ifstream inFile( ProcfsFile.c_str() );

    if ( inFile ) {
        
        inFile >> _maxSemaphoreSetSize; 
        
    } else {
        cerr << "ThreadSemFactory::readKernelSpecs(): Can not read procfs file '" 
             << ProcfsFile << "'. Using defaults." << endl;

             _maxSemaphoreSetSize = default_maxSemaphoreSetSize;
    }

    //DS: The first semaphore of each set is used by someone else.
    //    Maybe kernels selfprotection 

    _maxSemaphoreSetSize -= 1;
    
    _debug && cerr << "ThreadSemFactory::readKernelSpecs(): "
                   << "max semaphores per set: " << _maxSemaphoreSetSize << endl;
}


void
ThreadSemFactory::printStatistic(){
    lock();
    cerr << "ThreadSemFactiory::printStatistic():" << endl
         << "\tmax semaphores per sets   : " << _maxSemaphoreSetSize << endl
         << "\tused semaphore sets       : " << _usedSemaphoreSets.size() << endl
         << "\tcurrently used semaphores : " << _usedLocks.size() << endl
         << "\trecyclable semaphores     : " << _recyclableLocks.size() << endl
         << "\ttotal allocated semaphores: " << _usedLocks.size() + _recyclableLocks.size() 
         << endl;
    unlock();
}


void
ThreadSemFactory::lock(){
    static sembuf semop_P[1] = { 0, -1, SEM_UNDO };
    semop( _myMemoryProtectionId, semop_P, 1 );
}


void
ThreadSemFactory::unlock(){
    static sembuf semop_V[1] = { 0, 1, SEM_UNDO };
    semop( _myMemoryProtectionId, semop_V, 1 );
}


//=============================================================================
//
// $Log: ThreadSemFactory.cpp,v $
// Revision 1.1.1.1  2003/05/12 14:20:23  uzadow
// no message
//
// Revision 1.1.1.1  2002/09/17 15:37:05  wolfger
// initial checkin
//
// Revision 1.2  2002/09/06 18:13:28  valentin
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
// Revision 1.1.2.4  2002/08/30 16:25:28  martin
// more std-ization
//
// Revision 1.1.2.3  2002/02/27 09:56:34  stefan
// - added getThreadID() in PosixThread. cosmetic stuff in other files
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
// Revision 1.1.2.1  2002/02/07 09:28:02  stefan
// - semaphores for threads. unlike the MpLock/MpLockFactory classes these
//   sempahore classes do not rely upon Performer, and are not suitable
//   for multi-processing
//
//
//=============================================================================
