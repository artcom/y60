//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioScheduler.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.10 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_AUDIOSCHEDULER
#define INCL_AUDIOSCHEDULER

#include "AudioModule.h"
#include "AudioAsyncModule.h"

#include <asl/PosixThread.h>
#include <asl/Singleton.h>
#include <asl/settings.h>
#include <list>

#ifdef WIN32
#include <windows.h>
#endif

namespace asl {
    class ThreadLock;
}

namespace AudioBase {

class AudioOutput;
class AudioInput;

class AudioScheduler : private PosixThread, public asl::Singleton<AudioScheduler> {

    friend class asl::SingletonManager;
public:    
    virtual ~AudioScheduler();
    void process();
    
    void start();
    void start(int myPriorityClass, int myPriority);
    void stop();
    bool isRunning();

    void addModule (AudioAsyncModule * theModule);
    void connect(AudioOutput*, AudioInput*);
    
    void sendStop (AudioAsyncModule * theModule); 
    int getNumAsyncModules();
    
    // Assumes that the caller clears the list inside of a lock/unlock pair.
    void getAndClearStoppedList(AsyncModuleList& theList);
    
    void dump(bool verbose);
   
    // This lock makes sure that the module graph doesn't change while 
    // the graph is running. 
    void lock(); 
    void unlock(); 

    void makeRunnable(AudioModule* theModule) {
        ModuleList::iterator it;
        it = _myRunnable.begin();
        it++;
        _myRunnable.insert(it, theModule);
    }

    unsigned getBufferSize() const {
        return _myBufferSize;
    }

    void setBufferSize(unsigned theBufferSize) {
        _myBufferSize = theBufferSize;
    }

/*    static AudioScheduler * get() {
        return _myInstance;
    }
*/
private:
    AudioScheduler();

    struct Connection {
        AudioOutput * _myOutput;
        AudioInput * _myInput;
    
        Connection (AudioOutput * myOutput, AudioInput * myInput)
            : _myOutput (myOutput), _myInput(myInput)
        {}
    };
    typedef std::list<Connection> ConnectionList;
   
    void processGraphChanges();
    void run();
    void dumpState();

    AsyncModuleList _myRootset;
    ModuleList _myRunnable;

    AsyncModuleList _myStoppingList;
    AsyncModuleList _myStartingList;
    AsyncModuleList _myStoppedList;
    ConnectionList _myConnectingList;
   
    bool _myRunning;
    asl::ThreadLock* _myLock;
    int _myPriorityClass;
    int _myPriority;
    
    // At the moment, this is only used for debugging purposes.
    asl::Unsigned64 _curSample; 

    unsigned _myBufferSize;
    
//    static AudioScheduler * _myInstance;
    
};


}

#endif
