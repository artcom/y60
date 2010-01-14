/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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

// own header
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
//    cerr << "Destroying singletons." << endl;
    mySingletonManagerDestroyed = true;
    _mySingletonMap.clear();

    // Call stop on all singletons to give them a chance to shut down correctly, while
    // all other singletons are still alive.
    for (SingletonList::iterator it = _mySingletonList.begin(); it != _mySingletonList.end(); ++it) {
        AC_DEBUG << "SingletonManager: stopping "<< typeid(**it).name();
        (*it)->stop();
    }

    // destroy singletons in reverse order of construction
    _mySingletonList.reverse();
    for (SingletonList::iterator it = _mySingletonList.begin(); it != _mySingletonList.end(); ) {
        // Delete logger last, to allow other singletons to use the logger in their destructor
        if (dynamic_cast_Ptr<Logger>(*it) == 0) {
            AC_DEBUG << "  destroying singleton " << typeid(**it).name() << " size: "
                     << _mySingletonList.size() << endl;
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

