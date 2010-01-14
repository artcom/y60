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
//    $RCSfile: SingletonManager.h,v $
//
//     $Author: pavel $
//
//   $Revision: 1.14 $
//
// Description:
//
//=============================================================================

#ifndef _SINGLETON_MANAGER_INCLUDED_
#define _SINGLETON_MANAGER_INCLUDED_

#include "asl_base_settings.h"

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <typeinfo>

#include "Ptr.h"
#include "Exception.h"
#include "SingletonBase.h"

#define DB(x) //x
#define DB2(x) //x

namespace asl {


/*! \addtogroup aslbase */
/* @{ */


/** Singletonmanager - The Why and How
 *
 * Dynamically linking plug-ins to our process breaks the classic
 * Singleton Pattern (see "Effective C++") if these Plug-Ins are statically linked
 * with the same base libraries that are also used in the main binary.
 * This is because duplicating the library code also duplicates local static variables
 * which are typically used to ensure that Singletons are instantiated only once.
 * This problem is solved by passing a pointer to a SingletonManager to each
 * dynamically loaded module at load time. This SingletonManager, which is a classic Singleton
 * and therefore exists once per module, is then used as a delegate for the module-local
 * SingletonManager instance. To put it another way: Each module has its own SingletonManager that
 * forwards its calls to main module's Singletonmanager instances. -jbo
 * WARNING: When the SingletonManager is destroyed, it destructs all Singletons in reverse
 * order of construction. Make sure no Singletons are accessed after the SingletonManager
 * was destroyed!
 */

class ASL_BASE_DECL SingletonManager {
public:
    ~SingletonManager();

    // This function is called by the destructor. It is also called by the windows ctrl-break
    // handler to make sure that all singletons are destroyed even if the (static)
    // SingletonManager isn't.
    void destroyAllSingletons();

    // The SingletonManager itself is a classic Singleton
    // (Under Windows it will be instantiated once per module/dll/plugin)
    static SingletonManager & get();

    // needs to be called by each dynamically loaded Plug-In
    // to delegate singleton lookup to the one and only
    // SingletonManager in charge for the running process.
    void setDelegate(SingletonManager * theDelegate);

    template<class T>
    T& getSingletonInstance() const {
        // NOTE: this is only called once per module and Singleton class,
        // so performance doesn't really matter.
        DB2(AC_TRACE << "getSingletonInstance with " << this << std::endl);
        if (_myDelegate) {
            DB2(AC_TRACE << "delegating call to getSingletonInstance()" << std::endl);
            return _myDelegate->getSingletonInstance<T>();
        }
        DB2(AC_TRACE << "looking up Singleton " << typeid(T).name() << std::endl);
        SingletonMap::iterator i = _mySingletonMap.find(&typeid(T));

        if (i != _mySingletonMap.end()) {
            DB2(AC_TRACE << "cache hit" << std::endl);
            return *(dynamic_cast_Ptr<T>(i->second));
        } else {
            DB2(AC_TRACE << "cache miss!!" << std::endl);
            SingletonBase * sb = new T();
            Ptr<SingletonBase> mySingletonPtr = Ptr<SingletonBase>(sb);
            DB(AC_TRACE << "Singleton '" << typeid(T).name() << "' instantiated. " << std::endl);
            _mySingletonList.push_back(mySingletonPtr);

            _mySingletonMap.insert(std::make_pair(&typeid(T), mySingletonPtr));
            return *dynamic_cast<T*>(sb);
        }
    }
private:
    SingletonManager() :
        _myDelegate(0)
    {}

    class LessTypeInfoPtr {
    public:
        bool operator()(const std::type_info *theLeft, const std::type_info *theRight) const {
            return 0 != theLeft->before(*theRight);
        }
    };

    // don't even think of copying this ;)
    SingletonManager(const SingletonManager&) {}
    SingletonManager & operator=(const SingletonManager &) {return *this;}

    SingletonManager *_myDelegate;

    typedef std::map< const std::type_info *, Ptr<SingletonBase>, LessTypeInfoPtr> SingletonMap;
    typedef std::list< Ptr<SingletonBase> > SingletonList;

    mutable SingletonList _mySingletonList; // for keeping track of order of construction
    mutable SingletonMap _mySingletonMap;   // for fast lookup
};


/** SingletonDeleter
 *
 * When using dynamic linking (DLLs) in Windows, DLLs are loaded and unloaded
 * in some order that the OS determines. Therefor it might happen that a DLL
 * already is unloaded when the SingletonManager, from its destructor, tries
 * to call code in that DLL in order to destroy a singleton.
 *
 * To prevent this, the global constant singletonDeleter instance, instantiated
 * once for every DLL, will destroy all singletons from its destructor as soon
 * as the first DLL is unloaded.
 */
class ASL_BASE_DECL SingletonDeleter {
public:
    SingletonDeleter() {}
    ~SingletonDeleter()
    {
        asl::SingletonManager::get().destroyAllSingletons();
    }
private:
    SingletonDeleter(const SingletonDeleter&);
    SingletonDeleter& operator=(const SingletonDeleter&);
};

const SingletonDeleter ourSingletonDeleter;

/* @} */

}

#undef DB
#undef DB2

#endif
