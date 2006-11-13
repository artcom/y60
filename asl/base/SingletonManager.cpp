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
//    $RCSfile: SingletonManager.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.4 $
//
// Description:
//
//=============================================================================

#include "SingletonManager.h"
#include "Logger.h"

using namespace std;

namespace asl {

SingletonManager::~SingletonManager() {
//    cerr << "Destroying SingletonManager" << endl;
    destroyAllSingletons();
}

void
SingletonManager::destroyAllSingletons() {
    // Note that using the Logger here is unsafe. The Logger itself is a singleton that
    // will be destroyed at some point.
    static bool mySingletonManagerDestroyed = false;
    if (mySingletonManagerDestroyed) {
        // XXX: This happens if the SingletonManager destructor and another handler 
        //      both call destroyAllSingletons. In that case, this is invalid in the
        //      second call. 
//        cerr << "  --- Already gone" << endl;
        return;
    }
    cerr << "Destroying singletons." << endl;
    mySingletonManagerDestroyed = true;
    _mySingletonMap.clear();

    // Call stop on all singletons to give them a chance to shut down correctly, while
    // all other singletons are still alive.
    for (SingletonList::iterator it = _mySingletonList.begin(); it != _mySingletonList.end(); ++it) {
        (*it)->stop();
    }

    // destroy singletons in reverse order of construction
    _mySingletonList.reverse();
    for (SingletonList::iterator it = _mySingletonList.begin(); it != _mySingletonList.end(); ) {
        // Delete logger last, to allow other singletons to use the logger in their destructor
        if (dynamic_cast_Ptr<Logger>(*it) == 0) {
//            cerr << "  destroying singleton " << typeid(**it).name() << " size: " 
//                    << _mySingletonList.size() << endl;
            it = _mySingletonList.erase(it);
        } else {
            ++it;
        }
    }

    _mySingletonList.clear();
//  cerr << "All singletons destroyed." << endl;
}

SingletonManager &
SingletonManager::get() {
    static SingletonManager myModuleSingletonManager;
    return myModuleSingletonManager;
}

void
SingletonManager::setDelegate(SingletonManager * theDelegate) {
    if (theDelegate != this) {
        _myDelegate = theDelegate;
        AC_DEBUG << "delegating SingletonManager from " << this << " > " << theDelegate;
    } else {
        AC_DEBUG << "delegation to self ignored.";
    }
}


}

