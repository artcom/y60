//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioScheduler.cpp,v $
//     $Author: thomas $
//   $Revision: 1.29 $
//
//=============================================================================

#include "AudioScheduler.h"
#include "AudioModule.h"
#include "AudioDefs.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioSyncModule.h"
#include "SoundException.h"
#include "AudioBuffer.h"
#include "IFileReader.h"

#include <asl/ThreadLock.h>
#include <asl/Auto.h>
#include <asl/Time.h>
#include <asl/Logger.h>

#include <signal.h>
#ifdef LINUX
#include <unistd.h>
#endif
#include <algorithm>
#include <iostream>
#include <set>

using namespace std;

namespace AudioBase {

//#define TRACE_GRAPH
//#define TRACE_CALLS

using namespace asl;

#ifdef TRACE_CALLS
#define TRACE_CALL(msg) \
    cerr  << setw(9) << setfill('*') << _curSample << " AudioScheduler" << msg;
#else
#define TRACE_CALL(msg)
#endif

#define DB(x) // x

AudioScheduler::AudioScheduler() :
    PosixThread(),
    _myRunning(false),
    _curSample(0),
    _myBufferSize(4096)
{
    AC_DEBUG << "AudioScheduler";
    _myLock = new ThreadLock;
}

AudioScheduler::~AudioScheduler() {
    AC_DEBUG << "~AudioScheduler";
    stop();
    delete _myLock;
}


void
AudioScheduler::start() {
#ifdef WIN32
    start(SCHED_OTHER, THREAD_PRIORITY_TIME_CRITICAL);
    //start(SCHED_OTHER, THREAD_PRIORITY_NORMAL);
#else
    start(SCHED_RR, PosixThread::getMaxPriority(SCHED_RR));
#endif
}

void
AudioScheduler::start(int myPriorityClass, int myPriority) {
    TRACE_CALL("::start(" << myPriorityClass << ", "
            << myPriority << ")" << endl);
    if(!_myRunning) {
        _myRunning = true;
        _curSample = 0;
        fork();
        bool amIRoot = setPriority (myPriorityClass, myPriority);
        if (!amIRoot) {
            setPriority (SCHED_OTHER, PosixThread::getMaxPriority(SCHED_OTHER));
        } else {
            AC_INFO << "Audio running with realtime priority.";
        }
    } else {
        throw SoundException("AudioScheduler::start called twice!",PLUS_FILE_LINE);
    }
}

void
AudioScheduler::stop() {
    TRACE_CALL("::stop()" << endl);
    if (_myRunning) {
        _myRunning = false;
        join();

        //Handle any pending stops.
        processGraphChanges();

        AsyncModuleList::iterator iter;
        for (iter = _myRootset.begin(); iter != _myRootset.end(); iter++) {
            _myStoppingList.push_back(*iter);
        }
        processGraphChanges();
    }
}

bool
AudioScheduler::isRunning() {
    return _myRunning;
}

void
AudioScheduler::lock() {
    _myLock->lock();
}

void
AudioScheduler::unlock() {
    _myLock->unlock();
}

void
AudioScheduler::addModule(AudioAsyncModule * theModule) {
    TRACE_CALL("::addModule(): " << theModule->getName() << "[" << theModule << "]" << endl);
    _myStartingList.push_back(theModule);
    if (!_myRunning) {
        processGraphChanges();
    }
}

void
AudioScheduler::connect(AudioOutput* theOutput, AudioInput* theInput) {
    TRACE_CALL("::connect()" << endl);
    _myConnectingList.push_back (Connection (theOutput, theInput));
    if (!_myRunning) {
        processGraphChanges();
    }
}


void
AudioScheduler::sendStop(AudioAsyncModule* theModule) {
    TRACE_CALL("::sendStop(" << theModule << ")" << endl);
    if (_myRunning) {
        AutoLocker<AudioScheduler> myLocker(*this);
        _myStoppingList.push_back(theModule);
    } else {
        AC_WARNING << "AudioScheduler::sendStop for '" << theModule->getName() << "' called while not running!";
    }
}

int
AudioScheduler::getNumAsyncModules() {
    return _myRootset.size();
}

void
AudioScheduler::getAndClearStoppedList(AsyncModuleList& theList) {
    TRACE_CALL("::getAndClearStoppedList()" << endl);
    AutoLocker<AudioScheduler> myLocker(*this);
    theList = _myStoppedList;
    _myStoppedList.clear();
}


void AudioScheduler::dump(bool verbose) {
    AutoLocker<AudioScheduler> myLocker(*this);
    cerr << "AudioScheduler: " << endl;
    cerr << "  Rootset: " << endl;
    AsyncModuleList::iterator iter;
    for (iter = _myRootset.begin(); iter != _myRootset.end(); iter++) {
        if (verbose) {
            (*iter)->dump("    ");
        } else {
            cerr << "    " << (*iter)->getName();
        }
    }
    ModuleList allModules;
    allModules.insert(allModules.begin(), _myRootset.begin(), _myRootset.end());
    ModuleList::iterator iter2;
    for (iter2 = allModules.begin(); iter2 != allModules.end(); iter2++) {
        ModuleList newModules = (*iter2)->getConnectedModules();
        allModules.splice (allModules.end(), newModules);
    }
    set<AudioModule *> allModuleSet;
    allModuleSet.insert (allModules.begin(), allModules.end());

    cerr << "  Connected modules: " << endl;
    set<AudioModule *>::iterator iter3;
    for (iter3 = allModuleSet.begin(); iter3 != allModuleSet.end(); iter3++) {
        if (verbose) {
            (*iter3)->dump("    ");
        } else {
            cerr << "    " << (*iter3)->getName() << endl;
        }
    }
}

void AudioScheduler::process() {
    processGraphChanges();
    TRACE_CALL("::process()" << endl);

    _myRunnable.clear();
    _myRunnable.insert (_myRunnable.begin(), _myRootset.begin(), _myRootset.end());
    if (!_myRunnable.size()) {
        asl::msleep (1);
    }
    while(_myRunnable.size()) {
        AudioSyncModule *mySyncModule = dynamic_cast<AudioSyncModule *>(_myRunnable.front());
        if (mySyncModule) {
            mySyncModule->callListeners();
        }
        _myRunnable.front()->process();
        _myRunnable.front()->postProcess();
        _myRunnable.pop_front();
    }
    _curSample += AudioBuffer::getNumSamples();
}

void
AudioScheduler::processGraphChanges() {
    // The module graph can't be changed while it is running,
    // so changes are scheduled and executed here.
    AutoLocker<AudioScheduler> myLocker(*this);
    TRACE_CALL("::processGraphChanges()" << endl);

    bool graphChanged = false;
    AsyncModuleList::iterator iter;
    for (iter = _myStartingList.begin(); iter != _myStartingList.end(); iter++) {
        graphChanged = true;
        _myRootset.push_front (*iter);
    }
    _myStartingList.clear();

    ConnectionList::iterator iter2;
    for (iter2 = _myConnectingList.begin();
            iter2 != _myConnectingList.end(); iter2++) {
        graphChanged = true;
        (*iter2)._myOutput->connect((*iter2)._myInput);
    }
    _myConnectingList.clear();

    for (iter = _myStoppingList.begin(); iter != _myStoppingList.end(); iter++) {
         graphChanged = true;
         if (dynamic_cast<IFileReader *>(*iter)) {
             _myStoppedList.push_back(*iter);
         } else {
             AC_DEBUG << "Won't put unknown Reader of Class: '" << typeid(*(*iter)).name() << "' into stopped list.";
         }
         _myRootset.remove (*iter);
         AC_TRACE << "AudioScheduler deleting '" << typeid(*(*iter)).name() << "' " << (void*)(*iter);
         delete (*iter);
         // XXX [TS] For some strange reason _myStoppingList.clear() crashes if we
         // don't set this to NULL. So we do. This is programming by coincidence,
         // I know. :(
         *iter = NULL;
    }
    _myStoppingList.clear();

#ifdef TRACE_GRAPH
    if (graphChanged) {
        dump(true);
    }
#endif
}

void
AudioScheduler::run() {
    TRACE_CALL("::run()" << endl);
    try {
        while(_myRunning) {
            process();
        }
    } catch (const asl::Exception & e) {
        AC_ERROR << "AudioScheduler::run: Unhandled exception.";
        AC_ERROR << e;
        dumpState();
    } catch (...) {
        try {
            AC_ERROR << "AudioScheduler::run: Unhandled exception.";
            dumpState();
        } catch (...) {
            AC_ERROR << "AudioScheduler::run: Exception during exception handling.";
        }
    }
}

void AudioScheduler::dumpState() {
    cerr << "AudioScheduler state: " << endl;
    cerr << "  _myRunning = " << _myRunning << endl;
    cerr << "  _myRootset.size() = " << _myRootset.size() << endl;
    cerr << "  _myRunnable.size() = " << _myRunnable.size() << endl;
    cerr << "  _myStartingList.size() = " << _myStartingList.size() << endl;
    cerr << "  _myStoppingList.size() = " << _myStoppingList.size() << endl;
    cerr << "  _myConnectingList.size() = " << _myConnectingList.size() << endl;
    dump(true);
}

}
