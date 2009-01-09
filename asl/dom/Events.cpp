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
//    $RCSfile: Events.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: tiny fast XML-Parser and DOM
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "Events.h"


#define DB(x) //  x
#define DB2(x) //  x

using namespace std;
using namespace dom;

const char * dom::PhaseTypeName[] = {"undefined", "capturing", "target", "bubbling"};

EventFactory::EventFactory() {
}

EventPtr
EventFactory::createEvent(const DOMString & theType) const {
    EventPtr myPrototype = findPrototype(theType); 
    if (myPrototype) {
        DB(AC_TRACE << "EventFactory::createEvent('"<<theType<<"')"<<" returns value"<<std::endl;)
        return myPrototype->clone();
    }
    DB(AC_TRACE << "EventFactory::createEvent('"<<theType<<"')"<<" returns 0"<<std::endl;)
    return EventPtr(0);
}

void
EventFactory::registerPrototype(const DOMString & theType, EventPtr thePrototype) {
    DB(AC_TRACE << "EventFactory::registerPrototype('"<<theType<<"',"<<(void*)&(*thePrototype)<<")"<<std::endl;)
    _myPrototypes[theType] = EventPtr(thePrototype->clone());
}

const EventPtr
EventFactory::findPrototype(const DOMString & theType) const {
    ProtoMap::const_iterator myPrototype = _myPrototypes.find(theType);
    if (myPrototype != _myPrototypes.end()) {
        return myPrototype->second;
    }
    return EventPtr(0);
}

void dom::registerStandardTypes(EventFactory & theFactory) {
    theFactory.registerPrototype("generic", EventPtr(new GenericEvent<int>("generic",true, true)));
    theFactory.registerPrototype("test", EventPtr(new GenericEvent<int>("test",true, true)));
}
