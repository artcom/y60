//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Event.cpp,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

#include "Event.h"

namespace y60 {

    Event::Event(Type theType) : type(theType), when()
    {
    };

    Event::Event(Type theType, const asl::Time & theTime) : type(theType), when(theTime) {
    }

    Event::~Event() {
    };

}

