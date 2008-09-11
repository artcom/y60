//=============================================================================
// Copyright (C) 2006 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef Y60_CLASS_MAP_SINGLETON_INCLUDED
#define Y60_CLASS_MAP_SINGLETON_INCLUDED

#include <asl/base/Singleton.h>

#include <map>

#include "jssettings.h"
#include <js/spidermonkey/jsapi.h>


class ClassMapSingleton :
    public asl::Singleton<ClassMapSingleton>
{
    public:
        typedef std::map<const char *, JSClass> ClassMap;

        ClassMap & getClassMap();
        friend class asl::SingletonManager;
    private:
        ClassMap _myClassMap;

};

#endif // Y60_CLASS_MAP_SINGLETON_INCLUDED
